extern "C"
{
    #include <psp2/libdbg.h>
}

#include <sstream>
#include <vector>

#include "EGL/eglInstance.h"
#include "io.h"
#include "logger.h"

EGLInstance::EGLInstance(Logger* in_logger_ptr)
    :m_display          (nullptr),
     m_egl_config_ptr   (nullptr),
     m_egl_context      (nullptr),
     m_egl_surface      (nullptr),
     m_gl_extensions_ptr(nullptr),
     m_logger_ptr       (in_logger_ptr),
     m_never_bound      (true)
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

    if (m_never_bound)
    {
        /* Log base GL info & available GL extensions. */
        auto es_extensions_ptr = ::eglQueryString(m_display,
                                                  EGL_EXTENSIONS);
        auto renderer_ptr      = ::glGetString   (GL_RENDERER);
        auto vendor_ptr        = ::glGetString   (GL_VENDOR);
        auto version_ptr       = ::glGetString   (GL_VERSION);

        m_gl_extensions_ptr = reinterpret_cast<const char*>(::glGetString(GL_EXTENSIONS) );

        m_logger_ptr->log("Renderer version: %s\n"
                          "Renderer:         %s\n"
                          "Vendor:           %s\n",
                          version_ptr,
                          renderer_ptr,
                          vendor_ptr);
        m_logger_ptr->log("ES Extensions:    %s\n",
                          es_extensions_ptr);
        m_logger_ptr->log("GL Extensions:    %s\n",
                          m_gl_extensions_ptr);

        /* Init extension entrypoints */
        if (strstr(m_gl_extensions_ptr,
                   "GL_EXT_draw_instanced") != nullptr)
        {
            const ExtensionEntrypoint entrypoints[] =
            {
                {"glDrawArraysInstancedEXT",   reinterpret_cast<void**>(&m_entrypoints_gl_ext_draw_instanced.glDrawArraysInstancedEXT)},
                {"glDrawElementsInstancedEXT", reinterpret_cast<void**>(&m_entrypoints_gl_ext_draw_instanced.glDrawElementsInstancedEXT)},
            };

            if (!init_extension_entrypoints(entrypoints,
                                            sizeof(entrypoints) / sizeof(entrypoints[0])) )
            {
                SCE_DBG_ASSERT(false);

                result = false;
                goto end;
            }
        }

        if (strstr(m_gl_extensions_ptr,
                   "GL_EXT_instanced_arrays") != nullptr)
        {
            const ExtensionEntrypoint entrypoints[] =
            {
                {"glDrawArraysInstancedEXT",   reinterpret_cast<void**>(&m_entrypoints_gl_ext_instanced_arrays.glDrawArraysInstancedEXT)},
                {"glDrawElementsInstancedEXT", reinterpret_cast<void**>(&m_entrypoints_gl_ext_instanced_arrays.glDrawElementsInstancedEXT)},
                {"glVertexAttribDivisorEXT",   reinterpret_cast<void**>(&m_entrypoints_gl_ext_instanced_arrays.glVertexAttribDivisorEXT)},
            };

            if (!init_extension_entrypoints(entrypoints,
                                            sizeof(entrypoints) / sizeof(entrypoints[0])) )
            {
                SCE_DBG_ASSERT(false);

                result = false;
                goto end;
            }
        }

        if (strstr(m_gl_extensions_ptr,
                   "GL_EXT_texture_storage") != nullptr)
        {
            const ExtensionEntrypoint entrypoints[] =
            {
                {"glTexStorage2DEXT", reinterpret_cast<void**>(&m_entrypoints_gl_ext_texture_storage.glTexStorage2DEXT)},
            };

            if (!init_extension_entrypoints(entrypoints,
                                            sizeof(entrypoints) / sizeof(entrypoints[0])) )
            {
                SCE_DBG_ASSERT(false);

                result = false;
                goto end;
            }
        }

        if (strstr(m_gl_extensions_ptr,
                   "GL_SCE_piglet_shader_binary") != nullptr)
        {
            const ExtensionEntrypoint entrypoints[] =
            {
                {"glPigletGetShaderBinarySCE", reinterpret_cast<void**>(&m_entrypoints_gl_sce_piglet_shader_binary.glPigletGetShaderBinarySCE) },
            };

            if (!init_extension_entrypoints(entrypoints,
                                            sizeof(entrypoints) / sizeof(entrypoints[0])) )
            {
                SCE_DBG_ASSERT(false);

                result = false;
                goto end;
            }
        }

        if (strstr(m_gl_extensions_ptr,
                   "GL_SCE_texture_resource") != nullptr)
        {
            const ExtensionEntrypoint entrypoints[] =
            {
                {"glMapTextureResourceSCE",   reinterpret_cast<void**>(&m_entrypoints_gl_sce_texture_resource_entrypoints.glMapTextureResourceSCE)},
                {"glTexImageResourceSCE",     reinterpret_cast<void**>(&m_entrypoints_gl_sce_texture_resource_entrypoints.glTexImageResourceSCE)},
                {"glUnmapTextureResourceSCE", reinterpret_cast<void**>(&m_entrypoints_gl_sce_texture_resource_entrypoints.glUnmapTextureResourceSCE)},
            };

            if (!init_extension_entrypoints(entrypoints,
                                            sizeof(entrypoints) / sizeof(entrypoints[0])) )
            {
                SCE_DBG_ASSERT(false);

                result = false;
                goto end;
            }
        }

        /* Done */
        m_never_bound = false;
    }

    SCE_DBG_ASSERT(result);
