#include "ES/renderbuffer_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

RenderbufferCreateInfo::RenderbufferCreateInfo(const RenderbufferFormat& in_internal_format,
                                               const GLsizei&            in_width,
                                               const GLsizei&            in_height)
    :m_height         (in_height),
     m_internal_format(in_internal_format),
     m_width          (in_width)
{
    /* Stub */
}

RenderbufferCreateInfo::~RenderbufferCreateInfo()
{
    /* Stub */
}

std::unique_ptr<RenderbufferCreateInfo> RenderbufferCreateInfo::create(const RenderbufferFormat& in_internal_format,
                                                                       const GLsizei&            in_width,
                                                                       const GLsizei&            in_height)
{
    std::unique_ptr<RenderbufferCreateInfo> result_ptr;

    result_ptr.reset(
        new RenderbufferCreateInfo(in_internal_format,
                                   in_width,
                                   in_height)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    return result_ptr;
}
