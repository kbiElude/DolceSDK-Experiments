extern "C"
{
    #include <GLES2/gl2.h>
    #include <psp2/kernel/modulemgr.h>
    #include <psp2/kernel/processmgr.h>
    #include <psp2/ctrl.h>
    #include <psp2/libdbg.h>
    #include <psp2/shacccg.h>
}

#include "baseApp.h"
#include "logger.h"
#include <cstdlib>

BaseApp::BaseApp()
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

    ::sceShaccCgSetMemAllocator(malloc,
                                free);

    /* Set up the logger */
    m_logger_ptr = Logger::create();
    SCE_DBG_ASSERT(m_logger_ptr != nullptr);

    m_logger_ptr->log(false, /* in_flush_and_wait */
                      "Base app initialized.\n");
}

BaseApp::~BaseApp()
{
    m_logger_ptr->log(false, /* in_flush_and_wait */
                      "Base app terminating.\n");

    m_logger_ptr.reset();

    ::sceKernelExitProcess(0);
}

int BaseApp::getResult()
{
    return 0;
}