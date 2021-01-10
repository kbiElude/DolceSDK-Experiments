#include "thread.h"

extern "C"
{
    #include <psp2/kernel/processmgr.h>
    #include <psp2/kernel/threadmgr.h>
    #include <psp2/libdbg.h>
    #include <psp2/kernel/types.h>
}

Thread::Thread(const std::function<void()>& in_callback_func)
    :m_callback_func(in_callback_func),
     m_thread_id    (~0u)
{
    /* Stub */
}

Thread::~Thread()
{
    if (m_thread_id != ~0u)
    {
        block_until_done(nullptr);

        ::sceKernelDeleteThread(m_thread_id);
    }
}

bool Thread::block_until_done(const SceUInt32* in_opt_timeout_in_microseconds_ptr) const
{
    return ::sceKernelWaitThreadEnd(m_thread_id,
                                    nullptr,  /* pExitStatus */
                                    const_cast<SceUInt32*>(in_opt_timeout_in_microseconds_ptr) ) >= 0;
}

std::unique_ptr<Thread> Thread::create_and_start(const char*                  in_opt_name_ptr,
                                                 const std::function<void()>& in_callback_func,
                                                 const ThreadPriority&        in_priority,
                                                 const int*                   in_opt_stack_size_ptr,
                                                 const uint8_t&               in_cpu_affinity_mask)
{
    std::unique_ptr<Thread> result_ptr;

    result_ptr.reset(
        new Thread(in_callback_func)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_opt_name_ptr,
                              in_priority,
                              in_opt_stack_size_ptr,
                              in_cpu_affinity_mask) )
        {
            result_ptr.reset();
        }
    }

    return result_ptr;
}

SceUID Thread::get_current_thread_id()
{
    return ::sceKernelGetThreadId();
}

bool Thread::init(const char*                  in_opt_name_ptr,
                  const ThreadPriority&        in_priority,
                  const int*                   in_opt_stack_size_ptr,
                  const uint8_t&               in_cpu_affinity_mask)
{
    bool result = false;

    m_thread_id = ::sceKernelCreateThread(in_opt_name_ptr,
                                          kernel_thread_entrypoint,
                                          static_cast<SceInt32>(in_priority),
                                          (in_opt_stack_size_ptr != nullptr) ? *in_opt_stack_size_ptr
                                                                             :  32768,
                                          0, /* attr */
                                          in_cpu_affinity_mask,
                                          nullptr /* pOptParam */);

    result = (m_thread_id >= 0);
    SCE_DBG_ASSERT(result);

    ::sceKernelStartThread(m_thread_id,
                           sizeof(this),
                           this);

    return result;
}

int Thread::kernel_thread_entrypoint(SceSize /* args */,
                                     void*   argp)
{
    Thread* this_ptr = reinterpret_cast<Thread*>(argp);

    this_ptr->m_callback_func();

    return 0;
}
