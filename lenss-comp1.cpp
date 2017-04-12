//-------------------------------------------------------------
// CSCE 438: Distributed objects programming
// HW4: A Scalable and Highly Available Twitter Service
// 
// lenss-comp1
// Vincent Velarde, Kyle Rowland
// March 2017
//
// NOTES:
// 	- THIS IS THE MASTER PROCESS!!
//	- RUN THIS ON "lenss-comp1.cse.tamu.edu"
//
//-------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <cstdint>
#include <grpc++/grpc++.h>
#include <sys/types.h>
#include <unistd.h>

#include "fb.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using HW2::UserBase;
using HW2::User;
using HW2::Action;
using HW2::SendMsg;
using HW2::Tweeter;
using std::chrono::system_clock;

//this class handles the RPCs
class masterServer final : public Tweeter::Service {
	UserBase userList;
	
	//Welcome RPC
	//accepts new client connections and assigns them to workers
	Status Welcome(ServerContext* context, const HW2::User* user,
					SendMsg* welcome) override {
		/*
		
		ASSIGN NEW CLIENTS TO WORKERS
		
		*/
		
		welcome->set_message("server message (worker assignment)");
		
		return Status::OK;
	}
};

//setup and run the server
void runServer(std::string p) {
	//server address and port number from command line argument
	std::string server_address("0.0.0.0:");
	server_address += p;
	
	masterServer service;
	
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	service.getUserList();
	
	server->Wait();
}

//sent child process here to wait for parent death
void holdingPattern() {
	while(getppid() > 1) {}	//while parent lives, do nothing
	
	//if parent dies
	pid_t pid;
	pid = fork();	//create new child process
	if(pid == 0)
		holdingPattern();	//hold until new parent dies
	else return;	//take over as parent
}

int main(int argc, char** argv) {
	std::string port = "1876";
	
	pid_t pid;
	pid = fork();
	if(pid == 0)	//child process
		holdingPattern();	//hold until parent dies
	
	runServer(port);
	
	return 0;
}
