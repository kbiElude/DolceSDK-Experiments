#include "ES/program_create_info.h"

ProgramCreateInfo::ProgramCreateInfo(const char*   in_name_ptr,
                                     const Shader* in_vs_ptr,
                                     const Shader* in_fs_ptr)
    :m_fs_ptr(in_fs_ptr),
     m_name  (in_name_ptr),
     m_vs_ptr(in_vs_ptr)
{
    /* Stub */
}

ProgramCreateInfo::~ProgramCreateInfo()
{
    /* Stub */
}

std::unique_ptr<ProgramCreateInfo> ProgramCreateInfo::create(const char*   in_name_ptr,
                                                             const Shader* in_vs_ptr,
                                                             const Shader* in_fs_ptr)
{
    std::unique_ptr<ProgramCreateInfo> result_ptr;

    result_ptr.reset(
        new ProgramCreateInfo(in_name_ptr,
                              in_vs_ptr,
                              in_fs_ptr)
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    return result_ptr;
}