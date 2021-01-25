#ifndef BUFFER_H
#define BUFFER_H

#include <GLES2/gl2.h>
#include <memory>

class BufferCreateInfo;
class Logger;


class Buffer
{
public:
    /* Public functions */
    static std::unique_ptr<Buffer> create(std::unique_ptr<BufferCreateInfo> in_create_info_ptr,
                                          Logger*                           in_logger_ptr);

    ~Buffer();
private:
    /* Private functions */
    Buffer(std::unique_ptr<BufferCreateInfo> in_create_info_ptr,
           Logger*                           in_logger_ptr);

    bool init();

    /* Private variables */
    const std::unique_ptr<BufferCreateInfo> m_create_info_ptr;
    Logger* const                           m_logger_ptr;

    GLuint                                  m_id;
};

#endif /* BUFFER_H */