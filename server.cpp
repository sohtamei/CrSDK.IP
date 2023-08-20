#include "WebsocketServer.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <asio/io_service.hpp>

//The port number the WebSocket server listens on
#define PORT_NUMBER 8080

int remoteCli_init(void);
void setup_camera_props(void);

void af_shutter(void);
int32_t get_live_view(uint8_t* buf[]);
int32_t SetSelectDeviceProperty(uint32_t setCode, uint32_t setData);
int32_t GetSelectDeviceProperty(uint32_t getCode, uint32_t& getData, uint32_t& writable);

WebsocketServer server;
ClientConnection conn_;

int main(int argc, char* argv[])
{
	int ret = remoteCli_init();
	if(ret) return -1;

	//Create the event loop for the main thread, and the WebSocket server
	asio::io_service mainEventLoop;
	
	//Register our network callbacks, ensuring the logic is run on the main thread's event loop
	server.connect([&mainEventLoop](ClientConnection conn)
	{
		conn_ = conn;
		mainEventLoop.post([conn]()
		{
			std::clog << "Connection opened." << std::endl;
			std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;

			setup_camera_props();
		//	_main();
		});
	});

	server.disconnect([&mainEventLoop](ClientConnection conn)
	{
		mainEventLoop.post([conn]()
		{
			std::clog << "Connection closed." << std::endl;
			std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
		});
	});

	server.message([&mainEventLoop](ClientConnection conn, const Json::Value& args)
	{
		mainEventLoop.post([conn, args]()
		{
			std::clog << "Message payload:" << server.stringifyJson(args) << std::endl;
		/*
			std::clog << "message handler on the main thread" << std::endl;
			std::clog << "Message payload:" << std::endl;
			for (auto key : args.getMemberNames()) {
				std::clog << "\t" << key << ": " << args[key].asString() << std::endl;
			}
		*/
			if(args.isMember("cmd")) {
				switch(args["cmd"].asUInt()) {
				case 1:
					af_shutter();
					break;
				case 2: {
					uint8_t* imageBuf = NULL;
					int imageSize = get_live_view(&imageBuf);
					if(imageSize > 0) {
						server.send(conn, imageBuf, imageSize);
						delete[] imageBuf;
						return;
					}
					break;
				}
				default:
					break;
				}
			} else if(args.isMember("getCode")) {
				uint32_t data = 0;
				uint32_t writable = 0;
				GetSelectDeviceProperty(args["getCode"].asUInt(), data, writable);

				Json::Value getResult;
				getResult["getData"] = data;
				getResult["writable"] = writable;
				std::clog << "Message payload:" << server.stringifyJson(getResult) << std::endl;
				server.sendMessage(conn, getResult);
				return;

			} else if(args.isMember("setCode")) {
				int ret = 0;//SetSelectDeviceProperty(args["setCode"].asUInt(), args["setData"].asUInt());

				Json::Value getResult;
				getResult["result"] = ret;
				std::clog << "Message payload:" << server.stringifyJson(getResult) << std::endl;
				server.sendMessage(conn, getResult);
				return;
			}
			//Echo the message pack to the client
			server.sendMessage(conn, args);
		});
	});
	
	//Start the networking thread
	std::thread serverThread([]() {
		server.run(PORT_NUMBER);
	});
	
	//Start a keyboard input thread that reads from stdin
	std::thread inputThread([&mainEventLoop]()
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

void uploadFile(const wchar_t* filename)
{
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	if (!ifs) return;

	ifs.seekg(0, std::ios::end);
	int filesize = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::ios::beg);

	uint8_t* filebuf = new uint8_t[filesize];
	if(!filebuf) {
		ifs.close();
		return;
	}
	ifs.read((char*)filebuf, filesize);
	ifs.close();

	server.send(conn_, filebuf, filesize);
	delete[] filebuf;
	return;
}
