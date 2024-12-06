#pragma once
#include <string>
#include <optional>
#include <coreinit/filesystem.h>
#include <coreinit/core.h>

struct Version {
    std::string tag;
    std::string download_url;
};

class UpdCheck {
public:
    static std::optional<Version> checkForUpdates(const std::string& current_version);
    static bool downloadUpdate(const std::string& url, const std::string& output_path);
private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t WriteFile(void* ptr, size_t size, size_t nmemb, void* userp);
    static FSClient* fsClient;
    static FSCmdBlock* cmdBlock;
};
