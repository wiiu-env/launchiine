#include "UpdCheck.h"
#include <curl/curl.h>
#include <jansson.h>

#include <coreinit/core.h>
#include <coreinit/memory.h>
#include <coreinit/filesystem.h>

#include <whb/log.h>

struct FileWriteData {
    FSFileHandle handle;
    FSClient* client;
    FSCmdBlock* cmdBlock;
};

FSClient* UpdCheck::fsClient = nullptr;
FSCmdBlock* UpdCheck::cmdBlock = nullptr;

size_t UpdCheck::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t UpdCheck::WriteFile(void* ptr, size_t size, size_t nmemb, void* userp) {
    FileWriteData* fwd = (FileWriteData*)userp;
    uint32_t bytesWritten = 0;
    FSStatus status = FSWriteFile(
        fwd->client,
        fwd->cmdBlock,
        static_cast<uint8_t*>(ptr),
        size * nmemb,
        1,
        fwd->handle,
        0,
        FS_ERROR_FLAG_NONE);
    
    if (status < 0) {
        return 0;
    }
    return size * nmemb;
}

std::string url = "https://github.com/rpdistiso/launchiine/releases/download/latest/men.rpx";
std::string output_path = "vol/external01/wiiu/environments/aroma/men.rpx"

bool UpdCheck::downloadUpdate(const std::string& url, const std::string& output_path) {
    if (!fsClient) {
        fsClient = (FSClient*)OSAllocFromSystem(sizeof(FSClient), 4);
        FSAddClient(fsClient, FS_ERROR_FLAG_NONE);
    }
    
    if (!cmdBlock) {
        cmdBlock = (FSCmdBlock*)OSAllocFromSystem(sizeof(FSCmdBlock), 4);
        FSInitCmdBlock(cmdBlock);
    }

    FSFileHandle fileHandle;
    FSStatus status = FSOpenFile(fsClient, cmdBlock, 
                                output_path.c_str(), 
                                "w", 
                                &fileHandle, 
                                FS_ERROR_FLAG_NONE);
    
    if (status < 0) {
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        FSCloseFile(fsClient, cmdBlock, fileHandle, FS_ERROR_FLAG_NONE);
        return false;
    }

    FileWriteData fwd = {fileHandle, fsClient, cmdBlock};

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fwd);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
    FSCloseFile(fsClient, cmdBlock, fileHandle, FS_ERROR_FLAG_NONE);

    return res == CURLE_OK;
}
