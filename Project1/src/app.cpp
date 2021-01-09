#include <psp2/kernel/processmgr.h>

#include "app.h"
#include "debugScreen.h"

App::App()
{
    /* Stub */
}

App::~App()
{
    sceKernelExitProcess(0);
}

int App::getResult()
{
    return 0;
}

void App::run()
{
    psvDebugScreenInit();

    psvDebugScreenPrintf("I am a test\n");

    sceKernelDelayThread(3 * 1000000);
}