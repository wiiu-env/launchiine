#pragma once
#include <string>
#include <coreinit/filesystem.h>

class KeyboardHelper {
public:
    KeyboardHelper();

    ~KeyboardHelper();

    bool checkResult();

    void drawTV();

    void drawDRC();

    bool openKeyboard();

    bool isReady(){
        return keyboardCreated;
    }

    std::string getResult();

private:

    void * workMemory = NULL;
    FSClient * fsClient = NULL;
    bool keyboardOpen = false;
    bool keyboardCreated = false;
    std::string resultStr = "";
};
