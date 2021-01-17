#include "ES/framebuffer.h"
#include "ES/framebuffer_create_info.h"
#include "ES/renderbuffer.h"
#include "ES/texture.h"
#include "ES/texture_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

Framebuffer::Framebuffer(std::unique_ptr<FramebufferCreateInfo> in_create_info_ptr)
    :m_create_info_ptr(std::move(in_create_info_ptr) ),
     m_fb_id          (0)
{
    /* Stub */
}

Framebuffer::~Framebuffer()
{
    if (m_fb_id != 0)
    {
        ::glDeleteFramebuffers(1, /* n */
                              &m_fb_id);

        m_fb_id = 0;
    }
}

std::unique_ptr<Framebuffer> Framebuffer::create(std::unique_ptr<FramebufferCreateInfo> in_create_info_ptr)
{
    std::unique_ptr<Framebuffer> result_ptr;

    result_ptr.reset(
        new Framebuffer(std::move(in_create_info_ptr) )
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

bool Framebuffer::init()
{
    bool                                           result                              = false;
    static const std::pair<AttachmentType, GLenum> attachment_types_to_gl_enum_pairs[] =
    {
        {AttachmentType::COLOR_0, GL_COLOR_ATTACHMENT0},
        {AttachmentType::DEPTH,   GL_DEPTH_ATTACHMENT},
        {AttachmentType::STENCIL, GL_STENCIL_ATTACHMENT}
    };

    ::glGenFramebuffers(1, /* n */
                       &m_fb_id);

    SCE_DBG_ASSERT(m_fb_id != 0);

    ::glBindFramebuffer(GL_FRAMEBUFFER,
                        m_fb_id);

    for (const auto& current_iterator : attachment_types_to_gl_enum_pairs)
    {
        auto       current_attachment_object_type = AttachmentObjectType::NONE;
        const auto current_attachment_type        = current_iterator.first;

        m_create_info_ptr->get_bound_attachment_object_type(current_attachment_type,
                                                           &current_attachment_object_type);

        switch (current_attachment_object_type)
        {
            case AttachmentObjectType::RENDERBUFFER:
            {
                const Renderbuffer* rb_ptr = nullptr;

                m_create_info_ptr->get_bound_attachment_props(current_attachment_type,
                                                             &rb_ptr);

                ::glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                            current_iterator.second,
                                            GL_RENDERBUFFER,
                                            rb_ptr->get_renderbuffer_id() );

                break;
            }

            case AttachmentObjectType::TEXTURE:
            {
                auto           cm_texture_face = CMTextureFace::UNKNOWN;
                uint32_t       texture_n_mip   = 0;
                const Texture* texture_ptr     = nullptr;

                m_create_info_ptr->get_bound_attachment_props(current_attachment_type,
                                                             &texture_ptr,
                                                             &texture_n_mip,
                                                             &cm_texture_face);

                const auto texture_target_gl = (texture_ptr->get_create_info_ptr()->get_type() == TextureType::_2D) ? GL_TEXTURE_2D
                                             : (cm_texture_face                                == CMTextureFace::NEGATIVE_X) ? GL_TEXTURE_CUBE_MAP_NEGATIVE_X
                                             : (cm_texture_face                                == CMTextureFace::NEGATIVE_Y) ? GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
                                             : (cm_texture_face                                == CMTextureFace::NEGATIVE_Z) ? GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
                                             : (cm_texture_face                                == CMTextureFace::POSITIVE_X) ? GL_TEXTURE_CUBE_MAP_POSITIVE_X
                                             : (cm_texture_face                                == CMTextureFace::POSITIVE_Y) ? GL_TEXTURE_CUBE_MAP_POSITIVE_Y
                                             : (cm_texture_face                                == CMTextureFace::POSITIVE_Z) ? GL_TEXTURE_CUBE_MAP_POSITIVE_Z
                                                                                                                             : 0;

                ::glFramebufferTexture2D(GL_FRAMEBUFFER,
                                         current_iterator.second,
                                         texture_target_gl,
                                         texture_ptr->get_texture_id(),
                                         texture_n_mip);

                break;
            }

            default:
            {
                SCE_DBG_ASSERT(false);

                goto end;
            }
        }
    }

    result = true;
end:
    return result;
}