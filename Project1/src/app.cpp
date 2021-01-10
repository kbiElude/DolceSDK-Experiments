extern "C"
{
    #include <psp2/kernel/modulemgr.h>
    #include <psp2/kernel/processmgr.h>
    #include <psp2/libdbg.h>
    #include <psp2/shacccg.h>
}

#include "app.h"
#include "eglInstance.h"

App::App()
    :m_sce_piglet_id(UINT32_MAX),
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

void App::run()
{
    /* Set up an ES context */
    m_egl_instance_ptr = EGLInstance::create(false,  /* in_require_depth_buffer   */
                                             false); /* in_require_stencil_buffer */

    SCE_DBG_ASSERT(m_egl_instance_ptr != nullptr);
}