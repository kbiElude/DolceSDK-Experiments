#ifndef FRAMEBUFFER_INFO_H
#define FRAMEBUFFER_INFO_H

#include <GLES2/gl2.h>
#include <memory>

class FramebufferCreateInfo;

class Framebuffer
{
public:
    /* Public functions */
    ~Framebuffer();

    std::unique_ptr<Framebuffer> create(std::unique_ptr<FramebufferCreateInfo> in_create_info_ptr);

    GLuint get_fb_id() const
    {
        return m_fb_id;
    }

private:
    Framebuffer(std::unique_ptr<FramebufferCreateInfo> in_create_info_ptr);

    /* Private functions */
    bool init();

    /* Private variables */
    std::unique_ptr<FramebufferCreateInfo> m_create_info_ptr;

    GLuint m_fb_id;
};

#endif /* FRAMEBUFFER_H */