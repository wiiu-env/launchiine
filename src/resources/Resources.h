#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

//! forward declaration
class GuiImageData;

class GuiSound;

class Resources {
public:
    Resources() = default;
    ~Resources() = default;

    static void Clear();

    static std::vector<uint8_t> GetFile(std::string_view filename);

    static std::shared_ptr<GuiImageData> GetImageData(std::string_view filename);

    static std::unique_ptr<GuiSound> GetSound(std::string_view filename);

private:
    static std::unique_ptr<Resources> instance;

    std::map<std::string, std::pair<uint32_t, std::shared_ptr<GuiImageData>>> imageDataMap;
    std::map<std::string, std::pair<uint32_t, std::shared_ptr<GuiSound>>> soundDataMap;
};