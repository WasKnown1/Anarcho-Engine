#include <aeThread.hpp>

u0 aeCreateThread(AEThread *thread, u0 *function) {
    *thread = std::thread([function]() {
        ((u0(*)())function)();
    });
}

AEPThreadTask aeAssignLoopedThread(u0 *allocator, u0 *function, ThreadSafe<bool> *stopFlag) {
    u0 *(*newAllocator)(u32) = ((u0 * (*)(u32))allocator);
    AEPThreadTask task = (AEPThreadTask)newAllocator(sizeof(AEThreadTask));
    task->function = function;
    task->stopFlag = stopFlag;
    task->thread = nullptr;
    return task;
}

u0 aeStartLoopedThread(AEThreadTask *task) {
    task->thread = new AEThread([task]() {
        while (!task->stopFlag->load()) {
            ((u0(*)())task->function)();
        }
    });
}

u0 parallel::aeCreateLoopedThread(AEThread *thread, u0 *function, bool *stopFlag) {
    *thread = std::thread([function, stopFlag]() {
        while (!*stopFlag) {
            ((u0(*)())function)();
        }
    });
}

u0 parallel::aeCreateLoopedThread(AEThread *thread, u0 *function, ThreadSafe<bool> *stopFlag) {
    *thread = std::thread([function, stopFlag]() {
        while (!stopFlag->load()) {
            ((u0(*)())function)();
        }
    });
}

u0 parallel::aeJoinAndDetachThread(AEThread *thread) {
    if (thread->joinable()) {
        thread->join();
    }
    thread->detach();
}