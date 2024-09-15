#include "Resources.h"
#include "filelist.h"
#include "fs/FSUtils.h"
#include <chrono>
#include <gui/GuiImageData.h>
#include <gui/GuiSound.h>
#include <span>
#include <string>
#include <thread>

std::unique_ptr<Resources> Resources::instance = {};

void Resources::Clear() {
    for (int32_t i = 0; ResourceList[i].filename != nullptr; ++i) {
        ResourceList[i].CustomFileBuffer.clear();
    }

    instance.reset();
}

std::vector<uint8_t> Resources::GetFile(std::string_view filename) {
    for (int32_t i = 0; ResourceList[i].filename != nullptr; ++i) {
        if (filename == ResourceList[i].filename) {
            if (!ResourceList[i].CustomFileBuffer.empty()) {
                return ResourceList[i].CustomFileBuffer;
            }
            return {ResourceList[i].DefaultFile, ResourceList[i].DefaultFile + ResourceList[i].DefaultFileSize};
        }
    }

    return {};
}


std::shared_ptr<GuiImageData> Resources::GetImageData(std::string_view filename) {
    if (!instance) {
        instance = std::make_unique<Resources>();
    }

    auto itr = instance->imageDataMap.find(std::string(filename));
    if (itr != instance->imageDataMap.end()) {
        itr->second.first++;
        return itr->second.second;
    }

    for (int32_t i = 0; ResourceList[i].filename != nullptr; ++i) {
        if (filename == ResourceList[i].filename) {
            std::span<const uint8_t> fileData;
            if (!ResourceList[i].CustomFileBuffer.empty()) {
                fileData = ResourceList[i].CustomFileBuffer;
            } else {
                fileData = std::span(ResourceList[i].DefaultFile, ResourceList[i].DefaultFile + ResourceList[i].DefaultFileSize);
            }

            if (fileData.empty() || fileData.data() == nullptr) {
                return nullptr;
            }

            std::shared_ptr<GuiImageData> image                  = std::make_unique<GuiImageData>(fileData);
            instance->imageDataMap[std::string(filename)].first  = 1;
            instance->imageDataMap[std::string(filename)].second = image;

            return image;
        }
    }

    return nullptr;
}

std::unique_ptr<GuiSound> Resources::GetSound(std::string_view filename) {
    for (int32_t i = 0; ResourceList[i].filename != nullptr; ++i) {
        if (filename == ResourceList[i].filename) {
            std::vector<uint8_t> fileData;
            if (!ResourceList[i].CustomFileBuffer.empty()) {
                fileData = ResourceList[i].CustomFileBuffer;
            } else {
                fileData = std::vector(ResourceList[i].DefaultFile, ResourceList[i].DefaultFile + ResourceList[i].DefaultFileSize);
            }

            if (fileData.empty() || fileData.data() == nullptr) {
                return nullptr;
            }

            return std::make_unique<GuiSound>(std::move(fileData));
        }
    }

    throw std::runtime_error("Try to load invalid sound file");
}
