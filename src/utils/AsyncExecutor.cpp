#include "AsyncExecutor.h"
#include "utils/logger.h"

AsyncExecutor * AsyncExecutor::instance = NULL;

void AsyncExecutor::pushForDelete(GuiElement * ptr) {
    execute([ptr] {delete ptr;});
}
AsyncExecutor::AsyncExecutor() {
    thread = new std::thread([&]() {
        while(!exitThread) {
            instance->mutex.lock();
            auto it = instance->elements.begin();
            while (it != instance->elements.end()) {
                auto future = it;
                auto status = future->wait_for(std::chrono::seconds(0));
                if (status == std::future_status::ready) {
                    it = instance->elements.erase(it);
                } else {
                    ++it;
                }
            }
            instance->mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            DCFlushRange((void*)&exitThread, sizeof(exitThread));
        }
    });

    thread->detach();
}

void AsyncExecutor::execute(std::function<void()> func) {
    if(!instance) {
        instance = new AsyncExecutor();
    }

    while(instance->elements.size() > 25) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    instance->mutex.lock();
    instance->elements.push_back(std::async(std::launch::async,func));
    instance->mutex.unlock();
}
