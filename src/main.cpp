#include "stdafx.h"

#include "kernel/kernel.h"
#include "kernel/request_builder.h"
#include "kernel/response_parser.h"
#include "helpers/socket.h"
#include "helpers/xml.h"
#include "devices/timer.h"
#include "devices/adc.h"
#include "timer.h"


int main(int argc, char **argv) {

	if (argc < 2) {
		Flug::Kernel kern;
		kern.loadConfig("res/config.xml");
		kern.main();
	}

	return 0;
}
