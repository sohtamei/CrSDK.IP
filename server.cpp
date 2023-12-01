#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boost/thread/thread_only.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <locale>
#include <codecvt>
#include <chrono>
#include <iomanip>
#include <mutex>

#include "app/CRSDK/CameraRemote_SDK.h"
#include "app/CameraDevice.h"
#include "app/PropertyValueTable.h"
#include "server.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace asio = boost::asio;           // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace pt = boost::property_tree;

extern std::shared_ptr<cli::CameraDevice> camera;

int remoteCli_init(void);

std::string address = "0.0.0.0";
unsigned short portWS = 8080;
unsigned short portHttp = 81;

websocket::stream<tcp::socket> *p_ws = NULL;
static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstring_convert;


std::mutex crsdk_mutex;
std::mutex ws_mutex;

void write_json(websocket::stream<tcp::socket>& ws, pt::ptree resp_tree)
{
	std::unique_lock<std::mutex> ws_lock(ws_mutex);		// ws mutex

	std::stringstream resp_stream;
	pt::write_json(resp_stream, resp_tree);
	beast::flat_buffer resp_buffer;
	boost::beast::ostream(resp_buffer) << resp_stream.str();
	std::clog << resp_stream.str() << std::endl;
	ws.text(true);
	ws.write(resp_buffer.data());
}

std::string convVal2Text(struct cli::PropertyValue* prop, std::uint64_t value)
{
	if(prop->mapEnum) {
		auto iter = prop->mapEnum->find(value);
		if(iter != end(*prop->mapEnum)) {
			return iter->second;
		}
	} else if(prop->formatFunc) {
		return prop->formatFunc(value);
	}
	return "";
}


void sendProp(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool sendInfo)
{
	struct cli::PropertyValue* prop = camera->GetProp(id);

	pt::ptree resp_tree;
	pt::ptree item_tree;

	resp_tree.put("code", prop->tag);

	std::string _text = convVal2Text(prop, prop->current);
	if(_text != "") item_tree.put("text", _text);
	item_tree.put("value", std::to_string(prop->current));
	resp_tree.add_child("current", item_tree);

	if(sendInfo) {
		pt::ptree sub_tree;
		if((prop->dataType & SCRSDK::CrDataType::CrDataType_ArrayBit) && prop->possible.size() >= 1) {
			for(int i = 0; i < prop->possible.size(); i++) {
				item_tree.clear();
				_text = convVal2Text(prop, prop->possible[i]);
				if(_text != "") item_tree.put("text", _text);
				item_tree.put("value", std::to_string(prop->possible[i]));
				sub_tree.push_back(std::make_pair("", item_tree));
			}
			resp_tree.add_child("list", sub_tree);

		} else if((prop->dataType & SCRSDK::CrDataType::CrDataType_RangeBit) && prop->possible.size() >= 2) {
			item_tree.clear();
			item_tree.put("min", std::to_string(prop->possible[0]));
			item_tree.put("max", std::to_string(prop->possible[1]));
			if(prop->possible.size() >= 3)
				item_tree.put("step", std::to_string(prop->possible[2]));
			resp_tree.add_child("range", item_tree);
		}
	}

	int index = -1;
	for(int i = 0; i < prop->possible.size(); i++) {
		if(prop->current == prop->possible[i]) index = i;
	}

	std::string incrementable = "none";
	if(prop->writable != 1)
		;
	else if(index == 0)
		incrementable = "inc";
	else if(index == prop->possible.size()-1)
		incrementable = "dec";
	else if(index > 0 && index < prop->possible.size()-1)
		incrementable = "incdec";
	resp_tree.put("incrementable", incrementable);

	write_json(ws, resp_tree);
}

void incProp(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool inc_dec)
{
	struct cli::PropertyValue* prop = camera->GetProp(id);

	if(prop->writable == 1) {
		for(int i = 0; i < prop->possible.size(); i++) {
			if(prop->current == prop->possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop->possible.size()-1) {
					camera->SetProp(id, prop->possible[index]);
				} else {
					sendProp(ws, id, false);
				}
				break;
			}
		}
	}
//	std::this_thread::sleep_for(std::chrono::milliseconds(300));	// ADHOC
//	sendProp(ws, id, false);
}

void SendProp(SCRSDK::CrDevicePropertyCode id)
{
	if(p_ws) {
		sendProp(*p_ws, id, false);
	}
}


