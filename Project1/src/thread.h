#ifndef THREAD_H
#define THREAD_H

#include <psp2/kernel/types.h>

#include <functional>
#include <memory>

enum class ThreadPriority
{
    DEFAULT       =  0,
    LOW           =  19,
    HIGH          =  10,
    TIME_CRITICAL = -20,
};

class Thread
{
public:
    /* Public functions */
    static std::unique_ptr<Thread> create_and_start     (const char*                  in_opt_name_ptr,
                                                         const std::function<void()>& in_callback_func,
                                                         const ThreadPriority&        in_priority,
                                                         const int*                   in_opt_stack_size_ptr,
                                                         const uint8_t&               in_cpu_affinity_mask);
    static SceUID                  get_current_thread_id();

    ~Thread();

    bool block_until_done(const SceUInt32* in_opt_timeout_in_microseconds_ptr) const;

private:
    /* Private functions */
    Thread(const std::function<void()>& in_callback_func);

    bool init(const char*           in_opt_name_ptr,
              const ThreadPriority& in_priority,
              const int*            in_opt_stack_size_ptr,
              const uint8_t&        in_cpu_affinity_mask);

    static int kernel_thread_entrypoint(SceSize args,
                                        void*   argp);

    /* Private variables */
    const std::function<void()> m_callback_func;
    SceUID                      m_thread_id;
};

#endif /* THREAD_H */