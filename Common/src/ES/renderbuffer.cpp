#include "ES/renderbuffer.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

Renderbuffer::Renderbuffer(std::unique_ptr<RenderbufferCreateInfo> in_create_info_ptr)
    :m_create_info_ptr(std::move(in_create_info_ptr) ),
     m_rb_id          (0)
{
    /* Stub */
}

Renderbuffer::~Renderbuffer()
{
    if (m_rb_id != 0)
    {
        ::glDeleteRenderbuffers(1, /* n */
                               &m_rb_id);

        m_rb_id = 0;
    }
}

std::unique_ptr<Renderbuffer> Renderbuffer::create(std::unique_ptr<RenderbufferCreateInfo> in_create_info_ptr)
{
    std::unique_ptr<Renderbuffer> result_ptr;

    result_ptr.reset(
        new Renderbuffer(
            std::move(in_create_info_ptr)
        )
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init() )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

bool Renderbuffer::init()
{
    ::glGenRenderbuffers(1, /* n */
                        &m_rb_id);

    SCE_DBG_ASSERT(m_rb_id != 0);

    ::glBindRenderbuffer   (GL_RENDERBUFFER,
                            m_rb_id);
    ::glRenderbufferStorage(GL_RENDERBUFFER,
                            static_cast<GLenum>          (m_create_info_ptr->get_internal_format() ),
                            m_create_info_ptr->get_width (),
                            m_create_info_ptr->get_height() );

    return true;
}