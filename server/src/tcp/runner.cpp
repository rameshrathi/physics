//
//  runner.cpp
//  SysHealth
//
//  Created by Ramesh Kumar on 07/01/25.
//


#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <algorithm>
#include <set>
#include <string>

#include "include/client.hpp"
#include "include/runner.hpp"

// -----------------------------------------------------
// -----------------------------------------------------
int run_client ( const char * host, int port ) {
	
	Client cl(host, port);
	int status = cl.connect();

	if (status != 0) {
		return EXIT_FAILURE;
	}
	char msg[1024];
	msg[0] = 0;
	
	while (1) {
		std::string message;
		printf("\nInput Message > ");
		std::getline(std::cin, message);
		if (message == "exit") {
			break;
		}
		cl.send_message(message.c_str());
		
		// Wait for message
		cl.read_message_with_wait(msg, 1024);
		printf("Message : %s \n", msg);
		msg[0] = 0;
		sleep(1);
	}
	
	std::cout << "All clients stopped" << std::endl;
	return 0;
}


// -------------------------------------------------
// -------------------------------------------------
int run_server () {
	Server s;
	s.start();
	return 0;
}


// -------------------------------------------
// -------------------------------------------
int execute_command(std::string & command ) {
	if (command == "server") {
		return run_server();
	}
	else if (command == "client") {
		return run_client("127.0.0.1", 4433);
	}
	// return some error
	return 1;
}


// -----------------------------
// Convert char to lowercase
char to_lower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}


//  ------------------------------------
//  ------------------------------------
void run_task_loop() {
	std::string command;
	std::set<std::string> exit_commands = {
		"q", "quit", "exit"
	};
	while (true) {
		if (command.empty()) {
			printf("Input some command to execute > ");
			std::getline(std::cin, command);
		}
		std::transform(command.begin(), command.end(), command.begin(), to_lower);
		if (exit_commands.count(command) > 0 ) {
			break;
		}
		int status = execute_command(command);
		if (status == 0) {
			printf("Command executed: %s \n", command.c_str());
		}
		
		// clear command
		command.clear();
		
		// Sleep for 5 second and take another input
		sleep(2);
	}
}
