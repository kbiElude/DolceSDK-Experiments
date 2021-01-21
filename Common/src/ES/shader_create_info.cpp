#include "ES/shader_create_info.h"

ShaderCreateInfo::ShaderCreateInfo(const char*         in_name_ptr,
                                   const ShaderType&   in_type,
                                   const ShaderSource& in_source,
                                   const void*         in_data_ptr)
    :m_glsl( (in_source == ShaderSource::GLSL) ? reinterpret_cast<const char*>(in_data_ptr)
                                               : ""),
     m_name  (in_name_ptr),
     m_source(in_source),
     m_type  (in_type)
{
    SCE_DBG_ASSERT( (in_source == ShaderSource::BLOB) ||
                    (in_source == ShaderSource::GLSL) );
}

ShaderCreateInfo::~ShaderCreateInfo()
{
    /* Stub */
}

std::unique_ptr<ShaderCreateInfo> ShaderCreateInfo::create_from_glsl(const char*       in_name_ptr,
                                                                     const ShaderType& in_type,
                                                                     const char*       in_shader_code_ptr)
{
    std::unique_ptr<ShaderCreateInfo> result_ptr;

    result_ptr.reset(
        new ShaderCreateInfo(in_name_ptr,
                             in_type,
                             ShaderSource::GLSL,
                             in_shader_code_ptr)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    return result_ptr;
}