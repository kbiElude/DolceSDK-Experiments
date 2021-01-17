#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES2/gl2.h>

#include <memory>

class EGLInstance;
class TextureCreateInfo;

class Texture
{
public:
    /* Public functions */
    static std::unique_ptr<Texture> create(const EGLInstance*                 in_egl_instance_ptr,
                                           std::unique_ptr<TextureCreateInfo> in_create_info_ptr);

    ~Texture();

    const TextureCreateInfo* get_create_info_ptr() const
    {
        return m_create_info_ptr.get();
    }

    const GLuint& get_texture_id() const
    {
        return m_texture_id;
    }

private:
    /* Private functions */
    Texture(const EGLInstance*                 in_egl_instance_ptr,
            std::unique_ptr<TextureCreateInfo> in_create_info_ptr);

    bool init();

    /* Private variables */
    std::unique_ptr<TextureCreateInfo> m_create_info_ptr;
    const EGLInstance*                 m_egl_instance_ptr;

    GLuint m_texture_id;
};

#endif /* TEXTURE_H */