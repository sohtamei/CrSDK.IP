#if defined (_WIN32) || defined(_WIN64)

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <memory>
#include <map>

// TODO: reference additional headers your program requires here
#pragma warning (push)
#pragma warning (disable:4819)	// warning C4819: The file contains a character that cannot be represented in the current code page (932). Save the file in Unicode format to prevent data loss
#pragma warning (pop)
#endif

#include "CameraDevice.h"
#include <chrono>
#if defined(__GNUC__) && __GNUC__ < 8
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
#include <fstream>
#include <thread>
#include "CRSDK/CrDeviceProperty.h"
#include "Text.h"
#include "../server.h"


#if defined(__APPLE__) || defined(__linux__)
#include <sys/stat.h>
#include <vector>
#include <dirent.h>
#endif


#if defined(__APPLE__) || defined(__linux__)
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#else
#include <conio.h>
#endif


// Enumerator
enum Password_Key {

#if defined(__APPLE__) || defined(__linux__)
    Password_Key_Back = 127,
    Password_Key_Enter = 10
#else
    Password_Key_Back = 8,
    Password_Key_Enter = 13
#endif

};

#if defined(__APPLE__) || defined(__linux__)
/* reads from keypress, doesn't echo */
int getch_for_Nix(void)
{
    struct termios oldattr, newattr;
    int iptCh;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    iptCh = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return iptCh;
}
#endif


namespace SDK = SCRSDK;
using namespace std::chrono_literals;

constexpr int const ImageSaveAutoStartNo = -1;

namespace cli
{

using PCode = SDK::CrDevicePropertyCode;

//(.*)\t(.*)\t(.*)\t(.*)\t(.*)\t(.*)
//	{ PCode::CrDeviceProperty_\2,	{ -1, SDK::CrDataType::CrDataType_\4, Possible\5, "\1",		format_\6, } },
//Prop.at(PCode::CrDeviceProperty_FNumber)

//LensModelName		m_prop.lensModelNameStr
//shutter_speed_value	m_prop.shutter_speed_values
//CrDeviceProperty_ContentsTransferStatus
//CrDeviceProperty_LensModelName
//CrDeviceProperty_Media_FormatProgressRate
//CrDeviceProperty_ShutterSpeedValue


std::map<PCode, struct PropertyValue> Prop {
	{ PCode::CrDeviceProperty_APS_C_or_Full_SwitchingEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "aps_c_or_Full_SwitchingEnableStatus",		format_aps_c_or_full_switching_enable_status, } },
	{ PCode::CrDeviceProperty_APS_C_or_Full_SwitchingSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "aps_c_or_Full_SwitchingSetting",		format_aps_c_or_full_switching_setting, } },
	{ PCode::CrDeviceProperty_BaseLookValue,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "baseLookValue",		format_baselook_value, } },
	{ PCode::CrDeviceProperty_CameraSetting_ReadOperationEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "cameraSetting_ReadOperationEnableStatus",		format_camera_setting_read_operation, } },
	{ PCode::CrDeviceProperty_CameraSettingsResetEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "cameraSettingsResetEnableStatus",		format_camera_setting_reset_enable_status, } },
	{ PCode::CrDeviceProperty_CameraSetting_SaveOperationEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "cameraSetting_SaveOperationEnableStatus",		format_camera_setting_save_operation, } },
	{ PCode::CrDeviceProperty_CameraSetting_SaveRead_State,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "cameraSetting_SaveRead_State",		format_camera_setting_save_read_state, } },
	{ PCode::CrDeviceProperty_CustomWB_Execution_State,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "customWB_Execution_State",		format_customwb_capture_execution_state, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Operation,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "customWB_Capture_Operation",		format_customwb_capture_operation, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Standby,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "customWB_Capture_Standby",		format_customwb_capture_standby, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "customWB_Capture_Standby_Cancel",		format_customwb_capture_standby_cancel, } },
	{ PCode::CrDeviceProperty_DispMode,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "dispMode",		format_dispmode, } },
	{ PCode::CrDeviceProperty_ExposureCtrlType,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "exposureCtrlType",		format_exposure_control_type, } },
	{ PCode::CrDeviceProperty_ExposureProgramMode,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "exposureProgramMode",		format_exposure_program_mode, } },
	{ PCode::CrDeviceProperty_FNumber,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "aperture",		format_f_number, } },
	{ PCode::CrDeviceProperty_FocusArea,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "focusArea",		format_focus_area, } },
	{ PCode::CrDeviceProperty_FocusDrivingStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "focusDrivingStatus",		format_focus_driving_status, } },
	{ PCode::CrDeviceProperty_FocusIndication,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "focusIndication",		format_focus_indication, } },
	{ PCode::CrDeviceProperty_FocusMode,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "focusMode",		format_focus_mode, } },
	{ PCode::CrDeviceProperty_FocusPositionCurrentValue,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "focusPositionCurrentValue",		format_focus_position_value, } },
	{ PCode::CrDeviceProperty_GainBaseIsoSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "gainBaseIsoSensitivity",		format_gain_base_iso_sensitivity, } },
	{ PCode::CrDeviceProperty_GainBaseSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "gainBaseSensitivity",		format_gain_base_sensitivity, } },
	{ PCode::CrDeviceProperty_GainControlSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "gainControlSetting",		format_gain_control_setting, } },
	{ PCode::CrDeviceProperty_ImageStabilizationSteadyShot,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "imageStabilizationSteadyShot",		format_image_stabilization_steady_shot, } },
	{ PCode::CrDeviceProperty_IrisModeSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "irisModeSetting",		format_iris_mode_setting, } },
	{ PCode::CrDeviceProperty_IsoSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "iso",		format_iso_sensitivity, } },
	{ PCode::CrDeviceProperty_IsoCurrentSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "isoCurrentSensitivity",		format_iso_sensitivity, } },
	{ PCode::CrDeviceProperty_LiveView_Image_Quality,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "liveView_Image_Quality",		format_live_view_image_quality, } },
	{ PCode::CrDeviceProperty_S1,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "S1",		format_lock_indicator, } },
	{ PCode::CrDeviceProperty_FEL,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "FEL",		format_lock_indicator, } },
	{ PCode::CrDeviceProperty_AWBL,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "AWBL",		format_lock_indicator, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "mediaSLOT1_FormatEnableStatus",		format_media_slotx_format_enable_status, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "mediaSLOT1_QuickFormatEnableStatus",		format_media_slotx_format_enable_status, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "mediaSLOT2_FormatEnableStatus",		format_media_slotx_format_enable_status, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "mediaSLOT2_QuickFormatEnableStatus",		format_media_slotx_format_enable_status, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "mediaSLOT1_RecordingAvailableType",		format_media_slotx_rec_available, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "mediaSLOT2_RecordingAvailableType",		format_media_slotx_rec_available, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_Status,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "mediaSLOT1_Status",		format_media_slotx_status, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_Status,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "mediaSLOT2_Status",		format_media_slotx_status, } },
	{ PCode::CrDeviceProperty_MonitorLUTSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "monitorLUTSetting",		format_monitor_lut_setting, } },
	{ PCode::CrDeviceProperty_Movie_ImageStabilizationSteadyShot,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "movie_ImageStabilizationSteadyShot",		format_movie_image_stabilization_steady_shot, } },
	{ PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "movieRecButtonToggleEnableStatus",		format_movie_rec_button_toggle_enable_status, } },
	{ PCode::CrDeviceProperty_MovieShootingMode,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "movieShootingMode",		format_movie_shooting_mode, } },
	{ PCode::CrDeviceProperty_DispModeCandidate,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "dispModeCandidate",		format_number, } },
	{ PCode::CrDeviceProperty_DispModeSetting,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "dispModeSetting",		format_number, } },
	{ PCode::CrDeviceProperty_ExposureIndex,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "exposureIndex",		format_number, } },
	{ PCode::CrDeviceProperty_ZoomDistance,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleRange, "zoomDistance",		format_number, } },
	{ PCode::CrDeviceProperty_WhiteBalanceTint,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleRange, "whiteBalanceTint",		format_number, } },
	{ PCode::CrDeviceProperty_WhiteBalanceTintStep,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleRange, "whiteBalanceTintStep",		format_number, } },
	{ PCode::CrDeviceProperty_GaindBValue,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleRange, "gaindBValue",		format_number, } },
	{ PCode::CrDeviceProperty_FocusBracketShotNumber,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleRange, "focusBracketShotNumber",		format_number, } },
	{ PCode::CrDeviceProperty_FocusBracketFocusRange,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleRange, "focusBracketFocusRange",		format_number, } },
	{ PCode::CrDeviceProperty_FocusPositionSetting,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleRange, "focusPositionSetting",		format_number, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleNone, "customWB_Capture",		format_number, } },
	{ PCode::CrDeviceProperty_AF_Area_Position,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleNone, "af_Area_Position",		format_number, } },
	{ PCode::CrDeviceProperty_Zoom_Bar_Information,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleNone, "zoom_Bar_Information",		format_number, } },
	{ PCode::CrDeviceProperty_Zoom_Speed_Range,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleRange, "zoom_Speed_Range",		format_number, } },
	{ PCode::CrDeviceProperty_Zoom_Operation,	{ -1, SDK::CrDataType::CrDataType_Int8, PossibleRange, "zoom_Operation",		format_number, } },
	{ PCode::CrDeviceProperty_PlaybackMedia,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "playbackMedia",		format_playback_media, } },
	{ PCode::CrDeviceProperty_PriorityKeySettings,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "priorityKeySettings",		format_position_key_setting, } },
	{ PCode::CrDeviceProperty_Movie_Recording_Setting,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "movie_Recording_Setting",		format_recording_setting, } },
	{ PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "remocon_Zoom_Speed_Type",		format_remocon_zoom_speed_type, } },
	{ PCode::CrDeviceProperty_SdkControlMode,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "sdkControlMode",		format_SdkControlMode, } },
	{ PCode::CrDeviceProperty_ShutterModeSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "shutterModeSetting",		format_shutter_mode_setting, } },
	{ PCode::CrDeviceProperty_ShutterSpeed,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "shutterSpeed",		format_shutter_speed, } },
//	{ PCode::CrDeviceProperty_ShutterSpeedValue,	{ -1, SDK::CrDataType::CrDataType_UInt64, PossibleList, "shutterSpeedValue",		format_shutter_speed_value, } },
	{ PCode::CrDeviceProperty_ShutterType,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "shutterType",		format_shutter_type, } },
	{ PCode::CrDeviceProperty_SilentMode,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "silentMode",		format_silent_mode, } },
	{ PCode::CrDeviceProperty_SilentModeApertureDriveInAF,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "silentModeApertureDriveInAF",		format_silent_mode_aperture_drive_in_af, } },
	{ PCode::CrDeviceProperty_SilentModeAutoPixelMapping,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "silentModeAutoPixelMapping",		format_silent_mode_auto_pixel_mapping, } },
	{ PCode::CrDeviceProperty_SilentModeShutterWhenPowerOff,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "silentModeShutterWhenPowerOff",		format_silent_mode_shutter_when_power_off, } },
	{ PCode::CrDeviceProperty_DriveMode,	{ -1, SDK::CrDataType::CrDataType_UInt32, PossibleList, "driveMode",		format_still_capture_mode, } },
	{ PCode::CrDeviceProperty_WhiteBalance,	{ -1, SDK::CrDataType::CrDataType_UInt16, PossibleList, "whiteBalance",		format_white_balance, } },
	{ PCode::CrDeviceProperty_Zoom_Operation_Status,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "zoom_Operation_Status",		format_zoom_operation_status, } },
	{ PCode::CrDeviceProperty_Zoom_Setting,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "zoom_Setting",		format_zoom_setting_type, } },
	{ PCode::CrDeviceProperty_Zoom_Type_Status,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "zoom_Type_Status",		format_zoom_types_status, } },

	{ PCode::CrDeviceProperty_ZoomAndFocusPosition_Save,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "zoomAndFocusPosition_Save",		format_number, } },
	{ PCode::CrDeviceProperty_ZoomAndFocusPosition_Load,	{ -1, SDK::CrDataType::CrDataType_UInt8, PossibleList, "zoomAndFocusPosition_Load",		format_number, } },

};


CameraDevice::CameraDevice(std::int32_t no, SCRSDK::ICrCameraObjectInfo const* camera_info)
    : m_number(no)
    , m_device_handle(0)
    , m_connected(false)
    , m_conn_type(ConnectionType::UNKNOWN)
    , m_net_info()
    , m_usb_info()
    , m_prop()
    , m_lvEnbSet(true)
    , m_modeSDK(SCRSDK::CrSdkControlMode_Remote)
    , m_spontaneous_disconnection(false)
    , m_fingerprint("")
    , m_userPassword("")
{
    m_info = SDK::CreateCameraObjectInfo(
        camera_info->GetName(),
        camera_info->GetModel(),
        camera_info->GetUsbPid(),
        camera_info->GetIdType(),
        camera_info->GetIdSize(),
        camera_info->GetId(),
        camera_info->GetConnectionTypeName(),
        camera_info->GetAdaptorName(),
        camera_info->GetPairingNecessity(),
        camera_info->GetSSHsupport()
    );

    m_conn_type = parse_connection_type(m_info->GetConnectionTypeName());
    switch (m_conn_type)
    {
    case ConnectionType::NETWORK:
        m_net_info = parse_ip_info(m_info->GetId(), m_info->GetIdSize());
        break;
    case ConnectionType::USB:
        m_usb_info.pid = m_info->GetUsbPid();
        break;
    case ConnectionType::UNKNOWN:
        [[fallthrough]];
    default:
        // Do nothing
        break;
    }
}

CameraDevice::~CameraDevice()
{
    if (m_info) m_info->Release();
}

bool CameraDevice::getfingerprint()
{
    CrInt32u fpLen = 0;
    char fpBuff[128] = { 0 };
    SDK::CrError err = SDK::GetFingerprint(m_info, fpBuff, &fpLen);

    if (CR_SUCCEEDED(err))
    {
        m_fingerprint = std::string(fpBuff, fpLen);
        return true;
    }
    return false;
}

bool CameraDevice::connect(SCRSDK::CrSdkControlMode openMode, SCRSDK::CrReconnectingSet reconnect)
{
    const char* inputId = "admin";
    char inputPassword[32] = { 0 };
    if (SDK::CrSSHsupportValue::CrSSHsupport_ON == get_sshsupport())
    {
        if (!is_getfingerprint())
        {
            bool resFp = getfingerprint();
            if (resFp)
            {
                tout << "fingerprint: \n" << m_fingerprint.c_str() << std::endl;
                tout << std::endl << "Are you sure you want to continue connecting ? (y/n) > ";
                text yesno;
                std::getline(cli::tin, yesno);
                if (yesno != TEXT("y"))
                {
                    m_fingerprint.clear();
                    return false;
                }
            }
        }
        if (!is_setpassword())
        {
            cli::tout << "Please SSH password > ";
            cli::text userPw;
 
            // Stores the password
            char maskPw = '*';
            char ch_ipt = {};

            // Until condition is true
            while (true) {

#if defined (_WIN32) || defined(_WIN64)
                ch_ipt = getch();
#else
                ch_ipt = getch_for_Nix();
#endif

                // if the ch_ipt
                if (ch_ipt == Password_Key_Enter) {
                    tout << std::endl;
                    break;
                }
                else if (ch_ipt == Password_Key_Back
                    && userPw.length() != 0) {
                    userPw.pop_back();

                    // Cout statement is very
                    // important as it will erase
                    // previously printed character
                    tout << "\b \b";

                    continue;
                }

                // Without using this, program
                // will crash as \b can't be
                // print in beginning of line
                else if (ch_ipt == Password_Key_Back
                    && userPw.length() == 0) {
                    continue;
                }

                userPw.push_back(ch_ipt);
                tout << maskPw;
            }

#if defined(_WIN32) || (_WIN64)
            mbstate_t mbstate;
            size_t retPw;
            memset(&mbstate, 0, sizeof(mbstate_t));
            const wchar_t* wcsInStr = userPw.c_str();
            errno_t erno = wcsrtombs_s(&retPw, inputPassword, &wcsInStr, 32, &mbstate);
#else
            strncpy(inputPassword, (const char*)userPw.c_str(), userPw.size());
#endif
            m_userPassword = std::string(inputPassword, userPw.size());

        }
    }

    m_spontaneous_disconnection = false;
    auto connect_status = SDK::Connect(m_info, this, &m_device_handle, openMode, reconnect, inputId, m_userPassword.c_str(), m_fingerprint.c_str(), m_fingerprint.size());
    if (CR_FAILED(connect_status)) {
        text id(this->get_id());
        tout << std::endl << "Failed to connect: 0x" << std::hex << connect_status << std::dec << ". " << m_info->GetModel() << " (" << id.data() << ")\n";
        m_userPassword.clear();
        return false;
    }
    set_save_info();
    return true;
}

