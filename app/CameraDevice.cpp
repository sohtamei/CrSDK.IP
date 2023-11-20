#if defined (_WIN32) || defined(_WIN64)

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <memory>
#include <map>
#include <iostream>

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
//	{ PCode::CrDeviceProperty_\2,	{ -1, SDK::CrDataType::CrDataType_\3, "\1", format_\6, &map_\5, } },
//Prop.at(PCode::CrDeviceProperty_FNumber)

//LensModelName		m_prop.lensModelNameStr
//shutter_speed_value	m_prop.shutter_speed_values
//CrDeviceProperty_ContentsTransferStatus
//CrDeviceProperty_Media_FormatProgressRate
//CrDeviceProperty_LensModelName
//CrDeviceProperty_ShutterSpeedValue

#include "CrDeviceProperty.cpp"

std::map<PCode, struct PropertyValue> Prop {

	{ PCode::CrDeviceProperty_AEL,	{ -1, SDK::CrDataType::CrDataType_UInt16, "AEL", 0, &map_CrLockIndicator, } },
	{ PCode::CrDeviceProperty_AF_Area_Position,	{ -1, SDK::CrDataType::CrDataType_UInt32, "af_Area_Position", 0, 0, } },
	{ PCode::CrDeviceProperty_APS_C_or_Full_SwitchingEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "aps_c_or_Full_SwitchingEnableStatus", 0, &map_CrAPS_C_or_Full_SwitchingEnableStatus, } },
	{ PCode::CrDeviceProperty_APS_C_or_Full_SwitchingSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, "aps_c_or_Full_SwitchingSetting", 0, &map_CrAPS_C_or_Full_SwitchingSetting, } },
	{ PCode::CrDeviceProperty_AWBL,	{ -1, SDK::CrDataType::CrDataType_UInt16, "AWBL", 0, &map_CrLockIndicator, } },
	{ PCode::CrDeviceProperty_BaseLookValue,	{ -1, SDK::CrDataType::CrDataType_UInt16, "baseLookValue", 0, 0, } },
	{ PCode::CrDeviceProperty_CameraSetting_ReadOperationEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "cameraSetting_ReadOperationEnableStatus", 0, &map_CrCameraSettingReadOperation, } },
	{ PCode::CrDeviceProperty_CameraSetting_SaveOperationEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "cameraSetting_SaveOperationEnableStatus", 0, &map_CrCameraSettingSaveOperation, } },
	{ PCode::CrDeviceProperty_CameraSetting_SaveRead_State,	{ -1, SDK::CrDataType::CrDataType_UInt8, "cameraSetting_SaveRead_State", 0, &map_CrCameraSettingSaveReadState, } },
	{ PCode::CrDeviceProperty_CameraSettingsResetEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "cameraSettingsResetEnableStatus", 0, &map_CrCameraSettingsResetEnableStatus, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture,	{ -1, SDK::CrDataType::CrDataType_UInt32, "customWB_Capture", 0, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Operation,	{ -1, SDK::CrDataType::CrDataType_UInt16, "customWB_Capture_Operation", 0, &map_CrPropertyCustomWBOperation, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Standby,	{ -1, SDK::CrDataType::CrDataType_UInt16, "customWB_Capture_Standby", 0, &map_CrPropertyCustomWBOperation, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel,	{ -1, SDK::CrDataType::CrDataType_UInt16, "customWB_Capture_Standby_Cancel", 0, &map_CrPropertyCustomWBOperation, } },
	{ PCode::CrDeviceProperty_CustomWB_Execution_State,	{ -1, SDK::CrDataType::CrDataType_UInt16, "customWB_Execution_State", 0, &map_CrPropertyCustomWBExecutionState, } },
	{ PCode::CrDeviceProperty_DispMode,	{ -1, SDK::CrDataType::CrDataType_UInt8, "dispMode", 0, &map_CrDispMode, } },
	{ PCode::CrDeviceProperty_DispModeCandidate,	{ -1, SDK::CrDataType::CrDataType_UInt32, "dispModeCandidate", 0, 0, } },
	{ PCode::CrDeviceProperty_DispModeSetting,	{ -1, SDK::CrDataType::CrDataType_UInt32, "dispModeSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_DriveMode,	{ -1, SDK::CrDataType::CrDataType_UInt32, "driveMode", 0, &map_CrDriveMode, } },
	{ PCode::CrDeviceProperty_ExposureBiasCompensation,	{ -1, SDK::CrDataType::CrDataType_UInt16, "exposureBiasCompensation", format_f_number, 0, } },
	{ PCode::CrDeviceProperty_ExposureCtrlType,	{ -1, SDK::CrDataType::CrDataType_UInt8, "exposureCtrlType", 0, &map_CrExposureCtrlType, } },
	{ PCode::CrDeviceProperty_ExposureIndex,	{ -1, SDK::CrDataType::CrDataType_UInt16, "exposureIndex", 0, 0, } },
	{ PCode::CrDeviceProperty_ExposureProgramMode,	{ -1, SDK::CrDataType::CrDataType_UInt32, "exposureProgramMode", 0, &map_CrExposureProgram, } },
	{ PCode::CrDeviceProperty_FEL,	{ -1, SDK::CrDataType::CrDataType_UInt16, "FEL", 0, &map_CrLockIndicator, } },
	{ PCode::CrDeviceProperty_FlashCompensation,	{ -1, SDK::CrDataType::CrDataType_UInt16, "flashCompensation", format_f_number, 0, } },
	{ PCode::CrDeviceProperty_FNumber,	{ -1, SDK::CrDataType::CrDataType_UInt16, "aperture", format_f_number, 0, } },
	{ PCode::CrDeviceProperty_FocusArea,	{ -1, SDK::CrDataType::CrDataType_UInt16, "focusArea", 0, &map_CrFocusArea, } },
	{ PCode::CrDeviceProperty_FocusBracketFocusRange,	{ -1, SDK::CrDataType::CrDataType_UInt8, "focusBracketFocusRange", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketShotNumber,	{ -1, SDK::CrDataType::CrDataType_UInt16, "focusBracketShotNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusDrivingStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "focusDrivingStatus", 0, &map_CrFocusDrivingStatus, } },
	{ PCode::CrDeviceProperty_FocusIndication,	{ -1, SDK::CrDataType::CrDataType_UInt32, "focusIndication", 0, &map_CrFocusIndicator, } },
	{ PCode::CrDeviceProperty_FocusMode,	{ -1, SDK::CrDataType::CrDataType_UInt16, "focusMode", 0, &map_CrFocusMode, } },
	{ PCode::CrDeviceProperty_FocusPositionCurrentValue,	{ -1, SDK::CrDataType::CrDataType_UInt16, "focusPositionCurrentValue", format_focus_position_value, 0, } },
	{ PCode::CrDeviceProperty_FocusPositionSetting,	{ -1, SDK::CrDataType::CrDataType_UInt16, "focusPositionSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_GainBaseIsoSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt8, "gainBaseIsoSensitivity", 0, &map_CrGainBaseIsoSensitivity, } },
	{ PCode::CrDeviceProperty_GainBaseSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt8, "gainBaseSensitivity", 0, &map_CrGainBaseSensitivity, } },
	{ PCode::CrDeviceProperty_GainControlSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, "gainControlSetting", 0, &map_CrGainControlSetting, } },
	{ PCode::CrDeviceProperty_GaindBValue,	{ -1, SDK::CrDataType::CrDataType_UInt8, "gaindBValue", 0, 0, } },
	{ PCode::CrDeviceProperty_ImageStabilizationSteadyShot,	{ -1, SDK::CrDataType::CrDataType_UInt8, "imageStabilizationSteadyShot", 0, &map_CrImageStabilizationSteadyShot, } },
	{ PCode::CrDeviceProperty_IrisModeSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, "irisModeSetting", 0, &map_CrIrisModeSetting, } },
	{ PCode::CrDeviceProperty_IsoCurrentSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt32, "isoCurrentSensitivity", format_iso_sensitivity, 0, } },
	{ PCode::CrDeviceProperty_IsoSensitivity,	{ -1, SDK::CrDataType::CrDataType_UInt32, "iso", format_iso_sensitivity, 0, } },
	{ PCode::CrDeviceProperty_LiveView_Image_Quality,	{ -1, SDK::CrDataType::CrDataType_UInt16, "liveView_Image_Quality", 0, &map_CrPropertyLiveViewImageQuality, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "mediaSLOT1_FormatEnableStatus", 0, &map_CrMediaFormat, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "mediaSLOT1_QuickFormatEnableStatus", 0, &map_CrMediaFormat, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType,	{ -1, SDK::CrDataType::CrDataType_UInt8, "mediaSLOT1_RecordingAvailableType", 0, &map_CrMediaSlotRecordingAvailableType, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_Status,	{ -1, SDK::CrDataType::CrDataType_UInt16, "mediaSLOT1_Status", 0, &map_CrSlotStatus, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "mediaSLOT2_FormatEnableStatus", 0, &map_CrMediaFormat, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "mediaSLOT2_QuickFormatEnableStatus", 0, &map_CrMediaFormat, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType,	{ -1, SDK::CrDataType::CrDataType_UInt8, "mediaSLOT2_RecordingAvailableType", 0, &map_CrMediaSlotRecordingAvailableType, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_Status,	{ -1, SDK::CrDataType::CrDataType_UInt16, "mediaSLOT2_Status", 0, &map_CrSlotStatus, } },
	{ PCode::CrDeviceProperty_MonitorLUTSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, "monitorLUTSetting", 0, &map_CrMonitorLUTSetting, } },
	{ PCode::CrDeviceProperty_Movie_ImageStabilizationSteadyShot,	{ -1, SDK::CrDataType::CrDataType_UInt8, "movie_ImageStabilizationSteadyShot", 0, &map_CrImageStabilizationSteadyShotMovie, } },
	{ PCode::CrDeviceProperty_Movie_Recording_Setting,	{ -1, SDK::CrDataType::CrDataType_UInt16, "movie_Recording_Setting", 0, &map_CrRecordingSettingMovie, } },
	{ PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus,	{ -1, SDK::CrDataType::CrDataType_UInt8, "movieRecButtonToggleEnableStatus", 0, &map_CrMovieRecButtonToggleEnableStatus, } },
	{ PCode::CrDeviceProperty_MovieShootingMode,	{ -1, SDK::CrDataType::CrDataType_UInt16, "movieShootingMode", 0, &map_CrMovieShootingMode, } },
	{ PCode::CrDeviceProperty_PlaybackMedia,	{ -1, SDK::CrDataType::CrDataType_UInt8, "playbackMedia", 0, &map_CrPlaybackMedia, } },
	{ PCode::CrDeviceProperty_PriorityKeySettings,	{ -1, SDK::CrDataType::CrDataType_UInt16, "priorityKeySettings", 0, &map_CrPriorityKeySettings, } },
	{ PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type,	{ -1, SDK::CrDataType::CrDataType_UInt8, "remocon_Zoom_Speed_Type", 0, &map_CrRemoconZoomSpeedType, } },
	{ PCode::CrDeviceProperty_S1,	{ -1, SDK::CrDataType::CrDataType_UInt16, "S1", 0, &map_CrLockIndicator, } },
	{ PCode::CrDeviceProperty_SdkControlMode,	{ -1, SDK::CrDataType::CrDataType_UInt32, "sdkControlMode", 0, &map_CrSdkControlMode, } },
	{ PCode::CrDeviceProperty_ShutterModeSetting,	{ -1, SDK::CrDataType::CrDataType_UInt8, "shutterModeSetting", 0, &map_CrShutterMode, } },
	{ PCode::CrDeviceProperty_ShutterSpeed,	{ -1, SDK::CrDataType::CrDataType_UInt32, "shutterSpeed", format_shutter_speed, 0, } },
	{ PCode::CrDeviceProperty_ShutterType,	{ -1, SDK::CrDataType::CrDataType_UInt8, "shutterType", 0, &map_CrShutterType, } },
	{ PCode::CrDeviceProperty_SilentMode,	{ -1, SDK::CrDataType::CrDataType_UInt8, "silentMode", 0, &map_CrSilentMode, } },
	{ PCode::CrDeviceProperty_SilentModeApertureDriveInAF,	{ -1, SDK::CrDataType::CrDataType_UInt8, "silentModeApertureDriveInAF", 0, &map_CrSilentModeApertureDriveInAF, } },
	{ PCode::CrDeviceProperty_SilentModeAutoPixelMapping,	{ -1, SDK::CrDataType::CrDataType_UInt8, "silentModeAutoPixelMapping", 0, &map_CrSilentModeAutoPixelMapping, } },
	{ PCode::CrDeviceProperty_SilentModeShutterWhenPowerOff,	{ -1, SDK::CrDataType::CrDataType_UInt8, "silentModeShutterWhenPowerOff", 0, &map_CrSilentModeShutterWhenPowerOff, } },
	{ PCode::CrDeviceProperty_WhiteBalance,	{ -1, SDK::CrDataType::CrDataType_UInt16, "whiteBalance", 0, &map_CrWhiteBalanceSetting, } },
	{ PCode::CrDeviceProperty_WhiteBalanceTint,	{ -1, SDK::CrDataType::CrDataType_UInt8, "whiteBalanceTint", 0, 0, } },
	{ PCode::CrDeviceProperty_WhiteBalanceTintStep,	{ -1, SDK::CrDataType::CrDataType_UInt16, "whiteBalanceTintStep", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Bar_Information,	{ -1, SDK::CrDataType::CrDataType_UInt32, "zoom_Bar_Information", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Operation,	{ -1, SDK::CrDataType::CrDataType_Int8, "zoom_Operation", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Operation_Status,	{ -1, SDK::CrDataType::CrDataType_UInt8, "zoom_Operation_Status", 0, &map_CrZoomOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_Zoom_Setting,	{ -1, SDK::CrDataType::CrDataType_UInt8, "zoom_Setting", 0, &map_CrZoomSettingType, } },
	{ PCode::CrDeviceProperty_Zoom_Speed_Range,	{ -1, SDK::CrDataType::CrDataType_UInt8, "zoom_Speed_Range", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Type_Status,	{ -1, SDK::CrDataType::CrDataType_UInt8, "zoom_Type_Status", 0, &map_CrZoomTypeStatus, } },
	{ PCode::CrDeviceProperty_ZoomAndFocusPosition_Load,	{ -1, SDK::CrDataType::CrDataType_UInt8, "zoomAndFocusPosition_Load", 0, 0, } },
	{ PCode::CrDeviceProperty_ZoomAndFocusPosition_Save,	{ -1, SDK::CrDataType::CrDataType_UInt8, "zoomAndFocusPosition_Save", 0, 0, } },
	{ PCode::CrDeviceProperty_ZoomDistance,	{ -1, SDK::CrDataType::CrDataType_UInt32, "zoomDistance", 0, 0, } },

	{ PCode::CrDeviceProperty_AFAssist,	{ -1, (SDK::CrDataType)0, "aFAssist", 0, &map_CrAFAssist, } },
	{ PCode::CrDeviceProperty_AFSubjShiftSens,	{ -1, (SDK::CrDataType)0, "aFSubjShiftSens", 0, 0, } },
	{ PCode::CrDeviceProperty_AFTrackingSensitivity,	{ -1, (SDK::CrDataType)0, "aFTrackingSensitivity", 0, &map_CrAFTrackingSensitivity, } },
	{ PCode::CrDeviceProperty_AFTransitionSpeed,	{ -1, (SDK::CrDataType)0, "aFTransitionSpeed", 0, 0, } },
	{ PCode::CrDeviceProperty_AspectRatio,	{ -1, (SDK::CrDataType)0, "aspectRatio", 0, &map_CrAspectRatioIndex, } },
	{ PCode::CrDeviceProperty_AssignableButton1,	{ -1, (SDK::CrDataType)0, "assignableButton1", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton2,	{ -1, (SDK::CrDataType)0, "assignableButton2", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton3,	{ -1, (SDK::CrDataType)0, "assignableButton3", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton4,	{ -1, (SDK::CrDataType)0, "assignableButton4", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton5,	{ -1, (SDK::CrDataType)0, "assignableButton5", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton6,	{ -1, (SDK::CrDataType)0, "assignableButton6", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton7,	{ -1, (SDK::CrDataType)0, "assignableButton7", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton8,	{ -1, (SDK::CrDataType)0, "assignableButton8", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButton9,	{ -1, (SDK::CrDataType)0, "assignableButton9", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator1,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator1", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator2,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator2", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator3,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator3", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator4,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator4", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator5,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator5", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator6,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator6", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator7,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator7", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator8,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator8", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator9,	{ -1, (SDK::CrDataType)0, "assignableButtonIndicator9", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_AudioInputMasterLevel,	{ -1, (SDK::CrDataType)0, "audioInputMasterLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_AudioRecording,	{ -1, (SDK::CrDataType)0, "audioRecording", 0, &map_CrAudioRecording, } },
	{ PCode::CrDeviceProperty_AudioSignals,	{ -1, (SDK::CrDataType)0, "audioSignals", 0, &map_CrAudioSignals, } },
	{ PCode::CrDeviceProperty_AutoPowerOffTemperature,	{ -1, (SDK::CrDataType)0, "autoPowerOffTemperature", 0, &map_CrAutoPowerOffTemperature, } },
	{ PCode::CrDeviceProperty_AutoReview,	{ -1, (SDK::CrDataType)0, "autoReview", 0, 0, } },
	{ PCode::CrDeviceProperty_AutoSlowShutter,	{ -1, (SDK::CrDataType)0, "autoSlowShutter", 0, &map_CrAutoSlowShutter, } },
	{ PCode::CrDeviceProperty_AWB,	{ -1, (SDK::CrDataType)0, "aWB", 0, &map_CrAWB, } },
	{ PCode::CrDeviceProperty_BaseISOSwitchEI,	{ -1, (SDK::CrDataType)0, "baseISOSwitchEI", 0, 0, } },
	{ PCode::CrDeviceProperty_BaseLookImportOperationEnableStatus,	{ -1, (SDK::CrDataType)0, "baseLookImportOperationEnableStatus", 0, &map_CrBaseLookImportOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_BatteryLevel,	{ -1, (SDK::CrDataType)0, "batteryLevel", 0, &map_CrBatteryLevel, } },
	{ PCode::CrDeviceProperty_BatteryRemain,	{ -1, (SDK::CrDataType)0, "batteryRemain", 0, 0, } },
	{ PCode::CrDeviceProperty_BatteryRemainDisplayUnit,	{ -1, (SDK::CrDataType)0, "batteryRemainDisplayUnit", 0, &map_CrBatteryRemainDisplayUnit, } },
	{ PCode::CrDeviceProperty_BatteryRemainingInMinutes,	{ -1, (SDK::CrDataType)0, "batteryRemainingInMinutes", 0, 0, } },
	{ PCode::CrDeviceProperty_BatteryRemainingInVoltage,	{ -1, (SDK::CrDataType)0, "batteryRemainingInVoltage", 0, 0, } },
	{ PCode::CrDeviceProperty_BodyKeyLock,	{ -1, (SDK::CrDataType)0, "bodyKeyLock", 0, &map_CrBodyKeyLock, } },
	{ PCode::CrDeviceProperty_BracketOrder,	{ -1, (SDK::CrDataType)0, "bracketOrder", 0, &map_CrBracketOrder, } },
	{ PCode::CrDeviceProperty_BulbExposureTimeSetting,	{ -1, (SDK::CrDataType)0, "bulbExposureTimeSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_BulbTimerSetting,	{ -1, (SDK::CrDataType)0, "bulbTimerSetting", 0, &map_CrBulbTimerSetting, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable1,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable1", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable2,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable2", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable3,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable3", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable4,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable4", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable5,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable5", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable6,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable6", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable7,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable7", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable8,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable8", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable9,	{ -1, (SDK::CrDataType)0, "buttonAssignmentAssignable9", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentLensAssignable1,	{ -1, (SDK::CrDataType)0, "buttonAssignmentLensAssignable1", 0, 0, } },
	{ PCode::CrDeviceProperty_CameraEframing,	{ -1, (SDK::CrDataType)0, "cameraEframing", 0, &map_CrCameraEframing, } },
	{ PCode::CrDeviceProperty_CameraShakeStatus,	{ -1, (SDK::CrDataType)0, "cameraShakeStatus", 0, &map_CrCameraShakeStatus, } },
	{ PCode::CrDeviceProperty_Cancel_Media_FormatEnableStatus,	{ -1, (SDK::CrDataType)0, "cancel_Media_FormatEnableStatus", 0, &map_CrCancelMediaFormat, } },
	{ PCode::CrDeviceProperty_CancelRemoteTouchOperationEnableStatus,	{ -1, (SDK::CrDataType)0, "cancelRemoteTouchOperationEnableStatus", 0, &map_CrCancelRemoteTouchOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_ColorSpace,	{ -1, (SDK::CrDataType)0, "colorSpace", 0, &map_CrColorSpace, } },
	{ PCode::CrDeviceProperty_Colortemp,	{ -1, (SDK::CrDataType)0, "colortemp", 0, 0, } },
	{ PCode::CrDeviceProperty_ColortempStep,	{ -1, (SDK::CrDataType)0, "colortempStep", 0, 0, } },
	{ PCode::CrDeviceProperty_ColorTuningAB,	{ -1, (SDK::CrDataType)0, "colorTuningAB", 0, 0, } },
	{ PCode::CrDeviceProperty_ColorTuningGM,	{ -1, (SDK::CrDataType)0, "colorTuningGM", 0, 0, } },
	{ PCode::CrDeviceProperty_CompressionFileFormatStill,	{ -1, (SDK::CrDataType)0, "compressionFileFormatStill", 0, &map_CrCompressionFileFormat, } },
	{ PCode::CrDeviceProperty_ContentsTransferCancelEnableStatus,	{ -1, (SDK::CrDataType)0, "contentsTransferCancelEnableStatus", 0, &map_CrCancelContentsTransferEnableStatus, } },
	{ PCode::CrDeviceProperty_ContentsTransferProgress,	{ -1, (SDK::CrDataType)0, "contentsTransferProgress", 0, 0, } },
	{ PCode::CrDeviceProperty_ContentsTransferStatus,	{ -1, (SDK::CrDataType)0, "contentsTransferStatus", 0, &map_CrContentsTransferStatus, } },
	{ PCode::CrDeviceProperty_CreativeLook,	{ -1, (SDK::CrDataType)0, "creativeLook", 0, &map_CrCreativeLook, } },
	{ PCode::CrDeviceProperty_CreativeLook_Clarity,	{ -1, (SDK::CrDataType)0, "creativeLook_Clarity", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Contrast,	{ -1, (SDK::CrDataType)0, "creativeLook_Contrast", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_CustomLook,	{ -1, (SDK::CrDataType)0, "creativeLook_CustomLook", 0, &map_CrCreativeLook, } },
	{ PCode::CrDeviceProperty_CreativeLook_Fade,	{ -1, (SDK::CrDataType)0, "creativeLook_Fade", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Highlights,	{ -1, (SDK::CrDataType)0, "creativeLook_Highlights", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Saturation,	{ -1, (SDK::CrDataType)0, "creativeLook_Saturation", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Shadows,	{ -1, (SDK::CrDataType)0, "creativeLook_Shadows", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Sharpness,	{ -1, (SDK::CrDataType)0, "creativeLook_Sharpness", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_SharpnessRange,	{ -1, (SDK::CrDataType)0, "creativeLook_SharpnessRange", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLookResetEnableStatus,	{ -1, (SDK::CrDataType)0, "creativeLookResetEnableStatus", 0, &map_CrCreativeLookResetEnableStatus, } },
	{ PCode::CrDeviceProperty_CurrentSceneFileEdited,	{ -1, (SDK::CrDataType)0, "currentSceneFileEdited", 0, &map_CrCurrentSceneFileEdited, } },
	{ PCode::CrDeviceProperty_CustomWB_Capturable_Area,	{ -1, (SDK::CrDataType)0, "customWB_Capturable_Area", 0, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Frame_Size,	{ -1, (SDK::CrDataType)0, "customWB_Capture_Frame_Size", 0, 0, } },
	{ PCode::CrDeviceProperty_DateTime_Settings,	{ -1, (SDK::CrDataType)0, "dateTime_Settings", 0, 0, } },
	{ PCode::CrDeviceProperty_DCVoltage,	{ -1, (SDK::CrDataType)0, "dCVoltage", 0, 0, } },
	{ PCode::CrDeviceProperty_DeleteUserBaseLook,	{ -1, (SDK::CrDataType)0, "deleteUserBaseLook", 0, 0, } },
	{ PCode::CrDeviceProperty_DepthOfFieldAdjustmentInterlockingMode,	{ -1, (SDK::CrDataType)0, "depthOfFieldAdjustmentInterlockingMode", 0, &map_CrDepthOfFieldAdjustmentInterlockingMode, } },
	{ PCode::CrDeviceProperty_DepthOfFieldAdjustmentMode,	{ -1, (SDK::CrDataType)0, "depthOfFieldAdjustmentMode", 0, &map_CrDepthOfFieldAdjustmentMode, } },
	{ PCode::CrDeviceProperty_DeviceOverheatingState,	{ -1, (SDK::CrDataType)0, "deviceOverheatingState", 0, &map_CrDeviceOverheatingState, } },
	{ PCode::CrDeviceProperty_DigitalZoomScale,	{ -1, (SDK::CrDataType)0, "digitalZoomScale", 0, 0, } },
	{ PCode::CrDeviceProperty_DRO,	{ -1, (SDK::CrDataType)0, "dRO", 0, &map_CrDRangeOptimizer, } },
	{ PCode::CrDeviceProperty_EframingHDMICrop,	{ -1, (SDK::CrDataType)0, "eframingHDMICrop", 0, &map_CrEframingHDMICrop, } },
	{ PCode::CrDeviceProperty_EframingModeAuto,	{ -1, (SDK::CrDataType)0, "eframingModeAuto", 0, &map_CrEframingModeAuto, } },
	{ PCode::CrDeviceProperty_EframingRecordingImageCrop,	{ -1, (SDK::CrDataType)0, "eframingRecordingImageCrop", 0, &map_CrEframingRecordingImageCrop, } },
	{ PCode::CrDeviceProperty_EframingScaleAuto,	{ -1, (SDK::CrDataType)0, "eframingScaleAuto", 0, &map_CrEframingScaleAuto, } },
	{ PCode::CrDeviceProperty_EframingSpeedAuto,	{ -1, (SDK::CrDataType)0, "eframingSpeedAuto", 0, 0, } },
	{ PCode::CrDeviceProperty_ExtendedInterfaceMode,	{ -1, (SDK::CrDataType)0, "extendedInterfaceMode", 0, &map_CrExtendedInterfaceMode, } },
	{ PCode::CrDeviceProperty_FileType,	{ -1, SDK::CrDataType::CrDataType_UInt16, "fileType", 0, &map_CrFileType, } },
	{ PCode::CrDeviceProperty_FlashMode,	{ -1, (SDK::CrDataType)0, "flashMode", 0, &map_CrFlashMode, } },
	{ PCode::CrDeviceProperty_FlickerLessShooting,	{ -1, (SDK::CrDataType)0, "flickerLessShooting", 0, &map_CrFlickerLessShooting, } },
	{ PCode::CrDeviceProperty_FlickerScanEnableStatus,	{ -1, (SDK::CrDataType)0, "flickerScanEnableStatus", 0, &map_CrFlickerScanEnableStatus, } },
	{ PCode::CrDeviceProperty_FlickerScanStatus,	{ -1, (SDK::CrDataType)0, "flickerScanStatus", 0, &map_CrFlickerScanStatus, } },
	{ PCode::CrDeviceProperty_FocalDistanceInFeet,	{ -1, (SDK::CrDataType)0, "focalDistanceInFeet", 0, 0, } },
	{ PCode::CrDeviceProperty_FocalDistanceInMeter,	{ -1, (SDK::CrDataType)0, "focalDistanceInMeter", 0, 0, } },
	{ PCode::CrDeviceProperty_FocalDistanceUnitSetting,	{ -1, (SDK::CrDataType)0, "focalDistanceUnitSetting", 0, &map_CrFocalDistanceUnitSetting, } },
	{ PCode::CrDeviceProperty_Focus_Magnifier_Setting,	{ -1, (SDK::CrDataType)0, "focus_Magnifier_Setting", 0, 0, } },
	{ PCode::CrDeviceProperty_Focus_Operation,	{ -1, (SDK::CrDataType)0, "focus_Operation", 0, 0, } },
	{ PCode::CrDeviceProperty_Focus_Speed_Range,	{ -1, (SDK::CrDataType)0, "focus_Speed_Range", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketExposureLock1stImg,	{ -1, (SDK::CrDataType)0, "focusBracketExposureLock1stImg", 0, &map_CrFocusBracketExposureLock1stImg, } },
	{ PCode::CrDeviceProperty_FocusBracketIntervalUntilNextShot,	{ -1, (SDK::CrDataType)0, "focusBracketIntervalUntilNextShot", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketOrder,	{ -1, (SDK::CrDataType)0, "focusBracketOrder", 0, &map_CrFocusBracketOrder, } },
	{ PCode::CrDeviceProperty_FocusBracketShootingStatus,	{ -1, (SDK::CrDataType)0, "focusBracketShootingStatus", 0, &map_CrFocusBracketShootingStatus, } },
	{ PCode::CrDeviceProperty_FocusMagnificationTime,	{ -1, (SDK::CrDataType)0, "focusMagnificationTime", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusModeSetting,	{ -1, (SDK::CrDataType)0, "focusModeSetting", 0, &map_CrFocusModeSetting, } },
	{ PCode::CrDeviceProperty_FocusTouchSpotStatus,	{ -1, (SDK::CrDataType)0, "focusTouchSpotStatus", 0, &map_CrFocusTouchSpotStatus, } },
	{ PCode::CrDeviceProperty_FocusTrackingStatus,	{ -1, (SDK::CrDataType)0, "focusTrackingStatus", 0, &map_CrFocusTrackingStatus, } },
	{ PCode::CrDeviceProperty_FollowFocusPositionCurrentValue,	{ -1, (SDK::CrDataType)0, "followFocusPositionCurrentValue", 0, 0, } },
	{ PCode::CrDeviceProperty_FollowFocusPositionSetting,	{ -1, (SDK::CrDataType)0, "followFocusPositionSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_FTP_AutoTransfer,	{ -1, (SDK::CrDataType)0, "fTP_AutoTransfer", 0, &map_CrFTPAutoTransfer, } },
	{ PCode::CrDeviceProperty_FTP_AutoTransferTarget,	{ -1, (SDK::CrDataType)0, "fTP_AutoTransferTarget", 0, &map_CrFTPAutoTransferTarget, } },
	{ PCode::CrDeviceProperty_FTP_ConnectionErrorInfo,	{ -1, (SDK::CrDataType)0, "fTP_ConnectionErrorInfo", 0, &map_CrFTPConnectionErrorInfo, } },
	{ PCode::CrDeviceProperty_FTP_ConnectionStatus,	{ -1, (SDK::CrDataType)0, "fTP_ConnectionStatus", 0, &map_CrFTPConnectionStatus, } },
	{ PCode::CrDeviceProperty_FTP_Function,	{ -1, (SDK::CrDataType)0, "fTP_Function", 0, &map_CrFTPFunction, } },
	{ PCode::CrDeviceProperty_FTP_PowerSave,	{ -1, (SDK::CrDataType)0, "fTP_PowerSave", 0, &map_CrFTPPowerSave, } },
	{ PCode::CrDeviceProperty_FTP_TransferTarget,	{ -1, (SDK::CrDataType)0, "fTP_TransferTarget", 0, &map_CrFTPTransferTargetStill, } },
	{ PCode::CrDeviceProperty_FTPJobListDataVersion,	{ -1, (SDK::CrDataType)0, "fTPJobListDataVersion", 0, 0, } },
	{ PCode::CrDeviceProperty_FTPServerSettingOperationEnableStatus,	{ -1, (SDK::CrDataType)0, "fTPServerSettingOperationEnableStatus", 0, &map_CrFTPServerSettingOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_FTPServerSettingVersion,	{ -1, (SDK::CrDataType)0, "fTPServerSettingVersion", 0, 0, } },
	{ PCode::CrDeviceProperty_FTPTransferSetting_ReadOperationEnableStatus,	{ -1, (SDK::CrDataType)0, "fTPTransferSetting_ReadOperationEnableStatus", 0, &map_CrFTPTransferSettingReadOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_FTPTransferSetting_SaveOperationEnableStatus,	{ -1, (SDK::CrDataType)0, "fTPTransferSetting_SaveOperationEnableStatus", 0, &map_CrFTPTransferSettingSaveOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_FTPTransferSetting_SaveRead_State,	{ -1, (SDK::CrDataType)0, "fTPTransferSetting_SaveRead_State", 0, &map_CrFTPTransferSettingSaveReadState, } },
	{ PCode::CrDeviceProperty_FunctionOfRemoteTouchOperation,	{ -1, (SDK::CrDataType)0, "functionOfRemoteTouchOperation", 0, &map_CrFunctionOfRemoteTouchOperation, } },
	{ PCode::CrDeviceProperty_FunctionOfTouchOperation,	{ -1, (SDK::CrDataType)0, "functionOfTouchOperation", 0, &map_CrFunctionOfTouchOperation, } },
	{ PCode::CrDeviceProperty_GaindBCurrentValue,	{ -1, (SDK::CrDataType)0, "gaindBCurrentValue", 0, 0, } },
	{ PCode::CrDeviceProperty_GainUnitSetting,	{ -1, (SDK::CrDataType)0, "gainUnitSetting", 0, &map_CrGainUnitSetting, } },
	{ PCode::CrDeviceProperty_HDMIResolutionStillPlay,	{ -1, (SDK::CrDataType)0, "hDMIResolutionStillPlay", 0, &map_CrHDMIResolution, } },
	{ PCode::CrDeviceProperty_HighIsoNR,	{ -1, (SDK::CrDataType)0, "highIsoNR", 0, &map_CrHighIsoNR, } },
	{ PCode::CrDeviceProperty_HighResolutionShutterSpeed,	{ -1, (SDK::CrDataType)0, "highResolutionShutterSpeed", 0, 0, } },
	{ PCode::CrDeviceProperty_HighResolutionShutterSpeedSetting,	{ -1, (SDK::CrDataType)0, "highResolutionShutterSpeedSetting", 0, &map_CrHighResolutionShutterSpeedSetting, } },
	{ PCode::CrDeviceProperty_HLGStillImage,	{ -1, (SDK::CrDataType)0, "hLGStillImage", 0, &map_CrHLGStillImage, } },
	{ PCode::CrDeviceProperty_ImageID_Num,	{ -1, (SDK::CrDataType)0, "imageID_Num", 0, 0, } },
	{ PCode::CrDeviceProperty_ImageID_Num_Setting,	{ -1, (SDK::CrDataType)0, "imageID_Num_Setting", 0, &map_CrImageIDNumSetting, } },
	{ PCode::CrDeviceProperty_ImageID_String,	{ -1, (SDK::CrDataType)0, "imageID_String", 0, 0, } },
	{ PCode::CrDeviceProperty_ImageSize,	{ -1, (SDK::CrDataType)0, "imageSize", 0, &map_CrImageSize, } },
	{ PCode::CrDeviceProperty_Interval_Rec_Mode,	{ -1, (SDK::CrDataType)0, "interval_Rec_Mode", 0, &map_CrIntervalRecMode, } },
	{ PCode::CrDeviceProperty_Interval_Rec_Status,	{ -1, (SDK::CrDataType)0, "interval_Rec_Status", 0, &map_CrIntervalRecStatus, } },
	{ PCode::CrDeviceProperty_IntervalRec_AETrackingSensitivity,	{ -1, (SDK::CrDataType)0, "intervalRec_AETrackingSensitivity", 0, &map_CrIntervalRecAETrackingSensitivity, } },
	{ PCode::CrDeviceProperty_IntervalRec_NumberOfShots,	{ -1, (SDK::CrDataType)0, "intervalRec_NumberOfShots", 0, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShootingInterval,	{ -1, (SDK::CrDataType)0, "intervalRec_ShootingInterval", 0, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShootingStartTime,	{ -1, (SDK::CrDataType)0, "intervalRec_ShootingStartTime", 0, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShootIntervalPriority,	{ -1, (SDK::CrDataType)0, "intervalRec_ShootIntervalPriority", 0, &map_CrIntervalRecShootIntervalPriority, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShutterType,	{ -1, (SDK::CrDataType)0, "intervalRec_ShutterType", 0, &map_CrIntervalRecShutterType, } },
	{ PCode::CrDeviceProperty_IsoAutoMinShutterSpeedManual,	{ -1, (SDK::CrDataType)0, "isoAutoMinShutterSpeedManual", 0, 0, } },
	{ PCode::CrDeviceProperty_IsoAutoMinShutterSpeedMode,	{ -1, (SDK::CrDataType)0, "isoAutoMinShutterSpeedMode", 0, &map_CrIsoAutoMinShutterSpeedMode, } },
	{ PCode::CrDeviceProperty_IsoAutoMinShutterSpeedPreset,	{ -1, (SDK::CrDataType)0, "isoAutoMinShutterSpeedPreset", 0, &map_CrIsoAutoMinShutterSpeedPreset, } },
	{ PCode::CrDeviceProperty_LensAssignableButton1,	{ -1, (SDK::CrDataType)0, "lensAssignableButton1", 0, &map_CrAssignableButton, } },
	{ PCode::CrDeviceProperty_LensAssignableButtonIndicator1,	{ -1, (SDK::CrDataType)0, "lensAssignableButtonIndicator1", 0, &map_CrAssignableButtonIndicator, } },
	{ PCode::CrDeviceProperty_LensInformationEnableStatus,	{ -1, (SDK::CrDataType)0, "lensInformationEnableStatus", 0, &map_CrLensInformationEnableStatus, } },
	{ PCode::CrDeviceProperty_LensModelName,	{ -1, (SDK::CrDataType)0, "lensModelName", 0, 0, } },
	{ PCode::CrDeviceProperty_LensVersionNumber,	{ -1, (SDK::CrDataType)0, "lensVersionNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_LiveViewDisplayEffect,	{ -1, (SDK::CrDataType)0, "liveViewDisplayEffect", 0, &map_CrLiveViewDisplayEffect, } },
	{ PCode::CrDeviceProperty_LiveViewStatus,	{ -1, (SDK::CrDataType)0, "liveViewStatus", 0, &map_CrLiveViewStatus, } },
	{ PCode::CrDeviceProperty_LongExposureNR,	{ -1, (SDK::CrDataType)0, "longExposureNR", 0, &map_CrLongExposureNR, } },
	{ PCode::CrDeviceProperty_Media_FormatProgressRate,	{ -1, (SDK::CrDataType)0, "media_FormatProgressRate", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_FileType,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_FileType", 0, &map_CrFileType, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_ImageQuality,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_ImageQuality", 0, &map_CrImageQuality, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_ImageSize,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_ImageSize", 0, &map_CrImageSize, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RAW_FileCompressionType,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_RAW_FileCompressionType", 0, &map_CrRAWFileCompressionType, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RemainingNumber,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_RemainingNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RemainingTime,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_RemainingTime", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_WritingState,	{ -1, (SDK::CrDataType)0, "mediaSLOT1_WritingState", 0, &map_CrMediaSlotWritingState, } },
	{ PCode::CrDeviceProperty_MediaSLOT1Player,	{ -1, (SDK::CrDataType)0, "mediaSLOT1Player", 0, &map_CrMediaPlayer, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_FileType,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_FileType", 0, &map_CrFileType, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_ImageQuality,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_ImageQuality", 0, &map_CrImageQuality, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_ImageSize,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_ImageSize", 0, &map_CrImageSize, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RAW_FileCompressionType,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_RAW_FileCompressionType", 0, &map_CrRAWFileCompressionType, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RemainingNumber,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_RemainingNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RemainingTime,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_RemainingTime", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_WritingState,	{ -1, (SDK::CrDataType)0, "mediaSLOT2_WritingState", 0, &map_CrMediaSlotWritingState, } },
	{ PCode::CrDeviceProperty_MediaSLOT2Player,	{ -1, (SDK::CrDataType)0, "mediaSLOT2Player", 0, &map_CrMediaPlayer, } },
	{ PCode::CrDeviceProperty_MeteringMode,	{ -1, (SDK::CrDataType)0, "meteringMode", 0, &map_CrMeteringMode, } },
	{ PCode::CrDeviceProperty_MonitoringOutputDisplayHDMI,	{ -1, (SDK::CrDataType)0, "monitoringOutputDisplayHDMI", 0, &map_CrMonitoringOutputDisplayHDMI, } },
	{ PCode::CrDeviceProperty_Movie_File_Format,	{ -1, (SDK::CrDataType)0, "movie_File_Format", 0, &map_CrFileFormatMovie, } },
	{ PCode::CrDeviceProperty_Movie_FTP_AutoTransferTarget,	{ -1, (SDK::CrDataType)0, "movie_FTP_AutoTransferTarget", 0, &map_CrFTPAutoTransferTargetMovie, } },
	{ PCode::CrDeviceProperty_Movie_FTP_TransferTarget,	{ -1, (SDK::CrDataType)0, "movie_FTP_TransferTarget", 0, &map_CrFTPTransferTargetMovie, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutput4KSetting,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutput4KSetting", 0, &map_CrHDMIOutput4KSettingMovie, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputAudioCH,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputAudioCH", 0, &map_CrHDMIOutputAudioCH, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRAW,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputRAW", 0, &map_CrHDMIOutputRAWMovie, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRawSetting,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputRawSetting", 0, &map_CrHDMIOutputRawSettingMovie, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRecControl,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputRecControl", 0, &map_CrHDMIOutputRecControlMovie, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRecMedia,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputRecMedia", 0, &map_CrHDMIOutputRecMediaMovie, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputResolution,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputResolution", 0, &map_CrHDMIResolution, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputTimeCode,	{ -1, (SDK::CrDataType)0, "movie_HDMIOutputTimeCode", 0, &map_CrHDMIOutputTimeCodeMovie, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_CountDownIntervalTime,	{ -1, (SDK::CrDataType)0, "movie_IntervalRec_CountDownIntervalTime", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_FrameRateSetting,	{ -1, (SDK::CrDataType)0, "movie_IntervalRec_FrameRateSetting", 0, &map_CrRecordingFrameRateSettingMovie, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_IntervalTime,	{ -1, (SDK::CrDataType)0, "movie_IntervalRec_IntervalTime", 0, &map_CrIntervalRecTimeMovie, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_RecordingDuration,	{ -1, (SDK::CrDataType)0, "movie_IntervalRec_RecordingDuration", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_RecordingSetting,	{ -1, (SDK::CrDataType)0, "movie_IntervalRec_RecordingSetting", 0, &map_CrRecordingSettingMovie, } },
	{ PCode::CrDeviceProperty_Movie_ProxyFileFormat,	{ -1, (SDK::CrDataType)0, "movie_ProxyFileFormat", 0, &map_CrFileFormatMovie, } },
	{ PCode::CrDeviceProperty_Movie_Recording_FrameRateProxySetting,	{ -1, (SDK::CrDataType)0, "movie_Recording_FrameRateProxySetting", 0, &map_CrRecordingFrameRateSettingMovie, } },
	{ PCode::CrDeviceProperty_Movie_Recording_FrameRateSetting,	{ -1, (SDK::CrDataType)0, "movie_Recording_FrameRateSetting", 0, &map_CrRecordingFrameRateSettingMovie, } },
	{ PCode::CrDeviceProperty_Movie_Recording_ResolutionForMain,	{ -1, (SDK::CrDataType)0, "movie_Recording_ResolutionForMain", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_Recording_ResolutionForProxy,	{ -1, (SDK::CrDataType)0, "movie_Recording_ResolutionForProxy", 0, 0, } },
	{ PCode::CrDeviceProperty_MovieForwardButton,	{ -1, (SDK::CrDataType)0, "movieForwardButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MovieNextButton,	{ -1, (SDK::CrDataType)0, "movieNextButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MoviePlayButton,	{ -1, (SDK::CrDataType)0, "moviePlayButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MoviePlayingSpeed,	{ -1, (SDK::CrDataType)0, "moviePlayingSpeed", 0, 0, } },
	{ PCode::CrDeviceProperty_MoviePlayingState,	{ -1, (SDK::CrDataType)0, "moviePlayingState", 0, &map_CrMoviePlayingState, } },
	{ PCode::CrDeviceProperty_MoviePlayPauseButton,	{ -1, (SDK::CrDataType)0, "moviePlayPauseButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MoviePlayStopButton,	{ -1, (SDK::CrDataType)0, "moviePlayStopButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MoviePrevButton,	{ -1, (SDK::CrDataType)0, "moviePrevButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MovieRecReviewButton,	{ -1, (SDK::CrDataType)0, "movieRecReviewButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MovieRewindButton,	{ -1, (SDK::CrDataType)0, "movieRewindButton", 0, &map_CrMovieXButton, } },
	{ PCode::CrDeviceProperty_MovieShootingModeColorGamut,	{ -1, (SDK::CrDataType)0, "movieShootingModeColorGamut", 0, &map_CrMovieShootingModeColorGamut, } },
	{ PCode::CrDeviceProperty_MovieShootingModeTargetDisplay,	{ -1, (SDK::CrDataType)0, "movieShootingModeTargetDisplay", 0, &map_CrMovieShootingModeTargetDisplay, } },
	{ PCode::CrDeviceProperty_NDFilter,	{ -1, (SDK::CrDataType)0, "nDFilter", 0, &map_CrNDFilter, } },
	{ PCode::CrDeviceProperty_NDFilterMode,	{ -1, (SDK::CrDataType)0, "nDFilterMode", 0, &map_CrNDFilterMode, } },
	{ PCode::CrDeviceProperty_NDFilterModeSetting,	{ -1, (SDK::CrDataType)0, "nDFilterModeSetting", 0, &map_CrNDFilterModeSetting, } },
	{ PCode::CrDeviceProperty_NDFilterSwitchingSetting,	{ -1, (SDK::CrDataType)0, "nDFilterSwitchingSetting", 0, &map_CrNDFilterSwitchingSetting, } },
	{ PCode::CrDeviceProperty_NDFilterValue,	{ -1, (SDK::CrDataType)0, "nDFilterValue", 0, 0, } },
	{ PCode::CrDeviceProperty_NearFar,	{ -1, (SDK::CrDataType)0, "nearFar", 0, &map_CrNearFarEnableStatus, } },
	{ PCode::CrDeviceProperty_PictureEffect,	{ -1, (SDK::CrDataType)0, "pictureEffect", 0, &map_CrPictureEffect, } },
	{ PCode::CrDeviceProperty_PictureProfile,	{ -1, (SDK::CrDataType)0, "pictureProfile", 0, &map_CrPictureProfile, } },
	{ PCode::CrDeviceProperty_PictureProfile_BlackGammaLevel,	{ -1, (SDK::CrDataType)0, "pictureProfile_BlackGammaLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_BlackGammaRange,	{ -1, (SDK::CrDataType)0, "pictureProfile_BlackGammaRange", 0, &map_CrPictureProfileBlackGammaRange, } },
	{ PCode::CrDeviceProperty_PictureProfile_BlackLevel,	{ -1, (SDK::CrDataType)0, "pictureProfile_BlackLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthBlue,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorDepthBlue", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthCyan,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorDepthCyan", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthGreen,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorDepthGreen", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthMagenta,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorDepthMagenta", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthRed,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorDepthRed", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthYellow,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorDepthYellow", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorMode,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorMode", 0, &map_CrPictureProfileColorMode, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorPhase,	{ -1, (SDK::CrDataType)0, "pictureProfile_ColorPhase", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_Copy,	{ -1, (SDK::CrDataType)0, "pictureProfile_Copy", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustBWBalance,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailAdjustBWBalance", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustCrispening,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailAdjustCrispening", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustHiLightDetail,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailAdjustHiLightDetail", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustLimit,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailAdjustLimit", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustMode,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailAdjustMode", 0, &map_CrPictureProfileDetailAdjustMode, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustVHBalance,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailAdjustVHBalance", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailLevel,	{ -1, (SDK::CrDataType)0, "pictureProfile_DetailLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_Gamma,	{ -1, (SDK::CrDataType)0, "pictureProfile_Gamma", 0, &map_CrPictureProfileGamma, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeAutoSet_MaxPoint,	{ -1, (SDK::CrDataType)0, "pictureProfile_KneeAutoSet_MaxPoint", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeAutoSet_Sensitivity,	{ -1, (SDK::CrDataType)0, "pictureProfile_KneeAutoSet_Sensitivity", 0, &map_CrPictureProfileKneeAutoSetSensitivity, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeManualSet_Point,	{ -1, (SDK::CrDataType)0, "pictureProfile_KneeManualSet_Point", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeManualSet_Slope,	{ -1, (SDK::CrDataType)0, "pictureProfile_KneeManualSet_Slope", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeMode,	{ -1, (SDK::CrDataType)0, "pictureProfile_KneeMode", 0, &map_CrPictureProfileKneeMode, } },
	{ PCode::CrDeviceProperty_PictureProfile_Saturation,	{ -1, (SDK::CrDataType)0, "pictureProfile_Saturation", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfileResetEnableStatus,	{ -1, (SDK::CrDataType)0, "pictureProfileResetEnableStatus", 0, &map_CrPictureProfileResetEnableStatus, } },
	{ PCode::CrDeviceProperty_PixelMappingEnableStatus,	{ -1, (SDK::CrDataType)0, "pixelMappingEnableStatus", 0, &map_CrPixelMappingEnableStatus, } },
	{ PCode::CrDeviceProperty_PlaybackVolumeSettings,	{ -1, (SDK::CrDataType)0, "playbackVolumeSettings", 0, 0, } },
	{ PCode::CrDeviceProperty_PowerSource,	{ -1, (SDK::CrDataType)0, "powerSource", 0, &map_CrPowerSource, } },
	{ PCode::CrDeviceProperty_PrioritySetInAF_C,	{ -1, (SDK::CrDataType)0, "prioritySetInAF_C", 0, &map_CrPrioritySetInAF, } },
	{ PCode::CrDeviceProperty_PrioritySetInAF_S,	{ -1, (SDK::CrDataType)0, "prioritySetInAF_S", 0, &map_CrPrioritySetInAF, } },
	{ PCode::CrDeviceProperty_ProxyRecordingSetting,	{ -1, (SDK::CrDataType)0, "proxyRecordingSetting", 0, &map_CrProxyRecordingSetting, } },
	{ PCode::CrDeviceProperty_RAW_FileCompressionType,	{ -1, (SDK::CrDataType)0, "rAW_FileCompressionType", 0, &map_CrRAWFileCompressionType, } },
	{ PCode::CrDeviceProperty_RAW_J_PC_Save_Image,	{ -1, (SDK::CrDataType)0, "rAW_J_PC_Save_Image", 0, &map_CrPropertyRAWJPCSaveImage, } },
	{ PCode::CrDeviceProperty_RecognitionTarget,	{ -1, (SDK::CrDataType)0, "recognitionTarget", 0, &map_CrRecognitionTarget, } },
	{ PCode::CrDeviceProperty_RecorderClipName,	{ -1, (SDK::CrDataType)0, "recorderClipName", 0, 0, } },
	{ PCode::CrDeviceProperty_RecorderControlMainSetting,	{ -1, (SDK::CrDataType)0, "recorderControlMainSetting", 0, &map_CrRecorderControlSetting, } },
	{ PCode::CrDeviceProperty_RecorderControlProxySetting,	{ -1, (SDK::CrDataType)0, "recorderControlProxySetting", 0, &map_CrRecorderControlSetting, } },
	{ PCode::CrDeviceProperty_RecorderExtRawStatus,	{ -1, (SDK::CrDataType)0, "recorderExtRawStatus", 0, &map_CrRecorderStatus, } },
	{ PCode::CrDeviceProperty_RecorderMainStatus,	{ -1, (SDK::CrDataType)0, "recorderMainStatus", 0, &map_CrRecorderStatus, } },
	{ PCode::CrDeviceProperty_RecorderProxyStatus,	{ -1, (SDK::CrDataType)0, "recorderProxyStatus", 0, &map_CrRecorderStatus, } },
	{ PCode::CrDeviceProperty_RecorderSaveDestination,	{ -1, (SDK::CrDataType)0, "recorderSaveDestination", 0, &map_CrRecorderSaveDestination, } },
	{ PCode::CrDeviceProperty_RecorderStartMain,	{ -1, (SDK::CrDataType)0, "recorderStartMain", 0, &map_CrRecorderStart, } },
	{ PCode::CrDeviceProperty_RecorderStartProxy,	{ -1, (SDK::CrDataType)0, "recorderStartProxy", 0, &map_CrRecorderStart, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimer,	{ -1, (SDK::CrDataType)0, "recordingSelfTimer", 0, &map_CrMovieRecordingSelfTimer, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimerContinuous,	{ -1, (SDK::CrDataType)0, "recordingSelfTimerContinuous", 0, &map_CrMovieRecordingSelfTimerContinuous, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimerCountTime,	{ -1, (SDK::CrDataType)0, "recordingSelfTimerCountTime", 0, 0, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimerStatus,	{ -1, (SDK::CrDataType)0, "recordingSelfTimerStatus", 0, &map_CrMovieRecordingSelfTimerStatus, } },
	{ PCode::CrDeviceProperty_RecordingState,	{ -1, (SDK::CrDataType)0, "recordingState", 0, &map_CrMovie_Recording_State, } },
	{ PCode::CrDeviceProperty_RedEyeReduction,	{ -1, (SDK::CrDataType)0, "redEyeReduction", 0, &map_CrRedEyeReduction, } },
	{ PCode::CrDeviceProperty_RemoteTouchOperation,	{ -1, (SDK::CrDataType)0, "remoteTouchOperation", 0, 0, } },
	{ PCode::CrDeviceProperty_RemoteTouchOperationEnableStatus,	{ -1, (SDK::CrDataType)0, "remoteTouchOperationEnableStatus", 0, &map_CrRemoteTouchOperationEnableStatus, } },
	{ PCode::CrDeviceProperty_RightLeftEyeSelect,	{ -1, (SDK::CrDataType)0, "rightLeftEyeSelect", 0, &map_CrRightLeftEyeSelect, } },
	{ PCode::CrDeviceProperty_SceneFileIndex,	{ -1, (SDK::CrDataType)0, "sceneFileIndex", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectFinder,	{ -1, (SDK::CrDataType)0, "selectFinder", 0, &map_CrSelectFinder, } },
	{ PCode::CrDeviceProperty_SelectFTPServer,	{ -1, (SDK::CrDataType)0, "selectFTPServer", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectFTPServerID,	{ -1, (SDK::CrDataType)0, "selectFTPServerID", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectUserBaseLookToEdit,	{ -1, (SDK::CrDataType)0, "selectUserBaseLookToEdit", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectUserBaseLookToSetInPPLUT,	{ -1, (SDK::CrDataType)0, "selectUserBaseLookToSetInPPLUT", 0, 0, } },
	{ PCode::CrDeviceProperty_SensorCleaningEnableStatus,	{ -1, (SDK::CrDataType)0, "sensorCleaningEnableStatus", 0, &map_CrSensorCleaningEnableStatus, } },
	{ PCode::CrDeviceProperty_ShutterAngle,	{ -1, (SDK::CrDataType)0, "shutterAngle", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSFrequency,	{ -1, (SDK::CrDataType)0, "shutterECSFrequency", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSNumber,	{ -1, (SDK::CrDataType)0, "shutterECSNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSNumberStep,	{ -1, (SDK::CrDataType)0, "shutterECSNumberStep", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSSetting,	{ -1, (SDK::CrDataType)0, "shutterECSSetting", 0, &map_CrShutterECSSetting, } },
	{ PCode::CrDeviceProperty_ShutterMode,	{ -1, (SDK::CrDataType)0, "shutterMode", 0, &map_CrShutterModeSetting, } },
	{ PCode::CrDeviceProperty_ShutterModeStatus,	{ -1, (SDK::CrDataType)0, "shutterModeStatus", 0, &map_CrShutterModeStatus, } },
	{ PCode::CrDeviceProperty_ShutterSetting,	{ -1, (SDK::CrDataType)0, "shutterSetting", 0, &map_CrShutterSetting, } },
	{ PCode::CrDeviceProperty_ShutterSlow,	{ -1, (SDK::CrDataType)0, "shutterSlow", 0, &map_CrShutterSlow, } },
	{ PCode::CrDeviceProperty_ShutterSlowFrames,	{ -1, (SDK::CrDataType)0, "shutterSlowFrames", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterSpeedCurrentValue,	{ -1, (SDK::CrDataType)0, "shutterSpeedCurrentValue", 0, 0, } },
//	{ PCode::CrDeviceProperty_ShutterSpeedValue,	{ -1, SDK::CrDataType::CrDataType_UInt64, "shutterSpeedValue", format_shutter_speed_value, 0, } },
	{ PCode::CrDeviceProperty_SnapshotInfo,	{ -1, (SDK::CrDataType)0, "snapshotInfo", 0, 0, } },
	{ PCode::CrDeviceProperty_SoftSkinEffect,	{ -1, (SDK::CrDataType)0, "softSkinEffect", 0, &map_CrSoftSkinEffect, } },
	{ PCode::CrDeviceProperty_SoftwareVersion,	{ -1, (SDK::CrDataType)0, "softwareVersion", 0, 0, } },
	{ PCode::CrDeviceProperty_SQFrameRate,	{ -1, (SDK::CrDataType)0, "sQFrameRate", 0, 0, } },
	{ PCode::CrDeviceProperty_SQRecordingFrameRateSetting,	{ -1, (SDK::CrDataType)0, "sQRecordingFrameRateSetting", 0, &map_CrRecordingFrameRateSettingMovie, } },
	{ PCode::CrDeviceProperty_SQRecordingSetting,	{ -1, (SDK::CrDataType)0, "sQRecordingSetting", 0, &map_CrRecordingSettingMovie, } },
	{ PCode::CrDeviceProperty_Still_Image_Trans_Size,	{ -1, (SDK::CrDataType)0, "still_Image_Trans_Size", 0, &map_CrPropertyStillImageTransSize, } },
	{ PCode::CrDeviceProperty_StillImageQuality,	{ -1, SDK::CrDataType::CrDataType_UInt16, "stillImageQuality", 0, &map_CrImageQuality, } },
	{ PCode::CrDeviceProperty_StillImageStoreDestination,	{ -1, (SDK::CrDataType)0, "stillImageStoreDestination", 0, &map_CrStillImageStoreDestination, } },
	{ PCode::CrDeviceProperty_SubjectRecognitionAF,	{ -1, (SDK::CrDataType)0, "subjectRecognitionAF", 0, &map_CrSubjectRecognitionAF, } },
	{ PCode::CrDeviceProperty_SubjectRecognitionInAF,	{ -1, (SDK::CrDataType)0, "subjectRecognitionInAF", 0, &map_CrSubjectRecognitionInAF, } },
	{ PCode::CrDeviceProperty_TimeCodeFormat,	{ -1, (SDK::CrDataType)0, "timeCodeFormat", 0, &map_CrTimeCodeFormat, } },
	{ PCode::CrDeviceProperty_TimeCodeMake,	{ -1, (SDK::CrDataType)0, "timeCodeMake", 0, &map_CrTimeCodeMake, } },
	{ PCode::CrDeviceProperty_TimeCodePreset,	{ -1, (SDK::CrDataType)0, "timeCodePreset", 0, 0, } },
	{ PCode::CrDeviceProperty_TimeCodePresetResetEnableStatus,	{ -1, (SDK::CrDataType)0, "timeCodePresetResetEnableStatus", 0, &map_CrTimeCodePresetResetEnableStatus, } },
	{ PCode::CrDeviceProperty_TimeCodeRun,	{ -1, (SDK::CrDataType)0, "timeCodeRun", 0, &map_CrTimeCodeRun, } },
	{ PCode::CrDeviceProperty_TouchOperation,	{ -1, (SDK::CrDataType)0, "touchOperation", 0, &map_CrTouchOperation, } },
	{ PCode::CrDeviceProperty_UpdateBodyStatus,	{ -1, (SDK::CrDataType)0, "updateBodyStatus", 0, &map_CrUpdateStatus, } },
	{ PCode::CrDeviceProperty_USBPowerSupply,	{ -1, (SDK::CrDataType)0, "uSBPowerSupply", 0, &map_CrUSBPowerSupply, } },
	{ PCode::CrDeviceProperty_UserBaseLookAELevelOffset,	{ -1, (SDK::CrDataType)0, "userBaseLookAELevelOffset", 0, 0, } },
	{ PCode::CrDeviceProperty_UserBaseLookInput,	{ -1, (SDK::CrDataType)0, "userBaseLookInput", 0, &map_CrUserBaseLookInput, } },
	{ PCode::CrDeviceProperty_UserBitPreset,	{ -1, (SDK::CrDataType)0, "userBitPreset", 0, 0, } },
	{ PCode::CrDeviceProperty_UserBitPresetResetEnableStatus,	{ -1, (SDK::CrDataType)0, "userBitPresetResetEnableStatus", 0, &map_CrUserBitPresetResetEnableStatus, } },
	{ PCode::CrDeviceProperty_UserBitTimeRec,	{ -1, (SDK::CrDataType)0, "userBitTimeRec", 0, &map_CrUserBitTimeRec, } },
	{ PCode::CrDeviceProperty_WhiteBalanceModeSetting,	{ -1, (SDK::CrDataType)0, "whiteBalanceModeSetting", 0, &map_CrWhiteBalanceModeSetting, } },
	{ PCode::CrDeviceProperty_WindNoiseReduct,	{ -1, (SDK::CrDataType)0, "windNoiseReduct", 0, &map_CrWindNoiseReduction, } },
	{ PCode::CrDeviceProperty_WirelessFlash,	{ -1, (SDK::CrDataType)0, "wirelessFlash", 0, &map_CrWirelessFlash, } },
	{ PCode::CrDeviceProperty_Zoom_Scale,	{ -1, (SDK::CrDataType)0, "zoom_Scale", 0, 0, } },
	{ PCode::CrDeviceProperty_ZoomDistanceUnitSetting,	{ -1, (SDK::CrDataType)0, "zoomDistanceUnitSetting", 0, &map_CrZoomDistanceUnitSetting, } },

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

std::string CameraDevice::PropCurrentText(PCode id) const
{
	auto iter1 = Prop.find(id);
	if(iter1 == end(Prop))
		return "";

	uint32_t current = iter1->second.current;
	if(iter1->second.mapEnum) {
		auto iter2 = iter1->second.mapEnum->find(current);
		if(iter2 != end(*iter1->second.mapEnum))
			return iter2->second;
//	} else if(iter1->second->formatFunc) {
//		return iter1->second->formatFunc(
	}
	return std::to_string(current);
}

void CameraDevice::get_select_media_format()
{
    load_properties();
    std::cout << "Media SLOT1 Full Format Enable Status: " << PropCurrentText(PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus) << std::endl;
    std::cout << "Media SLOT2 Full Format Enable Status: " << PropCurrentText(PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus) << std::endl;
    // Valid Quick format
    if (-1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus).writable || -1 != Prop.at(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus).writable){
        std::cout << "Media SLOT1 Quick Format Enable Status: " << PropCurrentText(PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus) << std::endl;
        std::cout << "Media SLOT2 Quick Format Enable Status: " << PropCurrentText(PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus) << std::endl;
    }
}

bool CameraDevice::get_custom_wb()
{
    bool state = false;
    load_properties();
    std::cout << "CustomWB Capture Standby Operation: " << PropCurrentText(PCode::CrDeviceProperty_CustomWB_Capture_Standby) << '\n';
    std::cout << "CustomWB Capture Standby CancelOperation: " << PropCurrentText(PCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel) << '\n';
    std::cout << "CustomWB Capture Operation: " << PropCurrentText(PCode::CrDeviceProperty_CustomWB_Capture_Operation) << '\n';
    std::cout << "CustomWB Capture Execution State: " << PropCurrentText(PCode::CrDeviceProperty_CustomWB_Execution_State) << '\n';
    if (Prop.at(PCode::CrDeviceProperty_CustomWB_Capture_Operation).current == 1) {
        state = true;
    }
    return state;
}

void CameraDevice::get_zoom_operation()
{
    load_properties();
    std::cout << "Zoom Operation Status: " << PropCurrentText(PCode::CrDeviceProperty_Zoom_Operation_Status) << '\n';
    if (Prop.at(PCode::CrDeviceProperty_Zoom_Setting).current > 0) {
        std::cout << "Zoom Setting Type    : " << PropCurrentText(PCode::CrDeviceProperty_Zoom_Setting) << '\n';
    }
    if (Prop.at(PCode::CrDeviceProperty_Zoom_Type_Status).current > 0) {
        std::cout << "Zoom Type Status     : " << PropCurrentText(PCode::CrDeviceProperty_Zoom_Type_Status) << '\n';
    }

    // Zoom Speed Range is not supported
    if (Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.size() < 2) {
        std::cout << "Zoom Speed Range     : -1 to 1" << std::endl 
             << "Zoom Speed Type      : " <<  PropCurrentText(PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type) << std::endl;
    }
    else {
        std::cout << "Zoom Speed Range     : " << (int)Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.at(0) << " to " << (int)Prop.at(PCode::CrDeviceProperty_Zoom_Speed_Range).possible.at(1) << std::endl
             << "Zoom Speed Type      : " <<  PropCurrentText(PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type) << std::endl;
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
        auto& prop = prop_list[0];
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
    std::cout << "APS-C/FULL Switching Enable Status: " <<  PropCurrentText(PCode::CrDeviceProperty_APS_C_or_Full_SwitchingEnableStatus) << '\n';
    std::cout << "APS-C/FULL Switching Setting: " <<  PropCurrentText(PCode::CrDeviceProperty_APS_C_or_Full_SwitchingSetting) << '\n';
    return true;
}

bool CameraDevice::get_camera_setting_saveread_state()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_CameraSetting_SaveRead_State).current == SDK::CrCameraSettingSaveReadState::CrCameraSettingSaveReadState_Reading) {
        tout << "Unable to download/upload Camera-Setting file. \n";
        return false;
    }
    std::cout << "Camera-Setting Save/Read State: " <<  PropCurrentText(PCode::CrDeviceProperty_CameraSetting_SaveRead_State) << '\n';
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
        tout << "BaseLook Value: Index" << (int)valLO << " " << (int)(valHI) << '\n';
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
    std::cout << "Media SLOT1 Status                  : " <<  PropCurrentText(PCode::CrDeviceProperty_MediaSLOT1_Status) << std::endl;
    if (0 <= Prop.at(PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType).writable)
    {
        std::cout << "Media SLOT1 Recording Available Type: " <<  PropCurrentText(PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType) << std::endl;
    }

    // SLOT2
    if (-1 == Prop.at(PCode::CrDeviceProperty_MediaSLOT2_Status).writable)
    {
        tout << "Media SLOT2 Status is not supported.\n";
    }
    else
    {
        std::cout << "Media SLOT2 Status                  : " <<  PropCurrentText(PCode::CrDeviceProperty_MediaSLOT2_Status) << std::endl;
        if (0 <= Prop.at(PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType).writable)
        {
            std::cout << "Media SLOT2 Recording Available Type: " <<  PropCurrentText(PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType) << std::endl;
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
    std::cout << "Movie Rec Button(Toggle) Enable Status: " <<  PropCurrentText(PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus) << '\n';
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
    std::cout << "White Balance: " <<  PropCurrentText(PCode::CrDeviceProperty_WhiteBalance) << '\n';

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
            tout << '[' << i << "] Index" << (int)get_baseLookValue << " " << (int)(baseValueSetter) << '\n';
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

    if (1 != Prop.at(PCode::CrDeviceProperty_FocusBracketShotNumber).writable
     || 1 != Prop.at(PCode::CrDeviceProperty_FocusBracketFocusRange).writable) {
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
	prop.dataType = devProp.GetValueType();

	int dataLen = 1;
	switch(prop.dataType & 0x100F) {
	case SDK::CrDataType::CrDataType_UInt8:   dataLen = sizeof(std::uint8_t); break;
	case SDK::CrDataType::CrDataType_Int8:    dataLen = sizeof(std::int8_t); break;
	case SDK::CrDataType::CrDataType_UInt16:  dataLen = sizeof(std::uint16_t); break;
	case SDK::CrDataType::CrDataType_Int16:   dataLen = sizeof(std::int16_t); break;
	case SDK::CrDataType::CrDataType_UInt32:  dataLen = sizeof(std::uint32_t); break;
	case SDK::CrDataType::CrDataType_UInt64:  dataLen = sizeof(std::uint64_t); break;
	default: return;
	}

	unsigned char const* buf = devProp.GetValues();
	std::uint32_t nval = devProp.GetValueSize() / dataLen;
	prop.possible.resize(nval);
	for (std::uint32_t i = 0; i < nval; ++i) {
		int32_t data = 0;
		switch(prop.dataType & 0x100F) {
		case SDK::CrDataType::CrDataType_UInt8:		data = (reinterpret_cast<std::uint8_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_Int8:		data = (reinterpret_cast<std::int8_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt16:	data = (reinterpret_cast<std::uint16_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_Int16:		data = (reinterpret_cast<std::int16_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt32:	data = (reinterpret_cast<std::uint32_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt64:	data = (reinterpret_cast<std::uint64_t const*>(buf))[i]; break;		// **
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
        // Got properties list
        for (std::int32_t i = 0; i < nprop; ++i) {
            auto devProp = prop_list[i];
			PCode id = (PCode)devProp.GetCode();
			auto iter = Prop.find(id);
#if 1
			std::cout << std::hex << id << ":";
			if ( iter != end(Prop) ) {
				std::cout << iter->second.tag.c_str();
			} else {
				std::cout << "unknown ";
			}
		//	std::cout << " " << devProp.GetValueType() << "\n";
			std::cout << " " << PropCurrentText(id) << "\n";
#endif

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

    std::cout << "Focus Driving Status: " <<  PropCurrentText(PCode::CrDeviceProperty_FocusDrivingStatus) << std::endl;
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

	SDK::CrDataType type = (SDK::CrDataType)(Prop.at(id).dataType & 0x100F);
	if(type == SDK::CrDataType::CrDataType_Undefined)
		type = SDK::CrDataType::CrDataType_UInt32;

	SDK::CrDeviceProperty devProp;
	devProp.SetCode(id);
	devProp.SetCurrentValue(value);
	devProp.SetValueType(type);
	return SDK::SetDeviceProperty(m_device_handle, &devProp);
}

}
// namespace cli

