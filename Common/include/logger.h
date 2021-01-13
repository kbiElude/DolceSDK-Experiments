#ifndef LOGGER_H
#define LOGGER_H

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

    void log(const char* in_format,
             ...);

private:
    /* Private functions */

    bool init();

    static int file_logger_thread_entrypoint(void* logger_raw_ptr);

    /* Private variables */
    volatile SceUID m_file_id;
    volatile bool   m_must_die;

    std::unique_ptr<Thread> m_logger_thread_ptr;
    std::unique_ptr<Event>  m_logger_thread_event_ptr;

    std::vector<std::string> m_pending_chunks_vec;
    std::unique_ptr<Mutex>   m_pending_chunks_vec_mutex_ptr;
};

#endif /* LOGGER_H */