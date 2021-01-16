#ifndef RENDERBUFFER_CREATE_INFO_H
#define RENDERBUFFER_CREATE_INFO_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <memory>

enum class RenderbufferFormat
{
    /* Color-renderable formats.. */
    BGRA      = GL_BGRA_EXT,        //< GL_EXT_texture_format_BGRA8888
    RGB565    = GL_RGB565,          //< core ES2
    RGBA4     = GL_RGBA4,           //< core ES2
    RGB5_A1   = GL_RGB5_A1,         //< core ES2
    RGB8      = GL_RGB8_OES,        //< GL_OES_rgb8_rgba8
    RGBA8_OES = GL_RGBA8_OES,       //< GL_OES_rgb8_rgba8

    /* Depth-renderable formats.. */
    DEPTH_16 = GL_DEPTH_COMPONENT16, //< core ES2
    DEPTH    = GL_DEPTH_COMPONENT,   //< GL_OES_depth_texture

    /* Stencil-renderable formats.. */
    STENCIL_8 = GL_STENCIL_INDEX8    //< core ES2
};

class RenderbufferCreateInfo
{
public:
    ~RenderbufferCreateInfo();

    static std::unique_ptr<RenderbufferCreateInfo> create(const RenderbufferFormat& in_internal_format,
                                                          const GLsizei&            in_width,
                                                          const GLsizei&            in_height);

    const GLsizei& get_height() const
    {
        return m_height;
    }

    const RenderbufferFormat& get_internal_format() const
    {
        return m_internal_format;
    }

    const GLsizei& get_width() const
    {
        return m_width;
    }

    void set_height(const GLsizei& in_height)
    {
        m_height = in_height;
    }

    void set_internal_format(const RenderbufferFormat& in_internal_format)
    {
        m_internal_format = in_internal_format;
    }

    void set_width(const GLsizei& in_width)
    {
        m_width = in_width;
    }

private:
    /* Private functions */
    RenderbufferCreateInfo(const RenderbufferFormat& in_internal_format,
                           const GLsizei&            in_width,
                           const GLsizei&            in_height);

    /* Private variables */
    GLsizei            m_height;
    RenderbufferFormat m_internal_format;
    GLsizei            m_width;
};
#endif /* RENDERBUFFER_CREATE_INFO_H */