#ifndef EGL_INSTANCE_H
#define EGL_INSTANCE_H

extern "C"
{
    #include <EGL/egl.h>
}

#include <memory>
#include <vector>

class EGLInstance
{
public:
    ~EGLInstance();

    static std::unique_ptr<EGLInstance> create();

private:
    /* Private functions */
    EGLInstance();

    bool init();

    /* Private type definitions */
    struct EGLConfigProps
    {
        EGLConfig egl_config;
        EGLint    egl_config_id;
        EGLint    max_pbuffer_height;
        EGLint    max_pbuffer_width;
        EGLint    max_swap_interval;
        EGLint    min_swap_interval;
        EGLint    n_alpha_bits;
        EGLint    n_blue_bits;
        EGLint    n_depth_bits;
        EGLint    n_green_bits;
        EGLint    n_red_bits;
        EGLint    n_samples;
        EGLint    n_stencil_bits;
        EGLint    native_renderable;

        EGLConfigProps()
        {
            egl_config         = nullptr;
            egl_config_id      = 0;
            max_pbuffer_height = 0;
            max_pbuffer_width  = 0;
            max_swap_interval  = 0;
            min_swap_interval  = 0;
            n_alpha_bits       = 0;
            n_blue_bits        = 0;
            n_depth_bits       = 0;
            n_green_bits       = 0;
            n_red_bits         = 0;
            n_samples          = 0;
            n_stencil_bits     = 0;
            native_renderable  = 0;
        }
    };

    /* Private variables */
    EGLDisplay                  m_display;
    std::vector<EGLConfigProps> m_egl_config_vec;
};

#endif /* EGL_INSTANCE_H */