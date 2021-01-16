#ifndef EVENT_H
#define EVENT_H

extern "C"
{
    #include <psp2/types.h>
}

#include <memory>

class Event
{
public:
    /* Public functions */
    ~Event();

    static std::unique_ptr<Event> create(const char* in_opt_name_ptr,
                                         const bool& in_support_multiple_simultaneous_waits,
                                         const bool& in_create_set);

    bool reset();
    bool set  ();
    bool wait (const bool&      in_should_reset_upon_successful_wait,
               const SceUInt32* in_opt_timeout_in_microseconds_ptr);

private:
    /* Private functions */
    Event();

    bool init(const char* in_opt_name_ptr,
              const bool& in_support_multiple_simultaneous_waits,
              const bool& in_create_set);

    /* Private variables */
    SceUID m_event_id;
};

#endif /* EVENT_H */