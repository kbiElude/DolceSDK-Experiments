#ifndef EGL_INSTANCE_H
#define EGL_INSTANCE_H

extern "C"
{
    /* This hack is needed for VS Code's Intellisense to correctly parse EGL Platform header.. */
    #if defined(__vita__)
        #undef __unix__
    #endif

    #include <EGL/egl.h>
}

#include <memory>
#include <vector>

class EGLInstance
{
public:
    ~EGLInstance();

    static std::unique_ptr<EGLInstance> create(const bool& in_require_depth_buffer,
                                               const bool& in_require_stencil_buffer);

private:
    /* Private functions */
    EGLInstance();

    bool init(const bool& in_require_depth_buffer,
              const bool& in_require_stencil_buffer);

    /* Private type definitions */
    struct EGLConfigProps
    {
        EGLConfig egl_config;
        EGLint    egl_config_id;
        EGLint    n_alpha_bits;
        EGLint    n_blue_bits;
        EGLint    n_depth_bits;
        EGLint    n_green_bits;
        EGLint    n_red_bits;
        EGLint    n_stencil_bits;

        EGLConfigProps()
        {
            egl_config         = nullptr;
            egl_config_id      = 0;
            n_alpha_bits       = 0;
            n_blue_bits        = 0;
            n_depth_bits       = 0;
            n_green_bits       = 0;
            n_red_bits         = 0;
            n_stencil_bits     = 0;
        }
    };

    /* Private variables */
    EGLDisplay                  m_display;
    const EGLConfigProps*       m_egl_config_ptr;
    std::vector<EGLConfigProps> m_egl_config_vec;
    EGLContext                  m_egl_context;
    EGLSurface                  m_egl_surface;
};

#endif /* EGL_INSTANCE_H */