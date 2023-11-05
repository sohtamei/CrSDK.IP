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
std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstring_convert;


std::mutex crsdk_mtx;

void write_json(websocket::stream<tcp::socket>& ws, pt::ptree resp_tree)
{
	std::stringstream resp_stream;
	pt::write_json(resp_stream, resp_tree);
	beast::flat_buffer resp_buffer;
	boost::beast::ostream(resp_buffer) << resp_stream.str();
	std::clog << resp_stream.str() << std::endl;
	ws.text(true);
	ws.write(resp_buffer.data());
}

void getProp(websocket::stream<tcp::socket>& ws,
	bool sendPossible, 
	cli::PropertyValueEntry<std::uint16_t>& prop,
	cli::text (*format_func)(std::uint16_t))
{
	pt::ptree resp_tree;
	pt::ptree item_tree;
	item_tree.put("text", wstring_convert.to_bytes(format_func(prop.current)));
	item_tree.put("value", std::to_string(prop.current));
	resp_tree.add_child("current", item_tree);

	pt::ptree sub_tree;
	int index = -1;
	for(int i = 0; i < prop.possible.size(); i++) {
		if(prop.current == prop.possible[i]) index = i;
		item_tree.put("text", wstring_convert.to_bytes(format_func(prop.possible[i])));
		item_tree.put("value", std::to_string(prop.possible[i]));
		sub_tree.push_back(std::make_pair("", item_tree));
	}
	if(sendPossible)
		resp_tree.add_child("possible", sub_tree);

	std::string incrementable = "none";
	if(prop.writable != 1)
		;
	else if(index == 0)
		incrementable = "inc";
	else if(index == prop.possible.size()-1)
		incrementable = "dec";
	else if(index > 0 && index < prop.possible.size()-1)
		incrementable = "incdec";
	resp_tree.put("incrementable", incrementable);

	write_json(ws, resp_tree);
}

void getProp(websocket::stream<tcp::socket>& ws,
	bool sendPossible, 
	cli::PropertyValueEntry<std::uint32_t>& prop,
	cli::text (*format_func)(std::uint32_t))
{
	pt::ptree resp_tree;
	pt::ptree item_tree;
	item_tree.put("text", wstring_convert.to_bytes(format_func(prop.current)));
	item_tree.put("value", std::to_string(prop.current));
	resp_tree.add_child("current", item_tree);

	pt::ptree sub_tree;
	int index = -1;
	for(int i = 0; i < prop.possible.size(); i++) {
		if(prop.current == prop.possible[i]) index = i;
		item_tree.put("text", wstring_convert.to_bytes(format_func(prop.possible[i])));
		item_tree.put("value", std::to_string(prop.possible[i]));
		sub_tree.push_back(std::make_pair("", item_tree));
	}
	if(sendPossible)
		resp_tree.add_child("possible", sub_tree);

	std::string incrementable = "none";
	if(prop.writable != 1)
		;
	else if(index == 0)
		incrementable = "inc";
	else if(index == prop.possible.size()-1)
		incrementable = "dec";
	else if(index > 0 && index < prop.possible.size()-1)
		incrementable = "incdec";
	resp_tree.put("incrementable", incrementable);

	write_json(ws, resp_tree);
}

void getAperture(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetAperture(prop);
	getProp(ws, sendPossible, prop, cli::format_f_number);
}

void getShutterSpeed(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetShutterSpeed(prop);
	getProp(ws, sendPossible, prop, cli::format_shutter_speed);
}

void getIso(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetIso(prop);
	getProp(ws, sendPossible, prop, cli::format_iso_sensitivity);
}

void getDriveMode(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetDriveMode(prop);
	getProp(ws, sendPossible, prop, cli::format_still_capture_mode);
}

void getExposureProgramMode(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetExposureProgramMode(prop);
	getProp(ws, sendPossible, prop, cli::format_exposure_program_mode);
}

void getWhiteBalance(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetWhiteBalance(prop);
	getProp(ws, sendPossible, prop, cli::format_white_balance);
}

void getFocusMode(websocket::stream<tcp::socket>& ws, bool sendPossible)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetFocusMode(prop);
	getProp(ws, sendPossible, prop, cli::format_focus_mode);
}

