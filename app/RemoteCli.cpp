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


int remoteCli_init(void)
{
    // Change global locale to native locale
//  std::locale::global(std::locale(""));
    std::locale::global(std::locale::classic());

    // Make the stream's locale the same as the current global locale
    cli::tin.imbue(std::locale());
    cli::tout.imbue(std::locale());

    std::cout << "Cr\"Nocode\"SDK running...\n";

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

    CrInt32u no = 1;
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
	int ret = camera->setProp(SDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality,
							SDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_Low);
	if(ret) {
		if (camera->is_connected()) {
			camera->disconnect();
		}
		std::cout << "ERROR: Please reboot this command(" << __LINE__ << ").\n";
	}
	camera->waitProp(SDK::CrDevicePropertyCode::CrDeviceProperty_LiveView_Image_Quality, 1000);

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
