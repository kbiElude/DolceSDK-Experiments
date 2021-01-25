#ifndef BUFFER_CREATE_INFO_H
#define BUFFER_CREATE_INFO_H

#include <memory>
#include <GLES2/gl2.h>

enum class BufferUsage : uint16_t
{
    // Data store contents will be modified repeatedly and used many times
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,

    // Data store contents will be modified once and used many times
    STATIC_DRAW  = GL_STATIC_DRAW,

    // Data store contents will be modified once and used at most a few times.
    STREAM_DRAW  = GL_STREAM_DRAW,
};

class BufferCreateInfo
{
public:
    /* Public functions */
    static std::unique_ptr<BufferCreateInfo> create(const uint32_t&    in_size,
                                                    const BufferUsage& in_usage);

    const uint32_t& get_size() const
    {
        return m_size;
    }

    const BufferUsage& get_usage() const
    {
        return m_usage;
    }

private:
    /* Private functions */
    BufferCreateInfo(const uint32_t& in_size,
                     const BufferUsage& in_usage);

    const uint32_t    m_size;
    const BufferUsage m_usage;
};

#endif /* BUFFER_CREATE_INFO_H */