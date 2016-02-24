#include "stdafx.h"

#include "kernel/Kernel.h"
#include "kernel/request_builder.h"
#include "kernel/response_parser.h"
#include "helpers/socket.h"


int main(int argc, char **argv) {

    if (argc > 1 && argv[1] == std::string("--daemon")) {
        int err = daemon (/*No chdir*/1, /*No close*/0);
        if (err) {
            throw std::runtime_error("Failed to go daemon");
        }
    }

	if (argc < 2) {
		Flug::Kernel kern;
		kern.initialize("res/config.json");
		kern.main();
	}

	return 0;
}

