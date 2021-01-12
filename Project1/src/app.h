#ifndef APP_H
#define APP_H

#include "baseApp.h"
#include "eglInstance.h"

#include <memory>

class App : public BaseApp
{
public:
    App();

    void run() final;

private:
    /* Private functions */
    static int rendering_thread_entrypoint(void* app_raw_ptr);

    /* Private variables */
    std::unique_ptr<EGLInstance> m_egl_instance_ptr;

    volatile bool m_must_die;
};

#endif /* APP_H */