bool CameraDevice::disconnect()
{
    // m_fingerprint.clear();  // Use as needed
    // m_userPassword.clear(); // Use as needed
    m_spontaneous_disconnection = true;
    tout << "Disconnect from camera...\n";
    auto disconnect_status = SDK::Disconnect(m_device_handle);
    if (CR_FAILED(disconnect_status)) {
        tout << "Disconnect failed to initialize.\n";
        return false;
    }
    return true;
}

bool CameraDevice::release()
{
    tout << "Release camera...\n";
    auto finalize_status = SDK::ReleaseDevice(m_device_handle);
    m_device_handle = 0; // clear
    if (CR_FAILED(finalize_status)) {
        tout << "Finalize device failed to initialize.\n";
        return false;
    }
    return true;
}

CrInt32u CameraDevice::get_sshsupport()
{
    return m_info->GetSSHsupport();
}

SCRSDK::CrSdkControlMode CameraDevice::get_sdkmode() 
{
    load_properties();
    if (SDK::CrSdkControlMode_ContentsTransfer == m_modeSDK) {
        tout << TEXT("Contents Transfer Mode\n");
    }
    else {
        tout << TEXT("Remote Control Mode\n");
    }
    return m_modeSDK;
}

void CameraDevice::capture_image() const
{
    tout << "Capture image...\n";
    tout << "Shutter down&up\n";
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam_Down);
    std::this_thread::sleep_for(35ms);
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam_Up);
}

void CameraDevice::s1_shooting() const
{
    tout << "S1 shooting...\n";
    tout << "Shutter Half Press down\n";
    SetProp(PCode::CrDeviceProperty_S1, (std::uint32_t)SDK::CrLockIndicator::CrLockIndicator_Locked);
    std::this_thread::sleep_for(1s);
    tout << "Shutter Half Press up\n";
    SetProp(PCode::CrDeviceProperty_S1, (std::uint32_t)SDK::CrLockIndicator::CrLockIndicator_Unlocked);
}

void CameraDevice::af_shutter(std::uint32_t delay_ms) const
{
    tout << "S1 shooting...\n";
    tout << "Shutter Half Press down\n";
    SetProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Locked);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    tout << "Shutter down&up\n";
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam::CrCommandParam_Down);
    std::this_thread::sleep_for(35ms);
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam::CrCommandParam_Up);

    std::this_thread::sleep_for(1s);
    tout << "Shutter Half Press up\n";
    SetProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Unlocked);
}

void CameraDevice::continuous_shooting()
{
    load_properties();
    tout << "Capture image...\n";
    tout << "Continuous Shooting\n";
    if (1 == Prop.at(PCode::CrDeviceProperty_PriorityKeySettings).writable) {
        auto err = SetProp(PCode::CrDeviceProperty_PriorityKeySettings, SDK::CrPriorityKeySettings::CrPriorityKey_PCRemote);
        if (CR_FAILED(err)) {
            tout << "Priority Key setting FAILED\n";
            return;
        }
        std::this_thread::sleep_for(500ms);
        load_properties();
    }

    // Set, still_capture_mode property
    SDK::CrDeviceProperty devProp;
    auto& values = Prop.at(PCode::CrDeviceProperty_DriveMode).possible;
    devProp.SetCode(PCode::CrDeviceProperty_DriveMode);
    if (find(values.begin(), values.end(), SDK::CrDriveMode::CrDrive_Continuous_Hi) != values.end()) {
        devProp.SetCurrentValue(SDK::CrDriveMode::CrDrive_Continuous_Hi);
    }
    else if (find(values.begin(), values.end(), SDK::CrDriveMode::CrDrive_Continuous) != values.end()) {
        devProp.SetCurrentValue(SDK::CrDriveMode::CrDrive_Continuous);
    }
    else if (1 == Prop.at(PCode::CrDeviceProperty_DriveMode).writable) {
        tout << "Continuous Shot hi or Continuous Shot is not supported.\n";
        return;
    }
    else {
        tout << "Still Capture Mode setting is not writable.\n";
        return;
    }
    devProp.SetValueType(SDK::CrDataType::CrDataType_UInt32Array);
    auto err_still_capture_mode = SDK::SetDeviceProperty(m_device_handle, &devProp);
    if (CR_FAILED(err_still_capture_mode)) {
        tout << "Still Capture Mode setting FAILED\n";
        return;
    }

    std::this_thread::sleep_for(1s);
    load_properties();

	if ((Prop.at(PCode::CrDeviceProperty_DriveMode).current == SDK::CrDriveMode::CrDrive_Continuous_Hi)||
		(Prop.at(PCode::CrDeviceProperty_DriveMode).current == SDK::CrDriveMode::CrDrive_Continuous)){
        tout << "Still Capture Mode setting SUCCESS\n";
        tout << "Shutter down\n";
        SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam::CrCommandParam_Down);

        // Wait, then send shutter up
        std::this_thread::sleep_for(500ms);
        tout << "Shutter up\n";
        SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam::CrCommandParam_Up);
    }
    else {
        tout << "Still Capture Mode setting FAILED\n";
    }
}

#if 0
void CameraDevice::get_live_view()
{
    tout << "GetLiveView...\n";

    CrInt32 num = 0;
    SDK::CrLiveViewProperty* property = nullptr;
    auto err = SDK::GetLiveViewProperties(m_device_handle, &property, &num);
    if (CR_FAILED(err)) {
        tout << "GetLiveView FAILED\n";
        return;
    }
    SDK::ReleaseLiveViewProperties(m_device_handle, property);

    SDK::CrImageInfo inf;
    err = SDK::GetLiveViewImageInfo(m_device_handle, &inf);
    if (CR_FAILED(err)) {
        tout << "GetLiveView FAILED\n";
        return;
    }

    CrInt32u bufSize = inf.GetBufferSize();
    if (bufSize < 1)
    {
        tout << "GetLiveView FAILED \n";
    }
    else
    {
        auto* image_data = new SDK::CrImageDataBlock();
        if (!image_data)
        {
            tout << "GetLiveView FAILED (new CrImageDataBlock class)\n";
            return;
        }
        CrInt8u* image_buff = new CrInt8u[bufSize];
        if (!image_buff)
        {
            delete image_data;
            tout << "GetLiveView FAILED (new Image buffer)\n";
            return;
        }
        image_data->SetSize(bufSize);
        image_data->SetData(image_buff);

        err = SDK::GetLiveViewImage(m_device_handle, image_data);
        if (CR_FAILED(err))
        {
            // FAILED
            if (err == SDK::CrWarning_Frame_NotUpdated) {
                tout << "Warning. GetLiveView Frame NotUpdate\n";
            }
            else if (err == SDK::CrError_Memory_Insufficient) {
                tout << "Warning. GetLiveView Memory insufficient\n";
            }
            delete[] image_buff; // Release
            delete image_data; // Release
        }
        else
        {
            if (0 < image_data->GetSize())
            {
                // Display
                // etc.
#if defined(__APPLE__)
                char path[MAC_MAX_PATH]; /*MAX_PATH*/
                memset(path, 0, sizeof(path));
                if(NULL == getcwd(path, sizeof(path) - 1)){
                    // FAILED
                    delete[] image_buff; // Release
                    delete image_data; // Release
                    tout << "Folder path is too long.\n";
                    return;
                }
                char filename[] ="/LiveView000000.JPG";
                if(strlen(path) + strlen(filename) > MAC_MAX_PATH){
                    // FAILED
                    delete[] image_buff; // Release
                    delete image_data; // Release
                    tout << "Failed to create save path.\n";
                    return;
                }
                strncat(path, filename, strlen(filename));
#else
                auto path = fs::current_path();
                path.append(TEXT("LiveView000000.JPG"));
#endif
                tout << path << '\n';

                std::ofstream file(path, std::ios::out | std::ios::binary);
                if (!file.bad())
                {
                    file.write((char*)image_data->GetImageData(), image_data->GetImageSize());
                    file.close();
                }
                tout << "GetLiveView SUCCESS\n";
                delete[] image_buff; // Release
                delete image_data; // Release
            }
            else
            {
                // FAILED
                delete[] image_buff; // Release
                delete image_data; // Release
            }
        }
    }
}
#endif
void CameraDevice::get_select_media_format()
{
    load_properties();
    tout << "Media SLOT1 Full Format Enable Status: " << format_media_slotx_format_enable_status(Prop.at(PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus).current) << std::endl;
    tout << "Media SLOT2 Full Format Enable Status: " << format_media_slotx_format_enable_status(Prop.at(PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus).current) << std::endl;
    // Valid Quick format
    if (-1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).writable || -1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).writable){
        tout << "Media SLOT1 Quick Format Enable Status: " << format_media_slotx_format_enable_status(Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).current) << std::endl;
        tout << "Media SLOT2 Quick Format Enable Status: " << format_media_slotx_format_enable_status(Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).current) << std::endl;
    }
}

bool CameraDevice::get_custom_wb()
{
    bool state = false;
    load_properties();
    tout << "CustomWB Capture Standby Operation: " << format_customwb_capture_standby(Prop.at(PCode::CrDeviceProperty_CustomWB_Capture_Standby).current) << '\n';
    tout << "CustomWB Capture Standby CancelOperation: " << format_customwb_capture_standby_cancel(Prop.at(PCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel).current) << '\n';
    tout << "CustomWB Capture Operation: " << format_customwb_capture_operation(Prop.at(PCode::CrDeviceProperty_CustomWB_Capture_Operation).current) << '\n';
    tout << "CustomWB Capture Execution State: " << format_customwb_capture_execution_state(Prop.at(PCode::CrDeviceProperty_CustomWB_Execution_State).current) << '\n';
    if (Prop.at(PCode::CrDeviceProperty_CustomWB_Capture_Operation).current == 1) {
        state = true;
    }
    return state;
}

void CameraDevice::get_zoom_operation()
{
    load_properties();
    tout << "Zoom Operation Status: " << format_zoom_operation_status(Prop.at(PCode::CrDeviceProperty_Zoom_Operation_Status).current) << '\n';
    if (Prop.at(PCode::CrDeviceProperty_Zoom_Setting).current > 0) {
        tout << "Zoom Setting Type    : " << format_zoom_setting_type(Prop.at(PCode::CrDeviceProperty_Zoom_Setting).current) << '\n';
    }
    if (Prop.at(PCode::CrDeviceProperty_Zoom_Type_Status).current > 0) {
        tout << "Zoom Type Status     : " << format_zoom_types_status(Prop.at(PCode::CrDeviceProperty_Zoom_Type_Status).current) << '\n';
    }

    // Zoom Speed Range is not supported
    if (Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.size() < 2) {
        tout << "Zoom Speed Range     : -1 to 1" << std::endl 
             << "Zoom Speed Type      : " << format_remocon_zoom_speed_type(Prop.at(PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type).current) << std::endl;
    }
    else {
        tout << "Zoom Speed Range     : " << (int)Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.at(0) << " to " << (int)Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.at(1) << std::endl
             << "Zoom Speed Type      : " << format_remocon_zoom_speed_type(Prop.at(PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type).current) << std::endl;
    }

    // Zoom Bar Information
    std::int32_t nprop = 0;
    SDK::CrDeviceProperty* prop_list = nullptr;
    CrInt32u getCode = PCode::CrDeviceProperty_Zoom_Bar_Information;
    auto status = SDK::GetSelectDeviceProperties(m_device_handle, 1, &getCode, &prop_list, &nprop);
    if (CR_FAILED(status)) {
        tout << "Failed to get Zoom Bar Information.\n";
        return;
    }
    if (prop_list && 0 < nprop) {
        auto prop = prop_list[0];
        if (PCode::CrDeviceProperty_Zoom_Bar_Information == prop.GetCode())
        {
            tout << "Zoom Bar Information : 0x" << std::hex << prop.GetCurrentValue() << std::dec << '\n';
        }
        SDK::ReleaseDeviceProperties(m_device_handle, prop_list);
    }

    // Zoom Distance
    if (-1 == Prop.at(PCode::CrDeviceProperty_ZoomDistance).writable) {
        tout << "Zoom Distance is not supported.\n";
    }
    else {
        tout << "Zoom Distance Current Value : " << Prop.at(PCode::CrDeviceProperty_ZoomDistance).current << std::endl;
        tout << "Zoom Distance min    : " << Prop.at(PCode::CrDeviceProperty_ZoomDistance).possible.at(0) << std::endl;
        tout << "Zoom Distance max    : " << Prop.at(PCode::CrDeviceProperty_ZoomDistance).possible.at(1) << std::endl;
        tout << "Zoom Distance step   : " << Prop.at(PCode::CrDeviceProperty_ZoomDistance).possible.at(2) << std::endl;
    }

    // Lens Model Name
    if (-1 == m_prop.lensModelNameStr.writable) {
        tout << "Lens Model Name is not supported.\n";
    }
    else {
        if (0 < m_prop.lensModelNameStr.length) {
            tout << "\nLens Model Name : " << m_prop.lensModelNameStr.current.c_str() << std::endl;
        }
        else {
            tout << "Lens Model Name could not be obtained.\n";
        }
    }
}

bool CameraDevice::get_aps_c_or_full_switching_setting()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_APS_C_or_Full_SwitchingSetting).current < SDK::CrAPS_C_or_Full_SwitchingSetting::CrAPS_C_or_Full_SwitchingSetting_Full)
    {
        tout << "APS-C/FULL Switching Setting is not supported\n";
        return false;
    }
    tout << "APS-C/FULL Switching Enable Status: " << format_aps_c_or_full_switching_enable_status(Prop.at(PCode::CrDeviceProperty_APS_C_or_Full_SwitchingEnableStatus).current) << '\n';
    tout << "APS-C/FULL Switching Setting: " << format_aps_c_or_full_switching_setting(Prop.at(PCode::CrDeviceProperty_APS_C_or_Full_SwitchingSetting).current) << '\n';
    return true;
}

bool CameraDevice::get_camera_setting_saveread_state()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_CameraSetting_SaveRead_State).current == SDK::CrCameraSettingSaveReadState::CrCameraSettingSaveReadState_Reading) {
        tout << "Unable to download/upload Camera-Setting file. \n";
        return false;
    }
    tout << "Camera-Setting Save/Read State: " << format_camera_setting_save_read_state(Prop.at(PCode::CrDeviceProperty_CameraSetting_SaveRead_State).current) << '\n';
    return true;
}

