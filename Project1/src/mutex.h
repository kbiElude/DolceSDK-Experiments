#ifndef MUTEX_H
#define MUTEX_H

#include <psp2/types.h>
#include <memory>

class Mutex
{
public:
    static std::unique_ptr<Mutex> create(const char* in_name_ptr,
                                         const int&  in_start_value);

    ~Mutex();

    bool lock    (const SceUInt32* in_opt_timeout_in_microseconds_ptr = nullptr);
    bool try_lock();

    bool unlock();

private:
    /* Private functions */
    Mutex();

    bool init(const char* in_name_ptr,
              const int&  in_start_value);

    /* Private variables */
    SceUID m_mutex_id;
};

#endif /* MUTEX_H */