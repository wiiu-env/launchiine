#ifndef RECOURCES_H_
#define RECOURCES_H_

#include <map>
#include <stdint.h>

//! forward declaration
class GuiImageData;
class GuiSound;

class Resources {
public:
    static void Clear();
    static bool LoadFiles(const char * path);
    static const uint8_t * GetFile(const char * filename);
    static uint32_t GetFileSize(const char * filename);

    static GuiImageData * GetImageData(const char * filename);
    static void RemoveImageData(GuiImageData * image);

    static GuiSound * GetSound(const char * filename);
    static void RemoveSound(GuiSound * sound);
private:
    static Resources *instance;

    Resources() {}
    ~Resources() {}

    std::map<std::string, std::pair<uint32_t, GuiImageData *> > imageDataMap;
    std::map<std::string, std::pair<uint32_t, GuiSound *> > soundDataMap;
};

#endif
