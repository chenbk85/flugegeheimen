#include "stdafx.h"
#include "kernel/kernel.h"

int main (int argc, char ** argv) {
	Flug::Kernel kern;
	kern.loadConfig ("res/config.xml");
	kern.main();
	return 0;
}
