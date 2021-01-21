#ifndef LOGGER_H
#define LOGGER_H

extern "C"
{
    #include <psp2/kernel/processmgr.h>
}

#include <cstring>
#include <memory>
#include <vector>

class Event;
class Mutex;
class Thread;

class Logger
{
public:
    /* Public functions */
    static std::unique_ptr<Logger> create();

     Logger();
    ~Logger();

    void log(const bool& in_flush_and_wait,
             const char* in_format,
             ...);

private:
    /* Private type definitions */
    struct Chunk
    {
        std::string       string;
        SceKernelSysClock time;

        Chunk(const std::string&       in_string,
              const SceKernelSysClock& in_time)
            :string(in_string),
             time   (in_time)
        {
            /* Stub */
        }

        Chunk(const Chunk& in_chunk)
        {
            string = in_chunk.string;

            memcpy(&time,
                   &in_chunk.time,
                   sizeof(time) );
        }
    };

    /* Private functions */

    bool init();

    static int file_logger_thread_entrypoint(void* logger_raw_ptr);

    /* Private variables */
    volatile SceUID   m_file_id;
    volatile bool     m_must_die;
    SceKernelSysClock m_start_time;

    std::unique_ptr<Thread> m_logger_thread_ptr;
    std::unique_ptr<Event>  m_logger_thread_event_ptr;

    std::vector<Chunk>     m_pending_chunks_vec;
    std::unique_ptr<Mutex> m_pending_chunks_vec_mutex_ptr;
};

#endif /* LOGGER_H */