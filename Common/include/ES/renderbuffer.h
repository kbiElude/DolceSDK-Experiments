#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <GLES2/gl2.h>

#include "renderbuffer_create_info.h"
#include <memory>

class Renderbuffer
{
public:
    /* Public functions */
    static std::unique_ptr<Renderbuffer> create(std::unique_ptr<RenderbufferCreateInfo> in_create_info_ptr);

    const GLuint& get_renderbuffer_id() const
    {
        return m_rb_id;
    }

    ~Renderbuffer();

private:
    /* Private functions */
    Renderbuffer(std::unique_ptr<RenderbufferCreateInfo> in_create_info_ptr);

    bool init();
    
    /* Private variables */
    std::unique_ptr<RenderbufferCreateInfo> m_create_info_ptr;

    GLuint m_rb_id;
};

#endif /* RENDERBUFFER_H */