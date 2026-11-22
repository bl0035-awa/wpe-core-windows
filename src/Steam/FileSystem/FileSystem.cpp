#include "FileSystem.h"
#include "WallpaperEngine/Logging/Log.h"
#include <climits>
#include <cstdlib>
#include <filesystem>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

std::vector<std::string> appDirectoryPaths = {
#ifdef _WIN32
    "d:/steam/steamapps/common",
    "c:/program files (x86)/steam/steamapps/common",
    "c:/program files/steam/steamapps/common",
    "e:/steam/steamapps/common",
    "f:/steam/steamapps/common",
    "g:/steam/steamapps/common",
#else
    ".steam/steam/steamapps/common",
    ".local/share/Steam/steamapps/common",
    ".var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/common",
    "snap/steam/common/.local/share/Steam/steamapps/common",
#endif
};

std::vector<std::string> workshopDirectoryPaths = {
#ifdef _WIN32
    "d:/steam/steamapps/workshop/content",
    "c:/program files (x86)/steam/steamapps/workshop/content",
    "c:/program files/steam/steamapps/workshop/content",
    "e:/steam/steamapps/workshop/content",
    "f:/steam/steamapps/workshop/content",
    "g:/steam/steamapps/workshop/content",
    "i:/steam/steamapps/workshop/content",
    "c:/steamlibrary/steamapps/workshop/content",
    "d:/steamlibrary/steamapps/workshop/content",
    "e:/steamlibrary/steamapps/workshop/content",
    "f:/steamlibrary/steamapps/workshop/content",
    "g:/steamlibrary/steamapps/workshop/content",
    "i:/steamlibrary/steamapps/workshop/content",
    "h:/steamlibrary/steamapps/workshop/content",
#else
    ".local/share/Steam/steamapps/workshop/content",
    ".steam/steam/steamapps/workshop/content",
    ".var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/workshop/content",
    "snap/steam/common/.local/share/Steam/steamapps/workshop/content",
#endif
};

std::filesystem::path detectHomepath () {
#ifdef _WIN32
    char* home = getenv ("USERPROFILE");
    if (home == nullptr) home = getenv ("HOMEDRIVE");
#else
    char* home = getenv ("HOME");
#endif

    if (home == nullptr) {
	sLog.exception ("Cannot find home directory for the current user");
    }

    const std::filesystem::path path = home;

    if (!std::filesystem::is_directory (path)) {
	sLog.exception ("Cannot find home directory for current user, ", home, " is not a directory");
    }

    return path;
}

std::filesystem::path Steam::FileSystem::workshopDirectory (int appID, const std::string& contentID) {
#ifdef _WIN32
    // Windows: 允许直接传入绝对路径（场景目录或 project.json）
    std::error_code ec;
    std::filesystem::path asPath = std::filesystem::u8path (contentID);
    if (asPath.is_absolute () && std::filesystem::exists (asPath, ec)) {
	if (std::filesystem::is_regular_file (asPath, ec)) {
	    return asPath.parent_path ();
	}
	return asPath;
    }
#endif
    auto homepath = detectHomepath ();

    for (const auto& current : workshopDirectoryPaths) {
	auto currentpath = std::filesystem::path (homepath) / current / std::to_string (appID) / contentID;

	if (!std::filesystem::exists (currentpath) || !std::filesystem::is_directory (currentpath)) {
	    continue;
	}

	return currentpath;
    }

    sLog.exception ("Cannot find workshop directory for steam app ", appID, " and content ", contentID);
}

std::filesystem::path Steam::FileSystem::appDirectory (const std::string& appDirectory, const std::string& path) {
    auto homepath = detectHomepath ();

    for (const auto& current : appDirectoryPaths) {
	auto currentpath = std::filesystem::path (homepath) / current / appDirectory / path;

	if (!std::filesystem::exists (currentpath) || !std::filesystem::is_directory (currentpath)) {
	    continue;
	}

	return currentpath;
    }

    sLog.exception ("Cannot find directory for steam app ", appDirectory, ": ", path);
}
