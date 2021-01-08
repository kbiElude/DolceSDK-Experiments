#include <psp2/kernel/processmgr.h>
#include "debugScreen.h"

int main(int argc, char *argv[])
{
	psvDebugScreenInit();

	psvDebugScreenPrintf("I am a test\n");

    sceKernelDelayThread(3 * 1000000);
	sceKernelExitProcess(0);

    return 0;
}
