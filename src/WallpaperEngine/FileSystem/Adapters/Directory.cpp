#include <filesystem>
#include <fstream>
#include <memory>

#include "Directory.h"

#include "WallpaperEngine/Assets/AssetLoadException.h"
#include "WallpaperEngine/Data/Utils/MemoryStream.h"

static std::string pkgPathToUtf8 (const std::filesystem::path& p) {
    try {
	const auto u8 = p.u8string ();
	return std::string (reinterpret_cast<const char*> (u8.data ()), u8.size ());
    } catch (...) {
	return {};
    }
}
using namespace WallpaperEngine::FileSystem;
using namespace WallpaperEngine::FileSystem::Adapters;

ReadStreamSharedPtr DirectoryAdapter::open (const std::filesystem::path& path) const {
    auto finalpath = std::filesystem::canonical (this->basepath / path);

    if (pkgPathToUtf8 (finalpath).find (pkgPathToUtf8 (this->basepath)) != 0) {
	throw std::filesystem::filesystem_error ("Cannot find file", path, std::error_code ());
    }

    const auto status = std::filesystem::status (finalpath);

    if (!std::filesystem::exists (finalpath)) {
	throw std::filesystem::filesystem_error ("Cannot find file", path, std::error_code ());
    }

    if (!std::filesystem::is_regular_file (status)) {
	throw std::filesystem::filesystem_error ("Expected file but found a directory", path, std::error_code ());
    }

    // Windows 移植修正：一次性读入内存，避免 ifstream + seek 的缓冲错位
    std::ifstream file (finalpath, std::ios::binary);
    if (!file) {
	throw std::filesystem::filesystem_error ("Cannot open file", path, std::error_code ());
    }
    file.seekg (0, std::ios::end);
    std::streamoff filesize = file.tellg ();
    file.seekg (0, std::ios::beg);
    if (filesize <= 0) {
	throw std::filesystem::filesystem_error ("Empty file", path, std::error_code ());
    }
    auto buffer = std::make_unique<char[]> (static_cast<size_t> (filesize));
    file.read (buffer.get (), filesize);
    return std::make_shared<WallpaperEngine::Data::Utils::MemoryStream> (std::move (buffer), static_cast<size_t> (filesize));
}

bool DirectoryAdapter::exists (const std::filesystem::path& path) const {
    try {
	const auto finalpath = std::filesystem::canonical (this->basepath / path);

	if (pkgPathToUtf8 (finalpath).find (pkgPathToUtf8 (this->basepath)) != 0) {
	    return false;
	}

	const auto status = std::filesystem::status (finalpath);

	if (!std::filesystem::exists (finalpath)) {
	    return false;
	}

	if (!std::filesystem::is_regular_file (status)) {
	    return false;
	}

	return true;
    } catch (std::filesystem::filesystem_error&) {
	return false;
    }
}

std::filesystem::path DirectoryAdapter::physicalPath (const std::filesystem::path& path) const {
    auto finalpath = std::filesystem::canonical (this->basepath / path);

    if (pkgPathToUtf8 (finalpath).find (pkgPathToUtf8 (this->basepath)) != 0) {
	throw std::filesystem::filesystem_error ("Cannot find file", path, std::error_code ());
    }

    return finalpath;
}

bool DirectoryFactory::handlesMountpoint (const std::filesystem::path& path) const {
    try {
	const auto finalpath = std::filesystem::canonical (path);
	const auto status = std::filesystem::status (finalpath);

	return std::filesystem::exists (finalpath) && std::filesystem::is_directory (status);
    } catch (std::filesystem::filesystem_error&) {
	return false;
    }
}

AdapterSharedPtr DirectoryFactory::create (const std::filesystem::path& path) const {
    auto finalpath = std::filesystem::canonical (path);
    const auto status = std::filesystem::status (finalpath);

    if (!std::filesystem::exists (finalpath)) {
	throw std::filesystem::filesystem_error ("Cannot find directory", path, std::error_code ());
    }

    if (!std::filesystem::is_directory (status)) {
	throw std::filesystem::filesystem_error ("Expected directory but found a file", path, std::error_code ());
    }

    return std::make_unique<DirectoryAdapter> (finalpath);
}