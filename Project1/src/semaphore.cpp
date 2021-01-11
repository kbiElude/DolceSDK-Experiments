#include "semaphore.h"

extern "C"
{
    #include <psp2/kernel/threadmgr.h>
    #include <psp2/libdbg.h>
}

Semaphore::Semaphore()
    :m_sem_id(0xFFFFFFFFu)
{
    /* Stub */
}

Semaphore::~Semaphore()
{
    if (m_sem_id != 0xFFFFFFFFu)
    {
        ::sceKernelDeleteSema(m_sem_id);
    }
}

std::unique_ptr<Semaphore> Semaphore::create(const char* in_name_ptr,
                                             const int&  in_start_value,
                                             const int&  in_max_value)
{
    std::unique_ptr<Semaphore> result_ptr;

    result_ptr.reset(
        new Semaphore()
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_name_ptr,
                              in_start_value,
                              in_max_value) )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

bool Semaphore::init(const char* in_name_ptr,
                     const int&  in_start_value,
                     const int&  in_max_value)
{
    m_sem_id = ::sceKernelCreateSema(in_name_ptr,
                                     0,             /* attr      */
                                     in_start_value,
                                     in_max_value,
                                     nullptr);      /* pOptParam */

    return true;
}

int Semaphore::peek() const
{
    SceKernelSemaInfo sem_info;
    auto              result    = ::sceKernelGetSemaInfo(m_sem_id,
                                                        &sem_info);

    SCE_DBG_ASSERT(result >= 0);

    return sem_info.currentCount;
}

bool Semaphore::signal()
{
    return (::sceKernelSignalSema(m_sem_id,
                                  1) ) >= 0;
}

bool Semaphore::wait(const int&       in_value,
                     const SceUInt32* in_opt_timeout_in_microseconds_ptr)
{
    return (::sceKernelWaitSema(m_sem_id,
                                in_value,
                                const_cast<SceUInt32*>(in_opt_timeout_in_microseconds_ptr)) ) >= 0;
}