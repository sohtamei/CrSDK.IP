#include <cstdlib>
#if defined(USE_EXPERIMENTAL_FS)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#if defined(__APPLE__)
#include <unistd.h>
#endif
#endif

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>
#include "CRSDK/CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "Text.h"

namespace SDK = SCRSDK;

typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
CameraDevicePtr camera = NULL;

std::vector<CameraDevicePtr> cameraList; // all

static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstring_convert;

int remoteCli_init(void)
{
    // Change global locale to native locale
//  std::locale::global(std::locale(""));
    std::locale::global(std::locale::classic());

    // Make the stream's locale the same as the current global locale
    cli::tin.imbue(std::locale());
    cli::tout.imbue(std::locale());

    auto init_success = SDK::Init();
    if (!init_success) {
        std::cout << "Failed to initialize Remote SDK. Terminating.\n";
        SDK::Release();
        return -1;
    }

    std::cout << "Enumerate connected camera devices...\n";
    SDK::ICrEnumCameraObjectInfo* camera_list = nullptr;
    auto enum_status = SDK::EnumCameraObjects(&camera_list);
    if (CR_FAILED(enum_status) || camera_list == nullptr) {
        std::cout << "No cameras detected. Connect a camera and retry.\n";
        SDK::Release();
        return -1;
    }
    auto ncams = camera_list->GetCount();
    std::cout << "Camera enumeration successful. " << ncams << " detected.\n";

    for (CrInt32u i = 0; i < ncams; ++i) {
        auto camera_info = camera_list->GetCameraObjectInfo(i);
        std::string model = wstring_convert.to_bytes(camera_info->GetModel());
        std::string id = wstring_convert.to_bytes((TCHAR*)camera_info->GetId());
        std::cout << '[' << i + 1 << "] " << model << " (" << id << ")\n";
    }

    CrInt32u no = 1;
    if(ncams >= 2) {
        std::cout << "Connect to camera with input number...\n";
        std::cout << "input> ";
        std::string connectNo;
        std::getline(std::cin, connectNo);
        std::cout << '\n';

        no = stoi(connectNo);
        if(no < 1 || no > ncams) {
            std::cout << "error!\n";
            no = 1;
        }
    }
    std::int32_t cameraNumUniq = 1;

    std::cout << "Connect to selected camera...\n";
    auto* camera_info = camera_list->GetCameraObjectInfo(no - 1);

    camera = CameraDevicePtr(new cli::CameraDevice(cameraNumUniq, camera_info));
    cameraList.push_back(camera); // add 1st
    camera_list->Release();

    if (camera->is_connected()) {
        std::cout << "Please disconnect\n";
    } else {
         camera->connect(SDK::CrSdkControlMode_Remote, SDK::CrReconnecting_ON);
    }
/*
    if (SDK::CrSdkControlMode_Remote != camera->get_sdkmode()) {
        goto Error;
    }
*/
    if ((SDK::CrSSHsupportValue::CrSSHsupport_ON == camera->get_sshsupport()) && (false == camera->is_getfingerprint())) {
        // Fingerprint is incorrect
        goto Error;
    }

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	camera->load_properties();
	auto id = SDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality;
	int ret = camera->SetProp_(id, "Low");
	if(ret) {
		if (camera->is_connected()) {
			camera->disconnect();
		}
		std::cout << "ERROR: Please reboot this command(" << __LINE__ << ").\n";
		SDK::Release();
		return -1;
	}
	camera->waitProp(id, 1000);

	return 0;

Error:
    if (camera->is_connected()) {
        camera->disconnect();
    }

    std::cout << "Release SDK resources.\n";
    SDK::Release();

    std::cout << "Exiting application.\n";
    return -1;
}
