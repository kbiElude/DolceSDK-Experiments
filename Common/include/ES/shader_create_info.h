#ifndef SHADER_CREATE_INFO_H
#define SHADER_CREATE_INFO_H

#include <GLES2/gl2.h>
#include <memory>
#include <vector>

extern "C"
{
    #include <psp2/libdbg.h>
}

enum class ShaderSource
{
    BLOB,
    CG,
    GLSL,

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
    static std::unique_ptr<ShaderCreateInfo> create_from_blob(const char*       in_name_ptr,
                                                              const ShaderType& in_type,
                                                              const void*       in_blob_ptr,
                                                              const uint32_t&   in_blob_size);
    static std::unique_ptr<ShaderCreateInfo> create_from_cg  (const char*       in_name_ptr,
                                                              const ShaderType& in_type,
                                                              const char*       in_shader_code_ptr);
    static std::unique_ptr<ShaderCreateInfo> create_from_glsl(const char*       in_name_ptr,
                                                              const ShaderType& in_type,
                                                              const char*       in_shader_code_ptr);

    ~ShaderCreateInfo();

    const void* get_blob_ptr() const
    {
        SCE_DBG_ASSERT(m_source == ShaderSource::BLOB);

        return m_blob.data();
    }

    uint32_t get_blob_size() const
    {
        SCE_DBG_ASSERT(m_source == ShaderSource::BLOB);

        return static_cast<uint32_t>(m_blob.size() );
    }

    const char* get_cg() const
    {
        SCE_DBG_ASSERT(m_source == ShaderSource::CG);

        return m_shader_code.c_str();
    }

    const char* get_glsl() const
    {
        SCE_DBG_ASSERT(m_source == ShaderSource::GLSL);

        return m_shader_code.c_str();
    }

    const char* get_name() const
    {
        return m_name.c_str();
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
                     const void*         in_data_ptr,
                     const uint32_t*     in_opt_data_size_ptr);

    /* Private variables */
    std::vector<uint8_t> m_blob;
    std::string          m_name;
    std::string          m_shader_code;
    ShaderSource         m_source;
    ShaderType           m_type;
};

#endif /* SHADER_CREATE_INFO_H */