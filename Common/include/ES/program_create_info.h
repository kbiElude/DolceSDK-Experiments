#ifndef PROGRAM_CREATE_INFO_H
#define PROGRAM_CREATE_INFO_H

#include <GLES2/gl2.h>
#include <memory>

extern "C"
{
    #include <psp2/libdbg.h>
}

class Shader;

class ProgramCreateInfo
{
public:
    /* Public functions */
    std::unique_ptr<ProgramCreateInfo> create(const char*   in_name_ptr,
                                              const Shader* in_vs_ptr,
                                              const Shader* in_fs_ptr);

    ~ProgramCreateInfo();

    const Shader* get_fs_ptr() const
    {
        return m_fs_ptr;
    }

    const char* get_name() const
    {
        return m_name.data();
    }

    const Shader* get_vs_ptr() const
    {
        return m_vs_ptr;
    }

private:
    /* Private functions */

    ProgramCreateInfo(const char*   in_name_ptr,
                      const Shader* in_vs_ptr,
                      const Shader* in_fs_ptr);

    /* Private variables */
    const Shader* m_fs_ptr;
    std::string   m_name;
    const Shader* m_vs_ptr;
};

#endif /* PROGRAM_CREATE_INFO_H */