void do_thread_ws(void)
{
	asio::io_context ioc;
	tcp::endpoint endpoint(asio::ip::make_address(address), portWS);
	tcp::acceptor acceptor(ioc, endpoint);

	beast::error_code err;

	while(true) {
		try {
			tcp::socket socket(ioc);
			acceptor.accept(socket);

			websocket::stream<tcp::socket> ws(std::move(socket));
			p_ws = &ws;

			// Set a decorator to change the Server of the handshake
			ws.set_option(websocket::stream_base::decorator(
				[](websocket::response_type& res)
				{
					res.set(http::field::server,
						std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-sync");
				}));
			ws.accept();

			std::clog << "Connected." << std::endl;

			while(true) {
				beast::flat_buffer buffer;
			//	beast::multi_buffer buffer;
				ws.read(buffer, err);
				if(err) break;

				if(ws.got_text()) {
					std::unique_lock<std::mutex> crsdk_lock(crsdk_mutex);		// CrSDK mutex

					std::cout << beast::buffers_to_string(buffer.data()) << std::endl;

					std::string cmd_data(beast::buffers_to_string(buffer.data()));
					std::istringstream cmd_is(cmd_data);
					pt::ptree cmd_tree;
					pt::read_json(cmd_is, cmd_tree);

					if(cmd_tree.get_child_optional("cmd")) {
						std::string cmd = cmd_tree.get<std::string>("cmd");
						std::cout << "Received JSON: " << cmd << std::endl;

						if(cmd == "getPropList") {
							std::vector<std::string> propList;
							camera->GetAvailablePropList(propList);
							pt::ptree resp_tree;
						//	for (const auto& iter : propList) {
						//		resp_tree.put(iter, "");
						//	}
							pt::ptree tmp1;
							for(auto& iter : propList) {
								pt::ptree tmp2;
								tmp2.put("", iter);
								tmp1.push_back(std::make_pair("", tmp2));
							}
							resp_tree.add_child("propList", tmp1);
							write_json(ws, resp_tree);
							continue;
						} else if(cmd == "afShutter") {
							uint32_t delay_ms = 500;
							if(cmd_tree.get_child_optional("delay"))
								delay_ms = cmd_tree.get<std::uint32_t>("delay");
							camera->af_shutter(delay_ms);

						} else if(cmd == "afHalfShutter") {
							camera->s1_shooting();

						} else if(cmd == "liveview") {
							uint8_t* imageBuf = NULL;
							int imageSize = camera->get_live_view(&imageBuf);
							if(imageSize > 0) {
							//	server.send(conn, imageBuf, imageSize);
								std::vector<uint8_t> v_buffer(imageBuf, imageBuf+imageSize);
								delete[] imageBuf;
								ws.text(false);
								ws.write(asio::buffer(v_buffer));
								continue;
							}
						} else if(cmd == "setSaveInfo") {
							std::string prefix = cmd_tree.get<std::string>("prefix");
							camera->set_save_info(wstring_convert.from_bytes(prefix));

						} else if(cmd == "setAperture") {
							std::uint16_t value = cmd_tree.get<std::uint16_t>("value");
							camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FNumber, value);

						} else if(cmd == "setShutterSpeed") {
							std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
							camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed, value);

						} else if(cmd == "setIso") {
							std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
							camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IsoSensitivity, value);

						} else if(cmd == "test") {
							auto id = SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality;
							auto prop = camera->GetProp(id);
							auto value = SCRSDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_Low;
							if(prop->current == SCRSDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_Low)
								value = SCRSDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_High;
							camera->setProp(id, value);
							camera->waitProp(id, 1000);

						} else {
							SCRSDK::CrDevicePropertyCode id = camera->Prop_tag2id(cmd);
							if(id) {
								std::string ope = cmd_tree.get<std::string>("ope");
								if(ope == "set") {
									if(cmd_tree.get_child_optional("value")) {
										std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
										camera->SetProp(id, value);
									} else if(cmd_tree.get_child_optional("text")) {
										std::string _text = cmd_tree.get<std::string>("text");
										camera->SetProp(id, _text);
									}
								//	std::this_thread::sleep_for(std::chrono::milliseconds(300));	// ADHOC
								//	sendProp(ws, id, false/*sendInfo*/);
								} else if(ope == "get") {
									sendProp(ws, id, false/*sendInfo*/);
								} else if(ope == "info") {
									sendProp(ws, id, true/*sendInfo*/);
								} else if(ope == "inc") {
									incProp(ws, id, true/*inc_dec*/);
								} else if(ope == "dec") {
									incProp(ws, id, false/*inc_dec*/);
								}
								continue;
							} else if(!camera->SendCommand(cmd)){

							} else {
								std::clog << "unknown command" << std::endl;
							}
						}
					}
				}
				//Echo the message pack to the client
				{
					std::unique_lock<std::mutex> ws_lock(ws_mutex);		// ws mutex
					ws.text(true);
					ws.write(buffer.data());
				}
			}
		}
		catch(beast::system_error const& se)
		{
			// This indicates that the session was closed
			if(se.code() != websocket::error::closed)
				std::cerr << "Error: " << se.code().message() << std::endl;
		}
		catch(std::exception const& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}

	//	ws.close(websocket::close_code::normal);
		p_ws = NULL;
		std::clog << "Disconnected." << std::endl;
	}
}