bool CameraDevice::get_baselook_value()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_BaseLookValue).possible.size() < 1) {
        tout << "BaseLook Value is not supported \n";
        return false;
    }
    //If you want to display a string, you need to execute RequestDisplayStringList() in advance.
    SDK::CrDisplayStringListInfo* pProperty = nullptr;
    SDK::CrDisplayStringType type;
    CrInt32u numOfList = 0;
    CrInt8u baseValueSetter;
    CrInt8u get_baseLookValue;

    SDK::CrError err = SDK::GetDisplayStringList(m_device_handle, SDK::CrDisplayStringType_BaseLook_Name_Display, &pProperty, &numOfList);

    if (CR_SUCCEEDED(err) && numOfList >0) {
        for (int i = 0; i < numOfList; ++i) {
            if (pProperty[i].value == Prop.at(PCode::CrDeviceProperty_BaseLookValue).current) {
                tout << "BaseLook Value: " << format_dispstrlist(pProperty[i]) << '\n';
                break;
            }
        }
        ReleaseDisplayStringList(m_device_handle, pProperty);
    }
    else {
        CrInt8u valHI = (Prop.at(PCode::CrDeviceProperty_BaseLookValue).current >>8 ) & 0x01;
        CrInt8u valLO = Prop.at(PCode::CrDeviceProperty_BaseLookValue).current & 0xFF;
        tout << "BaseLook Value: Index" << (int)valLO << " " << format_baselook_value(valHI) << '\n';
    }

    return true;
}

bool CameraDevice::get_white_balance_tint()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_WhiteBalanceTint).possible.size() < 1)
    {
        tout << "White Balance Tint is not supported.\n";
        return false;
    }
    if (Prop.at(PCode::CrDeviceProperty_WhiteBalanceTintStep).possible.size() < 1)
    {
        tout << "White Balance Tint Step is not supported.\n";
        return false;
    }
    tout << "White Balance Tint CurrentValue: " << (int)Prop.at(PCode::CrDeviceProperty_WhiteBalanceTint).current << '\n';
    tout << "White Balance Tint        Range: " << (int)Prop.at(PCode::CrDeviceProperty_WhiteBalanceTint).possible.at(0) << " to " << (int)Prop.at(PCode::CrDeviceProperty_WhiteBalanceTint).possible.at(1) << std::endl;
    tout << "White Balance Tint Step   Range: " << (int)Prop.at(PCode::CrDeviceProperty_WhiteBalanceTintStep).possible.at(0) << " to " << (int)Prop.at(PCode::CrDeviceProperty_WhiteBalanceTintStep).possible.at(1) << std::endl;
    return true;
}

void CameraDevice::get_media_slot_status()
{
    load_properties();

    // SLOT1
    tout << "Media SLOT1 Status                  : " << format_media_slotx_status(Prop.at(PCode::CrDeviceProperty_MediaSLOT1_Status).current) << std::endl;
    if (0 <= Prop.at(PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType).writable)
    {
        tout << "Media SLOT1 Recording Available Type: " << format_media_slotx_rec_available(Prop.at(PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType).current) << std::endl;
    }

    // SLOT2
    if (-1 == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).writable)
    {
        tout << "Media SLOT2 Status is not supported.\n";
    }
    else
    {
        tout << "Media SLOT2 Status                  : " << format_media_slotx_status(Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).current) << std::endl;
        if (0 <= Prop.at(PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType).writable)
        {
            tout << "Media SLOT2 Recording Available Type: " << format_media_slotx_rec_available(Prop.at(PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType).current) << std::endl;
        }
    }
}

bool CameraDevice::get_movie_rec_button_toggle_enable_status()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus).writable == -1)
    {
        tout << "Movie Rec Button(Toggle) is not supported\n";
        return false;
    }
    tout << "Movie Rec Button(Toggle) Enable Status: " << format_movie_rec_button_toggle_enable_status(Prop.at(PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus).current) << '\n';
    return true;
}

bool CameraDevice::set_save_info() const
{
#if defined(__APPLE__)
    text_char path[MAC_MAX_PATH]; /*MAX_PATH*/
    memset(path, 0, sizeof(path));
    if(NULL == getcwd(path, sizeof(path) - 1)){
        tout << "Folder path is too long.\n";
        return false;
    }
    auto save_status = SDK::SetSaveInfo(m_device_handle
        , path, (char*)"", ImageSaveAutoStartNo);
#else
    text path = fs::current_path().native();
    tout << path.data() << '\n';

    auto save_status = SDK::SetSaveInfo(m_device_handle
        , const_cast<text_char*>(path.data()), TEXT(""), ImageSaveAutoStartNo);
#endif
    if (CR_FAILED(save_status)) {
        tout << "Failed to set save path.\n";
        return false;
    }
    return true;
}

void CameraDevice::get_af_area_position()
{
    CrInt32 num = 0;
    SDK::CrLiveViewProperty* lvProperty = nullptr;
    CrInt32u getCode = SDK::CrLiveViewPropertyCode::CrLiveViewProperty_AF_Area_Position;
    auto err = SDK::GetSelectLiveViewProperties(m_device_handle, 1, &getCode, &lvProperty, &num);
    if (CR_FAILED(err)) {
        tout << "Failed to get AF Area Position [LiveViewProperties]\n";
        return;
    }

    if (lvProperty && 1 == num) {
        // Got AF Area Position
        auto prop = lvProperty[0];
        if (SDK::CrFrameInfoType::CrFrameInfoType_FocusFrameInfo == prop.GetFrameInfoType()) {
            int sizVal = prop.GetValueSize();
            int count = sizVal / sizeof(SDK::CrFocusFrameInfo);
            SDK::CrFocusFrameInfo* pFrameInfo = (SDK::CrFocusFrameInfo*)prop.GetValue();
            if (0 == sizVal || nullptr == pFrameInfo) {
                tout << "  FocusFrameInfo nothing\n";
            }
            else {
                for (std::int32_t frame = 0; frame < count; ++frame) {
                    auto lvprop = pFrameInfo[frame];
                    char buff[512];
                    memset(buff, 0, sizeof(buff));
#if defined(_WIN32) || (_WIN64)
                    sprintf_s(buff, "  FocusFrameInfo no[%d] pri[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                        frame + 1,
                        lvprop.priority,
                        lvprop.width, lvprop.height,
                        lvprop.xDenominator, lvprop.yDenominator,
                        lvprop.xNumerator, lvprop.yNumerator);
#else
                    snprintf(buff, sizeof(buff), "  FocusFrameInfo no[%d] pri[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                        frame + 1,
                        lvprop.priority,
                        lvprop.width, lvprop.height,
                        lvprop.xDenominator, lvprop.yDenominator,
                        lvprop.xNumerator, lvprop.yNumerator);
#endif
                      
                    tout << buff << std::endl;
                }
            }
        }
        SDK::ReleaseLiveViewProperties(m_device_handle, lvProperty);
    }
}

void CameraDevice::set_af_area_position()
{
    load_properties();

    if (1 == Prop.at(PCode::CrDeviceProperty_PriorityKeySettings).writable) {
        auto err = SetProp(PCode::CrDeviceProperty_PriorityKeySettings, SDK::CrPriorityKeySettings::CrPriorityKey_PCRemote);
        if (CR_FAILED(err)) {
            tout << "Priority Key setting FAILED\n";
            return;
        }
        std::this_thread::sleep_for(500ms);
        load_properties();
    }

    // Set, ExposureProgramMode property
    tout << std::endl << "Set the Exposure Program mode to P Auto" << std::endl;
    SDK::CrDeviceProperty devProp;
    devProp.SetCode(PCode::CrDeviceProperty_ExposureProgramMode);
    devProp.SetCurrentValue(SDK::CrExposureProgram::CrExposure_P_Auto);
    devProp.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);
    SDK::CrError err_expromode;
    bool execStat = false;
    int i = 0;
    while (i < 5)
    {
        err_expromode = SDK::SetDeviceProperty(m_device_handle, &devProp);
        if (CR_FAILED(err_expromode)) {
            tout << "Exposure Program mode FAILED\n";
            return;
        }
        std::this_thread::sleep_for(1000ms);
        load_properties();
        if (Prop.at(PCode::CrDeviceProperty_ExposureProgramMode).current == SDK::CrExposureProgram::CrExposure_P_Auto) {
            execStat = true;
            break;
        }
        i++;
    }
    if (false == execStat)
    {
        tout << std::endl << "Exposure Program mode FAILED\n";
        return;
    }

    if (1 != Prop.at(PCode::CrDeviceProperty_FocusArea).writable) {
        tout << "Focus Area is not writable\n";
        return;
    }

    tout << "Exposure Program mode SUCCESS.\n";

    // Set, FocusArea property
    tout << std::endl << "Set Focus Area to Flexible_Spot_S\n";

    SDK::CrDeviceProperty prop;
    prop.SetCode(PCode::CrDeviceProperty_FocusArea);
    prop.SetCurrentValue(SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S);
    prop.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);

    auto& values = Prop.at(PCode::CrDeviceProperty_FocusArea).possible;
    if(find(values.begin(), values.end(), SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S) != values.end()) {
        prop.SetCurrentValue(SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S);
    }
    else {
        tout << "Focus Area: Flexible_Spot_S is invalid.\n";
        tout << "Please confirm Focus Area Limit Setting in Camera Menu.\n";
        return;
    }

    auto err_prop = SDK::SetDeviceProperty(m_device_handle, &prop);
    execStat = false;
    i = 0;
    while (i < 5)
    {
        err_expromode = SDK::SetDeviceProperty(m_device_handle, &prop);
        if (CR_FAILED(err_prop)) {
            tout << "Focus Area FAILED\n";
            return;
        }
        std::this_thread::sleep_for(1000ms);
        load_properties();
        if (Prop.at(PCode::CrDeviceProperty_FocusArea).current == SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S) {
            execStat = true;
            break;
        }
        i++;
    }
    if (false == execStat)
    {
        tout << "Focus Area FAILED\n";
        return;
    }
    tout << "Focus Area SUCCESS\n";
    get_af_area_position();


    execute_pos_xy(PCode::CrDeviceProperty_AF_Area_Position);
}

void CameraDevice::set_select_media_format()
{
    bool validQuickFormat = false;
    SDK::CrCommandId ptpFormatType = SDK::CrCommandId::CrCommandId_MediaFormat;

    if ((SDK::CrMediaFormat::CrMediaFormat_Disable == Prop.at(PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus).current) &&
        (SDK::CrMediaFormat::CrMediaFormat_Disable == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus).current)) {
            // Not a settable property
        tout << std::endl << "Slot1 and Slot2 can not format\n";
        return;
    }

    if ((-1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).writable || -1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).writable)
        &&
         ((SDK::CrMediaFormat::CrMediaFormat_Enable == Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).current) ||
          (SDK::CrMediaFormat::CrMediaFormat_Enable == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).current))) {
            validQuickFormat = true;
    }

    text input;
    tout << std::endl << "Would you like to format the media? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip format.\n";
        return;
    }

    // Full or Quick
    if (validQuickFormat) {
        tout << "Choose a format type number:" << std::endl;
        tout << "[-1] Cancel input" << std::endl;
        tout << "[1] Full Format" << std::endl;
        tout << "[2] Quick Format" << std::endl;

        tout << std::endl << "input> ";
        std::getline(tin, input);
        text_stringstream sstype(input);
        int selected_type = 0;
        sstype >> selected_type;

        if ((selected_type < 1) || (2 < selected_type)) {
            tout << "Input cancelled.\n";
            return;
        }

        if (2 == selected_type) {
            ptpFormatType = SDK::CrCommandId::CrCommandId_MediaQuickFormat;
        }
    }

    tout << std::endl << "Choose a number Which media do you want to format ? \n";
    tout << "[-1] Cancel input\n";

    tout << "[1] SLOT1" << '\n';
    tout << "[2] SLOT2" << '\n';

    tout << std::endl << "input> ";
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;

    if ((selected_index < 1) || (2 < selected_index)) {
        tout << "Input cancelled.\n";
        return;
    }

    tout << std::endl << "All data will be deleted. Is it OK ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip format.\n";
        return;
    }

    // Get the latest status
    get_select_media_format();

    CrInt64u ptpValue = 0xFFFF;
    if (SDK::CrCommandId::CrCommandId_MediaQuickFormat == ptpFormatType) {
        if ((1 == selected_index) && (SDK::CrMediaFormat::CrMediaFormat_Enable == Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).current)) {
        ptpValue = SDK::CrCommandParam::CrCommandParam_Up;
        }
        else if ((2 == selected_index) && (SDK::CrMediaFormat::CrMediaFormat_Enable == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).current)) {
            ptpValue = SDK::CrCommandParam::CrCommandParam_Down;
        }
    }
    else
    {
        if ((1 == selected_index) && (SDK::CrMediaFormat::CrMediaFormat_Enable == Prop.at(PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus).current)) {
            ptpValue = SDK::CrCommandParam::CrCommandParam_Up;
        }
        else if ((2 == selected_index) && (SDK::CrMediaFormat::CrMediaFormat_Enable == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus).current)) {
            ptpValue = SDK::CrCommandParam::CrCommandParam_Down;
        }
    }

    if (0xFFFF == ptpValue)
    {
        tout << std::endl << "The Selected slot cannot be formatted.\n";
        return;
    }

    SDK::SendCommand(m_device_handle, ptpFormatType, (SDK::CrCommandParam)ptpValue);

    tout << std::endl << "Formatting .....\n";

    int startflag = 0;
    CrInt32u getCodes = PCode::CrDeviceProperty_Media_FormatProgressRate;

    std::int32_t nprop = 0;
    SDK::CrDeviceProperty* prop_list = nullptr;

    // check of progress
    while (true)
    {
        auto status = SDK::GetSelectDeviceProperties(m_device_handle, 1, &getCodes, &prop_list, &nprop);
        if (CR_FAILED(status)) {
            tout << "Failed to get Media FormatProgressRate.\n";
            return;
        }
        if (prop_list && 1 == nprop) {
            auto prop = prop_list[0];
        
            if (getCodes == prop.GetCode())
            {
                if ((0 == startflag) && (0 < prop.GetCurrentValue()))
                {
                    startflag = 1;
                }
                if ((1 == startflag) && (0 == prop.GetCurrentValue()))
                {
                    tout << std::endl << "Format completed " << '\n';
                    SDK::ReleaseDeviceProperties(m_device_handle, prop_list);
                    prop_list = nullptr;
                    break;
                }
                tout << "\r" << "FormatProgressRate: " << prop.GetCurrentValue();
            }
        }
        std::this_thread::sleep_for(250ms);
        SDK::ReleaseDeviceProperties(m_device_handle, prop_list);
        prop_list = nullptr;
    }
}

void CameraDevice::execute_movie_rec()
{
    load_properties();

    text input;
    tout << std::endl << "Operate the movie recording button ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip .\n";
        return;
    }

    tout << "Choose a number:\n";
    tout << "[-1] Cancel input\n";

    tout << "[1] Up" << '\n';
    tout << "[2] Down" << '\n';

    tout << "[-1] Cancel input\n";
    tout << "Choose a number:\n";

    tout << std::endl << "input> ";
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;

    if (selected_index < 0) {
        tout << "Input cancelled.\n";
        return;
    }

    CrInt64u ptpValue = 0;
    switch (selected_index) {
    case 1:
        ptpValue = SDK::CrCommandParam::CrCommandParam_Up;
        break;
    case 2:
        ptpValue = SDK::CrCommandParam::CrCommandParam_Down;
        break;
    default:
        selected_index = -1;
        break;
    }

    if (-1 == selected_index) {
        tout << "Input cancelled.\n";
        return;
    }

    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_MovieRecord, (SDK::CrCommandParam)ptpValue);

}

