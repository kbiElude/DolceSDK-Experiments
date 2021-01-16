extern "C"
{
    #include <psp2/kernel/threadmgr.h>
    #include <psp2/libdbg.h>
}

#include "OS/event.h"

#include <memory>

Event::Event()
    :m_event_id(~0u)
{
    /* Stub */
}

Event::~Event()
{
    if (m_event_id != ~0u)
    {
        ::sceKernelDeleteEventFlag(m_event_id);
    }
}

std::unique_ptr<Event> Event::create(const char* in_opt_name_ptr,
                                     const bool& in_support_multiple_simultaneous_waits,
                                     const bool& in_create_set)
{
    std::unique_ptr<Event> result_ptr;

    result_ptr.reset(
        new Event()
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_opt_name_ptr,
                              in_support_multiple_simultaneous_waits,
                              in_create_set) )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

bool Event::init(const char* in_opt_name_ptr,
                 const bool& in_support_multiple_simultaneous_waits,
                 const bool& in_create_set)
{
    const auto event_flags = (in_support_multiple_simultaneous_waits) ? SCE_KERNEL_EVF_ATTR_MULTI
                                                                      : SCE_KERNEL_EVF_ATTR_SINGLE;
    bool       result      = false;

    m_event_id = ::sceKernelCreateEventFlag(in_opt_name_ptr,
                                            event_flags,
                                            (in_create_set) ? 0x1  /* initPattern */
                                                            : 0x0,
                                            nullptr); /* pOptParam */

    result = (m_event_id >= 0);
    SCE_DBG_ASSERT(result);

    return result;
}

bool Event::reset()
{
    bool       result       = false;
    const auto reset_result = ::sceKernelClearEventFlag(m_event_id,
                                                        0x1); /* bits */

    result = (reset_result >= 0);
    SCE_DBG_ASSERT(result);

    return result;
}

bool Event::set()
{
    bool       result     = false;
    const auto set_result = ::sceKernelSetEventFlag(m_event_id,
                                                    0x1); /* bits */

    result = (set_result >= 0);
    SCE_DBG_ASSERT(result);

    return result;
}

bool Event::wait(const bool&      in_should_reset_upon_successful_wait,
                 const SceUInt32* in_opt_timeout_in_microseconds_ptr)
{
    bool      result         = false;
    SceUInt32 result_pattern = 0;

    const auto wait_result = ::sceKernelWaitEventFlag(m_event_id,
                                                      0x1, /* bitPattern */
                                                      (in_should_reset_upon_successful_wait) ? SCE_KERNEL_EVF_WAITMODE_CLEAR_ALL
                                                                                             : SCE_KERNEL_EVF_WAITMODE_AND,
                                                     &result_pattern,
                                                      const_cast<SceUInt32*>(in_opt_timeout_in_microseconds_ptr) );

    result = (wait_result >= 0);
    return result;
}

