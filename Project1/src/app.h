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
    SceUID m_sce_piglet_id;
    SceUID m_shacc_cg_id;

    std::unique_ptr<EGLInstance> m_egl_instance_ptr;
};

#endif /* APP_H */