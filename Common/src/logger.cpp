#include "event.h"
#include "logger.h"
#include "mutex.h"
#include "thread.h"

#include <cstdarg>
#include <mutex>
#include <string.h>

extern "C"
{
    #include <psp2/libdbg.h>
    #include <psp2/kernel/iofilemgr.h>
}

#define _1MS_IN_MICROSECONDS (1000u)
#define _1S_IN_MICROSECONDS  (1000000u)
#define _1S_IN_NANOSECONDS   (1000000000u)


Logger::Logger()
    :m_file_id (~0u),
     m_must_die(false)
{
     ::sceKernelGetProcessTime(&m_start_time);
}

Logger::~Logger()
{
    /* Set the kill flag */
    m_must_die = true;

    /* Wake up the IO thread */
    if (m_logger_thread_event_ptr != nullptr)
    {
        m_logger_thread_event_ptr->set();
    }

    /* Wait till it dies and clean up */
    if (m_logger_thread_ptr != nullptr)
    {
        m_logger_thread_ptr->block_until_done(nullptr /* in_opt_timeout_in_microseconds_ptr */);
        m_logger_thread_ptr.reset            ();
    }

    m_logger_thread_event_ptr.reset();

    /* Should be safe to close the file handle at this point */
    if (m_file_id != ~0u)
    {
        ::sceIoClose(m_file_id);
    }
}

std::unique_ptr<Logger> Logger::create()
{
    std::unique_ptr<Logger> result_ptr;

    result_ptr.reset(
        new Logger()
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init() )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

int Logger::file_logger_thread_entrypoint(void* logger_raw_ptr)
{
    Logger*    logger_ptr = reinterpret_cast<Logger*>(logger_raw_ptr);
    const auto timeout    = _1S_IN_MICROSECONDS;

    /* 1. Create/reset existing log file */
    logger_ptr->m_file_id = ::sceIoOpen("ux0:data/elude.log",
                                        SCE_O_CREAT | SCE_O_WRONLY,
                                        0777); /* mode */

    if (logger_ptr->m_file_id < 0)
    {
        SCE_DBG_ASSERT(logger_ptr->m_file_id >= 0);

        goto end;
    }

    while (!logger_ptr->m_must_die)
    {
        logger_ptr->m_logger_thread_event_ptr->wait(true, /* in_should_reset_upon_successful_wait */
                                                   &timeout);

        {
            std::unique_lock<Mutex> lock            (*logger_ptr->m_pending_chunks_vec_mutex_ptr);
            const auto              n_pending_chunks( logger_ptr->m_pending_chunks_vec.size() );

            for (uint32_t n_pending_chunk = 0;
                          n_pending_chunk < n_pending_chunks;
                        ++n_pending_chunk)
            {
                const auto& current_pending_chunk = logger_ptr->m_pending_chunks_vec.at(n_pending_chunk);
                char        timestamp[32];
                const auto  time_delta            = current_pending_chunk.time.quad_t - logger_ptr->m_start_time.quad_t;
                const auto  time_delta_s          = static_cast<uint32_t>((time_delta / _1S_IN_MICROSECONDS) );
                const auto  time_delta_ms         = static_cast<uint32_t>((time_delta % _1S_IN_MICROSECONDS) / _1MS_IN_MICROSECONDS) % 1000 /* ms in 1s */;

                snprintf(timestamp,
                         sizeof(timestamp),
                         "%02d.%03d: ",
                         time_delta_s,
                         time_delta_ms);

                ::sceIoWrite(logger_ptr->m_file_id,
                             timestamp,
                             strlen(timestamp) );
                ::sceIoWrite(logger_ptr->m_file_id,
                             current_pending_chunk.string.data(),
                             static_cast<SceSize>(current_pending_chunk.string.size() ));
            }

            logger_ptr->m_pending_chunks_vec.clear();
        }
    }

end:
    return 0;
}

bool Logger::init()
{
    bool result = false;

    /* 1. Spawn helper objects */
    m_logger_thread_event_ptr = Event::create("Logger IO wake-up event",
                                              false,  /* in_support_multiple_simultaneous_waits */
                                              false); /* in_create_set                          */

    if (m_logger_thread_event_ptr == nullptr)
    {
        SCE_DBG_ASSERT(m_logger_thread_event_ptr != nullptr);

        goto end;
    }

    m_pending_chunks_vec_mutex_ptr = Mutex::create("Logger IO helper mutex",
                                                   0); /* in_start_value */

    if (m_pending_chunks_vec_mutex_ptr == nullptr)
    {
        SCE_DBG_ASSERT(m_pending_chunks_vec_mutex_ptr != nullptr);

        goto end;
    }

    /* 2. Spawn the IO thread */
    m_logger_thread_ptr = Thread::create_and_start("Logger IO thread",
                                                  &Logger::file_logger_thread_entrypoint,
                                                   this,
                                                   nullptr, /* in_opt_stack_size_ptr */
                                                   0xFF);   /* in_cpu_affinity_mask  */

    if (m_logger_thread_ptr == nullptr)
    {
        SCE_DBG_ASSERT(m_logger_thread_ptr != nullptr);

        goto end;
    }

    result = true;
end:
    return result;
}

void Logger::log(const char* in_format,
                 ...)
{
    char    buffer[1024];
    int     n_result_bytes = 0;
    va_list list;

    memset(buffer,
           0,
           sizeof(buffer) );

    va_start(list,
             in_format);
    {
        n_result_bytes = vsnprintf(buffer,
                                   sizeof(buffer),
                                   in_format,
                                   list);
    }
    va_end(list);

    /* NOTE: We deliberately NOT wake up the file logger thread here, we don't want to put too much pressure on
     *       the available CPU cores.
     */
    {
        std::unique_lock<Mutex> lock    (*m_pending_chunks_vec_mutex_ptr);
        SceKernelSysClock       log_time;

        ::sceKernelGetProcessTime(&log_time);

        m_pending_chunks_vec.push_back(
            Chunk(std::string(buffer,
                              n_result_bytes),
                  log_time)
        );
    }
}
