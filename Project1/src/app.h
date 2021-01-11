#ifndef APP_H
#define APP_H

extern "C"
{
    #include <psp2/types.h>
}

#include <memory>

class EGLInstance;

class App
{
public:
     App();
    ~App();

    int  getResult();
    void run      ();

private:
    /* Private functions */
    static int rendering_thread_entrypoint(void* app_raw_ptr);

    /* Private variables */
    SceUID m_sce_piglet_id;
    SceUID m_shacc_cg_id;

    std::unique_ptr<EGLInstance> m_egl_instance_ptr;

    volatile bool m_must_die;
};

#endif /* APP_H */