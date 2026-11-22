#include <map>
#include <memory>

#include "Container.h"

#include "Adapters/Directory.h"
#include "Adapters/Package.h"
#include "Adapters/Types.h"
#include "Adapters/Virtual.h"
#include "WallpaperEngine/Assets/AssetLoadException.h"
#include "WallpaperEngine/Logging/Log.h"

// Windows 移植修正：UTF-8 安全路径转换（pkg 内可含中文文件名）
static std::string pathToUtf8 (const std::filesystem::path& p) {
    try {
	const auto u8 = p.u8string ();
	return std::string (reinterpret_cast<const char*> (u8.data ()), u8.size ());
    } catch (...) {
	return {};
    }
}

static std::filesystem::path utf8ToPath (const std::string& s) {
    return std::filesystem::u8path (s);
}
using namespace WallpaperEngine::FileSystem;
using namespace WallpaperEngine::FileSystem::Adapters;

/**
 * Normalizes a file path to get rid of relative stuff as most as possible
 * This is not a security measure but helps keep adapters that do not really have an actual filesystem
 * behind to trust the input data without much validation
 * @see https://en.cppreference.com/w/cpp/filesystem/path/lexically_normal
 */
std::filesystem::path normalize_path (const std::filesystem::path& input_path) {
    return input_path.lexically_normal ();
}

Container::Container () {
    // register all available factories
    this->m_factories.push_back (std::make_unique<VirtualFactory> ());
    this->m_factories.push_back (std::make_unique<PackageFactory> ());
    this->m_factories.push_back (std::make_unique<DirectoryFactory> ());

    this->m_vfs = std::make_shared<VirtualAdapter> ();
    this->m_mountpoints.emplace_back ("/", this->m_vfs);
}

ReadStreamSharedPtr Container::read (const std::filesystem::path& path) const {
    const auto normalized = normalize_path (path);

    ReadStreamSharedPtr result = this->resolveAdapterForFile (path).open (normalized);

    if (result->fail ()) {
	throw std::runtime_error ("Failed to open file: " + pathToUtf8 (normalized));
    }

    return result;
}

std::string Container::readString (const std::filesystem::path& path) const {
    const auto stream = this->read (path);
    std::stringstream buffer;
    buffer << stream->rdbuf ();
    return buffer.str ();
}

std::filesystem::path Container::physicalPath (const std::filesystem::path& path) const {
    const auto normalized = normalize_path (path);

    return this->resolveAdapterForFile (path).physicalPath (normalized);
}

AdapterSharedPtr Container::mount (const std::filesystem::path& path, const std::filesystem::path& mountPoint) {
    // check if any adapter can handle the path
    for (const auto& factory : this->m_factories) {
	if (factory->handlesMountpoint (path) == false) {
	    continue;
	}

	return this->m_mountpoints.emplace_back (mountPoint, factory->create (path)).second;
    }

    throw std::filesystem::filesystem_error (
	"The specified mount cannot be handled by any of the filesystem adapters", path, std::error_code ()
    );
}

VirtualAdapter& Container::getVFS () const { return *this->m_vfs; }

Adapter& Container::resolveAdapterForFile (const std::filesystem::path& path) const {
    return this->resolveAdapterForFileImpl (path, 0);
}

Adapter& Container::resolveAdapterForFileImpl (const std::filesystem::path& path, int depth) const {
    const auto normalized = normalize_path (path);
    // 统一正斜杠 + UTF-8 比较（generic_string 在 Windows 用 ANSI，遇到中文文件名会抛 No mapping）
    std::string normalizedStr = pathToUtf8 (normalized);
    std::replace (normalizedStr.begin (), normalizedStr.end (), '\\', '/');
    if (depth > 3) {
	throw std::filesystem::filesystem_error ("resolveAdapterForFile recursion too deep", path, std::error_code ());
    }

    for (const auto& [root, adapter] : this->m_mountpoints) {
	std::string rootStr = pathToUtf8 (root);
	std::replace (rootStr.begin (), rootStr.end (), '\\', '/');
	if (normalizedStr.starts_with (rootStr) == false) {
	    continue;
	}

	const auto relative = normalizedStr.substr (rootStr.length ());
	if (adapter->exists (utf8ToPath (relative)) == false) {
	    continue;
	}

	return *adapter;
    }

    if (normalizedStr.starts_with ("/") == false) {
	return this->resolveAdapterForFileImpl ("/" + normalizedStr, depth + 1);
    }

    throw std::filesystem::filesystem_error (
	"Cannot find requested file in any of the mountpoints", path, std::error_code ()
    );
}

void Container::registerAdapterFactory (FactoryUniquePtr factory) { this->m_factories.push_back (std::move (factory)); }