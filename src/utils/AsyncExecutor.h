#pragma once

#include <vector>
#include <future>
#include <gui/GuiElement.h>

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

    AsyncExecutor() {}
    ~AsyncExecutor() {}

    std::vector<std::future<void>> elements;
};
