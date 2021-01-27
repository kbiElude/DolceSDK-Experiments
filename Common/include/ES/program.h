#ifndef PROGRAM_H
#define PROGRAM_H

#include <GLES2/gl2.h>
#include <memory>
#include <unordered_map>
#include <vector>

class Logger;
class ProgramCreateInfo;

enum class CGVariableType : uint16_t
{
    FLOAT      = GL_FLOAT,
    FLOAT_VEC2 = GL_FLOAT_VEC2,
    FLOAT_VEC3 = GL_FLOAT_VEC3,
    FLOAT_VEC4 = GL_FLOAT_VEC4,
    FLOAT_MAT2 = GL_FLOAT_MAT2,
    FLOAT_MAT3 = GL_FLOAT_MAT3,
    FLOAT_MAT4 = GL_FLOAT_MAT4,
    INT        = GL_INT,
    INT_VEC2   = GL_INT_VEC2,
    INT_VEC3   = GL_INT_VEC3,
    INT_VEC4   = GL_INT_VEC4,

    UNKNOWN
};

struct VariableProperties
{
    GLint          array_size;
    GLint          location;
    std::string    name;
    CGVariableType type;

    VariableProperties()
        :array_size(0),
         location  (-1),
         type      (CGVariableType::UNKNOWN)
    {
        /* Stub */
    }
};

class Program
{
public:
    /* Public functions */
    static std::unique_ptr<Program> create(std::unique_ptr<ProgramCreateInfo> in_create_info_ptr,
                                           Logger*                            in_logger_ptr);

    ~Program();

    const VariableProperties* get_active_attribute_props_ptr_by_name(const char* in_name) const;
    const VariableProperties* get_active_uniform_props_ptr_by_name  (const char* in_name) const;

    const GLuint& get_program_id() const
    {
        return m_program_id;
    }

private:
    /* Private functions */

    Program(std::unique_ptr<ProgramCreateInfo> in_create_info_ptr,
            Logger*                            in_logger_ptr);

    bool init();

    /* Private variables */
    std::unique_ptr<ProgramCreateInfo> m_create_info_ptr;
    Logger* const                      m_logger_ptr;
    GLuint                             m_program_id;

    std::vector<VariableProperties> m_active_attribute_vec;
    std::vector<VariableProperties> m_active_uniform_vec;

    std::unordered_map<std::string, uint32_t> m_name_to_active_attribute_index_map;
    std::unordered_map<std::string, uint32_t> m_name_to_active_uniform_index_map;

};

#endif /* PROGRAM_H */