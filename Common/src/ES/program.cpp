#include "ES/program.h"
#include "ES/program_create_info.h"
#include "ES/shader.h"
#include "ES/shader_create_info.h"
#include "logger.h"

extern "C"
{
    #include <GLES2/gl2.h>
    #include <psp2/libdbg.h>
}

#include <vector>


Program::Program(std::unique_ptr<ProgramCreateInfo> in_create_info_ptr,
               Logger*                              in_logger_ptr)
    :m_create_info_ptr(std::move(in_create_info_ptr) ),
     m_logger_ptr     (in_logger_ptr),
     m_program_id      (0)
{
    /* Stub */
}

Program::~Program()
{
    if (m_program_id != 0)
    {
        ::glDeleteProgram(m_program_id);

        m_program_id = 0;
    }
}

std::unique_ptr<Program> Program::create(std::unique_ptr<ProgramCreateInfo> in_create_info_ptr,
                                         Logger*                            in_logger_ptr)
{
    std::unique_ptr<Program> result_ptr;

    result_ptr.reset(
        new Program(std::move(in_create_info_ptr),
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


bool Program::init()
{
    bool result = false;

    m_program_id = ::glCreateProgram();
    SCE_DBG_ASSERT(m_program_id != 0);

    ::glAttachShader(m_program_id,
                     m_create_info_ptr->get_fs_ptr()->get_shader_id() );
    ::glAttachShader(m_program_id,
                     m_create_info_ptr->get_vs_ptr()->get_shader_id() );

    {
        GLint link_status = 0;

        ::glLinkProgram (m_program_id);
        ::glGetProgramiv(m_program_id,
                         GL_LINK_STATUS,
                        &link_status);

        if (link_status == GL_FALSE)
        {
            GLsizei n_bytes_needed_minus_terminator = 0;

            ::glGetProgramInfoLog(m_program_id,
                                  0,                              /* bufSize */
                                 &n_bytes_needed_minus_terminator,
                                  nullptr);                       /* infoLog */

            SCE_DBG_ASSERT(n_bytes_needed_minus_terminator);
            if (n_bytes_needed_minus_terminator > 0)
            {
                std::string info_log;
                std::string info_log_prefix;

                info_log.resize(n_bytes_needed_minus_terminator + 1);

                ::glGetProgramInfoLog(m_program_id,
                                      static_cast<GLsizei>(info_log.size() ),
                                     &n_bytes_needed_minus_terminator,
                                      reinterpret_cast<GLchar*>(const_cast<char*>(info_log.data() )));

                info_log_prefix = "Program [" + std::string(m_create_info_ptr->get_name() ) + "] failed to link:\n>>>\n";

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
            m_logger_ptr->log(false, /* in_flush_and_wait */
                              "Program [%s] compiled successfully.\n",
                              m_create_info_ptr->get_name() );
        }
    }

    result = true;
end:
    return result;
}