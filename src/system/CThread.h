#ifndef CTHREAD_H
#define CTHREAD_H

#include <coreinit/thread.h>
#include <malloc.h>
#include <unistd.h>

class CThread {
public:
    typedef void (*Callback)(CThread *thread, void *arg);

    //! constructor
    CThread(int32_t iAttr, int32_t iPriority = 16, int32_t iStackSize = 0x8000, CThread::Callback callback = nullptr, void *callbackArg = nullptr)
        : pThread(nullptr), pThreadStack(nullptr), pCallback(callback), pCallbackArg(callbackArg) {
        //! save attribute assignment
        iAttributes = iAttr;
        //! allocate the thread
        pThread = (OSThread *) memalign(8, sizeof(OSThread));
        //! allocate the stack
        pThreadStack = (uint8_t *) memalign(0x20, iStackSize);
        //! create the thread
        if (pThread && pThreadStack)
            OSCreateThread(pThread, &CThread::threadCallback, 1, (char *) this, pThreadStack + iStackSize, iStackSize, iPriority, iAttributes);
    }

    //! destructor
    virtual ~CThread() {
        shutdownThread();
    }

    static CThread *create(CThread::Callback callback, void *callbackArg, int32_t iAttr = eAttributeNone, int32_t iPriority = 16, int32_t iStackSize = 0x8000) {
        return (new CThread(iAttr, iPriority, iStackSize, callback, callbackArg));
    }

    //! Get thread ID
    virtual void *getThread() const {
        return pThread;
    }
    //! Thread entry function
    virtual void executeThread(void) {
        if (pCallback)
            pCallback(this, pCallbackArg);
    }
    //! Suspend thread
    virtual void suspendThread(void) {
        if (isThreadSuspended())
            return;
        if (pThread)
            OSSuspendThread(pThread);
    }
    //! Resume thread
    virtual void resumeThread(void) {
        if (!isThreadSuspended())
            return;
        if (pThread)
            OSResumeThread(pThread);
    }
    //! Set thread priority
    virtual void setThreadPriority(int32_t prio) {
        if (pThread)
            OSSetThreadPriority(pThread, prio);
    }
    //! Check if thread is suspended
    virtual bool isThreadSuspended(void) const {
        if (pThread)
            return OSIsThreadSuspended(pThread);
        return false;
    }
    //! Check if thread is terminated
    virtual bool isThreadTerminated(void) const {
        if (pThread)
            return OSIsThreadTerminated(pThread);
        return false;
    }
    //! Check if thread is running
    virtual bool isThreadRunning(void) const {
        return !isThreadSuspended() && !isThreadRunning();
    }
    //! Shutdown thread
    virtual void shutdownThread(void) {
        //! wait for thread to finish
        if (pThread && !(iAttributes & eAttributeDetach)) {
            if (isThreadSuspended())
                resumeThread();

            OSJoinThread(pThread, nullptr);
        }
        //! free the thread stack buffer
        if (pThreadStack)
            free(pThreadStack);
        if (pThread)
            free(pThread);

        pThread      = nullptr;
        pThreadStack = nullptr;
    }
    //! Thread attributes
    enum eCThreadAttributes {
        eAttributeNone      = 0x07,
        eAttributeAffCore0  = 0x01,
        eAttributeAffCore1  = 0x02,
        eAttributeAffCore2  = 0x04,
        eAttributeDetach    = 0x08,
        eAttributePinnedAff = 0x10
    };

private:
    static int32_t threadCallback(int32_t argc, const char **argv) {
        //! After call to start() continue with the internal function
        ((CThread *) argv)->executeThread();
        return 0;
    }
    int32_t iAttributes;
    OSThread *pThread;
    uint8_t *pThreadStack;
    Callback pCallback;
    void *pCallbackArg;
};

#endif
