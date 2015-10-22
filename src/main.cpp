#include "stdafx.h"

#include "kernel/kernel.h"
#include "kernel/request_builder.h"
#include "kernel/response_parser.h"
#include "helpers/socket.h"


int main(int argc, char **argv) {

	if (argc < 2) {
		Flug::Kernel kern;
		kern.initialize("res/config.json");
		kern.main();
	}

	return 0;
}
