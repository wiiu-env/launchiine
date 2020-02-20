#ifndef ___ASYNCEXECUTER_H_
#define ___ASYNCEXECUTER_H_


#include <vector>
#include <future>
#include <thread>
#include <gui/GuiElement.h>
#include <system/CMutex.h>
#include <coreinit/cache.h>
#include "utils/logger.h"

class AsyncExecutor {
public:
    static void pushForDelete(GuiElement * element);
    static void execute(std::function<void()> func);

    static void destroyInstance() {
        if(instance) {
            delete instance;
            instance = NULL;
        }
    }

private:
    static AsyncExecutor *instance;

    AsyncExecutor();

    ~AsyncExecutor() {
        exitThread = true;
        DCFlushRange((void*)&exitThread, sizeof(exitThread));
    }

    CMutex mutex;
    std::thread *  thread;
    volatile bool exitThread = false;

    std::vector<std::future<void>> elements;
};
#endif
