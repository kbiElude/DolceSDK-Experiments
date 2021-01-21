#ifndef SHADER_H
#define SHADER_H

#include <GLES2/gl2.h>
#include <memory>

class  Logger;
struct SceShaccCgDiagnosticMessage;
class  ShaderCreateInfo;

class Shader
{
public:
    /* Public functions */
    static std::unique_ptr<Shader> create(std::unique_ptr<ShaderCreateInfo> in_create_info_ptr,
                                          Logger*                           in_logger_ptr);

    ~Shader();

    const ShaderCreateInfo* get_create_info_ptr() const
    {
        return m_create_info_ptr.get();
    }

    const GLuint& get_shader_id() const
    {
        return m_shader_id;
    }
private:
    /* Private functions */

    Shader(std::unique_ptr<ShaderCreateInfo> in_create_info_ptr,
           Logger*                           in_logger_ptr);

    bool init              ();
    void log_cg_diagnostics(const int&                         in_n_diagnostics,
                            const SceShaccCgDiagnosticMessage* in_diagnostics_ptr) const;

    /* Private variables */
    std::unique_ptr<ShaderCreateInfo> m_create_info_ptr;
    Logger* const                     m_logger_ptr;
    GLuint                            m_shader_id;
};

#endif /* SHADER_H */