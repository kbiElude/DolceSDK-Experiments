#include "ES/texture_create_info.h"

extern "C"
{
    #include <psp2/libdbg.h>
}

TextureCreateInfo::TextureCreateInfo(const TextureType&      in_type,
                                     const TextureFormat&    in_internal_format,
                                     const uint32_t&         in_n_mips,
                                     const GLsizei&          in_base_mip_width,
                                     const GLsizei&          in_base_mip_height,
                                     const TextureMagFilter& in_start_mag_filter,
                                     const TextureMinFilter& in_start_min_filter,
                                     const TextureWrap&      in_start_texture_wrap_s,
                                     const TextureWrap&      in_start_texture_wrap_t)
    :m_base_mip_height     (in_base_mip_height),
     m_base_mip_width      (in_base_mip_width),
     m_internal_format     (in_internal_format),
     m_n_mips              (in_n_mips),
     m_start_mag_filter    (in_start_mag_filter),
     m_start_min_filter    (in_start_min_filter),
     m_start_texture_wrap_s(in_start_texture_wrap_s),
     m_start_texture_wrap_t(in_start_texture_wrap_t),
     m_type                (in_type)
{
    /* Stub */
}

TextureCreateInfo::~TextureCreateInfo()
{
    /* Stub */
}

std::unique_ptr<TextureCreateInfo> TextureCreateInfo::create(const TextureType&      in_type,
                                                             const TextureFormat&    in_internal_format,
                                                             const uint32_t&         in_n_mips,
                                                             const GLsizei&          in_base_mip_width,
                                                             const GLsizei&          in_base_mip_height,
                                                             const TextureMagFilter& in_start_mag_filter,
                                                             const TextureMinFilter& in_start_min_filter,
                                                             const TextureWrap&      in_start_texture_wrap_s,
                                                             const TextureWrap&      in_start_texture_wrap_t)
{
    std::unique_ptr<TextureCreateInfo> result_ptr;

    result_ptr.reset(
        new TextureCreateInfo(in_type,
                              in_internal_format,
                              in_n_mips,
                              in_base_mip_width,
                              in_base_mip_height,
                              in_start_mag_filter,
                              in_start_min_filter,
                              in_start_texture_wrap_s,
                              in_start_texture_wrap_t)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    return result_ptr;
}

