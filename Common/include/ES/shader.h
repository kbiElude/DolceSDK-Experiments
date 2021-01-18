#ifndef SHADER_H
#define SHADER_H

#include <GLES2/gl2.h>
#include <memory>

class Logger;
class ShaderCreateInfo;

class Shader
{
public:
    /* Public functions */
    std::unique_ptr<Shader> create(std::unique_ptr<ShaderCreateInfo> in_create_info_ptr,
                                   Logger*                           in_logger_ptr);

    ~Shader();

private:
    /* Private functions */

    Shader(std::unique_ptr<ShaderCreateInfo> in_create_info_ptr,
           Logger*                           in_logger_ptr);

    bool init();

    /* Private variables */
    std::unique_ptr<ShaderCreateInfo> m_create_info_ptr;
    Logger* const                     m_logger_ptr;
    GLuint                            m_shader_id;
};

#endif /* SHADER_H */