void CameraDevice::set_custom_wb()
{
    load_properties();
    if (-1 == Prop.at(PCode::CrDeviceProperty_CustomWB_Execution_State).writable) {
        tout << "Custom WB Capture is Not Supported.\n";
        return ;
    }
    if (1 == Prop.at(PCode::CrDeviceProperty_PriorityKeySettings).writable) {
        auto err = SetProp(PCode::CrDeviceProperty_PriorityKeySettings, SDK::CrPriorityKeySettings::CrPriorityKey_PCRemote);
        if (CR_FAILED(err)) {
            tout << "Priority Key setting FAILED\n";
            return;
        }
        std::this_thread::sleep_for(500ms);
        load_properties();
    }

    // Set, ExposureProgramMode property
    tout << std::endl << "Set the Exposure Program mode to P mode" << std::endl;
    SDK::CrDeviceProperty expromode;
    expromode.SetCode(PCode::CrDeviceProperty_ExposureProgramMode);
    expromode.SetCurrentValue(SDK::CrExposureProgram::CrExposure_P_Auto);
    expromode.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);
    SDK::CrError err_expromode;
    bool execStat = false;
    int i = 0;
    while (i < 5)
    {
        err_expromode = SDK::SetDeviceProperty(m_device_handle, &expromode);
        if (CR_FAILED(err_expromode)) {
            tout << "Exposure Program mode FAILED\n";
            return;
        }
        std::this_thread::sleep_for(1000ms);
        load_properties();
        if (Prop.at(PCode::CrDeviceProperty_ExposureProgramMode).current == SDK::CrExposureProgram::CrExposure_P_Auto) {
            execStat = true;
            break;
        }
        i++;
    }
    if (false == execStat)
    {
        tout << std::endl << "Exposure Program mode FAILED\n";
        return;
    }
    tout << "Exposure Program mode SUCCESS.\n";

    // Set, White Balance property
    tout << std::endl << "Set the White Balance to Custom1\n";
    SDK::CrDeviceProperty wb;
    wb.SetCode(PCode::CrDeviceProperty_WhiteBalance);
    wb.SetCurrentValue(SDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_1);
    wb.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);
    auto err_wb = SDK::SetDeviceProperty(m_device_handle, &wb);
    if (CR_FAILED(err_wb)) {
        tout << "White Balance FAILED\n";
        return;
    }
    std::this_thread::sleep_for(2000ms);
    load_properties();
    tout << "White Balance: " << format_white_balance(Prop.at(PCode::CrDeviceProperty_WhiteBalance).current) << '\n';

    execStat = false;
    // Set, custom WB capture standby 
    tout << std::endl << "Set custom WB capture standby " << std::endl;

    i = 0;
    while ((false == execStat) && (i < 5))
    {
        execute_downup_property(PCode::CrDeviceProperty_CustomWB_Capture_Standby);
        std::this_thread::sleep_for(1000ms);
        tout << std::endl;
        execStat = get_custom_wb();
        i++;

    }

    if (false == execStat)
    {
        tout << std::endl << "CustomWB Capture Standby FAILED\n";
        return;
    }

    // Set, custom WB capture 
    tout << std::endl << "Set custom WB capture ";
    execute_pos_xy(PCode::CrDeviceProperty_CustomWB_Capture);

    std::this_thread::sleep_for(5000ms);

    // Set, custom WB capture standby cancel 
    text input;
    tout << std::endl << "Set custom WB capture standby cancel. Please enter something. " << std::endl;
    std::getline(tin, input);
    if (0 == input.size() || 0 < input.size()) {
        execute_downup_property(PCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel);
        get_custom_wb();
        tout << std::endl << "Finish custom WB capture\n";
    }
    else
    {
        tout << std::endl << "Did not finish normally\n";
    }
}

void CameraDevice::set_zoom_operation()
{
    text input;
    tout << std::endl << "Operate the zoom ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip .\n";
        return;
    }
    // Get the latest value
    load_properties();
    while (true)
    {
        CrInt64 ptpValue = 0;
        bool cancel = false;

        // Zoom Speed Range is not supported
        if (Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.size() < 2) {
            tout << std::endl << "Choose a number:\n";
            tout << "[-1] Cancel input\n";

            tout << "[0] Stop" << '\n';
            tout << "[1] Wide" << '\n';
            tout << "[2] Tele" << '\n';

            tout << "[-1] Cancel input\n";
            tout << "Choose a number:\n";

            tout << std::endl << "input> ";
            std::getline(tin, input);
            text_stringstream ss(input);
            int selected_index = 0;
            ss >> selected_index;

            switch (selected_index) {
            case 0:
                ptpValue = SDK::CrZoomOperation::CrZoomOperation_Stop;
                break;
            case 1:
                ptpValue = SDK::CrZoomOperation::CrZoomOperation_Wide;
                break;
            case 2:
                ptpValue = SDK::CrZoomOperation::CrZoomOperation_Tele;
                break;
            default:
                tout << "Input cancelled.\n";
                return;
                break;
            }
        }
        else{
            tout << std::endl << "Set the value of zoom speed (Out-of-range value to Cancel):\n";
            tout << std::endl << "input> ";
            std::getline(tin, input);
            text_stringstream ss(input);
            int input_value = 0;
            ss >> input_value;
            // Get the latest value
            load_properties();
            //Stop zoom and return to the top menu when out-of-range values or non-numeric values are entered
            if (((input_value == 0) && (input != TEXT("0"))) || (input_value < (int)Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.at(0)) || ((int)Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.at(1) < input_value))
            {
                cancel = true;
                ptpValue = SDK::CrZoomOperation::CrZoomOperation_Stop;
                tout << "Input cancelled.\n";
            }
            else {
                ptpValue = (CrInt64)input_value;
            }
        }
        if (SDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Enable != Prop.at(PCode::CrDeviceProperty_Zoom_Operation_Status).current) {
            tout << "Zoom Operation is not executable.\n";
            return;
        }
        SDK::CrDeviceProperty prop;
        prop.SetCode(PCode::CrDeviceProperty_Zoom_Operation);
        prop.SetCurrentValue((CrInt64u)ptpValue);
        prop.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);
        SDK::SetDeviceProperty(m_device_handle, &prop);
        if (cancel == true) {
            return;
        }
        get_zoom_operation();
    }
}

bool CameraDevice::set_drive_mode(CrInt64u Value)
{
    SDK::CrDeviceProperty mode;
    mode.SetCode(PCode::CrDeviceProperty_DriveMode);
    mode.SetCurrentValue(Value);
    mode.SetValueType(SDK::CrDataType::CrDataType_UInt32Array);
    auto err_still_capture_mode = SDK::SetDeviceProperty(m_device_handle, &mode);
    if (CR_FAILED(err_still_capture_mode)) {
        return false;
    }
    else {
        return true;
    }
}

void CameraDevice::execute_camera_setting_reset()
{
    load_properties();
    if (SDK::CrCameraSettingsResetEnableStatus::CrCameraSettingsReset_Enable == Prop.at(PCode::CrDeviceProperty_CameraSettingsResetEnableStatus).current) {
        tout << "Camera Setting Reset Enable Status: Enable \n";
    } else {
        tout << "Camera Setting Reset not executable.\n";
        return;
    }

    text input;
    tout << std::endl << "The camera settings will be initialized! Are you sure you want to do this ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip.\n";
        return;
    }
    // Get the latest status
    load_properties();
    if (SDK::CrCameraSettingsResetEnableStatus::CrCameraSettingsReset_Enable != Prop.at(PCode::CrDeviceProperty_CameraSettingsResetEnableStatus).current) {
        tout << "Camera Setting Reset not executable.\n";
        return;
    }
    SDK::CrCommandId ptpFormatType = SDK::CrCommandId::CrCommandId_CameraSettingsReset;
    SDK::SendCommand(m_device_handle, ptpFormatType, SDK::CrCommandParam::CrCommandParam_Down);
    std::this_thread::sleep_for(35ms);
    SDK::SendCommand(m_device_handle, ptpFormatType, SDK::CrCommandParam::CrCommandParam_Up);
}

void CameraDevice::set_baselook_value()
{
    if (false == get_baselook_value())
        return;

    if (1 != Prop.at(PCode::CrDeviceProperty_BaseLookValue).writable) {
        tout << "BaseLook Value is not writable\n";
        return;
    }

    text input;
    tout << "Would you like to set a new BaseLook value? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip setting a new value.\n";
        return;
    }

    tout << "Choose a number set a new BaseLook value:\n";
    tout << "[-1] Cancel input\n";
    auto& values = Prop.at(PCode::CrDeviceProperty_BaseLookValue).possible;

    //If you want to display a string, you need to execute RequestDisplayStringList() in advance.
    SDK::CrDisplayStringListInfo* pProperty = nullptr;
    SDK::CrDisplayStringType type;
    CrInt32u numOfList = 0;

    SDK::CrError err = SDK::GetDisplayStringList(m_device_handle, SDK::CrDisplayStringType_BaseLook_Name_Display, &pProperty, &numOfList);

    if (CR_SUCCEEDED(err) && numOfList != 0) {
        for (std::size_t i = 0; i < values.size(); ++i) {
            for (int j = 0; j < numOfList; ++j) {
                if (pProperty[j].value == values[i]) {
                    tout << '[' << i << "] " << format_dispstrlist(pProperty[j])<< '\n';
                    break;
                }
            }
        }
        ReleaseDisplayStringList(m_device_handle, pProperty);
    }
    else {
        CrInt8u baseValueSetter;
        CrInt8u get_baseLookValue;
        for (std::size_t i = 0; i < values.size(); ++i) {
            baseValueSetter = (values[i] >> 8 ) & 0x01;
            get_baseLookValue = values[i] & 0xFF;
            tout << '[' << i << "] Index" << (int)get_baseLookValue << " " << format_baselook_value(baseValueSetter) << '\n';
        }
    }

    tout << "[-1] Cancel input\n";
    tout << "Choose a number set a new BaseLook value:\n" << std::endl;
    tout << "input> ";
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;

    if (selected_index < 0 || values.size() <= selected_index) {
        tout << "Input cancelled.\n";
        return;
    }

    SDK::CrDeviceProperty prop;
    prop.SetCode(PCode::CrDeviceProperty_BaseLookValue);
    prop.SetCurrentValue(values[selected_index]);
    prop.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);
    SDK::SetDeviceProperty(m_device_handle, &prop);
}

void CameraDevice::execute_downup_property(CrInt16u code)
{
    SDK::CrDeviceProperty prop;
    prop.SetCode(code);
    prop.SetValueType(SDK::CrDataType::CrDataType_UInt16Array);

    // Down
    prop.SetCurrentValue(SDK::CrPropertyCustomWBCaptureButton::CrPropertyCustomWBCapture_Down);
    SDK::SetDeviceProperty(m_device_handle, &prop);

    std::this_thread::sleep_for(500ms);

    // Up
    prop.SetCurrentValue(SDK::CrPropertyCustomWBCaptureButton::CrPropertyCustomWBCapture_Up);
    SDK::SetDeviceProperty(m_device_handle, &prop);

    std::this_thread::sleep_for(500ms);
}

void CameraDevice::execute_pos_xy(CrInt16u code)
{
    load_properties();

    text input;
    tout << std::endl << "Change position ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip.\n";
        return;
    }

    tout << std::endl << "Set the value of X (decimal)" << std::endl;
    tout << "Regarding details of usage, please check API doc." << std::endl;

    tout << std::endl << "input X> ";
    std::getline(tin, input);
    text_stringstream ss1(input);
    CrInt32u x = 0;
    ss1 >> x;

    if (x < 0 || x > 639) {
        tout << "Input cancelled.\n";
        return;
    }

    tout << "input X = " << x << '\n';

    std::this_thread::sleep_for(1000ms);

    tout << std::endl << "Set the value of Y (decimal)" << std::endl;

    tout << std::endl << "input Y> ";
    std::getline(tin, input);
    text_stringstream ss2(input);
    CrInt32u y = 0;
    ss2 >> y;

    if (y < 0 || y > 479 ) {
        tout << "Input cancelled.\n";
        return;
    }

    tout << "input Y = "<< y << '\n';

    std::this_thread::sleep_for(1000ms);

    int x_y = x << 16 | y;

    tout << std::endl << "input X_Y = 0x" << std::hex << x_y << std::dec << '\n';

    SDK::CrDeviceProperty prop;
    prop.SetCode(code);
    prop.SetCurrentValue((CrInt64u)x_y);
    prop.SetValueType(SDK::CrDataType::CrDataType_UInt32);
    SDK::SetDeviceProperty(m_device_handle, &prop);
}

void CameraDevice::execute_preset_focus()
{
    load_properties();

    auto& values_save = Prop.at(PCode::CrDeviceProperty_ZoomAndFocusPosition_Save).possible;
    auto& values_load = Prop.at(PCode::CrDeviceProperty_ZoomAndFocusPosition_Load).possible;

    if ((1 != Prop.at(PCode::CrDeviceProperty_ZoomAndFocusPosition_Save).writable) &&
        (1 != Prop.at(PCode::CrDeviceProperty_ZoomAndFocusPosition_Load).writable)){
        // Not a settable property
        tout << "Preset Focus and Zoom is not supported.\n";
        return;
    }

    tout << std::endl << "Save Zoom and Focus Position Enable Preset number: " << std::endl;
    for (int i = 0; i < values_save.size(); i++)
    {
        tout << " " << (int)values_save.at(i) << std::endl;
    }

    tout << std::endl << "Load Zoom and Focus Position Enable Preset number: " << std::endl;
    for (int i = 0; i < values_load.size(); i++)
    {
        tout << " " << (int)values_load.at(i) << std::endl;
    }

    tout << std::endl << "Set the value of operation:\n";
    tout << "[-1] Cancel input\n";

    tout << "[1] Save Zoom and Focus Position\n";
    tout << "[2] Load Zoom and Focus Position\n";

    tout << "[-1] Cancel input\n";
    tout << "Choose a number:\n";

    text input;
    tout << std::endl << "input> ";
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;

    CrInt32u code = 0;
    if ((1 == selected_index) && (1 == Prop.at(PCode::CrDeviceProperty_ZoomAndFocusPosition_Save).writable)) {
        code = PCode::CrDeviceProperty_ZoomAndFocusPosition_Save;
    }
    else if ((2 == selected_index) && (1 == Prop.at(PCode::CrDeviceProperty_ZoomAndFocusPosition_Load).writable)) {
        code = PCode::CrDeviceProperty_ZoomAndFocusPosition_Load;
    }
    else {
        tout << "Input cancelled.\n";
        return;
    }

    tout << "Set the value of Preset number:\n";

    tout << std::endl << "input> ";
    std::getline(tin, input);
    text_stringstream ss_slot(input);
    int input_value = 0;
    ss_slot >> input_value;

    if (code == PCode::CrDeviceProperty_ZoomAndFocusPosition_Save) {
        if (find(values_save.begin(), values_save.end(), input_value) == values_save.end()) {
            tout << "Input cancelled.\n";
            return;
        }
    }
    else {
        if (find(values_load.begin(), values_load.end(), input_value) == values_load.end()) {
            tout << "Input cancelled.\n";
            return;
        }
    }

    SDK::CrDeviceProperty prop;
    prop.SetCode(code);
    prop.SetCurrentValue(input_value);
    prop.SetValueType(SDK::CrDataType::CrDataType_UInt8);
    SDK::SetDeviceProperty(m_device_handle, &prop);
}

void CameraDevice::execute_APS_C_or_Full()
{
    if (false == get_aps_c_or_full_switching_setting())
        return;

    text input;
    tout << std::endl << "Execute APS-C or FULL switching control of the camera ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip.\n";
        return;
    }
    // Get the latest status
    load_properties();
    if (SDK::CrAPS_C_or_Full_SwitchingEnableStatus::CrAPS_C_or_Full_Switching_Enable != Prop.at(PCode::CrDeviceProperty_APS_C_or_Full_SwitchingEnableStatus).current) {
        tout << std::endl << "APS-C/Full switching is not executable.\n";
        return;
    }
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_APS_C_or_Full_Switching, SDK::CrCommandParam_Down);
    std::this_thread::sleep_for(100ms);
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_APS_C_or_Full_Switching, SDK::CrCommandParam_Up);
    tout << std::endl << "Executed the switch between APS-C or Full.\n";
}

