#pragma once

#include "utils/logger.h"
#include <coreinit/cache.h>
#include <future>
#include <gui/GuiElement.h>
#include <queue>
#include <thread>
#include <vector>

class AsyncExecutor {
public:
    static void pushForDelete(GuiElement *element) {
        if (!instance) {
            instance = new AsyncExecutor();
        }
        instance->pushForDeleteInternal(element);
    }

    static void execute(std::function<void()> func) {
        if (!instance) {
            instance = new AsyncExecutor();
        }
        instance->executeInternal(func);
    }

    static void destroyInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

private:
    static AsyncExecutor *instance;

    AsyncExecutor();

    ~AsyncExecutor();

    void pushForDeleteInternal(GuiElement *element);

    void executeInternal(std::function<void()> func);

    std::recursive_mutex mutex;
    std::thread *thread;
    volatile bool exitThread = false;

    std::vector<std::future<void>> elements;

    std::recursive_mutex deleteListMutex;
    std::queue<GuiElement *> deleteList;
};
