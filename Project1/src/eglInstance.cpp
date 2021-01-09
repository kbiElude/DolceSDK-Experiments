extern "C"
{
    #include <psp2/libdbg.h>
}

#include <sstream>
#include <vector>

#include "eglInstance.h"
#include "io.h"

EGLInstance::EGLInstance()
    :m_display(nullptr)
{
    /* Stub */
}

EGLInstance::~EGLInstance()
{
    /* Stub */
}

std::unique_ptr<EGLInstance> EGLInstance::create()
{
    std::unique_ptr<EGLInstance> result_ptr;

    result_ptr.reset(
        new EGLInstance()
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

bool EGLInstance::init()
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
                {EGL_MAX_PBUFFER_HEIGHT, &current_egl_config_props.max_pbuffer_height},
                {EGL_MAX_PBUFFER_WIDTH,  &current_egl_config_props.max_pbuffer_width},
                {EGL_MAX_SWAP_INTERVAL,  &current_egl_config_props.max_swap_interval},
                {EGL_MIN_SWAP_INTERVAL,  &current_egl_config_props.min_swap_interval},
                {EGL_NATIVE_RENDERABLE,  &current_egl_config_props.native_renderable},
                {EGL_RED_SIZE,           &current_egl_config_props.n_red_bits},
                {EGL_SAMPLES,            &current_egl_config_props.n_samples},
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

    {
        std::stringstream result_sstream;

        for (auto n_egl_config = 0;
                  n_egl_config < m_egl_config_vec.size();
                ++n_egl_config)
        {
            const auto& current_egl_config = m_egl_config_vec.at(n_egl_config);

            result_sstream << "EGL config [" << n_egl_config << "]\n"
                              "\n"
                              "Alpha size:         " << current_egl_config.n_alpha_bits       << "\n"
                              "Blue size:          " << current_egl_config.n_blue_bits        << "\n"
                              "Config ID:          " << current_egl_config.egl_config_id      << "\n"
                              "Depth size:         " << current_egl_config.n_depth_bits       << "\n"
                              "Green size:         " << current_egl_config.n_green_bits       << "\n"
                              "Max pbuffer height: " << current_egl_config.max_pbuffer_height << "\n"
                              "Max pbuffer width:  " << current_egl_config.max_pbuffer_width  << "\n"
                              "Max swap interval:  " << current_egl_config.max_swap_interval  << "\n"
                              "Min swap interval:  " << current_egl_config.min_swap_interval  << "\n"
                              "Native renderable:  " << current_egl_config.native_renderable  << "\n"
                              "Red size:           " << current_egl_config.n_red_bits         << "\n"
                              "Samples:            " << current_egl_config.n_samples          << "\n"
                              "Stencil size:       " << current_egl_config.n_stencil_bits     << "\n"
                              "\n"
                              "---\n";
        }

        IO::write_file("ux0:data/log.txt",
                       result_sstream.str().c_str(),
                       static_cast<uint32_t>(result_sstream.str().size() ));
    }

    return true;
}

