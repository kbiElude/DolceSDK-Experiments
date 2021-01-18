#include "ES/shader.h"
#include "ES/shader_create_info.h"
#include "logger.h"

extern "C"
{
    #include <GLES2/gl2.h>
    #include <psp2/libdbg.h>
}

#include <vector>


Shader::Shader(std::unique_ptr<ShaderCreateInfo> in_create_info_ptr,
               Logger*                           in_logger_ptr)
    :m_create_info_ptr(std::move(in_create_info_ptr) ),
     m_logger_ptr     (in_logger_ptr),
     m_shader_id      (0)
{
    /* Stub */
}

Shader::~Shader()
{
    if (m_shader_id != 0)
    {
        ::glDeleteShader(m_shader_id);

        m_shader_id = 0;
    }
}

std::unique_ptr<Shader> Shader::create(std::unique_ptr<ShaderCreateInfo> in_create_info_ptr,
                                       Logger*                           in_logger_ptr)
{
    std::unique_ptr<Shader> result_ptr;

    result_ptr.reset(
        new Shader(std::move(in_create_info_ptr),
                   in_logger_ptr)
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


bool Shader::init()
{
    bool       result              = false;
    const auto shader_type_gl_enum = (m_create_info_ptr->get_type() == ShaderType::FRAGMENT) ? GL_FRAGMENT_SHADER
                                                                                             : GL_VERTEX_SHADER;

    m_shader_id = ::glCreateShader(shader_type_gl_enum);
    SCE_DBG_ASSERT(m_shader_id != 0);

    switch (m_create_info_ptr->get_source() )
    {
        case ShaderSource::BLOB:
        {
            // todo
            SCE_DBG_ASSERT(false);

            goto end;
        }

        case ShaderSource::CG:
        {
            const char* cg_ptr         = m_create_info_ptr->get_cg();
            GLint       compile_status = 0;

            ::glShaderSource(m_shader_id,
                             1,                          /* count */
                            &cg_ptr,
                             nullptr);                   /* length */

            ::glCompileShader(m_shader_id);
            ::glGetShaderiv  (m_shader_id,
                              GL_COMPILE_STATUS,
                             &compile_status);

            if (compile_status == GL_FALSE)
            {
                GLsizei n_bytes_needed_minus_terminator = 0;

                ::glGetShaderInfoLog(m_shader_id,
                                     0,                              /* bufSize */
                                    &n_bytes_needed_minus_terminator,
                                     nullptr);                       /* infoLog */

                SCE_DBG_ASSERT(n_bytes_needed_minus_terminator);
                if (n_bytes_needed_minus_terminator > 0)
                {
                    std::string info_log;
                    std::string info_log_prefix;

                    info_log.resize(n_bytes_needed_minus_terminator + 1);

                    ::glGetShaderInfoLog(m_shader_id,
                                         static_cast<GLsizei>(info_log.size() ),
                                        &n_bytes_needed_minus_terminator,
                                         reinterpret_cast<GLchar*>(const_cast<char*>(info_log.data() )));

                    info_log_prefix = "Shader [" + std::string(m_create_info_ptr->get_name() ) + "] failed to compile:\n>>>\n";

                    info_log.insert(0,
                                    info_log_prefix);

                    info_log += "\n<<\n";

                    m_logger_ptr->log(info_log.data() );

                    SCE_DBG_ASSERT(false);
                    goto end;
                }
            }
            else
            {
                m_logger_ptr->log("Shader [%s] compiled successfully.\n",
                                  m_create_info_ptr->get_name() );
            }

            break;
        }

        default:
        {
            SCE_DBG_ASSERT(false);

            goto end;
        }
    }

    result = true;
end:
    return result;
}