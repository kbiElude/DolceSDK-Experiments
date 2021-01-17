#include "ES/framebuffer_create_info.h"
#include "ES/texture.h"
#include "ES/texture_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

FramebufferCreateInfo::FramebufferCreateInfo()
{
    /* Stub */
}

FramebufferCreateInfo::~FramebufferCreateInfo()
{
    /* Stub */
}

std::unique_ptr<FramebufferCreateInfo> FramebufferCreateInfo::create()
{
    std::unique_ptr<FramebufferCreateInfo> result_ptr;

    result_ptr.reset(
        new FramebufferCreateInfo()
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    return result_ptr;
}

void FramebufferCreateInfo::get_bound_attachment_object_type(const AttachmentType& in_attachment_type,
                                                             AttachmentObjectType* out_attachment_object_type_ptr) const
{
    const AttachmentProps* props_ptr = (in_attachment_type == AttachmentType::COLOR_0) ? &m_color_attachment0
                                     : (in_attachment_type == AttachmentType::DEPTH)   ? &m_depth_attachment
                                     : (in_attachment_type == AttachmentType::STENCIL) ? &m_stencil_attachment
                                                                                       : nullptr;

    *out_attachment_object_type_ptr = (props_ptr->rb_ptr      != nullptr) ? AttachmentObjectType::RENDERBUFFER
                                    : (props_ptr->texture_ptr != nullptr) ? AttachmentObjectType::TEXTURE
                                                                          : AttachmentObjectType::NONE;
}

void FramebufferCreateInfo::get_bound_attachment_props(const AttachmentType& in_attachment_type,
                                                       const Renderbuffer**  out_rb_ptr_ptr) const
{
    const AttachmentProps* props_ptr = (in_attachment_type == AttachmentType::COLOR_0) ? &m_color_attachment0
                                     : (in_attachment_type == AttachmentType::DEPTH)   ? &m_depth_attachment
                                     : (in_attachment_type == AttachmentType::STENCIL) ? &m_stencil_attachment
                                                                                       : nullptr;

    SCE_DBG_ASSERT(props_ptr->rb_ptr != nullptr);

    *out_rb_ptr_ptr = props_ptr->rb_ptr;
}

void FramebufferCreateInfo::get_bound_attachment_props(const AttachmentType& in_attachment_type,
                                                       const Texture**       out_texture_ptr_ptr,
                                                       uint32_t*             out_n_mip_ptr,
                                                       CMTextureFace*        out_cube_map_texture_face_ptr) const
{
    const AttachmentProps* props_ptr = (in_attachment_type == AttachmentType::COLOR_0) ? &m_color_attachment0
                                     : (in_attachment_type == AttachmentType::DEPTH)   ? &m_depth_attachment
                                     : (in_attachment_type == AttachmentType::STENCIL) ? &m_stencil_attachment
                                                                                       : nullptr;

    SCE_DBG_ASSERT(props_ptr->texture_ptr != nullptr);

    *out_n_mip_ptr       = props_ptr->n_mip;
    *out_texture_ptr_ptr = props_ptr->texture_ptr;

    if (out_cube_map_texture_face_ptr != nullptr)
    {
        *out_cube_map_texture_face_ptr = props_ptr->texture_face;
    }
}

void FramebufferCreateInfo::set_attachment(const AttachmentType& in_attachment_type,
                                           const Texture*        in_texture_ptr,
                                           const uint32_t&       in_n_mip,
                                           const CMTextureFace&  in_opt_cube_map_texture_face)
{
    AttachmentProps* props_ptr = (in_attachment_type == AttachmentType::COLOR_0) ? &m_color_attachment0
                               : (in_attachment_type == AttachmentType::DEPTH)   ? &m_depth_attachment
                               : (in_attachment_type == AttachmentType::STENCIL) ? &m_stencil_attachment
                                                                                 : nullptr;

    SCE_DBG_ASSERT(in_texture_ptr != nullptr);

    props_ptr->rb_ptr      = nullptr;
    props_ptr->texture_ptr = in_texture_ptr;
    props_ptr->n_mip       = in_n_mip;

    if (props_ptr->texture_ptr->get_create_info_ptr()->get_type() == TextureType::CUBE)
    {
        SCE_DBG_ASSERT(in_opt_cube_map_texture_face != CMTextureFace::UNKNOWN);

        props_ptr->texture_face = in_opt_cube_map_texture_face;
    }
}

void FramebufferCreateInfo::set_attachment(const AttachmentType& in_attachment_type,
                                           const Renderbuffer*   in_renderbuffer_ptr)
{
    AttachmentProps* props_ptr = (in_attachment_type == AttachmentType::COLOR_0) ? &m_color_attachment0
                               : (in_attachment_type == AttachmentType::DEPTH)   ? &m_depth_attachment
                               : (in_attachment_type == AttachmentType::STENCIL) ? &m_stencil_attachment
                                                                                 : nullptr;

    SCE_DBG_ASSERT(in_renderbuffer_ptr != nullptr);

    props_ptr->texture_ptr  = nullptr;
    props_ptr->n_mip        = UINT32_MAX;
    props_ptr->rb_ptr       = in_renderbuffer_ptr;
    props_ptr->texture_face = CMTextureFace::UNKNOWN;
}