void uploadFile(const wchar_t* filename)
{
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	if(!ifs) return;

	ifs.seekg(0, std::ios::end);
	auto filesize = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::ios::beg);

	uint8_t* filebuf = new uint8_t[filesize];
	if(!filebuf) {
		ifs.close();
		return;
	}
	ifs.read((char*)filebuf, filesize);
	ifs.close();

//	server.send(conn_, filebuf, filesize);
	std::vector<uint8_t> v_buffer(filebuf, filebuf+filesize);
	delete[] filebuf;
	if(p_ws) {
		std::unique_lock<std::mutex> ws_lock(ws_mutex);		// ws mutex
		p_ws->text(false);
		p_ws->write(asio::buffer(v_buffer));
	}
	return;
}

//------------------------------------------------------------------------------

void
fail(beast::error_code err, char const* what)
{
	std::cerr << what << ": " << err.message() << "\n";
}

// Handles an HTTP server connection
void do_thread_http(void)
{
	asio::io_context ioc;
	tcp::endpoint endpoint(asio::ip::make_address(address), portHttp);
	tcp::acceptor acceptor(ioc, endpoint);

	beast::error_code err;
	beast::flat_buffer read_buf;

	while(true)
	{
		tcp::socket socket2(ioc);
		acceptor.accept(socket2);

		do
		{
			http::request<http::string_body> req;
			http::read(socket2, read_buf, req, err);
			if(err == http::error::end_of_stream)
				break;
			if(err) {
				fail(err, "read");
				break;
			}

			http::response<http::empty_body> res(http::status::ok, req.version());
			res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
			res.set(http::field::access_control_allow_origin, "*");
			res.set(http::field::content_type, "multipart/x-mixed-replace; boundary=frame");
			res.keep_alive();
			http::response_serializer<http::empty_body> sr(res);
			http::write_header(socket2, sr);

			while(true) {
				uint8_t* imageBuf = NULL;
				int imageSize = 0;
				{
					std::unique_lock<std::mutex> crsdk_lock(crsdk_mutex);		// CrSDK mutex
					imageSize = camera->get_live_view(&imageBuf);
				}
				if(imageSize > 0) {
				//	std::cout << ".";

					beast::error_code err;
					std::vector<uint8_t> v_buffer(imageBuf, imageBuf+imageSize);
					delete[] imageBuf;

					http::response<http::vector_body<uint8_t>> res(std::piecewise_construct,
								std::make_tuple(std::move(v_buffer)),
								std::make_tuple(http::status::ok, req.version()));
					res.set(http::field::body, "--frame");
					res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
					res.set(http::field::content_type, "image/jpeg");
					res.content_length(imageSize);
					res.keep_alive(req.keep_alive());
					http::write(socket2, res, err);
					std::this_thread::sleep_for(std::chrono::milliseconds(30));
					if(err) {
						fail(err, "write");
						break;
					}
				}
			}
		} while(false);

		socket2.shutdown(tcp::socket::shutdown_send, err);
		std::clog << "closed." << std::endl;
	}
}

void add_array(pt::ptree& pt_, std::string key, std::vector<std::string> array)
{
	pt::ptree tmp1;
	for(auto& iter : array) {
		pt::ptree tmp2;
		tmp2.put("", iter);
		tmp1.push_back(std::make_pair("", tmp2));
	}
	pt_.add_child(key, tmp1);

/*
	pt::ptree pt3;
	std::vector<std::string> temp = {"123","456"};
	add_array(pt3, "tmp2", temp);

	std::stringstream ss2;
	pt::write_json(ss2, pt3); std::cout << ss2.str() << "\n";
*/
}

int main(int argc, char* argv[])
{
	std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>);

	int ret = remoteCli_init();
	if(ret) return -1;

	std::thread serverThread1(do_thread_ws);
	std::thread serverThread2(do_thread_http);

	while(1) {
		std::cout << "To exit, please enter 'q'.\n";

		std::string input;
		std::getline(std::cin, input);
		if(input == "q" || input == "Q") {
			if (camera->is_connected()) {
				camera->disconnect();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			serverThread1.detach();
			serverThread2.detach();
			return 0;
		}
	}
//	serverThread1.join();
//	serverThread2.join();

	return 0;
}
