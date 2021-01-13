#include "thread.h"

extern "C"
{
    #include <psp2/kernel/processmgr.h>
    #include <psp2/kernel/threadmgr.h>
    #include <psp2/libdbg.h>
    #include <psp2/kernel/types.h>
}

Thread::Thread(const PFNTHREADCALLBACKPROC& in_callback_func,
               void*                        in_callback_func_arg)
    :m_callback_func    (in_callback_func),
     m_callback_func_arg(in_callback_func_arg),
     m_this_raw_ptr     (nullptr),
     m_thread_id        (~0u)
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

std::unique_ptr<Thread> Thread::create_and_start(const char*           in_opt_name_ptr,
                                                 PFNTHREADCALLBACKPROC in_callback_func,
                                                 void*                 in_callback_func_arg,
                                                 const int*            in_opt_stack_size_ptr,
                                                 const uint8_t&        in_cpu_affinity_mask)
{
    std::unique_ptr<Thread> result_ptr;

    result_ptr.reset(
        new Thread(in_callback_func,
                   in_callback_func_arg)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_opt_name_ptr,
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
                  const int*                   in_opt_stack_size_ptr,
                  const uint8_t&               in_cpu_affinity_mask)
{
    const SceInt32 cpu_affinity_mask = ((in_cpu_affinity_mask & 0x1) ? SCE_KERNEL_CPU_MASK_USER_0 : 0) |
                                       ((in_cpu_affinity_mask & 0x2) ? SCE_KERNEL_CPU_MASK_USER_1 : 0) |
                                       ((in_cpu_affinity_mask & 0x4) ? SCE_KERNEL_CPU_MASK_USER_2 : 0);

    m_this_raw_ptr = this;

    m_thread_id = ::sceKernelCreateThread(in_opt_name_ptr,
                                          kernel_thread_entrypoint,
                                          0, /* initPriority */
                                          (in_opt_stack_size_ptr != nullptr) ? *in_opt_stack_size_ptr
                                                                             :  32768,
                                          0, /* attr */
                                          cpu_affinity_mask,
                                          nullptr /* pOptParam */);

    ::sceKernelStartThread(m_thread_id,
                           sizeof(m_this_raw_ptr),
                          &m_this_raw_ptr);

    return true;
}

int Thread::kernel_thread_entrypoint(SceSize /* args */,
                                     void*   argp)
{
    Thread* this_ptr = *reinterpret_cast<Thread**>(argp);

    return this_ptr->m_callback_func(this_ptr->m_callback_func_arg);
}
