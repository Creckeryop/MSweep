#include <psp2/kernel/processmgr.h>
#include <iostream>
#include <time.h>
#include "game.h"

int main() {
	srand(time(NULL));
	game za;
	za.run();
	sceKernelExitProcess(0);
	return 0;
}