#ifndef anarcho_thread_hpp
#define anarcho_thread_hpp

#include <thread>
#include <aeTypes.hpp>
#include <atomic>
#include <functional>

using AEThread = std::thread;

template<typename T>
using ThreadSafe = std::atomic<T>;

template<typename T>
inline T aeGetThreadSafeValue(ThreadSafe<T> *threadSafe) {
    return threadSafe->load();
}

template<typename T>
inline void aeSetThreadSafeValue(ThreadSafe<T> *threadSafe, T value) {
    threadSafe->store(value);
}

typedef struct {
    u0 *function;
    ThreadSafe<bool> *stopFlag;
    AEThread *thread;
} AEThreadTask, *AEPThreadTask;

u0 aeCreateThread(AEThread *thread, u0 *function);
AEPThreadTask aeAssignLoopedThread(u0 *allocator, u0 *function, ThreadSafe<bool> *stopFlag);
u0 aeStartLoopedThread(AEThreadTask *task);

namespace parallel {
    u0 aeCreateLoopedThread(AEThread *thread, u0 *function, bool *stopFlag);
    u0 aeCreateLoopedThread(AEThread *thread, u0 *function, ThreadSafe<bool> *stopFlag);

    u0 aeJoinAndDetachThread(AEThread *thread);
}

#endif // anarcho_thread_hpp