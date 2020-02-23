#include "AsyncExecutor.h"
#include "utils/logger.h"

AsyncExecutor * AsyncExecutor::instance = NULL;

void AsyncExecutor::pushForDeleteInternal(GuiElement * ptr) {
    deleteListMutex.lock();
    deleteList.push(ptr);
    deleteListMutex.unlock();
}
AsyncExecutor::AsyncExecutor() {
    thread = new std::thread([&]() {
        while(!exitThread) {
            mutex.lock();
            bool emptyList = elements.empty();
            auto it = elements.begin();
            while (it != elements.end()) {
                auto future = it;
                auto status = future->wait_for(std::chrono::seconds(0));
                if (status == std::future_status::ready) {
                    it = elements.erase(it);
                } else {
                    ++it;
                }
            }
            if(!emptyList && elements.empty()){
                DEBUG_FUNCTION_LINE("All tasks are done\n");
            }
            mutex.unlock();
            deleteListMutex.lock();
            while(!deleteList.empty()) {
                GuiElement * ptr = deleteList.front();
                deleteList.pop();
                delete ptr;
            }
            deleteListMutex.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            DCFlushRange((void*)&exitThread, sizeof(exitThread));
        }
    });
}

AsyncExecutor::~AsyncExecutor() {
    exitThread = true;
    DCFlushRange((void*)&exitThread, sizeof(exitThread));
    thread->join();
}

void AsyncExecutor::executeInternal(std::function<void()> func) {
    if(elements.size() > 10) {
        DEBUG_FUNCTION_LINE("Warning, many tasks running currently\n");
        //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    DEBUG_FUNCTION_LINE("Add new task\n");
    mutex.lock();
    elements.push_back(std::async(std::launch::async,func));
    mutex.unlock();
}