void CameraDevice::execute_movie_rec_toggle()
{
    if (false == get_movie_rec_button_toggle_enable_status()) {
        return;
    }

    text input;
    tout << std::endl << "Operate the movie recording button toggle? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip .\n";
        return;
    }

    tout << "Choose a number:\n";
    tout << "[-1] Cancel input\n";

    tout << "[1] Up" << '\n';
    tout << "[2] Down" << '\n';

    tout << "[-1] Cancel input\n";
    tout << "Choose a number:\n";

    tout << std::endl << "input> ";
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;

    if (selected_index < 0) {
        tout << "Input cancelled.\n";
        return;
    }

    CrInt64u ptpValue = 0;
    switch (selected_index) {
    case 1:
        ptpValue = SDK::CrCommandParam::CrCommandParam_Up;
        break;
    case 2:
        ptpValue = SDK::CrCommandParam::CrCommandParam_Down;
        break;
    default:
        selected_index = -1;
        break;
    }

    if (-1 == selected_index) {
        tout << "Input cancelled.\n";
        return;
    }
    // Get the latest status
    load_properties();
    if (SDK::CrMovieRecButtonToggleEnableStatus::CrMovieRecButtonToggle_Enable != Prop.at(PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus).current) {
        tout << "Unable to execute the movie recording button toggle.\n";
        return;
    }
    SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_MovieRecButtonToggle, (SDK::CrCommandParam)ptpValue);

}

void CameraDevice::execute_focus_bracket()
{
    load_properties();

    if (1 != Prop.at(PCode::CrDeviceProperty_FocusBracketShotNumber).writable || 1 != Prop.at(PCode::CrDeviceProperty_FocusBracketFocusRange).writable) {
        tout << "Focus Bracket Shooting is not executable\n";
        return;
    }

    tout << "Execute Focus Bracket shooting \n";

    // Set, PriorityKeySettings property
    auto err = SetProp(PCode::CrDeviceProperty_PriorityKeySettings, SDK::CrPriorityKeySettings::CrPriorityKey_PCRemote);
    if (CR_FAILED(err)) {
        tout << "Priority Key setting FAILED\n";
        return;
    }

    // Set, FocusBracket
    bool modeset_flg = set_drive_mode(SDK::CrDriveMode::CrDrive_FocusBracket);
    if (!modeset_flg) {
        tout << "Still Capture Mode setting FAILED\n";
        return;
    }

    bool continueFlag = false;
    for (int i = 0; i < 10; i++)
    {
        tout << ". ";
        std::this_thread::sleep_for(500ms);
        load_properties();
        if (SDK::CrDriveMode::CrDrive_FocusBracket == Prop.at(PCode::CrDeviceProperty_DriveMode).current) {
            tout << "\nStill Capture Mode setting SUCCESS\n";
            continueFlag = true;
            break;
        }
    }
    if (false == continueFlag) {
        tout << "\nStill Capture Mode setting FAILED\n";
        return;
    }

    capture_image();
}

void CameraDevice::do_download_camera_setting_file()
{
    if (false == get_camera_setting_saveread_state())
        return;

    if (Prop.at(PCode::CrDeviceProperty_CameraSetting_SaveOperationEnableStatus).current == SDK::CrCameraSettingSaveOperation::CrCameraSettingSaveOperation_Enable) {
        tout << "Camera-Setting Save Operation: Enable\n";
    } else {
        tout << "Camera-Setting Save Operation: Disable\n";
    }

    // File Name
    tout << "\nPlease Enter the name of the file you want to save. \n";
    tout << "(ex. CUMSET.DAT)\n";
    tout << "If you do not specify a file name, the file will be saved with the default save name.\n" << std::endl;
    tout << "[-1] Cancel input\n" << std::endl;
    tout << "file name > ";
    text name;
    std::getline(tin, name);
    text_stringstream ss(name);
    int selected_index = 0;
    ss >> selected_index;
    if (-1 == selected_index) {
        tout << "Input cancelled.\n";
        return;
    }

    // Get the latest status
    load_properties();
    if (SDK::CrCameraSettingSaveOperation::CrCameraSettingSaveOperation_Enable != Prop.at(PCode::CrDeviceProperty_CameraSetting_SaveOperationEnableStatus).current) {
        tout << "Unable to download Camera-Setting file. \n";
        return;
    }
    // File Path
#if defined(__APPLE__)
    char path[MAC_MAX_PATH]; /*MAX_PATH*/
    memset(path, 0, sizeof(path));
    if(NULL == getcwd(path, sizeof(path) - 1)){
        tout << "Folder path is too long.\n";
        return;
    }
    char* delimit = "/";
    if(strlen(path) + strlen(delimit) > MAC_MAX_PATH){
        tout << "Folder path is too long.\n";
        return;
    }
    strncat(path, delimit, strlen(delimit));
    auto err = SDK::DownloadSettingFile(m_device_handle, SDK::CrDownloadSettingFileType::CrDownloadSettingFileType_Setup,(CrChar*)path, (CrChar*)name.c_str());

if (CR_FAILED(err)) {
    tout << "Download Camera-Setting file FAILED\n";
}
#else
    auto path = fs::current_path();
    auto err = SDK::DownloadSettingFile(m_device_handle, SDK::CrDownloadSettingFileType::CrDownloadSettingFileType_Setup,(CrChar*)path.c_str(), (CrChar*)name.c_str());

    if (CR_FAILED(err)) {
        tout << "Download Camera-Setting file FAILED\n";
    }
#endif
}

void CameraDevice::do_upload_camera_setting_file()
{
    if (false == get_camera_setting_saveread_state())
        return;

    if (Prop.at(PCode::CrDeviceProperty_CameraSetting_ReadOperationEnableStatus).current == SDK::CrCameraSettingReadOperation::CrCameraSettingReadOperation_Enable) {
        tout << "Camera-Setting Read Operation: Enable\n";
    } else {
        tout << "Camera-Setting Read Operation: Disable\n";
    }

    tout << "Have the camera load the configuration file on the PC.\n";

    //Search for *.DAT in current_path
    std::vector<text> file_names;
    getFileNames(file_names);

    if (file_names.size() == 0) {
        tout << "DAT file not found.\n\n";
        return;
    }
    
    tout << std::endl << "Choose a number:\n";
    tout << "[-1] Cancel input" << std::endl;
    for (size_t i = 0; i < file_names.size(); i++)
    {
        tout << "[" << i << "]" << file_names[i] << '\n';
    }
    tout << "[-1] Cancel input\n" << std::endl;

    tout << "input>";
    text input;
    std::getline(tin, input);

    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;
    if ((selected_index == 0 && input != TEXT("0")) || selected_index < 0 || selected_index >= file_names.size()) {
        tout << "Input cancelled.\n";
        return;
    }

#if defined(__APPLE__)
    char filename[MAC_MAX_PATH]; /*MAX_PATH*/
    memset(filename, 0, sizeof(filename));
    if(NULL == getcwd(filename, sizeof(filename) - 1)){
        tout << "Folder path is too long.\n";
        return;
    }
    char* delimit = "/";
    if(strlen(filename) + strlen(delimit) + file_names[selected_index].length() > MAC_MAX_PATH){
        tout << "Please shorten the file path. \n";
        return;
    }
    strncat(filename, delimit, strlen(delimit));
    strncat(filename, file_names[selected_index].c_str(), file_names[selected_index].length());
#else 
    auto filepath = fs::current_path();
    filepath.append(file_names[selected_index]);
    CrChar* filename = (CrChar*)filepath.c_str();
#endif

    tout << filename << "\n";
    // Get the latest status
    load_properties();
    if (SDK::CrCameraSettingReadOperation::CrCameraSettingReadOperation_Enable != Prop.at(PCode::CrDeviceProperty_CameraSetting_ReadOperationEnableStatus).current) {
        tout << "Unable to upload Camera-Setting file. \n";
        return;
    }

    auto err = SDK::UploadSettingFile(m_device_handle, SDK::CrUploadSettingFileType::CrUploadSettingFileType_Setup, filename);

    if (CR_FAILED(err)) {
        tout << "Upload Camera-Setting file FAILED\n";
    }
}

void CameraDevice::getFileNames(std::vector<text> &file_names)
{
#if defined(__APPLE__)
    char search_name[MAC_MAX_PATH]; /*MAX_PATH*/
    memset(search_name, 0, sizeof(search_name));
    if(NULL == getcwd(search_name, sizeof(search_name) - 1)){
        tout << "Folder path is too long.\n";
        return;
    }
#else
    auto search_name = fs::current_path();
#if defined (WIN32) || defined(WIN64)
    search_name.append(TEXT("*.DAT"));
#endif
#endif

#if defined(__APPLE__) || defined(__linux__)
    DIR* dp;
    int i = 0;
    struct dirent* ep;
#if defined(__APPLE__)
    dp = opendir(search_name);
#else
    dp = opendir(search_name.c_str());
#endif
    if (dp != NULL)
    {
        while ((ep = readdir(dp)) != NULL) {
            if (ep->d_name[0] == '.') {
            }
            else {
                text    str(ep->d_name);
                if(((ep->d_name[str.length()-4]=='.')
                	&&(ep->d_name[str.length()-3]=='d')
                	&&(ep->d_name[str.length()-2]=='a')
                	&&(ep->d_name[str.length()-1]=='t'))
                	||((ep->d_name[str.length()-4]=='.')
                	&&(ep->d_name[str.length()-3]=='D')
                	&&(ep->d_name[str.length()-2]=='A')
                	&&(ep->d_name[str.length()-1]=='T'))){
                file_names.push_back(str);
                i++;
                }
            }
        }
        (void)closedir(dp);
    }
    // End Mac & Linux implementation
#else

    WIN32_FIND_DATA win32fd;
    HANDLE hff = FindFirstFile(search_name.c_str(), &win32fd);

    if (hff != INVALID_HANDLE_VALUE) {
        do {
            if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            }
            else {
                text str(win32fd.cFileName);
                file_names.push_back(str);
            }
        } while (FindNextFile(hff, &win32fd));
        FindClose(hff);
    }
    else {
        return ;
    }
    return ;
#endif
}

void CameraDevice::change_live_view_enable()
{
    m_lvEnbSet = !m_lvEnbSet;
    SDK::SetDeviceSetting(m_device_handle, SDK::Setting_Key_EnableLiveView, (CrInt32u)m_lvEnbSet);
}

///////////////////////////////////////////////

bool CameraDevice::is_connected() const
{
    return m_connected.load();
}

std::uint32_t CameraDevice::ip_address() const
{
    if (m_conn_type == ConnectionType::NETWORK)
        return m_net_info.ip_address;
    return 0;
}

text CameraDevice::ip_address_fmt() const
{
    if (m_conn_type == ConnectionType::NETWORK)
    {
        return m_net_info.ip_address_fmt;
    }
    return text(TEXT("N/A"));
}

text CameraDevice::mac_address() const
{
    if (m_conn_type == ConnectionType::NETWORK)
        return m_net_info.mac_address;
    return text(TEXT("N/A"));
}

std::int16_t CameraDevice::pid() const
{
    if (m_conn_type == ConnectionType::USB)
        return m_usb_info.pid;
    return 0;
}

text CameraDevice::get_id()
{
    if (ConnectionType::NETWORK == m_conn_type) {
        return m_net_info.mac_address;
    }
    else
        return text((TCHAR*)m_info->GetId());
}

void CameraDevice::OnConnected(SDK::DeviceConnectionVersioin version)
{
    m_connected.store(true);
    text id(this->get_id());
    tout << "Connected to " << m_info->GetModel() << " (" << id.data() << ")\n";
}

void CameraDevice::OnDisconnected(CrInt32u error)
{
    m_connected.store(false);
    text id(this->get_id());
    tout << "Disconnected from " << m_info->GetModel() << " (" << id.data() << ")\n";
    if ((false == m_spontaneous_disconnection) && (SDK::CrSdkControlMode_ContentsTransfer == m_modeSDK))
    {
        tout << "Please input '0' to return to the TOP-MENU\n";
    }
}

void CameraDevice::OnPropertyChanged()
{
    // tout << "Property changed.\n";
}

void CameraDevice::OnLvPropertyChanged()
{
    // tout << "LvProperty changed.\n";
}

void CameraDevice::OnCompleteDownload(CrChar* filename, CrInt32u type )
{
    text file(filename);
    switch (type)
    {
    case SCRSDK::CrDownloadSettingFileType_None:
        tout << "Complete download. File: " << file.data() << '\n';
        uploadFile(filename);
        break;
    case SCRSDK::CrDownloadSettingFileType_Setup:
        tout << "Complete download. Camera Setting File: " << file.data() << '\n';
        break;
    default:
        break;
    }
}

void CameraDevice::OnNotifyContentsTransfer(CrInt32u notify, SDK::CrContentHandle contentHandle, CrChar* filename)
{
    // Start
    if (SDK::CrNotify_ContentsTransfer_Start == notify)
    {
        tout << "[START] Contents Handle: 0x " << std::hex << contentHandle << std::dec << std::endl;
    }
    // Complete
    else if (SDK::CrNotify_ContentsTransfer_Complete == notify)
    {
        text file(filename);
        tout << "[COMPLETE] Contents Handle: 0x" << std::hex << contentHandle << std::dec << ", File: " << file.data() << std::endl;
    }
    // Other
    else
    {
        text msg = get_message_desc(notify);
        if (msg.empty()) {
            tout << "[-] Content transfer failure. 0x" << std::hex << notify << ", handle: 0x" << contentHandle << std::dec << std::endl;
        } else {
            tout << "[-] Content transfer failure. handle: 0x" << std::hex << contentHandle  << std::dec << std::endl << "    -> ";
            tout << msg.data() << std::endl;
        }
    }
}

void CameraDevice::OnWarning(CrInt32u warning)
{
    text id(this->get_id());
    if (SDK::CrWarning_Connect_Reconnecting == warning) {
        tout << "Device Disconnected. Reconnecting... " << m_info->GetModel() << " (" << id.data() << ")\n";
        return;
    }
    switch (warning)
    {
    case SDK::CrWarning_ContentsTransferMode_Invalid:
    case SDK::CrWarning_ContentsTransferMode_DeviceBusy:
    case SDK::CrWarning_ContentsTransferMode_StatusError:
        tout << "\nThe camera is in a condition where it cannot transfer content.\n\n";
        tout << "Please input '0' to return to the TOP-MENU and connect again.\n";
        break;
    case SDK::CrWarning_ContentsTransferMode_CanceledFromCamera:
        tout << "\nContent transfer mode canceled.\n";
        tout << "If you want to continue content transfer, input '0' to return to the TOP-MENU and connect again.\n\n";
        break;
    case SDK::CrWarning_CameraSettings_Read_Result_OK:
        tout << "\nConfiguration file read successfully.\n\n";
        break;
    case SDK::CrWarning_CameraSettings_Read_Result_NG:
        tout << "\nFailed to load configuration file\n\n";
        break;
    case SDK::CrWarning_CameraSettings_Save_Result_NG:
        tout << "\nConfiguration file save request failed.\n\n";
        break;
    case SDK::CrWarning_RequestDisplayStringList_Success:
        tout << "\nRequest for DisplayStringList  successfully\n\n";        
        break;
    case SDK::CrWarning_RequestDisplayStringList_Error: 
        tout << "\nFailed to Request for DisplayStringList\n\n";
        break;
    case SDK::CrWarning_CustomWBCapture_Result_OK:
        tout << "\nCustom WB capture successful.\n\n";
        break;
    case SDK::CrWarning_CustomWBCapture_Result_Invalid:
    case SDK::CrWarning_CustomWBCapture_Result_NG:
        tout << "\nCustom WB capture failure.\n\n";
        break;
    case SDK::CrWarning_FocusPosition_Result_Invalid:
        tout << "\nFocus Position Result Invalid.\n\n";
        break;
    case SDK::CrWarning_FocusPosition_Result_OK:
        tout << "\nFocus Position Result OK.\n\n";
        break;
    case SDK::CrWarning_FocusPosition_Result_NG:
        tout << "\nFocus Position Result NG.\n\n";
        break;
    default:
        return;
    }
}

