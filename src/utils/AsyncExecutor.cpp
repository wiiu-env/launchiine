#include "AsyncExecutor.h"
#include "utils/logger.h"

AsyncExecutor * AsyncExecutor::instance = NULL;

void AsyncExecutor::pushForDelete(GuiElement * ptr) {
    execute([ptr] {delete ptr;});
}

void AsyncExecutor::execute(std::function<void()> func) {
    if(!instance) {
        instance = new AsyncExecutor();
    }
    instance->elements.push_back(std::async(std::launch::async,func));
}
