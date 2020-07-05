#include <malloc.h>
#include <string.h>
#include <string>
#include "Resources.h"
#include "filelist.h"
#include <gui/GuiSound.h>
#include <gui/GuiImageData.h>
#include "fs/FSUtils.h"
#include "utils/AsyncExecutor.h"

#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <strings.h>


Resources * Resources::instance = NULL;

void Resources::Clear() {
    for(int32_t i = 0; RecourceList[i].filename != NULL; ++i) {
        if(RecourceList[i].CustomFile) {
            free(RecourceList[i].CustomFile);
            RecourceList[i].CustomFile = NULL;
        }

        if(RecourceList[i].CustomFileSize != 0)
            RecourceList[i].CustomFileSize = 0;
    }

    if(instance)
        delete instance;

    instance = NULL;
}

bool Resources::LoadFiles(const char * path) {
    if(!path)
        return false;

    bool result = false;
    Clear();

    for(int32_t i = 0; RecourceList[i].filename != NULL; ++i) {
        std::string fullpath(path);
        fullpath += "/";
        fullpath += RecourceList[i].filename;

        uint8_t * buffer = NULL;
        uint32_t filesize = 0;

        FSUtils::LoadFileToMem(fullpath.c_str(), &buffer, &filesize);

        RecourceList[i].CustomFile = buffer;
        RecourceList[i].CustomFileSize = (uint32_t) filesize;
        result |= (buffer != 0);
    }

    return result;
}

const uint8_t * Resources::GetFile(const char * filename) {
    for(int32_t i = 0; RecourceList[i].filename != NULL; ++i) {
        if(strcasecmp(filename, RecourceList[i].filename) == 0) {
            return (RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile);
        }
    }

    return NULL;
}

uint32_t Resources::GetFileSize(const char * filename) {
    for(int32_t i = 0; RecourceList[i].filename != NULL; ++i) {
        if(strcasecmp(filename, RecourceList[i].filename) == 0) {
            return (RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize);
        }
    }
    return 0;
}

GuiImageData * Resources::GetImageData(const char * filename) {
    if(!instance)
        instance = new Resources;

    std::map<std::string, std::pair<uint32_t, GuiImageData *> >::iterator itr = instance->imageDataMap.find(std::string(filename));
    if(itr != instance->imageDataMap.end()) {
        itr->second.first++;
        return itr->second.second;
    }

    for(int32_t i = 0; RecourceList[i].filename != NULL; ++i) {
        if(strcasecmp(filename, RecourceList[i].filename) == 0) {
            const uint8_t * buff = RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile;
            const uint32_t size = RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize;

            if(buff == NULL)
                return NULL;

            GuiImageData * image = new GuiImageData(buff, size);
            instance->imageDataMap[std::string(filename)].first = 1;
            instance->imageDataMap[std::string(filename)].second = image;

            return image;
        }
    }

    return NULL;
}

void Resources::RemoveImageData(GuiImageData * image) {
    std::map<std::string, std::pair<uint32_t, GuiImageData *> >::iterator itr;

    for(itr = instance->imageDataMap.begin(); itr != instance->imageDataMap.end(); itr++) {
        if(itr->second.second == image) {
            itr->second.first--;

            if(itr->second.first == 0) {
                AsyncExecutor::pushForDelete(itr->second.second);

                instance->imageDataMap.erase(itr);
            }
            break;
        }
    }
}

GuiSound * Resources::GetSound(const char * filename) {
    if(!instance)
        instance = new Resources;

    std::map<std::string, std::pair<uint32_t, GuiSound *> >::iterator itr = instance->soundDataMap.find(std::string(filename));
    if(itr != instance->soundDataMap.end()) {
        itr->second.first++;
        return itr->second.second;
    }

    for(int32_t i = 0; RecourceList[i].filename != NULL; ++i) {
        if(strcasecmp(filename, RecourceList[i].filename) == 0) {
            const uint8_t * buff = RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile;
            const uint32_t size = RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize;

            if(buff == NULL)
                return NULL;

            GuiSound * sound = new GuiSound(buff, size);
            instance->soundDataMap[std::string(filename)].first = 1;
            instance->soundDataMap[std::string(filename)].second = sound;

            return sound;
        }
    }

    return NULL;
}

void Resources::RemoveSound(GuiSound * sound) {
    std::map<std::string, std::pair<uint32_t, GuiSound *> >::iterator itr;

    for(itr = instance->soundDataMap.begin(); itr != instance->soundDataMap.end(); itr++) {
        if(itr->second.second == sound) {
            itr->second.first--;

            if(itr->second.first == 0) {
                AsyncExecutor::pushForDelete(itr->second.second);
                instance->soundDataMap.erase(itr);
            }
            break;
        }
    }
}
