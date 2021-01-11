#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <psp2/types.h>
#include <memory>

class Semaphore
{
public:
    static std::unique_ptr<Semaphore> create(const char*     in_name_ptr,
                                             const SceInt32& in_start_value,
                                             const SceInt32& in_max_value);

    ~Semaphore();

    int  peek  ()                                                    const;
    bool signal();
    bool wait  (const int&       in_value,
                const SceUInt32* in_opt_timeout_in_microseconds_ptr);

private:
    /* Private functions */
    Semaphore();

    bool init(const char* in_name_ptr,
              const int&  in_start_value,
              const int&  in_max_value);

    /* Private variables */
    SceUID m_sem_id;
};

#endif /* SEMAPHORE_H */