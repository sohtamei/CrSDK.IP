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

void sendProp8(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool sendPossible)
{
	cli::PropertyValueEntry<std::uint8_t> prop;
	camera->GetProp(id, prop);

	pt::ptree resp_tree;
	pt::ptree item_tree;
	item_tree.put("text", wstring_convert.to_bytes(camera->GetFormatMsg(id, prop.current)));
	item_tree.put("value", std::to_string(prop.current));
	resp_tree.add_child("current", item_tree);

	pt::ptree sub_tree;
	int index = -1;
	for(int i = 0; i < prop.possible.size(); i++) {
		if(prop.current == prop.possible[i]) index = i;
		item_tree.put("text", wstring_convert.to_bytes(camera->GetFormatMsg(id, prop.possible[i])));
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

void sendProp16(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool sendPossible)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetProp(id, prop);

	pt::ptree resp_tree;
	pt::ptree item_tree;
	item_tree.put("text", wstring_convert.to_bytes(camera->GetFormatMsg(id, prop.current)));
	item_tree.put("value", std::to_string(prop.current));
	resp_tree.add_child("current", item_tree);

	pt::ptree sub_tree;
	int index = -1;
	for(int i = 0; i < prop.possible.size(); i++) {
		if(prop.current == prop.possible[i]) index = i;
		item_tree.put("text", wstring_convert.to_bytes(camera->GetFormatMsg(id, prop.possible[i])));
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

void sendProp32(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool sendPossible)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetProp(id, prop);

	pt::ptree resp_tree;
	pt::ptree item_tree;
	item_tree.put("text", wstring_convert.to_bytes(camera->GetFormatMsg(id, prop.current)));
	item_tree.put("value", std::to_string(prop.current));
	resp_tree.add_child("current", item_tree);

	pt::ptree sub_tree;
	int index = -1;
	for(int i = 0; i < prop.possible.size(); i++) {
		if(prop.current == prop.possible[i]) index = i;
		item_tree.put("text", wstring_convert.to_bytes(camera->GetFormatMsg(id, prop.possible[i])));
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

void incProp8(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool inc_dec)
{
	cli::PropertyValueEntry<std::uint8_t> prop;
	camera->GetProp(id, prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetProp(id, prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	sendProp8(ws, id, false);
}

void incProp16(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool inc_dec)
{
	cli::PropertyValueEntry<std::uint16_t> prop;
	camera->GetProp(id, prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetProp(id, prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	sendProp16(ws, id, false);
}

void incProp32(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	bool inc_dec)
{
	cli::PropertyValueEntry<std::uint32_t> prop;
	camera->GetProp(id, prop);

	if(prop.writable == 1) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(prop.current == prop.possible[i]) {
				int index = i;
				if(inc_dec) index++;
				else index--;
				if(index >= 0 && index <= prop.possible.size()-1) {
					camera->SetProp(id, prop.possible[index]);
				}
				break;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	sendProp32(ws, id, false);
}

void opeProp8(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	std::string ope)
{
	if(ope == "get") {
		sendProp8(ws, id, true/*sendPossible*/);
	} else if(ope == "inc") {
		incProp8(ws, id, true/*inc_dec*/);
	} else if(ope == "dec") {
		incProp8(ws, id, false/*inc_dec*/);
	}
}

void opeProp16(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	std::string ope)
{
	if(ope == "get") {
		sendProp16(ws, id, true/*sendPossible*/);
	} else if(ope == "inc") {
		incProp16(ws, id, true/*inc_dec*/);
	} else if(ope == "dec") {
		incProp16(ws, id, false/*inc_dec*/);
	}
}

void opeProp32(websocket::stream<tcp::socket>& ws,
	SCRSDK::CrDevicePropertyCode id,
	std::string ope)
{
	if(ope == "get") {
		sendProp32(ws, id, true/*sendPossible*/);
	} else if(ope == "inc") {
		incProp32(ws, id, true/*inc_dec*/);
	} else if(ope == "dec") {
		incProp32(ws, id, false/*inc_dec*/);
	}
}

void SendProp32(SCRSDK::CrDevicePropertyCode id)
{
	if(p_ws) {
		sendProp32(*p_ws, id, false);
	}
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
							uint32_t delay_ms = 500;
							if(cmd_tree.get_child_optional("delay"))
								delay_ms = cmd_tree.get<std::uint32_t>("delay");
							camera->af_shutter(delay_ms);
						} else if (cmd == "afHalfShutter") {
							camera->s1_shooting();
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
						} else if (cmd == "setSaveInfo") {
							std::string prefix = cmd_tree.get<std::string>("prefix");
							camera->set_save_info(wstring_convert.from_bytes(prefix));

						} else if (cmd == "setAperture") {
							std::uint16_t value = cmd_tree.get<std::uint16_t>("value");
							camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FNumber, value);

						} else if (cmd == "setShutterSpeed") {
							std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
							camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed, value);

						} else if (cmd == "setIso") {
							std::uint32_t value = cmd_tree.get<std::uint32_t>("value");
							camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IsoSensitivity, value);
						} else {
							std::string ope = cmd_tree.get<std::string>("ope");
							if(0) {
							} else if (cmd == "remocon_Zoom_Speed_Type") {			opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Remocon_Zoom_Speed_Type, ope); continue;
							} else if (cmd == "playbackMedia") {					opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_PlaybackMedia, ope); continue;
							} else if (cmd == "gainBaseSensitivity") {				opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_GainBaseSensitivity, ope); continue;
							} else if (cmd == "gainBaseIsoSensitivity") {			opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_GainBaseIsoSensitivity, ope); continue;
							} else if (cmd == "monitorLUTSetting") {				opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_MonitorLUTSetting, ope); continue;
							} else if (cmd == "irisModeSetting") {					opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IrisModeSetting, ope); continue;
							} else if (cmd == "shutterModeSetting") {				opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterModeSetting, ope); continue;
							} else if (cmd == "gainControlSetting") {				opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_GainControlSetting, ope); continue;
							} else if (cmd == "exposureCtrlType") {					opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ExposureCtrlType, ope); continue;
							} else if (cmd == "imageStabilizationSteadyShot") {		opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ImageStabilizationSteadyShot, ope); continue;
							} else if (cmd == "movie_ImageStabilizationSteadyShot") {opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Movie_ImageStabilizationSteadyShot, ope); continue;
							} else if (cmd == "silentMode") {						opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_SilentMode, ope); continue;
							} else if (cmd == "silentModeApertureDriveInAF") {		opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_SilentModeApertureDriveInAF, ope); continue;
							} else if (cmd == "silentModeShutterWhenPowerOff") {	opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_SilentModeShutterWhenPowerOff, ope); continue;
							} else if (cmd == "silentModeAutoPixelMapping") {		opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_SilentModeAutoPixelMapping, ope); continue;
							} else if (cmd == "shutterType") {						opeProp8(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterType, ope); continue;

							} else if (cmd == "aperture") {						opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FNumber, ope); continue;
							} else if (cmd == "priorityKeySettings") {			opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_PriorityKeySettings, ope); continue;
							} else if (cmd == "focusMode") {					opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusMode, ope); continue;
							} else if (cmd == "focusArea") {					opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusArea, ope); continue;
							} else if (cmd == "liveView_Image_Quality") {		opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality, ope); continue;
							} else if (cmd == "whiteBalance") {					opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_WhiteBalance, ope); continue;
							} else if (cmd == "baseLookValue") {				opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_BaseLookValue, ope); continue;
							} else if (cmd == "movie_Recording_Setting") {		opeProp16(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_Movie_Recording_Setting, ope); continue;

							} else if (cmd == "iso") {							opeProp32(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_IsoSensitivity, ope); continue;
							} else if (cmd == "shutterSpeed") {					opeProp32(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ShutterSpeed, ope); continue;
							} else if (cmd == "exposureProgramMode") {			opeProp32(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_ExposureProgramMode, ope); continue;
							} else if (cmd == "driveMode") {					opeProp32(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_DriveMode, ope); continue;

							} else if (cmd == "focusIndication") {				opeProp32(ws, SCRSDK::CrDevicePropertyCode::CrDeviceProperty_FocusIndication, ope); continue;
							} else {
								std::clog << "unknown command" << std::endl;
							}
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

int main(int argc, char* argv[])
{
	std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>);

	int ret = remoteCli_init();
	if(ret) return -1;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	camera->load_properties();
	camera->SetProp(SCRSDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality, static_cast<std::uint16_t>(0));

	std::thread serverThread1(do_thread_ws);
	std::thread serverThread2(do_thread_http);

	serverThread1.join();
	serverThread2.join();

	return 0;
}
