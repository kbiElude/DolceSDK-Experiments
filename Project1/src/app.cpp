extern "C"
{
    #include <GLES2/gl2.h>
    #include <psp2/kernel/modulemgr.h>
    #include <psp2/kernel/processmgr.h>
    #include <psp2/ctrl.h>
    #include <psp2/libdbg.h>
    #include <psp2/shacccg.h>
}

#include <functional>

#include "app.h"
#include "eglInstance.h"
#include "thread.h"

App::App()
    :m_must_die     (false),
     m_sce_piglet_id(UINT32_MAX),
     m_shacc_cg_id  (UINT32_MAX)
{
    /* Set up EGL & GLES2 support */
    m_sce_piglet_id = sceKernelLoadStartModule("app0:/module/libScePiglet.suprx",
                                               0,        /* args   */
                                               nullptr,  /* argp   */
                                               0,        /* flags  */
                                               nullptr,  /* option */
                                               nullptr); /* status */

    m_shacc_cg_id   = sceKernelLoadStartModule("app0:/module/libshacccg.suprx",
                                               0,        /* args   */
                                               nullptr,  /* argp   */
                                               0,        /* flags  */
                                               nullptr,  /* option */
                                               nullptr); /* status */

    SCE_DBG_ASSERT(m_sce_piglet_id != 0);
    SCE_DBG_ASSERT(m_shacc_cg_id   != 0);

    sceShaccCgSetMemAllocator(malloc,
                              free);
}

App::~App()
{
    sceKernelDelayThread(2 * 1000000);
    sceKernelExitProcess(0);
}

int App::getResult()
{
    return 0;
}

int App::rendering_thread_entrypoint(void* app_raw_ptr)
{
    App*     app_ptr           = reinterpret_cast<App*>(app_raw_ptr);
    uint32_t n_frames_rendered = 0;

    app_ptr->m_egl_instance_ptr->bind_to_current_thread();

    while (!app_ptr->m_must_die)
    {
        const float intensity = static_cast<float>(n_frames_rendered % 256) / 255.0f;

        ::glClearColor(intensity, intensity, intensity, 1.0f);
        ::glClear     (GL_COLOR_BUFFER_BIT);

        app_ptr->m_egl_instance_ptr->swap_buffers();

        n_frames_rendered++;
    }

    return 0;
}

void App::run()
{
    /* Set up an ES context */
    m_egl_instance_ptr = EGLInstance::create(false,  /* in_require_depth_buffer   */
                                             false); /* in_require_stencil_buffer */

    SCE_DBG_ASSERT(m_egl_instance_ptr != nullptr);

    /* Spawn a separate renderer thread */
    auto renderer_thread_ptr = Thread::create_and_start("Renderer thread",
                                                       &App::rendering_thread_entrypoint,
                                                        this,
                                                        ThreadPriority::DEFAULT,
                                                        nullptr, /* in_opt_stack_size_ptr */
                                                        0xFF);

    /* Monitor control buttons.. */
    ::sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITAL);

    while (!m_must_die)
    {
        SceCtrlData control_data;

        ::sceCtrlPeekBufferPositive(0, /* port */
                                   &control_data,
                                    1 /* count */);

        if (control_data.buttons != 0)
        {
            m_must_die = true;
        }
    }

    /* Wait for the renderer thread to die */
    renderer_thread_ptr->block_until_done(nullptr);
    renderer_thread_ptr.reset            ();

    /* Done */
}