#pragma once

#include <coreinit/filesystem.h>
#include <string>

class KeyboardHelper {
public:
    KeyboardHelper();

    ~KeyboardHelper();

    bool checkResult();

    static void drawTV();

    static void drawDRC();

    bool openKeyboard();

    bool isReady() {
        return keyboardCreated;
    }

    std::string getResult();

private:
    void *workMemory      = nullptr;
    FSClient *fsClient    = nullptr;
    bool keyboardOpen     = false;
    bool keyboardCreated  = false;
    std::string resultStr = "";
};
