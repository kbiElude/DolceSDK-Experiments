#include "EGL/eglInstance.h"
#include "ES/texture.h"
#include "ES/texture_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

Texture::Texture(const EGLInstance*                 in_egl_instance_ptr,
                 std::unique_ptr<TextureCreateInfo> in_create_info_ptr)
    :m_create_info_ptr (std::move(in_create_info_ptr) ),
     m_egl_instance_ptr(in_egl_instance_ptr),
     m_texture_id      (0)
{
    /* Stub */
}

Texture::~Texture()
{
    if (m_texture_id != 0)
    {
        ::glDeleteTextures(1, /* n */
                          &m_texture_id);

        m_texture_id = 0;
    }
}

std::unique_ptr<Texture> Texture::create(const EGLInstance*                 in_egl_instance_ptr,
                                         std::unique_ptr<TextureCreateInfo> in_create_info_ptr)
{
    std::unique_ptr<Texture> result_ptr;

    result_ptr.reset(
        new Texture(in_egl_instance_ptr,
                    std::move(in_create_info_ptr) )
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

bool Texture::init()
{
    const auto ext_texture_storage_entrypoints_ptr = m_egl_instance_ptr->get_ext_texture_storage_entrypoints_ptr();
    bool       result                              = false;
    const auto texture_type                        = m_create_info_ptr->get_type();
    const auto texture_binding                     = (texture_type == TextureType::_2D) ? GL_TEXTURE_2D
                                                                                        : GL_TEXTURE_CUBE_MAP;

    SCE_DBG_ASSERT((texture_type == TextureType::_2D)  ||
                   (texture_type == TextureType::CUBE) );

    ::glGenTextures(1, /* n */
                   &m_texture_id);

    SCE_DBG_ASSERT(m_texture_id != 0);

    ::glBindTexture(texture_binding,
                    m_texture_id);

    ext_texture_storage_entrypoints_ptr->glTexStorage2DEXT(texture_binding,
                                                           m_create_info_ptr->get_n_mips         (),
                                                           static_cast<GLenum>                   (m_create_info_ptr->get_internal_format() ),
                                                           m_create_info_ptr->get_base_mip_width (),
                                                           m_create_info_ptr->get_base_mip_height() );

    if (m_create_info_ptr->get_start_min_filter() != TextureMinFilter::NEAREST_MIP_LINEAR) //< default state
    {
        ::glTexParameteri(texture_binding,
                          GL_TEXTURE_MIN_FILTER,
                          static_cast<GLenum>(m_create_info_ptr->get_start_min_filter() ));
    }

    if (m_create_info_ptr->get_start_mag_filter() != TextureMagFilter::LINEAR) //< default state
    {
        ::glTexParameteri(texture_binding,
                          GL_TEXTURE_MAG_FILTER,
                          static_cast<GLenum>(m_create_info_ptr->get_start_mag_filter() ));
    }

    if (m_create_info_ptr->get_start_texture_wrap_s() != TextureWrap::REPEAT)
    {
        ::glTexParameteri(texture_binding,
                          GL_TEXTURE_WRAP_S,
                          static_cast<GLenum>(m_create_info_ptr->get_start_texture_wrap_s() ));
    }

    if (m_create_info_ptr->get_start_texture_wrap_t() != TextureWrap::REPEAT)
    {
        ::glTexParameteri(texture_binding,
                          GL_TEXTURE_WRAP_T,
                          static_cast<GLenum>(m_create_info_ptr->get_start_texture_wrap_t() ));
    }

    return result;
}
