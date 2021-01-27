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

const VariableProperties* Program::get_active_attribute_props_ptr_by_name(const char* in_name) const
{
    auto                      iterator   = m_name_to_active_attribute_index_map.find(std::string(in_name) );
    const VariableProperties* result_ptr = nullptr;

    if (iterator != m_name_to_active_attribute_index_map.cend() )
    {
        result_ptr = m_active_attribute_vec.data() + iterator->second;
    }

    return result_ptr;
}

const VariableProperties* Program::get_active_uniform_props_ptr_by_name(const char* in_name) const
{
    auto                      iterator   = m_name_to_active_uniform_index_map.find(std::string(in_name) );
    const VariableProperties* result_ptr = nullptr;

    if (iterator != m_name_to_active_uniform_index_map.cend() )
    {
        m_logger_ptr->log(true, "Unif found\n");

        result_ptr = m_active_uniform_vec.data() + iterator->second;
    }
    else
    {
        m_logger_ptr->log(true, "Unif not found\n");
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

            ::glGetProgramiv(m_program_id,
                             GL_INFO_LOG_LENGTH,
                            &n_bytes_needed_minus_terminator);

            SCE_DBG_ASSERT(n_bytes_needed_minus_terminator);
            if (n_bytes_needed_minus_terminator > 0)
            {
                std::string info_log;
                std::string info_log_prefix;

                info_log.resize(n_bytes_needed_minus_terminator + 1);

                ::glGetProgramInfoLog(m_program_id,
                                      static_cast<GLsizei>(info_log.size() ),
                                      nullptr, /* length */
                                      reinterpret_cast<GLchar*>(&info_log[0]));

                m_logger_ptr->log(true, /* in_flush_and_wait */
                                  "Program [%s] failed to link:\n>>\n%s\n<<\n",
                                  m_create_info_ptr->get_name(),
                                  info_log.data              () );

                SCE_DBG_ASSERT(false);
                goto end;
            }
            else
            {
                m_logger_ptr->log(true, /* in_flush_and_wait */
                                  "Program [%s] failed to link.\n",
                                  m_create_info_ptr->get_name() );

                goto end;
            }
        }
        else
        {
            m_logger_ptr->log(false, /* in_flush_and_wait */
                              "Program [%s] was linked successfully.\n",
                              m_create_info_ptr->get_name() );
        }
    }

    /* Cache active attributes */
    {
        std::vector<uint8_t> attribute_name_u8_vec;
        GLint                max_attribute_name_length = 0;
        GLint                n_active_attributes       = 0;

        ::glGetProgramiv(m_program_id,
                         GL_ACTIVE_ATTRIBUTES,
                        &n_active_attributes);
        ::glGetProgramiv(m_program_id,
                         GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                        &max_attribute_name_length);

        m_active_attribute_vec.resize(n_active_attributes);
        attribute_name_u8_vec.resize (max_attribute_name_length + 1);

        for (GLint n_current_attribute = 0;
                   n_current_attribute < n_active_attributes;
                 ++n_current_attribute)
        {
            VariableProperties current_attribute_props;
            GLint              dummy                   = 0;
            GLenum             var_type                = 0;

            ::glGetActiveAttrib(m_program_id,
                                n_current_attribute,
                                max_attribute_name_length,
                               &dummy,
                               &current_attribute_props.array_size,
                               &var_type,
                                reinterpret_cast<GLchar*>(attribute_name_u8_vec.data() ));

            current_attribute_props.location = ::glGetAttribLocation      (m_program_id,
                                                                           current_attribute_props.name.c_str() );
            current_attribute_props.name     = std::string                (reinterpret_cast<char*>         (attribute_name_u8_vec.data() ));
            current_attribute_props.type     = static_cast<CGVariableType>(var_type);

            SCE_DBG_ASSERT(current_attribute_props.location != -1);

            m_active_attribute_vec.at           (n_current_attribute)          = current_attribute_props;
            m_name_to_active_attribute_index_map[current_attribute_props.name] = n_current_attribute;
        }
    }

    /* Cache active uniforms */
    {
        GLint                max_uniform_name_length = 0;
        GLint                n_active_uniforms       = 0;
        std::vector<uint8_t> uniform_name_u8_vec;

        ::glGetProgramiv(m_program_id,
                         GL_ACTIVE_UNIFORMS,
                        &n_active_uniforms);
        ::glGetProgramiv(m_program_id,
                         GL_ACTIVE_UNIFORM_MAX_LENGTH,
                        &max_uniform_name_length);

        m_active_uniform_vec.resize(n_active_uniforms);
        uniform_name_u8_vec.resize (max_uniform_name_length + 1);

        for (GLint n_current_uniform = 0;
                   n_current_uniform < n_active_uniforms;
                 ++n_current_uniform)
        {
            VariableProperties current_uniform_props;
            GLint              dummy                 = 0;
            GLenum             var_type              = 0;

            ::glGetActiveUniform(m_program_id,
                                 n_current_uniform,
                                 max_uniform_name_length + 1,
                                &dummy,
                                &current_uniform_props.array_size,
                                &var_type,
                                 reinterpret_cast<GLchar*>(uniform_name_u8_vec.data() ));

            current_uniform_props.location = ::glGetUniformLocation     (m_program_id,
                                                                         current_uniform_props.name.c_str() );
            current_uniform_props.name     = std::string                (reinterpret_cast<char*>         (uniform_name_u8_vec.data() ));
            current_uniform_props.type     = static_cast<CGVariableType>(var_type);

            SCE_DBG_ASSERT(current_uniform_props.location != -1);

            m_active_uniform_vec.at           (n_current_uniform)          = current_uniform_props;
            m_name_to_active_uniform_index_map[current_uniform_props.name] = n_current_uniform;
        }
    }

    result = true;
end:
    return result;
}