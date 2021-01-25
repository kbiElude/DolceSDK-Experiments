#include "ES/buffer.h"
#include "ES/buffer_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}


Buffer::Buffer(std::unique_ptr<BufferCreateInfo> in_create_info_ptr,
               Logger*                           in_logger_ptr)
    :m_create_info_ptr(std::move(in_create_info_ptr) ),
     m_logger_ptr     (in_logger_ptr)
{
    /* Stub */
}

Buffer::~Buffer()
{
    if (m_id != 0)
    {
        ::glDeleteBuffers(1, /* n */
                         &m_id);

        m_id = 0;
    }
}

std::unique_ptr<Buffer> Buffer::create(std::unique_ptr<BufferCreateInfo> in_create_info_ptr,
                                       Logger*                           in_logger_ptr)
{
    std::unique_ptr<Buffer> result_ptr;

    result_ptr.reset(
        new Buffer(std::move(in_create_info_ptr),
                   in_logger_ptr)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init() )
        {
            result_ptr.reset();

            SCE_DBG_ASSERT(false);
        }
    }

    return result_ptr;
}

bool Buffer::init()
{
    bool result = false;

    ::glGenBuffers(1, /* n */
                  &m_id);

    SCE_DBG_ASSERT(m_id != 0);

    ::glBindBuffer(GL_ARRAY_BUFFER,
                   m_id);
    ::glBufferData(GL_ARRAY_BUFFER,
                   m_create_info_ptr->get_size(),
                   nullptr, /* data */
                   static_cast<GLenum>(m_create_info_ptr->get_usage() ));

    result = true;
    return result;
}