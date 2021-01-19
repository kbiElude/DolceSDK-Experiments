#ifndef PROGRAM_H
#define PROGRAM_H

#include <GLES2/gl2.h>
#include <memory>

class Logger;
class ProgramCreateInfo;

class Program
{
public:
    /* Public functions */
    static std::unique_ptr<Program> create(std::unique_ptr<ProgramCreateInfo> in_create_info_ptr,
                                           Logger*                            in_logger_ptr);

    ~Program();

private:
    /* Private functions */

    Program(std::unique_ptr<ProgramCreateInfo> in_create_info_ptr,
            Logger*                            in_logger_ptr);

    bool init();

    /* Private variables */
    std::unique_ptr<ProgramCreateInfo> m_create_info_ptr;
    Logger* const                      m_logger_ptr;
    GLuint                             m_program_id;
};

#endif /* PROGRAM_H */