end:
    return result;
}

std::unique_ptr<EGLInstance> EGLInstance::create(Logger*     in_logger_ptr,
                                                 const bool& in_require_depth_buffer,
                                                 const bool& in_require_stencil_buffer)
{
    std::unique_ptr<EGLInstance> result_ptr;

    result_ptr.reset(
        new EGLInstance(in_logger_ptr)
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

const EXTDrawInstancedEntrypoints* EGLInstance::get_ext_draw_instanced_entrypoints_ptr() const
{
    return &m_entrypoints_gl_ext_draw_instanced;
}

const EXTInstancedArraysEntrypoints* EGLInstance::get_ext_instanced_arrays_entrypoints_ptr() const
{
    return &m_entrypoints_gl_ext_instanced_arrays;
}

const EXTTextureStorageEntrypoints* EGLInstance::get_ext_texture_storage_entrypoints_ptr() const
{
    return &m_entrypoints_gl_ext_texture_storage;
}

const SCEPigletShaderBinaryEntrypoints* EGLInstance::get_sce_piglet_shader_binary_entrypoints_ptr() const
{
    return &m_entrypoints_gl_sce_piglet_shader_binary;
}

const SCETextureResourceEntrypoints* EGLInstance::get_sce_texture_resource_entrypoints_ptr() const
{
    return &m_entrypoints_gl_sce_texture_resource_entrypoints;
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

            if ((best_score == 0xFFFFFFFFu) ||
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

bool EGLInstance::init_extension_entrypoints(const ExtensionEntrypoint* in_ext_entrypoint_ptr,
                                             const uint32_t&            in_n_ext_entrypoints)
{
    bool result = false;

    for (uint32_t n_entrypoint = 0;
                  n_entrypoint < in_n_ext_entrypoints;
                ++n_entrypoint)
    {
        const auto& current_entrypoint = in_ext_entrypoint_ptr[n_entrypoint];

        *current_entrypoint.func_ptr_ptr = reinterpret_cast<void*>(::eglGetProcAddress(current_entrypoint.func_name_ptr) );

        if (*current_entrypoint.func_ptr_ptr == nullptr)
        {
            SCE_DBG_ASSERT(false);

            goto end;
        }
    }

    result = true;
end:
    return result;
}

void EGLInstance::swap_buffers()
{
    ::eglSwapBuffers(m_display,
                     m_egl_surface);
}