#ifndef SHADER_CREATE_INFO_H
#define SHADER_CREATE_INFO_H

#include <GLES2/gl2.h>
#include <memory>

extern "C"
{
    #include <psp2/libdbg.h>
}

enum class ShaderSource
{
    BLOB,
    CG,

    UNKNOWN
};

enum class ShaderType
{
    FRAGMENT,
    VERTEX,

    UNKNOWN
};

class ShaderCreateInfo
{
public:
    /* Public functions */
    std::unique_ptr<ShaderCreateInfo> create_from_cg(const char*       in_name_ptr,
                                                     const ShaderType& in_type,
                                                     const char*       in_shader_code_ptr);

    ~ShaderCreateInfo();

    const char* get_cg() const
    {
        SCE_DBG_ASSERT(m_source == ShaderSource::CG);

        return m_cg.data();
    }

    const char* get_name() const
    {
        return m_name.data();
    }

    const ShaderSource& get_source() const
    {
        return m_source;
    }

    const ShaderType& get_type() const
    {
        return m_type;
    }

private:
    /* Private functions */

    ShaderCreateInfo(const char*         in_name_ptr,
                     const ShaderType&   in_type,
                     const ShaderSource& in_source,
                     const void*         in_data_ptr);

    /* Private variables */
    std::string  m_cg;
    std::string  m_name;
    ShaderSource m_source;
    ShaderType   m_type;
};

#endif /* SHADER_CREATE_INFO_H */