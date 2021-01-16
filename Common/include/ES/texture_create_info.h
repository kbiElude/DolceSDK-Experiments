#ifndef TEXTURE_CREATE_INFO_H
#define TEXTURE_CREATE_INFO_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <memory>

enum class TextureFormat
{
    ALPHA8            = GL_ALPHA8_EXT,            //< GL_EXT_texture_storage
    LUMINANCE8        = GL_LUMINANCE8_EXT,        //< GL_EXT_texture_storage
    LUMINANCE8_ALPHA8 = GL_LUMINANCE8_ALPHA8_EXT, //< GL_EXT_texture_storage

    ALPHA16F           = GL_ALPHA16F_EXT,           //< GL_OES_texture_half_float
    LUMINANCE_ALPHA16F = GL_LUMINANCE_ALPHA16F_EXT, //< GL_OES_texture_half_float
    LUMINANCE16F       = GL_LUMINANCE16F_EXT,       //< GL_OES_texture_half_float
    R16F               = GL_R16F_EXT,               //< GL_OES_texture_half_float
    RG16F              = GL_RG16F_EXT,              //< GL_OES_texture_half_float
    RGBA16F            = GL_RGBA16F_EXT,            //< GL_OES_texture_half_float
    RGB16F             = GL_RGB16F_EXT,             //< GL_OES_texture_half_float

    BGRA8              = GL_BGRA8_EXT,              //< GL_EXT_texture_format_BGRA8888

    COMPRESSED_RGB_RGB_S3TC_DXT1  = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,  //< GL_EXT_texture_compression_dxt1
    COMPRESSED_RGBA_RGB_S3TC_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, //< GL_EXT_texture_compression_dxt1

    COMPRESSED_RGBA_S3TC_DXT3       = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       //< ? GL_EXT_texture_compression_dxt3 == GL_EXT_texture_compression_s3tc      ?
    COMPRESSED_SRGB_ALPHA_S3TC_DXT3 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, //< ? GL_EXT_texture_compression_dxt3 == GL_EXT_texture_compression_s3tc_srgb ?

    COMPRESSED_RGBA_S3TC_DXT5       = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       //< ? GL_EXT_texture_compression_dxt5 == GL_EXT_texture_compression_s3tc      ?
    COMPRESSED_SRGB_ALPHA_S3TC_DXT5 = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, //< ? GL_EXT_texture_compression_dxt5 == GL_EXT_texture_compression_s3tc_srgb ?
};

enum class TextureMagFilter
{
    LINEAR  = GL_LINEAR,
    NEAREST = GL_NEAREST
};

enum class TextureMinFilter
{
    LINEAR             = GL_LINEAR,
    LINEAR_MIP_LINEAR  = GL_LINEAR_MIPMAP_LINEAR,
    LINEAR_MIP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,

    NEAREST             = GL_NEAREST,
    NEAREST_MIP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
    NEAREST_MIP_NEAREST = GL_NEAREST_MIPMAP_NEAREST
};

enum class TextureType
{
    _2D,
    CUBE
};

enum class TextureWrap
{
    CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    REPEAT          = GL_REPEAT
};

class TextureCreateInfo
{
public:
    ~TextureCreateInfo();

    static std::unique_ptr<TextureCreateInfo> create(const TextureType&      in_type,
                                                     const TextureFormat&    in_internal_format,
                                                     const uint32_t&         in_n_mips,
                                                     const GLsizei&          in_base_mip_width,
                                                     const GLsizei&          in_base_mip_height,
                                                     const TextureMagFilter& in_start_mag_filter,
                                                     const TextureMinFilter& in_start_min_filter,
                                                     const TextureWrap&      in_start_texture_wrap_s,
                                                     const TextureWrap&      in_start_texture_wrap_t);

    const GLsizei& get_base_mip_height() const
    {
        return m_base_mip_height;
    }

    const GLsizei& get_base_mip_width() const
    {
        return m_base_mip_width;
    }

    const TextureFormat& get_internal_format() const
    {
        return m_internal_format;
    }

    const uint32_t& get_n_mips() const
    {
        return m_n_mips;
    }

    const TextureMagFilter& get_start_mag_filter() const
    {
        return m_start_mag_filter;
    }

    const TextureMinFilter& get_start_min_filter() const
    {
        return m_start_min_filter;
    }

    const TextureWrap& get_start_texture_wrap_s() const
    {
        return m_start_texture_wrap_s;
    }

    const TextureType& get_type() const
    {
        return m_type;
    }

    const TextureWrap& get_start_texture_wrap_t() const
    {
        return m_start_texture_wrap_t;
    }

    void set_base_mip_height(const GLsizei& in_height)
    {
        m_base_mip_height = in_height;
    }

    void set_base_mip_width(const GLsizei& in_width)
    {
        m_base_mip_width = in_width;
    }

    void set_internal_format(const TextureFormat& in_internal_format)
    {
        m_internal_format = in_internal_format;
    }

    void set_start_mag_filter(const TextureMagFilter& in_texture_mag_filter)
    {
        m_start_mag_filter = in_texture_mag_filter;
    }

    void set_start_min_filter(const TextureMinFilter& in_texture_min_filter)
    {
        m_start_min_filter = in_texture_min_filter;
    }

    void set_start_texture_wrap_s(const TextureWrap& in_texture_wrap_s)
    {
        m_start_texture_wrap_s = in_texture_wrap_s;
    }

    void set_start_texture_wrap_t(const TextureWrap& in_texture_wrap_t)
    {
        m_start_texture_wrap_t = in_texture_wrap_t;
    }

private:
    /* Private functions */
    TextureCreateInfo(const TextureType&      in_type,
                      const TextureFormat&    in_internal_format,
                      const uint32_t&         in_n_mips,
                      const GLsizei&          in_base_mip_width,
                      const GLsizei&          in_base_mip_height,
                      const TextureMagFilter& in_start_mag_filter,
                      const TextureMinFilter& in_start_min_filter,
                      const TextureWrap&      in_start_texture_wrap_s,
                      const TextureWrap&      in_start_texture_wrap_t);

    /* Private variables */
    GLsizei       m_base_mip_height;
    GLsizei       m_base_mip_width;
    TextureFormat m_internal_format;
    uint32_t      m_n_mips;
    TextureType   m_type;

    TextureMagFilter m_start_mag_filter;
    TextureMinFilter m_start_min_filter;
    TextureWrap      m_start_texture_wrap_s;
    TextureWrap      m_start_texture_wrap_t;
};

#endif /* TEXTURE_CREATE_INFO_H */