void incAperture(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetAperture(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetAperture(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getAperture(ws, false);
}

void incShutterSpeed(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetShutterSpeed(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetShutterSpeed(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getShutterSpeed(ws, false);
}

void incIso(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetIso(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetIso(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getIso(ws, false);
}

void incDriveMode(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetDriveMode(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetDriveMode(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getDriveMode(ws, false);
}

void incExposureProgramMode(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetExposureProgramMode(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetExposureProgramMode(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getExposureProgramMode(ws, false);
}

void incWhiteBalance(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetWhiteBalance(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetWhiteBalance(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getWhiteBalance(ws, false);
}

void incFocusMode(websocket::stream<tcp::socket>& ws, bool inc_dec)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetFocusMode(prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetFocusMode(prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	getFocusMode(ws, false);
}


void do_thread_ws(void)
{
	asio::io_context ioc;
	tcp::endpoint endpoint(asio::ip::make_address(address), portWS);
	tcp::acceptor acceptor(ioc, endpoint);

	beast::error_code err;

	while(true)
	{
		try
		{
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

				if (ws.got_text()) {
					std::unique_lock<std::mutex> crsdk_lock(crsdk_mtx);		// CrSDK mutex

					std::cout << beast::buffers_to_string(buffer.data()) << std::endl;

					std::string cmd_data(beast::buffers_to_string(buffer.data()));
					std::istringstream cmd_is(cmd_data);
					pt::ptree cmd_tree;
					pt::read_json(cmd_is, cmd_tree);

					std::cout << "Received JSON: " << cmd_tree.get<std::string>("cmd") << std::endl;

					if(cmd_tree.get_child_optional("cmd")) {
						std::string cmd = cmd_tree.get<std::string>("cmd");
						if (cmd == "afShutter") {
							camera->af_shutter();
						} else if (cmd == "liveview") {
							uint8_t* imageBuf = NULL;
							int imageSize = camera->get_live_view(&imageBuf);
							if (imageSize > 0) {
							//	server.send(conn, imageBuf, imageSize);
								std::vector<uint8_t> v_buffer(imageBuf, imageBuf+imageSize);
								delete[] imageBuf;
								ws.text(false);
								ws.write(asio::buffer(v_buffer));
								continue;
							}
						} else if (cmd == "aperture") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getAperture(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incAperture(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incAperture(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "shutterSpeed") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getShutterSpeed(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incShutterSpeed(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incShutterSpeed(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "iso") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getIso(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incIso(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incIso(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "driveMode") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getDriveMode(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incDriveMode(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incDriveMode(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "exposureProgramMode") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getExposureProgramMode(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incExposureProgramMode(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incExposureProgramMode(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "whiteBalance") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getWhiteBalance(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incWhiteBalance(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incWhiteBalance(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "focusMode") {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(ope == "get") {
								getFocusMode(ws, true/*sendPossible*/);
							} else if(ope == "inc") {
								incFocusMode(ws, true/*inc_dec*/);
							} else if(ope == "dec") {
								incFocusMode(ws, false/*inc_dec*/);
							}
							continue;
						} else if (cmd == "setAperture") {
							std::uint16_t value = cmd_tree.get<std::uint16_t>("value");
							camera->SetAperture(value);

						} else if (cmd == "setShutterSpeed") {
							std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
							camera->SetShutterSpeed(value);

						} else if (cmd == "setIso") {
							std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
							camera->SetIso(value);
						} else {
							std::clog << "unknown command" << std::endl;
						}
					}
				}
				//Echo the message pack to the client
				ws.text(true);
				ws.write(buffer.data());
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
	if (!ifs) return;

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
					std::unique_lock<std::mutex> crsdk_lock(crsdk_mtx);		// CrSDK mutex
					imageSize = camera->get_live_view(&imageBuf);
				}
				if (imageSize > 0) {
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
				//	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
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

int main(int argc, char* argv[])
{
	int ret = remoteCli_init();
	if(ret) return -1;

	std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>);

	asio::io_context ioContext;
	asio::steady_timer timer(ioContext);
	timer.expires_after(std::chrono::milliseconds(1000));
	timer.async_wait([](const boost::system::error_code& error) {
		if (error) std::cerr << "error." << std::endl;

		camera->set_live_view_image_quality(0);

		std::thread serverThread1(do_thread_ws);
		std::thread serverThread2(do_thread_http);

		serverThread1.join();
		serverThread2.join();
	});
	ioContext.run();

	return 0;
}