void CameraDevice::OnWarningExt(CrInt32u warning, CrInt32 param1, CrInt32 param2, CrInt32 param3)
{
    tout << "<Receive>\n";
#if defined(_WIN64)
    printf_s("warning: 0x%08X\n", warning);
    printf_s(" param1: 0x%08X\n", param1);
    printf_s(" param2: 0x%08X\n", param2);
    printf_s(" param3: 0x%08X\n", param3);
#else // temporary
    printf("warning: 0x%08X\n", warning);
    printf(" param1: 0x%08X\n", param1);
    printf(" param2: 0x%08X\n", param2);
    printf(" param3: 0x%08X\n", param3);
#endif
    tout << "\n<warning>\n";
    tout << " 0x00060001: CrWarningExt_AFStatus\n";
    tout << "             <param1> Focus Indication\n";
    tout << " 0x00060002: CrWarningExt_OperationResults\n";
    tout << "             <param1> enum CrSdkApi\n";
    tout << "                      0x00000002: CrSdkApi_SetDeviceProperty\n";
    tout << "                      0x00000003: CrSdkApi_SendCommand\n";
    tout << "             <param2> CrDevicePropertyCode or CrCommandId\n";
    tout << "             <param3> enum CrWarningExt_OperationResultsParam\n";
    tout << "                      0x00000000: CrWarningExt_OperationResultsParam_Invalid\n";
    tout << "                      0x00000001: CrWarningExt_OperationResultsParam_OK\n";
    tout << "                      0x00000002: CrWarningExt_OperationResultsParam_NG\n";
}

void CameraDevice::OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
    //tout << "Property changed.  num = " << std::dec << num;
    //tout << std::hex;
    //for (std::int32_t i = 0; i < num; ++i)
    //{
    //    tout << ", 0x" << codes[i];
    //}
    //tout << std::endl << std::dec;
	for (std::int32_t i = 0; i < num; ++i)
	{
		auto id = static_cast<PCode>(codes[i]);
		switch(id) {
		case PCode::CrDeviceProperty_FocusIndication:
			SendProp(id);
			break;
		default:
			break;
		}
	}
}

void CameraDevice::OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
    //tout << "LvProperty changed.  num = " << std::dec << num;
    //tout << std::hex;
    //for (std::int32_t i = 0; i < num; ++i)
    //{
    //    tout << ", 0x" << codes[i];
    //}
    //tout << std::endl;
#if 0 
    SDK::CrLiveViewProperty* lvProperty = nullptr;
    int32_t nprop = 0;
    SDK::CrError err = SDK::GetSelectLiveViewProperties(m_device_handle, num, codes, &lvProperty, &nprop);
    if (CR_SUCCEEDED(err) && lvProperty) {
        for (int32_t i=0 ; i<nprop ; i++) {
            auto prop = lvProperty[i];
            if (SDK::CrFrameInfoType::CrFrameInfoType_FocusFrameInfo == prop.GetFrameInfoType()) {
                int sizVal = prop.GetValueSize();
                int count = sizVal / sizeof(SDK::CrFocusFrameInfo);
                SDK::CrFocusFrameInfo* pFrameInfo = (SDK::CrFocusFrameInfo*)prop.GetValue();
                if (0 == sizVal || nullptr == pFrameInfo) {
                    tout << "  FocusFrameInfo nothing\n";
                }
                else {
                    for (std::int32_t frame = 0; frame < count; ++frame) {
                        auto lvprop = pFrameInfo[frame];
                        char buff[512];
                        memset(buff, 0, sizeof(buff));
#if defined(_WIN32) || (_WIN64)
                        sprintf_s(buff, "  FocusFrameInfo no[%d] type[%d] state[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                            frame + 1,
                            lvprop.type,
                            lvprop.state,
                            lvprop.width, lvprop.height,
                            lvprop.xDenominator, lvprop.yDenominator,
                            lvprop.xNumerator, lvprop.yNumerator);
#else
                        snprintf(buff, sizeof(buff), "  FocusFrameInfo no[%d] type[%d] state[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                            frame + 1,
                            lvprop.type,
                            lvprop.state,
                            lvprop.width, lvprop.height,
                            lvprop.xDenominator, lvprop.yDenominator,
                            lvprop.xNumerator, lvprop.yNumerator);
#endif                           
                        tout << buff << std::endl;
                    }
                }
            }
            else if (SDK::CrFrameInfoType::CrFrameInfoType_FaceFrameInfo == prop.GetFrameInfoType()) {
                int sizVal = prop.GetValueSize();
                int count = sizVal / sizeof(SDK::CrFaceFrameInfo);
                SDK::CrFaceFrameInfo* pFrameInfo = (SDK::CrFaceFrameInfo*)prop.GetValue();
                if (0 == sizVal || nullptr == pFrameInfo) {
                    tout << "  FaceFrameInfo nothing\n";
                }
                else {
                    for (std::int32_t frame = 0; frame < count; ++frame) {
                        auto lvprop = pFrameInfo[frame];
                        char buff[512];
                        memset(buff, 0, sizeof(buff));
#if defined(_WIN32) || (_WIN64)
                        sprintf_s(buff, "  FaceFrameInfo no[%d] type[%d] state[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                            frame + 1,
                            lvprop.type,
                            lvprop.state,
                            lvprop.width, lvprop.height,
                            lvprop.xDenominator, lvprop.yDenominator,
                            lvprop.xNumerator, lvprop.yNumerator);
#else
                        snprintf(buff, sizeof(buff), "  FaceFrameInfo no[%d] type[%d] state[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                            frame + 1,
                            lvprop.type,
                            lvprop.state,
                            lvprop.width, lvprop.height,
                            lvprop.xDenominator, lvprop.yDenominator,
                            lvprop.xNumerator, lvprop.yNumerator);
#endif                           
                        tout << buff << std::endl;
                    }
                }
            }
            else if (SDK::CrFrameInfoType::CrFrameInfoType_TrackingFrameInfo == prop.GetFrameInfoType()) {
                int sizVal = prop.GetValueSize();
                int count = sizVal / sizeof(SDK::CrTrackingFrameInfo);
                SDK::CrTrackingFrameInfo* pFrameInfo = (SDK::CrTrackingFrameInfo*)prop.GetValue();
                if (0 == sizVal || nullptr == pFrameInfo) {
                    tout << "  TrackingFrameInfo nothing\n";
                }
                else {
                    for (std::int32_t frame = 0; frame < count; ++frame) {
                        auto lvprop = pFrameInfo[frame];
                        char buff[512];
                        memset(buff, 0, sizeof(buff));
#if defined(_WIN32) || (_WIN64)
                        sprintf_s(buff, "  TrackingFrameInfo no[%d] type[%d] state[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                            frame + 1,
                            lvprop.type,
                            lvprop.state,
                            lvprop.width, lvprop.height,
                            lvprop.xDenominator, lvprop.yDenominator,
                            lvprop.xNumerator, lvprop.yNumerator);
#else
                        snprintf(buff, sizeof(buff), "  TrackingFrameInfo no[%d] type[%d] state[%d] w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                            frame + 1,
                            lvprop.type,
                            lvprop.state,
                            lvprop.width, lvprop.height,
                            lvprop.xDenominator, lvprop.yDenominator,
                            lvprop.xNumerator, lvprop.yNumerator);
#endif                           
                        tout << buff << std::endl;
                    }
                }
            }
            else if (SDK::CrFrameInfoType::CrFrameInfoType_Magnifier_Position == prop.GetFrameInfoType()) {
                int sizVal = prop.GetValueSize();
                int count = sizVal / sizeof(SDK::CrMagPosInfo);
                SDK::CrMagPosInfo* pMagPosInfo = (SDK::CrMagPosInfo*)prop.GetValue();
                if (0 == sizVal || nullptr == pMagPosInfo) {
                    tout << "  MagPosInfo nothing\n";
                }
                else {
                    char buff[512];
                    memset(buff, 0, sizeof(buff));
#if defined(_WIN32) || (_WIN64)
                    sprintf_s(buff, "  MagPosInfo w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                        pMagPosInfo->width, pMagPosInfo->height,
                        pMagPosInfo->xDenominator, pMagPosInfo->yDenominator,
                        pMagPosInfo->xNumerator, pMagPosInfo->yNumerator);
#else
                    snprintf(buff, sizeof(buff), "  MagPosInfo w[%d] h[%d] Deno[%d-%d] Nume[%d-%d]",
                        pMagPosInfo->width, pMagPosInfo->height,
                        pMagPosInfo->xDenominator, pMagPosInfo->yDenominator,
                        pMagPosInfo->xNumerator, pMagPosInfo->yNumerator);
#endif
                    tout << buff << std::endl;
                }
            }
        }
        SDK::ReleaseLiveViewProperties(m_device_handle, lvProperty);
    }
#endif
    tout << std::dec;
}

void CameraDevice::OnError(CrInt32u error)
{
    text id(this->get_id());
    text msg = get_message_desc(error);
    if (!msg.empty()) {
        // output is 2 line
        tout << std::endl << msg.data() << std::endl;
        tout << m_info->GetModel() << " (" << id.data() << ")" << std::endl;
        if (SDK::CrError_Connect_TimeOut == error) {
            // append 1 line
            tout << "Please input '0' after Connect camera" << std::endl;
            return;
        }
        if (SDK::CrError_Connect_Disconnected == error)
        {
            return;
        }
        if (SDK::CrError_Connect_SSH_ServerConnectFailed == error
            || SDK::CrError_Connect_SSH_InvalidParameter == error
            || SDK::CrError_Connect_SSH_ServerAuthenticationFailed == error
            || SDK::CrError_Connect_SSH_UserAuthenticationFailed == error
            || SDK::CrError_Connect_SSH_PortForwardFailed == error
            || SDK::CrError_Connect_SSH_GetFingerprintFailed == error)
        {
            m_fingerprint.clear();
            m_userPassword.clear();
        }
        tout << "Please input '0' to return to the TOP-MENU\n";
    }
}

