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
    instance->elements.push(std::async(std::launch::async,func));
    if(instance->elements.size() >= 25){
        //DEBUG_FUNCTION_LINE("Wait on queue %d\n",instance->elements.size());
        instance->elements.front().get();
        instance->elements.pop();
    }
}
