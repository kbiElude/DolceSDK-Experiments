#ifndef THREAD_H
#define THREAD_H

#include <psp2/kernel/types.h>

#include <memory>

enum class ThreadPriority
{
    DEFAULT       =  0,
    LOW           =  19,
    HIGH          =  10,
    TIME_CRITICAL = -20,
};

typedef int (*PFNTHREADCALLBACKPROC)(void* arg);

class Thread
{
public:
    /* Public functions */
    static std::unique_ptr<Thread> create_and_start     (const char*           in_opt_name_ptr,
                                                         PFNTHREADCALLBACKPROC in_callback_func,
                                                         void*                 in_callback_func_arg,
                                                         const ThreadPriority& in_priority,
                                                         const int*            in_opt_stack_size_ptr,
                                                         const uint8_t&        in_cpu_affinity_mask);
    static SceUID                  get_current_thread_id();

    ~Thread();

    bool block_until_done(const SceUInt32* in_opt_timeout_in_microseconds_ptr) const;

private:
    /* Private functions */
    Thread(const PFNTHREADCALLBACKPROC& in_callback_func,
           void*                        in_callback_func_arg);

    bool init(const char*           in_opt_name_ptr,
              const ThreadPriority& in_priority,
              const int*            in_opt_stack_size_ptr,
              const uint8_t&        in_cpu_affinity_mask);

    static int kernel_thread_entrypoint(SceSize args,
                                        void*   argp);

    /* Private variables */
    PFNTHREADCALLBACKPROC m_callback_func;
    void*                 m_callback_func_arg;
    SceUID                m_thread_id;
    void*                 m_this_raw_ptr;
};

#endif /* THREAD_H */