void CameraDevice::parse_para(SDK::CrDeviceProperty& devProp, PCode id)
{
	PropertyValue& prop = Prop.at(id);

	prop.writable = devProp.IsSetEnableCurrentValue();
	prop.current = devProp.GetCurrentValue();

	unsigned char const* buf = devProp.GetValues();
	std::uint32_t nval = devProp.GetValueSize();

	switch(prop.dataType) {
	case SDK::CrDataType::CrDataType_UInt8:		nval /= sizeof(std::uint8_t); break;
	case SDK::CrDataType::CrDataType_Int8:		nval /= sizeof(std::int8_t); break;
	case SDK::CrDataType::CrDataType_UInt16:	nval /= sizeof(std::uint16_t); break;
	case SDK::CrDataType::CrDataType_Int16:		nval /= sizeof(std::int16_t); break;
	case SDK::CrDataType::CrDataType_UInt32:	nval /= sizeof(std::uint32_t); break;
	default: break;
	}

	prop.possible.resize(nval);
	for (std::uint32_t i = 0; i < nval; ++i) {
		int32_t data = 0;
		switch(prop.dataType) {
		case SDK::CrDataType::CrDataType_UInt8:		data = (reinterpret_cast<std::uint8_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_Int8:		data = (reinterpret_cast<std::int8_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt16:	data = (reinterpret_cast<std::uint16_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_Int16:		data = (reinterpret_cast<std::int16_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt32:	data = (reinterpret_cast<std::uint32_t const*>(buf))[i]; break;
		default: break;
		}
		prop.possible.at(i) = data;
	}
}

std::vector<std::uint64_t> CameraDevice::parse_uint64(unsigned char const* buf, std::uint32_t nval)
{
    using TargetType = std::uint64_t;
    TargetType const* source = reinterpret_cast<TargetType const*>(buf);
    std::vector<TargetType> result(nval);
    for (std::uint32_t i = 0; i < nval; ++i) {
        result[i] = source[i];
    }
    return result;
}

void CameraDevice::load_properties(CrInt32u num, CrInt32u* codes)
{
    std::int32_t nprop = 0;
    SDK::CrDeviceProperty* prop_list = nullptr;

    Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).writable = -1;
    Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).writable = -1;

    SDK::CrError status = SDK::CrError_Generic;
    if (0 == num){
        // Get all
        status = SDK::GetDeviceProperties(m_device_handle, &prop_list, &nprop);
    }
    else {
        // Get difference
        status = SDK::GetSelectDeviceProperties(m_device_handle, num, codes, &prop_list, &nprop);
    }

    if (CR_FAILED(status)) {
        tout << "Failed to get device properties.\n";
        return;
    }

    if (prop_list && nprop > 0) {
#if 1
		for (std::int32_t i = 0; i < nprop; ++i) {
			auto devProp = prop_list[i];
			uint32_t id = devProp.GetCode();
			tout << std::hex << id << ":";
			auto iter = Prop.find((PCode)id);
			if ( iter != end(Prop) ) {
				tout << iter->second.tag.c_str() << "\n";
			} else {
				tout << "unknown\n";
			}
		}
#endif
        // Got properties list
        for (std::int32_t i = 0; i < nprop; ++i) {
            auto devProp = prop_list[i];

			PCode id = (PCode)devProp.GetCode();
			auto iter = Prop.find((PCode)id);
			if ( iter != end(Prop) ) {
				parse_para(devProp, id);
				switch(id) {
				case PCode::CrDeviceProperty_SdkControlMode:
					m_modeSDK = (SDK::CrSdkControlMode)Prop.at(id).current;
					break;
				}
			} else {
	            int nval = 0;
				switch(id) {
	            case PCode::CrDeviceProperty_ShutterSpeedValue:
	                nval = devProp.GetValueSize() / sizeof(std::uint64_t);
	                m_prop.shutter_speed_value.writable = devProp.IsSetEnableCurrentValue();
	                m_prop.shutter_speed_value.current = static_cast<std::uint64_t>(devProp.GetCurrentValue());
	                if (0 < nval) {
	                    auto parsed_values = parse_uint64(devProp.GetValues(), nval);
	                    m_prop.shutter_speed_value.possible.swap(parsed_values);
	                }
	                break;
	            case PCode::CrDeviceProperty_LensModelName: {
	                m_prop.lensModelNameStr.writable = devProp.IsSetEnableCurrentValue();
	                CrInt16u* pCurrentStr = devProp.GetCurrentStr();
	                if (pCurrentStr)
	                {
	                    m_prop.lensModelNameStr.length = (int)*pCurrentStr;
	#if defined(WIN32) || defined(_WIN64)//#if defined(_UNICODE) || defined(UNICODE)
	                    m_prop.lensModelNameStr.current = text((CrChar*)&pCurrentStr[1]);
	#else
	                    char buff[128];
	                    memset(buff, 0, sizeof(buff));
	                    pCurrentStr++;
	                    for (int i = 0; i < (m_prop.lensModelNameStr.length - 1); ++i,pCurrentStr++)
	                    {
	                        wctomb(&buff[i], (wchar_t)*pCurrentStr);
	                    }
	                    if (0 < strlen(buff))
	                    {
	                        m_prop.lensModelNameStr.current = text((CrChar*)buff);
	                    }
	#endif
	                }
	                break;
	              }
	            default:
	                break;
	            }
	        }
        }
        SDK::ReleaseDeviceProperties(m_device_handle, prop_list);
    }
}

void CameraDevice::getContentsList()
{
    // check status
    std::int32_t nprop = 0;
    SDK::CrDeviceProperty* prop_list = nullptr;
    CrInt32u getCode = PCode::CrDeviceProperty_ContentsTransferStatus;
    SDK::CrError res = SDK::GetSelectDeviceProperties(m_device_handle, 1, &getCode, &prop_list, &nprop);
    bool bExec = false;
    if (CR_SUCCEEDED(res) && (1 == nprop)) {
        if ((getCode == prop_list[0].GetCode()) && (SDK::CrContentsTransfer_ON == prop_list[0].GetCurrentValue()))
        {
            bExec = true;
        }
        SDK::ReleaseDeviceProperties(m_device_handle, prop_list);
    }
    if (false == bExec) {
        tout << "GetContentsListEnableStatus is Disable. Do it after it becomes Enable.\n";
        return;
    }

    for (CRFolderInfos* pF : m_foldList)
    {
        delete pF;
    }
    m_foldList.clear();
    for (SCRSDK::CrMtpContentsInfo* pC : m_contentList)
    {
        delete pC;
    }
    m_contentList.clear();

    CrInt32u f_nums = 0;
    CrInt32u c_nums = 0;
    SDK::CrMtpFolderInfo* f_list = nullptr;
    SDK::CrError err = SDK::GetDateFolderList(m_device_handle, &f_list, &f_nums);
    if (CR_SUCCEEDED(err) && 0 < f_nums)
    {
        if (f_list)
        {
            tout << "NumOfFolder [" << f_nums << "]" << std::endl;

            for (int i = 0; i < f_nums; ++i)
            {
                auto pFold = new SDK::CrMtpFolderInfo();
                pFold->handle = f_list[i].handle;
                pFold->folderNameSize = f_list[i].folderNameSize;
                CrInt32u lenByOS = sizeof(CrChar) * pFold->folderNameSize;
                pFold->folderName = new CrChar[lenByOS];
                MemCpyEx(pFold->folderName, f_list[i].folderName, lenByOS);
                CRFolderInfos* pCRF = new CRFolderInfos(pFold, 0); // 2nd : fill in later
                m_foldList.push_back(pCRF);
            }
            SDK::ReleaseDateFolderList(m_device_handle, f_list);
        }

        if (0 == m_foldList.size())
        {
            return;
        }

        MtpFolderList::iterator it = m_foldList.begin();
        for (int fcnt = 0; it != m_foldList.end(); ++fcnt, ++it)
        {
            SDK::CrContentHandle* c_list = nullptr;
            err = SDK::GetContentsHandleList(m_device_handle, (*it)->pFolder->handle, &c_list, &c_nums);
            if (CR_SUCCEEDED(err) && 0 < c_nums)
            {
                if (c_list)
                {
                    tout << "(" << (fcnt + 1) << "/" << f_nums << ") NumOfContents [" << c_nums << "]" << std::endl;
                    (*it)->numOfContents = c_nums;
                    for (int i = 0; i < c_nums; i++)
                    {
                        SDK::CrMtpContentsInfo* pContents = new SDK::CrMtpContentsInfo();
                        err = SDK::GetContentsDetailInfo(m_device_handle, c_list[i], pContents);
                        if (CR_SUCCEEDED(err))
                        {
                            m_contentList.push_back(pContents);
                            // progress
                            if (0 == ((i + 1) % 100))
                            {
                                tout << "  ... " << (i + 1) << "/" << c_nums << std::endl;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    SDK::ReleaseContentsHandleList(m_device_handle, c_list);
                }
            }
            if (CR_FAILED(err))
            {
                break;
            }
        }
    }
    else if (CR_SUCCEEDED(err) && 0 == f_nums)
    {
        tout << "No images in memory card." << std::endl;
        return;
    }
    else
    {
        // err
        tout << "Failed SDK::GetContentsList()" << std::endl;
        return;
    }

    if (CR_SUCCEEDED(err))
    {
        MtpFolderList::iterator itF = m_foldList.begin();
        for (std::int32_t f_sep = 0; itF != m_foldList.end(); ++f_sep, ++itF)
        {
            text fname((*itF)->pFolder->folderName);
 
            tout << "===== ";
            tout.fill(' ');
            tout.width(3);
            tout << (f_sep + 1) << ": ";
            
            tout << fname;

            tout << " (0x";
            std::ostringstream f_handle_hex;
            f_handle_hex << std::hex << std::uppercase << (*itF)->pFolder->handle;
            tout << std::dec;
            std::string f_handle_str = f_handle_hex.str();
            f_handle_str.erase(std::remove(f_handle_str.begin(), f_handle_str.end(), ','), f_handle_str.end());
            const char* f_handle_char = f_handle_str.c_str();
            tout.fill('0');
            tout.width(8);
            tout << f_handle_char << ") , ";
            tout << "contents[" << (*itF)->numOfContents << "] ===== \n";

            MtpContentsList::iterator itC = m_contentList.begin();
            for (std::int32_t i = 0; itC != m_contentList.end(); ++i, ++itC)
            {
                if ((*itC)->parentFolderHandle == (*itF)->pFolder->handle)
                {
                    text fname((*itC)->fileName);

                    tout << "  ";
                    tout.fill(' ');
                    tout.width(3);
                    tout << (i + 1);
                    tout << ": (0x";
                    std::ostringstream c_handle_hex;
                    c_handle_hex << std::hex << std::uppercase << (*itC)->handle;
                    tout << std::dec;
                    std::string c_handle_str = c_handle_hex.str();
                    c_handle_str.erase(std::remove(c_handle_str.begin(), c_handle_str.end(), ','), c_handle_str.end());
                    const char* c_handle_char = c_handle_str.c_str();
                    tout.fill('0');
                    tout.width(8);
                    tout << c_handle_char << "), ";
                 
                    tout << fname << std::endl;
                }
            }
        }

        while (1)
        {
            if (m_connected == false) {
                break;
            }
            text input;
            tout << std::endl << "Select the number of the contents you want to download:";
            tout << std::endl << "(Returns to the previous menu for invalid numbers)" << std::endl << std::endl;
            tout << std::endl << "input> ";
            std::getline(tin, input);
            text_stringstream ss(input);
            int selected_index = 0;
            ss >> selected_index;
            if (selected_index < 1 || m_contentList.size() < selected_index)
            {
                if (m_connected != false) {
                    tout << "Input cancelled.\n";
                }
                break;
            }
            else
            {
                while (1)
                {
                    if (m_connected == false) {
                        break;
                    }
                    auto targetHandle = m_contentList[selected_index - 1]->handle;

                    tout << "Selected(0x ";
                    std::ostringstream targetHandle_hex;
                    targetHandle_hex << std::hex << std::uppercase << targetHandle;
                    tout << std::dec;
                    std::string targetHandle_str = targetHandle_hex.str();
                    targetHandle_str.erase(std::remove(targetHandle_str.begin(), targetHandle_str.end(), ','), targetHandle_str.end());
                    const char* targetHandle_char = targetHandle_str.c_str();
                    tout.fill('0');
                    tout.width(4);
                    tout << targetHandle_char << ") ... \n";


                    text input;
                    tout << std::endl << "Select the number of the content size you want to download:";
                    tout << std::endl << "[-1] Cancel input";
                    tout << std::endl << "[1] Original";
                    tout << std::endl << "[2] Thumbnail";
                    text selected_filename(m_contentList[selected_index - 1]->fileName);
                    text ext = selected_filename.substr(selected_filename.length() - 4, 4);
                    int checkMax = 2;
                    if ((0 == ext.compare(TEXT(".JPG"))) || 
                        (0 == ext.compare(TEXT(".ARW"))) || 
                        (0 == ext.compare(TEXT(".HIF"))))
                    {
                        checkMax = 3;
                        tout << std::endl << "[3] 2M" << std::endl;
                    }

                    tout << std::endl << "input> ";
                    std::getline(tin, input);
                    text_stringstream ss(input);
                    int selected_contentSize = 0;
                    ss >> selected_contentSize;
                    if (m_connected == false) {
                        break;
                    }
                    if (selected_contentSize < 1 || checkMax < selected_contentSize)
                    {
                        if (m_connected != false) {
                            tout << "Input cancelled.\n";
                        }
                        break;
                    }
                    switch (selected_contentSize)
                    {
                    case 1:
                        // [async] get contents
                        pullContents(targetHandle);
                        break;
                    case 2:
                        // [sync] get thumbnail jpeg
                        getThumbnail(targetHandle);
                        break;
                    case 3:
                        // [async] [only still] get screennail jpeg
                        getScreennail(targetHandle);
                        break;
                    default:
                        break;
                    }
                    std::this_thread::sleep_for(2s);
                }
            }
        }
    }
}

void CameraDevice::pullContents(SDK::CrContentHandle content)
{
    SDK::CrError err = SDK::PullContentsFile(m_device_handle, content);

    if (SDK::CrError_None != err)
    {
        text id(this->get_id());
        text msg = get_message_desc(err);
        if (!msg.empty()) {
            // output is 2 line
            tout << std::endl << msg.data() << ", handle=" << std::hex << content << std::dec << std::endl;
            tout << m_info->GetModel() << " (" << id.data() << ")" << std::endl;
        }
    }
}

void CameraDevice::getScreennail(SDK::CrContentHandle content)
{
    SDK::CrError err = SDK::PullContentsFile(m_device_handle, content, SDK::CrPropertyStillImageTransSize_SmallSize);

    if (SDK::CrError_None != err)
    {
        text id(this->get_id());
        text msg = get_message_desc(err);
        if (!msg.empty()) {
            // output is 2 line
            tout << std::endl << msg.data() << ", handle=" << std::hex << content << std::dec << std::endl;
            tout << m_info->GetModel() << " (" << id.data() << ")" << std::endl;
        }
    }
}

void CameraDevice::getThumbnail(SDK::CrContentHandle content)
{
    CrInt32u bufSize = 0x28000; // @@@@ temp

    auto* image_data = new SDK::CrImageDataBlock();
    if (!image_data)
    {
        tout << "getThumbnail FAILED (new CrImageDataBlock class)\n";
        return;
    }
    CrInt8u* image_buff = new CrInt8u[bufSize];
    if (!image_buff)
    {
        delete image_data;
        tout << "getThumbnail FAILED (new Image buffer)\n";
        return;
    }
    image_data->SetSize(bufSize);
    image_data->SetData(image_buff);

    SDK::CrFileType fileType = SDK::CrFileType_None;
    SDK::CrError err = SDK::GetContentsThumbnailImage(m_device_handle, content, image_data, &fileType);
    if (CR_FAILED(err))
    {
        text id(this->get_id());
        text msg = get_message_desc(err);
        if (!msg.empty()) {
            // output is 2 line
            tout << std::endl << msg.data() << ", handle=" << std::hex << content << std::dec << std::endl;
            tout << m_info->GetModel() << " (" << id.data() << ")" << std::endl;
        }
    }
    else
    {
        if (0 < image_data->GetSize() && fileType != SDK::CrFileType_None)
        {
            text filename(TEXT("Thumbnail.JPG"));
            if (fileType == SDK::CrFileType_Heif) {
                filename= (TEXT("Thumbnail.HIF"));
            }

#if defined(__APPLE__)
            char path[MAC_MAX_PATH]; /*MAX_PATH*/
            memset(path, 0, sizeof(path));
            if(NULL == getcwd(path, sizeof(path) - 1)){
                // FAILED
                delete[] image_buff; // Release
                delete image_data; // Release
                tout << "Folder path is too long.\n";
                return;
            };
            char* delimit = "/";
            if(strlen(path) + strlen(delimit) + filename.length() > MAC_MAX_PATH){
                // FAILED
                delete[] image_buff; // Release
                delete image_data; // Release
                tout << "Failed to create save path\n";
                return;
            }
            strncat(path, delimit, strlen(delimit));
            strncat(path, (CrChar*)filename.c_str(), filename.length());
#else
            auto path = fs::current_path();
            path.append(filename);
#endif
            tout << path << '\n';

            std::ofstream file(path, std::ios::out | std::ios::binary);
            if (!file.bad())
            {
                std::uint32_t len = image_data->GetImageSize();
                file.write((char*)image_data->GetImageData(), len);
                file.close();
            }
        }
    }
    delete[] image_buff; // Release
    delete image_data; // Release
}

text CameraDevice::format_display_string_type(SDK::CrDisplayStringType type) {
    text_stringstream ts;
    switch (type)
    {
    case SCRSDK::CrDisplayStringType_AllList:
        ts << "All Display List";
        break;
    case SCRSDK::CrDisplayStringType_BaseLook_AELevelOffset_ExposureValue:
        ts << "[BaseLook] AELevelOffset ExposureValue";
        break;
    case SCRSDK::CrDisplayStringType_BaseLook_Input_Display:
        ts << "[BaseLook] Input Display";
        break;
    case SCRSDK::CrDisplayStringType_BaseLook_Name_Display:
        ts << "[BaseLook] Name Display";
        break;
    case SCRSDK::CrDisplayStringType_BaseLook_Output_Display:
        ts << "[BaseLook] Output Display";
        break;
    case SCRSDK::CrDisplayStringType_SceneFile_Name_Display:
        ts << "[SceneFile] Name Display";
        break;
    case SCRSDK::CrDisplayStringType_ShootingMode_Cinema_ColorGamut_Display:
        ts << "[ShootingMode] Cinema ColorGamut Display";
        break;
    case SCRSDK::CrDisplayStringType_ShootingMode_TargetDisplay_Display:
        ts << "[ShootingMode] TargetDisplay Display";
        break;
    case SCRSDK::CrDisplayStringType_Camera_Gain_BaseISO_Display:
        ts << "[Camera Gain BaseISO] Display";
        break;
    case SCRSDK::CrDisplayStringType_Video_EIGain_Display:
        ts << "[Video EIGain] Display";
        break;
    case SCRSDK::CrDisplayStringType_Button_Assign_Display:
        ts << "[Button Assign] Display";
        break;
    case SCRSDK::CrDisplayStringType_Button_Assign_ShortDisplay:
        ts << "[Button Assign] ShortDisplay";
        break;
    case SCRSDK::CrDisplayStringType_FTP_ServerName_Display:
        ts << "[FTP] Server Name Display";
        break;
    case SCRSDK::CrDisplayStringType_FTP_UpLoadDirectory_Display:
        ts << "[FTP] UpLoad Directory Display";
        break;
    case SCRSDK::CrDisplayStringType_FTP_JobStatus_Display:
        ts << "[FTP] Job Status Display";
        break;
    case SCRSDK::CrDisplayStringType_Reserved4:
        ts << "reserved";
        break;
    case SCRSDK::CrDisplayStringType_Reserved5:
        ts << "reserved";
        break;
    case SCRSDK::CrDisplayStringType_Reserved6:
        ts << "reserved";
        break;
    case SCRSDK::CrDisplayStringType_Reserved7:
        ts << "reserved";
        break;
    case SCRSDK::CrDisplayStringType_CreativeLook_Name_Display:
        ts << "[Creative Look] Name Display";
        break;
    default:
        ts << "end";
        break;
    }
    return ts.str();
}

void CameraDevice::execute_request_displaystringlist() {
    SDK::CrDisplayStringType type;

    tout << "Select Display String List Type. \n";
    tout << "[-1] Cancel input\n";
    
    std::vector<int> list_type;
    int i = SDK::CrDisplayStringType_AllList; // top of enum
    int num = 0;
    while(1)
    {
        text listName = format_display_string_type((SDK::CrDisplayStringType)i);
        if (0 == listName.compare(TEXT("end"))) break;
        if (0 != listName.compare(TEXT("reserved")))
        {
            tout << "[" << num << "] " << listName << "\n";
            list_type.push_back(i);
            num++;
        }
        i++;
    }
    tout << "[-1] Cancel input\n";
    tout << "input> ";
    text input;
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;
    if (selected_index < 0 || num <= selected_index || selected_index == 0 && input != TEXT("0")) {
        tout << "Input cancelled.\n";
        return;
    }
    type = (SDK::CrDisplayStringType)list_type[selected_index];
    SDK::RequestDisplayStringList(m_device_handle, type);
}

void CameraDevice::execute_get_displaystringtypes() {
    CrInt32u num;
    SDK::CrDisplayStringType* Types;
    SDK::CrError err = GetDisplayStringTypes(m_device_handle, &Types, &num);
    if (CR_SUCCEEDED(err)) {
        tout << "Successfully retrieved DisplayStringTypes.\n\n";
        tout << "----- ListType to be retrieved -----\n";
        m_dispStrTypeList.clear();
        for (int i = 0; i < num; i++)
        {
            text listName = format_display_string_type(Types[i]);
            if ((0 != listName.compare(TEXT("end"))) && (0 != listName.compare(TEXT("reserved"))))
            {
                m_dispStrTypeList.push_back(Types[i]);
                tout << listName << "\n";
            }
        }
        tout << "----- ListType to be retrieved -----\n";
        ReleaseDisplayStringTypes(m_device_handle, Types);
    }
    else {
        tout << "Failed to get DisplayStringTypes.\n";
    }
}

void CameraDevice::execute_get_displaystringlist() {
    if (m_dispStrTypeList.size() > 0) {
        SDK::CrDisplayStringListInfo* pProperty = nullptr;
        SDK::CrDisplayStringType type;
        CrInt32u numOfList = 0;
        tout << "Select Display String List Type. \n";
        tout << "[-1] Cancel input\n";
        tout << "[0] "<< format_display_string_type(SDK::CrDisplayStringType_AllList) << "\n";
        for (int i = 0; i < m_dispStrTypeList.size(); i++)
        {
                tout << "[" << i + 1 << "] " << format_display_string_type(m_dispStrTypeList[i]) << "\n";
        }
        tout << "[-1] Cancel input\n";
        tout << "input> ";
        text input;
        std::getline(tin, input);
        text_stringstream ss(input);
        int selected_index = 0;
        ss >> selected_index;
        if (selected_index < 0 || m_dispStrTypeList.size() < selected_index || selected_index == 0 && input != TEXT("0"))
        {
            tout << "Input cancelled.\n";
            return;
        }
        if (selected_index == 0) type = SDK::CrDisplayStringType_AllList;
        else type = m_dispStrTypeList[selected_index-1];
        SDK::CrError err = SDK::GetDisplayStringList(m_device_handle, type, &pProperty, &numOfList);

        if (err == SDK::CrError_Api_NoApplicableInformation) {
            tout << "\nFailed to get DisplayStringList\n";
        }
        if (CR_SUCCEEDED(err)) {
            tout << "\n";
            SDK::CrDisplayStringType titleType = SDK::CrDisplayStringType_AllList; // I never receive ALLList.
            for (int i = 0; i < numOfList; i++)
            {
                text listName = format_display_string_type((SDK::CrDisplayStringType)pProperty[i].listType);
                if ((0 == listName.compare(TEXT("end"))) || (0 == listName.compare(TEXT("reserved")))) continue;
                // Output the name of the new type when the type changes
                if(titleType != pProperty[i].listType)
                {
                    titleType = pProperty[i].listType;
                    tout << "----- " << listName << " -----\n"; // List name (title row)
                }
                tout << pProperty[i].value << ": ";
                tout << format_dispstrlist(pProperty[i]) << "\n";
            }
            ReleaseDisplayStringList(m_device_handle, pProperty);
        }
    }
    else {
        tout << "\nFailed to get DisplayStringList.\n";
    }
}

text CameraDevice::format_dispstrlist(SDK::CrDisplayStringListInfo list) {
    text_stringstream ts;

    CrInt32 length = (list.displayStringSize + 1) * sizeof(CrChar);
    CrChar* listStr = new CrChar[length];
    memset(listStr, 0, length);
    for (int i = 0; i < list.displayStringSize; i++)
    {
        listStr[i] = list.displayString[i];
    }
    ts << listStr;
    return ts.str();
}

void CameraDevice::get_mediaprofile()
{
    if (m_mediaprofileList.size() > 0) m_mediaprofileList.clear();

    get_media_slot_status();

    // If there are no slots with OK status, exit
    if ((0 <= Prop.at(PCode::CrDeviceProperty_MediaSLOT1_Status).writable) && (SDK::CrSlotStatus::CrSlotStatus_OK != Prop.at(PCode::CrDeviceProperty_MediaSLOT1_Status).current) 
        &&
        (0 <= Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).writable) && (SDK::CrSlotStatus::CrSlotStatus_OK != Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).current)) 
    {
        tout << "\nThere is no SLOT that can display Media profile.\n";
        return;
    }

    tout << "\nSelect Get Media profile SLOT. \n";
    tout << "[1] SLOT1\n";
    tout << "[2] SLOT2\n";
    tout << "[-1] Cancel input\n";
    tout << "input> ";
    text input;
    std::getline(tin, input);
    text_stringstream ss(input);
    int selected_index = 0;
    ss >> selected_index;
    SCRSDK::CrMediaProfile slot;

    bool canExec = false;
    load_properties();
    switch (selected_index)
    {
    case 1:
        slot = SCRSDK::CrMediaProfile_Slot1;
        if ((SDK::CrSlotStatus::CrSlotStatus_OK == Prop.at(PCode::CrDeviceProperty_MediaSLOT1_Status).current) 
            &&
            (-1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType).writable))
            canExec = true;
        break;
    case 2:
        slot = SCRSDK::CrMediaProfile_Slot2;
        if (-1 == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).writable) {
            tout << "Media SLOT2 is not supported.\n";
            selected_index = -1;
        }
        else if ((SDK::CrSlotStatus::CrSlotStatus_OK == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).current)
            &&
            (-1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType).writable))
            canExec = true;
        break;
    default:
        selected_index = -1;
        break;
    }

    if (selected_index == -1) {
        tout << "Input cancelled.\n";
        return;
    }
    if (false == canExec) {
        tout << "The specified slot is invalid.\n";
        return;
    }
    CrInt32u nums = 0;
    SCRSDK::CrMediaProfileInfo* mediaProfileList = nullptr;
    SCRSDK::CrError ret = SCRSDK::GetMediaProfile(m_device_handle, slot, &mediaProfileList, &nums);
    if (CR_SUCCEEDED(ret) && 0 < nums)
    {
        for (int i = 0; i < nums; i++)
        {
            auto pItem = new SCRSDK::CrMediaProfileInfo();
            CrInt32u len = strlen((char*)mediaProfileList[i].contentName);

            len = (len + 1) * sizeof(CrChar);
            pItem->contentName = new CrInt8u[len];
            MemCpyEx(pItem->contentName, mediaProfileList[i].contentName, len);

            len = strlen((char*)mediaProfileList[i].contentUrl);
            len = (len + 1) * sizeof(CrChar);
            pItem->contentUrl = new CrInt8u[len];
            MemCpyEx(pItem->contentUrl, mediaProfileList[i].contentUrl, len);

            if (NULL != mediaProfileList[i].proxyUrl)
            {
                len = strlen((char*)mediaProfileList[i].proxyUrl);
                len = (len + 1) * sizeof(CrChar);
                pItem->proxyUrl = new CrInt8u[len];
                MemCpyEx(pItem->proxyUrl, mediaProfileList[i].proxyUrl, len);
            }

            if (NULL != mediaProfileList[i].thumbnailUrl)
            {
                len = strlen((char*)mediaProfileList[i].thumbnailUrl);
                len = (len + 1) * sizeof(CrChar);
                pItem->thumbnailUrl = new CrInt8u[len];
                MemCpyEx(pItem->thumbnailUrl, mediaProfileList[i].thumbnailUrl, len);
            }
            m_mediaprofileList.push_back(pItem);
        }
        SCRSDK::ReleaseMediaProfile(m_device_handle, mediaProfileList);

        if (m_mediaprofileList.size() > 0)
        {
            for (int i = 0; i < m_mediaprofileList.size(); ++i)
            {

                tout << "\n";
                tout.fill(' ');
                tout.width(3);
                tout <<  i + 1 << ": " << (char*) m_mediaprofileList[i]->contentName << "\n";
                tout << "      Clip URL      : " << (char*) m_mediaprofileList[i]->contentUrl << "\n";
                tout << "      Thumbnail URL : ";
                if (NULL == m_mediaprofileList[i]->thumbnailUrl) {
                    tout << "-\n";
                }
                else {
                    tout << (char*)m_mediaprofileList[i]->thumbnailUrl << "\n";
                }

                tout << "      Proxy URL     : ";
                if (NULL == m_mediaprofileList[i]->proxyUrl) {
                    tout << "-\n";
                }
                else {
                    tout << (char*)m_mediaprofileList[i]->proxyUrl << "\n";
                }
            }
        }
    }
    else
    {
        tout << "Media Profile data is empty or GetMediaProfile is not supported.\n";
    }
}

