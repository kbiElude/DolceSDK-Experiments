#include "ES/buffer_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

BufferCreateInfo::BufferCreateInfo(const uint32_t& in_size,
                                   const BufferUsage& in_usage)
    :m_size (in_size),
     m_usage(in_usage)
{
    /* Stub */
}

std::unique_ptr<BufferCreateInfo> BufferCreateInfo::create(const uint32_t&    in_size,
                                                           const BufferUsage& in_usage)
{
    std::unique_ptr<BufferCreateInfo> result_ptr;

    result_ptr.reset(
        new BufferCreateInfo(in_size,
                             in_usage)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    return result_ptr;
}