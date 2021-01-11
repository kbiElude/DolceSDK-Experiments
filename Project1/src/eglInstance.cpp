extern "C"
{
    #include <psp2/libdbg.h>
}

#include <sstream>
#include <vector>

#include "eglInstance.h"
#include "io.h"

EGLInstance::EGLInstance()
    :m_display       (nullptr),
     m_egl_config_ptr(nullptr),
     m_egl_context   (nullptr),
     m_egl_surface   (nullptr)
{
    /* Stub */
}

EGLInstance::~EGLInstance()
{
    if (m_egl_context != nullptr)
    {
        ::eglDestroyContext(m_display,
                            m_egl_context);
    }

    if (m_egl_surface != nullptr)
    {
        ::eglDestroySurface(m_display,
                            m_egl_surface);
    }
}

bool EGLInstance::bind_to_current_thread()
{
    bool result = false;

    result = ::eglMakeCurrent(m_display,
                              m_egl_surface,
                              m_egl_surface,
                              m_egl_context) == EGL_TRUE;

    SCE_DBG_ASSERT(result);
    return result;
}

std::unique_ptr<EGLInstance> EGLInstance::create(const bool& in_require_depth_buffer,
                                                 const bool& in_require_stencil_buffer)
{
    std::unique_ptr<EGLInstance> result_ptr;

    result_ptr.reset(
        new EGLInstance()
    );

    SCE_DBG_ASSERT(result_ptr != nullptr);
    if (result_ptr != nullptr)
    {
        if (!result_ptr->init(in_require_depth_buffer,
                              in_require_stencil_buffer) )
        {
            SCE_DBG_ASSERT(false);

            result_ptr.reset();
        }
    }

    return result_ptr;
}

bool EGLInstance::init(const bool& in_require_depth_buffer,
                       const bool& in_require_stencil_buffer)
{
    EGLBoolean result;

    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    SCE_DBG_ASSERT(m_display != EGL_NO_DISPLAY);

    result = eglInitialize(m_display,
                           nullptr,  /* major */
                           nullptr); /* minor */
    SCE_DBG_ASSERT(result == EGL_TRUE);

    /* Enumerate available EGL configs */
    {
        std::vector<EGLConfig> egl_config_vec;
        EGLint                 n_egl_configs   = 0;

        result = eglGetConfigs(m_display,
                               nullptr, /* configs     */
                               0,       /* config_size */
                              &n_egl_configs);
        SCE_DBG_ASSERT(result == EGL_TRUE);

        egl_config_vec.resize  (n_egl_configs);
        m_egl_config_vec.resize(n_egl_configs);

        result = eglGetConfigs(m_display,
                               egl_config_vec.data(),
                               n_egl_configs,
                              &n_egl_configs);
        SCE_DBG_ASSERT(result == EGL_TRUE);

        for (uint32_t n_egl_config = 0;
                      n_egl_config < n_egl_configs;
                    ++n_egl_config)
        {
            const auto current_egl_config       = egl_config_vec.at  (n_egl_config);
            auto&      current_egl_config_props = m_egl_config_vec.at(n_egl_config);

            current_egl_config_props.egl_config = current_egl_config;

            const struct
            {
                EGLint  attribute;
                EGLint* result_ptr;
            } config_attribs[] =
            {
                {EGL_ALPHA_SIZE,         &current_egl_config_props.n_alpha_bits},
                {EGL_BLUE_SIZE,          &current_egl_config_props.n_blue_bits},
                {EGL_CONFIG_ID,          &current_egl_config_props.egl_config_id},
                {EGL_DEPTH_SIZE,         &current_egl_config_props.n_depth_bits},
                {EGL_GREEN_SIZE,         &current_egl_config_props.n_green_bits},
                {EGL_RED_SIZE,           &current_egl_config_props.n_red_bits},
                {EGL_STENCIL_SIZE,       &current_egl_config_props.n_stencil_bits}
            };

            for (const auto& current_config_attrib : config_attribs)
            {
                result = eglGetConfigAttrib(m_display,
                                            current_egl_config,
                                            current_config_attrib.attribute,
                                            current_config_attrib.result_ptr);

                SCE_DBG_ASSERT(result == EGL_TRUE);
            }
        }
    }

    /* On 3.60, we are reported 3 different configs, the only difference between the three being presence (or lack)
     * of depth and/or stencil buffer.
     *
     * Pick the right EGLConfig instance, depending on the input arguments.
     **/
    {
        uint32_t best_score = 0xFFFFFFFFu;

        for (const auto& current_egl_config : m_egl_config_vec)
        {
            uint32_t score = 0;

            if (( in_require_depth_buffer && current_egl_config.n_depth_bits != 0) ||
                (!in_require_depth_buffer && current_egl_config.n_depth_bits == 0) )
            {
                ++score;
            }

            if (( in_require_stencil_buffer && current_egl_config.n_stencil_bits != 0) ||
                (!in_require_stencil_buffer && current_egl_config.n_stencil_bits == 0) )
            {
                ++score;
            }

            if ((best_score == 0xFFFFFFu) ||
                (best_score <  score) )
            {
                best_score       = score;
                m_egl_config_ptr = &current_egl_config;
            }
        }
    }

    SCE_DBG_ASSERT(m_egl_config_ptr != nullptr);

    /* Create an ES context. */
    {
        static const EGLint attrib_list[] =
        {
            EGL_CONTEXT_MAJOR_VERSION, 2,
            EGL_CONTEXT_MINOR_VERSION, 0,
            EGL_NONE
        };

        m_egl_context = eglCreateContext(m_display,
                                         m_egl_config_ptr->egl_config,
                                         EGL_NO_CONTEXT, /* share_context */
                                         attrib_list);

        SCE_DBG_ASSERT(m_egl_context != EGL_NO_CONTEXT);
    }

    /* Create a rendering surface. */
    m_egl_surface = eglCreateWindowSurface(m_display,
                                           m_egl_config_ptr->egl_config,
                                           VITA_WINDOW_960X544,
                                           nullptr); /* attrib_list */
    SCE_DBG_ASSERT(m_egl_surface != EGL_NO_SURFACE);

    /* NOTE: Do not bind the context to the calling thread. It is caller's responsibility to invoke bind()
     *       from the right thread later on.
     */
    return (m_egl_context != nullptr &&
            m_egl_surface != nullptr);
}

