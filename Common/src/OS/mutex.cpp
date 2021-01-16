#include "OS/mutex.h"

extern "C"
{
    #include <psp2/kernel/threadmgr.h>
    #include <psp2/libdbg.h>
}

Mutex::Mutex()
    :m_mutex_id(0xFFFFFFFFu)
{
    /* Stub */
}

Mutex::~Mutex()
{
    if (m_mutex_id != 0xFFFFFFFFu)
    {
        ::sceKernelDeleteMutex(m_mutex_id);
    }
}

std::unique_ptr<Mutex> Mutex::create(const char* in_name_ptr,
                                     const int&  in_start_value)
{
    std::unique_ptr<Mutex> result_ptr;

    result_ptr.reset(
        new Mutex()
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_name_ptr,
                              in_start_value) )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

bool Mutex::init(const char* in_name_ptr,
                 const int&  in_start_value)
{
    m_mutex_id = ::sceKernelCreateMutex(in_name_ptr,
                                        0,              /* attr      */
                                        in_start_value,
                                        nullptr);       /* pOptParam */

    return true;
}

bool Mutex::lock(const SceUInt32* in_opt_timeout_in_microseconds_ptr)
{
    bool result = false;

    result = ::sceKernelLockMutex(m_mutex_id,
                                  1, /* lockCount */
                                  const_cast<SceUInt32*>(in_opt_timeout_in_microseconds_ptr) ) >= 0;

    SCE_DBG_ASSERT(result);
    return result;
}

bool Mutex::try_lock()
{
    bool result = false;

    result = ::sceKernelTryLockMutex(m_mutex_id,
                                     1) >= 0; /* lockCount */

    return result;
}

bool Mutex::unlock()
{
    bool result = false;

    result = ::sceKernelUnlockMutex(m_mutex_id,
                                    1) >= 0; /* lockCount */

    return result;
}
