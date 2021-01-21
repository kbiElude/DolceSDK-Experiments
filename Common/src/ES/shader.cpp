#include "ES/shader.h"
#include "ES/shader_create_info.h"
#include "logger.h"

extern "C"
{
    #include <GLES2/gl2.h>
    #include <psp2/libdbg.h>
    #include <psp2/shacccg.h>
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
            SceShaccCgCallbackList         callback_list;
            SceShaccCgCompileOptions       cg_options;
            const auto                     glsl_ptr      = m_create_info_ptr->get_cg();
            const SceShaccCgCompileOutput* result_ptr    = nullptr;

            ::sceShaccCgInitializeCompileOptions(&cg_options);
            ::sceShaccCgInitializeCallbackList  (&callback_list,
                                                 SCE_SHACCCG_TRIVIAL);

            cg_options.mainSourceFile = "shader";
            cg_options.targetProfile  = (m_create_info_ptr->get_type() == ShaderType::FRAGMENT) ? SCE_SHACCCG_PROFILE_FP
                                                                                                : SCE_SHACCCG_PROFILE_VP;

            callback_list.openFile =
            [](const char*                     in_file_name_ptr,
               const SceShaccCgSourceLocation* /* in_include_from_ptr    */,
               const SceShaccCgCompileOptions* /* in_compile_options_ptr */,
               void*                           in_user_data_ptr,
               const char**                    /* in_error_string_ptr    */)
            {
               SceShaccCgSourceFile* result_ptr = reinterpret_cast<SceShaccCgSourceFile*>(malloc(sizeof(SceShaccCgSourceFile) ));

               result_ptr->fileName = in_file_name_ptr;
               result_ptr->size     = strlen                       (reinterpret_cast<const char*>(in_user_data_ptr) );
               result_ptr->text     = reinterpret_cast<const char*>(in_user_data_ptr);

               return result_ptr;
            };

            callback_list.releaseFile =
            [](const SceShaccCgSourceFile*     in_file_name_ptr,
               const SceShaccCgCompileOptions* in_compile_options_ptr,
               void*                           in_user_data_ptr)
            {
                free(reinterpret_cast<void*>(const_cast<SceShaccCgSourceFile*>(in_file_name_ptr)));
            };

            result_ptr = ::sceShaccCgCompileProgram(&cg_options,
                                                    &callback_list,
                                                     reinterpret_cast<void*>(const_cast<char*>(glsl_ptr) ) );
            SCE_DBG_ASSERT(result_ptr != nullptr);

            log_cg_diagnostics(result_ptr->diagnosticCount,
                               result_ptr->diagnostics);

            if (result_ptr->programSize > 0)
            {
                m_logger_ptr->log(false, /* in_Flush_and_wait */
                                  "Shader [%s] has been compiled successfully\n",
                                  m_create_info_ptr->get_name() );

                ::glShaderBinary(1, /* count */
                                &m_shader_id,
                                 0, /* binaryformat */
                                 result_ptr->programData,
                                 result_ptr->programSize);
            }
            else
            {
                m_logger_ptr->log(true, /* in_Flush_and_wait */
                                  "Shader [%s] failed to compile.\n",
                                  m_create_info_ptr->get_name() );

                SCE_DBG_ASSERT(result_ptr->programSize > 0);

                goto end;
            }

            break;
        }

        case ShaderSource::GLSL:
        {
            GLint       compile_status = 0;
            const char* glsl_ptr       = m_create_info_ptr->get_glsl();

            ::glShaderSource(m_shader_id,
                             1,                          /* count */
                            &glsl_ptr,
                             nullptr);                   /* length */

            ::glCompileShader(m_shader_id);
            ::glGetShaderiv  (m_shader_id,
                              GL_COMPILE_STATUS,
                             &compile_status);

            if (compile_status == GL_FALSE)
            {
                GLsizei n_bytes_needed_minus_terminator = 0;

                ::glGetShaderiv(m_shader_id,
                                GL_INFO_LOG_LENGTH,
                               &n_bytes_needed_minus_terminator);

                SCE_DBG_ASSERT(n_bytes_needed_minus_terminator);
                if (n_bytes_needed_minus_terminator > 0)
                {
                    std::string info_log;
                    std::string info_log_prefix;

                    info_log.resize(n_bytes_needed_minus_terminator + 1);

                    ::glGetShaderInfoLog(m_shader_id,
                                         static_cast<GLsizei>(info_log.size() ),
                                         nullptr, /*length */
                                         reinterpret_cast<GLchar*>(&info_log[0]));

                    info_log_prefix = "Shader [" + std::string(m_create_info_ptr->get_name() ) + "] failed to compile:\n>>>\n";

                    info_log.insert(0,
                                    info_log_prefix);

                    info_log += "\n<<\n";

                    m_logger_ptr->log(true, /* in_flush_and_wait */
                                      info_log.data() );

                    SCE_DBG_ASSERT(false);
                    goto end;
                }
            }
            else
            {
                m_logger_ptr->log(true, /* in_flush_and_wait */
                                  "Shader [%s] compiled successfully.\n",
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

void Shader::log_cg_diagnostics(const int&                         in_n_diagnostics,
                                const SceShaccCgDiagnosticMessage* in_diagnostics_ptr) const
{
    bool errors_encountered = false;

    if (in_n_diagnostics == 0)
    {
        goto end;
    }

    m_logger_ptr->log(false, /* in_flush_and_wait */
                      "Shader [%s] diagnostics:\n>>\n",
                      m_create_info_ptr->get_name() );

    for (uint32_t n_current_diagnostic = 0;
                  n_current_diagnostic < in_n_diagnostics;
                ++n_current_diagnostic)
    {
        const auto& current_diagnostic = in_diagnostics_ptr[n_current_diagnostic];
        const char* current_level_ptr  = nullptr;

        switch (current_diagnostic.level)
        {
            case SceShaccCgDiagnosticLevel::SCE_SHACCCG_DIAGNOSTIC_LEVEL_ERROR:
            {
                current_level_ptr  = "Error";
                errors_encountered = true;

                break;
            }

            case SceShaccCgDiagnosticLevel::SCE_SHACCCG_DIAGNOSTIC_LEVEL_INFO:    current_level_ptr = "Info";    break;
            case SceShaccCgDiagnosticLevel::SCE_SHACCCG_DIAGNOSTIC_LEVEL_WARNING: current_level_ptr = "Warning"; break;

            default:
            {
                SCE_DBG_ASSERT(false);

                current_level_ptr = "!?";
            }
        }

        m_logger_ptr->log(false, /* in_flush_and_wait */
                          "%s (%d:%d): %s\n",
                          current_level_ptr,
                          static_cast<int>(current_diagnostic.location->lineNumber),
                          static_cast<int>(current_diagnostic.location->columnNumber),
                          current_diagnostic.message);
    }

    m_logger_ptr->log(errors_encountered,
                      "<<\n",nullptr);
end:
    ;
}