#include "WebsocketServer.h"

#include <iostream>
#include <thread>
#include <asio/io_service.hpp>

//The port number the WebSocket server listens on
#define PORT_NUMBER 8080

int remoteCli_init(void);
void af_shutter(void);

int main(int argc, char* argv[])
{
	int ret = remoteCli_init();
	if(ret) return -1;

	//Create the event loop for the main thread, and the WebSocket server
	asio::io_service mainEventLoop;
	WebsocketServer server;
	ClientConnection conn_;
	
	//Register our network callbacks, ensuring the logic is run on the main thread's event loop
	server.connect([&mainEventLoop, &server, &conn_](ClientConnection conn)
	{
		conn_ = conn;
		mainEventLoop.post([conn, &server]()
		{
			std::clog << "Connection opened." << std::endl;
			std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;

		//	_main();
		});
	});

	server.disconnect([&mainEventLoop, &server](ClientConnection conn)
	{
		mainEventLoop.post([conn, &server]()
		{
			std::clog << "Connection closed." << std::endl;
			std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
		});
	});

	server.message([&mainEventLoop, &server](ClientConnection conn, const Json::Value& args)
	{
		mainEventLoop.post([conn, args, &server]()
		{
			std::clog << "message handler on the main thread" << std::endl;
			std::clog << "Message payload:" << std::endl;
			for (auto key : args.getMemberNames()) {
				std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
			}
			af_shutter();
			
			//Echo the message pack to the client
			server.sendMessage(conn, args);
		});
	});
	
	//Start the networking thread
	std::thread serverThread([&server]() {
		server.run(PORT_NUMBER);
	});
	
	//Start a keyboard input thread that reads from stdin
	std::thread inputThread([&mainEventLoop, &server, &conn_]()
	{
		string input;
		while (1)
		{
			//Read user input from stdin
			std::getline(std::cin, input);
			
			//Broadcast the input to all connected clients (is sent on the network thread)
			Json::Value payload;
			payload["input"] = input;
			server.sendMessage(conn_, payload);
			
			//Debug output on the main thread
			mainEventLoop.post([]() {
				std::clog << "User input debug output on the main thread" << std::endl;
			});
		}
	});
	
	//Start the event loop for the main thread
	asio::io_service::work work(mainEventLoop);
	mainEventLoop.run();
	
	return 0;
}