bool CameraDevice::get_focus_position_setting()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).possible.size() < 1) {
        tout << "Focus Position Setting is not supported.\n";
        return false;
    }

    tout << "Focus Position Current Value : " << format_focus_position_value(Prop.at(PCode::CrDeviceProperty_FocusPositionCurrentValue).current) << std::endl;
    tout << "Focus Position Setting min   : " << format_focus_position_value(Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).possible.at(0)) << std::endl;
    tout << "Focus Position Setting max   : " << format_focus_position_value(Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).possible.at(1)) << std::endl;
    tout << "Focus Position Setting Step  : " << format_focus_position_value(Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).possible.at(2)) << std::endl;

    tout << "Focus Driving Status: " << format_focus_driving_status(Prop.at(PCode::CrDeviceProperty_FocusDrivingStatus).current) << std::endl;
    return true;
}

void CameraDevice::set_focus_position_setting()
{
    char keyin[100];
    int len;
    char* errPtr;
    int value = 0;

    if (false == get_focus_position_setting())
        return;

    if (1 != Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).writable) {
        // Not a settable property
        tout << "Focus Position Setting is not writable\n";
        return;
    }

    text input;
    tout << "\nWould you like to set a Focus Position Setting ? (y/n): ";
    std::getline(tin, input);
    if (input != TEXT("y")) {
        tout << "Skip.\n";
        return;
    }

    tout << std::endl << "Set a value within the Focus Position Setting (hex)" << std::endl;
    tout << "[-1] Cancel input\n" << std::endl;
    tout << "input> ";

    fgets(keyin, sizeof(keyin), stdin);

    len = strlen(keyin);
    if (keyin[len - 1] == '\n') {
        keyin[len - 1] = '\0';
    }
    value = strtol(keyin, &errPtr, 16);      // hex

    if (value == -1) {
        tout << "Input cancelled.\n";
        return;
    }
    if (*errPtr != '\0') {
        tout << std::endl << "Focus Position Setting FAILED\n" << std::endl;
        return;
    }

    if (((int)Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).possible.at(0) > value) || ((int)Prop.at(PCode::CrDeviceProperty_FocusPositionSetting).possible.at(1) < value)) {
        tout << "\nThe set value is out of range.\n";
        return;
    }

    tout << "Set the Focus Position Setting value: ";
    format_focus_position_value(value);

    SDK::CrDeviceProperty prop;
    prop.SetCode(PCode::CrDeviceProperty_FocusPositionSetting);
    prop.SetCurrentValue((CrInt64u)value);
    prop.SetValueType(SDK::CrDataType::CrDataType_UInt16);
    SDK::SetDeviceProperty(m_device_handle, &prop);

    tout << "Setting focus position settings...";
    std::this_thread::sleep_for(500ms);
    while (1)
    {
        load_properties();
        tout << "Focus Position Current Value : ";
        format_focus_position_value(Prop.at(PCode::CrDeviceProperty_FocusPositionCurrentValue).current);
        if (Prop.at(PCode::CrDeviceProperty_FocusDrivingStatus).current != SDK::CrFocusDrivingStatus::CrFocusDrivingStatus_Driving)
        {
            break;
        }
        if (Prop.at(PCode::CrDeviceProperty_FocusDrivingStatus).current == SDK::CrFocusDrivingStatus::CrFocusDrivingStatus_Driving)
        {
            if (true == execute_focus_position_cancel())
            {
                return;
            }
        }
        tout << "Setting focus position settings...";
        std::this_thread::sleep_for(500ms);
    }
    tout << std::endl << "Finish Focus Position Setting\n";
}

bool CameraDevice::execute_focus_position_cancel()
{
    text input;
    tout << std::endl << "Do you want to continue with Setting Focus Position?" << std::endl;
    tout << "[0] Continue" << std::endl;
    tout << "[1] Cancel" << std::endl;
    tout << "input> ";
    std::getline(tin, input);
    if (input != TEXT("1")) {
        return false;
    }

    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_FocusDrivingStatus).current == SDK::CrFocusDrivingStatus::CrFocusDrivingStatus_Driving) {
        SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_CancelFocusPosition, SDK::CrCommandParam::CrCommandParam_Down);
        //std::this_thread::sleep_for(10ms);
        SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_CancelFocusPosition, SDK::CrCommandParam::CrCommandParam_Up);

        tout << std::endl << "Execute Cancel Focus Position Setting\n";
    }
    else
    {
        tout << "Finish Focus Position Setting\n";
    }
    return true;
}

/*----- for Nocode SDK -----*/

bool CameraDevice::set_save_info(text prefix) const
{
    text path = fs::current_path().native();
    tout << path.data() << '\n';

    auto save_status = SDK::SetSaveInfo(m_device_handle
        , const_cast<text_char*>(path.data()), const_cast<text_char*>(prefix.data()), ImageSaveAutoStartNo);
    if (CR_FAILED(save_status)) {
        tout << "Failed to set save path.\n";
        return false;
    }
    return true;
}

std::int32_t CameraDevice::get_live_view(std::uint8_t* buf[])
{
    CrInt32 num = 0;
    SDK::CrLiveViewProperty* property = nullptr;
    auto err = SDK::GetLiveViewProperties(m_device_handle, &property, &num);
    if (CR_FAILED(err)) {
        tout << "GetLiveView FAILED\n";
        return -1;
    }
    SDK::ReleaseLiveViewProperties(m_device_handle, property);

    SDK::CrImageInfo inf;
    err = SDK::GetLiveViewImageInfo(m_device_handle, &inf);
    if (CR_FAILED(err)) {
        tout << "GetLiveView FAILED\n";
        return -1;
    }

    CrInt32u bufSize = inf.GetBufferSize();
    if (bufSize < 1)
    {
        tout << "GetLiveView FAILED \n";
    }
    else
    {
        auto* image_data = new SDK::CrImageDataBlock();
        if (!image_data)
        {
            tout << "GetLiveView FAILED (new CrImageDataBlock class)\n";
            return -1;
        }
        CrInt8u* image_buff = new CrInt8u[bufSize];
        if (!image_buff)
        {
            delete image_data;
            tout << "GetLiveView FAILED (new Image buffer)\n";
            return -1;
        }
        image_data->SetSize(bufSize);
        image_data->SetData(image_buff);

        err = SDK::GetLiveViewImage(m_device_handle, image_data);
        if (CR_FAILED(err))
        {
            // FAILED
            if (err == SDK::CrWarning_Frame_NotUpdated) {
                tout << "Warning. GetLiveView Frame NotUpdate\n";
            }
            else if (err == SDK::CrError_Memory_Insufficient) {
                tout << "Warning. GetLiveView Memory insufficient\n";
            }
            delete[] image_buff; // Release
            delete image_data; // Release
        }
        else
        {
            if (0 < image_data->GetSize())
            {
                // Display
                // etc.
                //tout << "GetLiveView SUCCESS\n";
                std::int32_t imageSize = image_data->GetImageSize();
                *buf = new std::uint8_t[imageSize];
                if(*buf) {
                    MemCpyEx(*buf, image_data->GetImageData(), imageSize);
                } else {
                    imageSize = -1;
                }
                delete[] image_buff; // Release
                delete image_data; // Release
                return imageSize;
            }
            else
            {
                // FAILED
                delete[] image_buff; // Release
                delete image_data; // Release
            }
        }
    }
    return -1;
}

PCode CameraDevice::Prop_tag2id(std::string tag) const
{
	for (const auto& _prop : Prop) {
		if(_prop.second.tag == tag) {
			return _prop.first;
		}
	}
	return (PCode)0;
}

struct PropertyValue* CameraDevice::GetProp(PCode id)
{
	CrInt32u code = static_cast<CrInt32u>(id);
	load_properties(1, &code);
	return &Prop.at(id);
}

std::int32_t CameraDevice::SetProp(PCode id, std::uint32_t value) const
{
	if (0 == Prop.at(id).writable) {
		tout << "not writable\n";
		return -1;
	}

	SDK::CrDeviceProperty devProp;
	devProp.SetCode(id);
	devProp.SetCurrentValue(value);
	devProp.SetValueType(Prop.at(id).dataType);
	return SDK::SetDeviceProperty(m_device_handle, &devProp);
}

}
// namespace cli

