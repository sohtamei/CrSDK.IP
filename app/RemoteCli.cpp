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
#include "CRSDK/CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "Text.h"

namespace SDK = SCRSDK;

typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
CameraDevicePtr camera = NULL;

typedef std::vector<CameraDevicePtr> CameraDeviceList;
CameraDeviceList cameraList; // all


int remoteCli_init(void)
{
    // Change global locale to native locale
    // Make the stream's locale the same as the current global locale
    std::locale::global(std::locale(""));
    cli::tin.imbue(std::locale());
    cli::tout.imbue(std::locale());

    cli::tout << "RemoteSampleApp v1.08.00 running...\n\n";

    auto init_success = SDK::Init();
    if (!init_success) {
        cli::tout << "Failed to initialize Remote SDK. Terminating.\n";
        SDK::Release();
        return -1;
    }

    cli::tout << "Enumerate connected camera devices...\n";
    SDK::ICrEnumCameraObjectInfo* camera_list = nullptr;
    auto enum_status = SDK::EnumCameraObjects(&camera_list);
    if (CR_FAILED(enum_status) || camera_list == nullptr) {
        cli::tout << "No cameras detected. Connect a camera and retry.\n";
        SDK::Release();
        return -1;
    }
    auto ncams = camera_list->GetCount();
    cli::tout << "Camera enumeration successful. " << ncams << " detected.\n\n";

    CrInt32u no = 1;

    std::int32_t cameraNumUniq = 1;

    cli::tout << "Connect to selected camera...\n";
    auto* camera_info = camera_list->GetCameraObjectInfo(no - 1);

    camera = CameraDevicePtr(new cli::CameraDevice(cameraNumUniq, camera_info));
    cameraList.push_back(camera); // add 1st
    camera_list->Release();

    if (camera->is_connected()) {
        cli::tout << "Please disconnect\n";
    }
    else {
         camera->connect(SDK::CrSdkControlMode_Remote, SDK::CrReconnecting_ON);
    }

    if (SDK::CrSdkControlMode_Remote != camera->get_sdkmode()) {
        goto Error;
    }

    if ((SDK::CrSSHsupportValue::CrSSHsupport_ON == camera->get_sshsupport()) && (false == camera->is_getfingerprint())) {
        // Fingerprint is incorrect
        goto Error;
    }

	return 0;

    /* Shutter/Rec Operation Menu */
    while (true) {
        cli::tout << "<< Shutter/Rec Operation Menu >>\nWhat would you like to do? Enter the corresponding number.\n";
        cli::tout
            << "(0) Return to REMOTE-MENU\n"
            << "(1) Shutter Release \n"
            << "(2) Shutter Half Release in AF mode \n"
            << "(3) Shutter Half and Full Release in AF mode \n"
            << "(4) Continuous Shooting \n"
            << "(5) Focus Bracket Shot \n"
            << "(6) Movie Rec Button \n"
            << "(7) Movie Rec Button(Toggle) \n"
            ;

        cli::tout << "input> ";
        cli::text select;
        std::getline(cli::tin, select);
        cli::tout << '\n';
        if (select == TEXT("1")) { /* Take photo */
            camera->capture_image();
        }
        else if (select == TEXT("2")) { /* S1 Shooting */
            camera->s1_shooting();
        }
        else if (select == TEXT("3")) { /* AF Shutter */
            camera->af_shutter();
        }
        else if (select == TEXT("4")) { /* Continuous Shooting */
            camera->continuous_shooting();
        }
        else if (select == TEXT("5")) { /* Focus Bracket shooting */
            camera->execute_focus_bracket();
        }
        else if (select == TEXT("6")) { /* Movie Rec Button */
            camera->execute_movie_rec();
        }
        else if (select == TEXT("7")) { /* Movie Rec Button(Toggle) */
            camera->execute_movie_rec_toggle();
        }
        else if (select == TEXT("0")) {
            cli::tout << "Return to REMOTE-MENU.\n";
            break;
        }
        cli::tout << std::endl;
    } // end of loop-Menu1

Error:
    if (camera->is_connected()) {
        camera->disconnect();
    }

    cli::tout << "Release SDK resources.\n";
    SDK::Release();

    cli::tout << "Exiting application.\n";
    return -1;
}

void af_shutter(void) {
    camera->af_shutter();
}
