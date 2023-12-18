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

#include <future>
#include <chrono>


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
static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstring_convert;

namespace cli
{

using PCode = SDK::CrDevicePropertyCode;

//(.*)\t(.*)\t(.*)
//	{ PCode::CrDeviceProperty_\1,	{ -1, "\1", &map_\2, format_\3, } },

#include "CrDeviceProperty.cpp"		// map_xx実装

//	{ SDK::CrCommandId::CrCommandId_\1, "\1" },
std::map<SDK::CrCommandId, std::string> Cmds {
	{ SDK::CrCommandId::CrCommandId_Release, "Release" },
	{ SDK::CrCommandId::CrCommandId_MovieRecord, "MovieRecord" },
	{ SDK::CrCommandId::CrCommandId_CancelShooting, "CancelShooting" },
	{ SDK::CrCommandId::CrCommandId_MediaFormat, "MediaFormat" },
	{ SDK::CrCommandId::CrCommandId_MediaQuickFormat, "MediaQuickFormat" },
	{ SDK::CrCommandId::CrCommandId_CancelMediaFormat, "CancelMediaFormat" },
	{ SDK::CrCommandId::CrCommandId_S1andRelease, "S1andRelease" },
	{ SDK::CrCommandId::CrCommandId_CancelContentsTransfer, "CancelContentsTransfer" },
	{ SDK::CrCommandId::CrCommandId_CameraSettingsReset, "CameraSettingsReset" },
	{ SDK::CrCommandId::CrCommandId_APS_C_or_Full_Switching, "APS_C_or_Full_Switching" },
	{ SDK::CrCommandId::CrCommandId_MovieRecButtonToggle, "MovieRecButtonToggle" },
	{ SDK::CrCommandId::CrCommandId_CancelRemoteTouchOperation, "CancelRemoteTouchOperation" },
	{ SDK::CrCommandId::CrCommandId_PixelMapping, "PixelMapping" },
	{ SDK::CrCommandId::CrCommandId_TimeCodePresetReset, "TimeCodePresetReset" },
	{ SDK::CrCommandId::CrCommandId_UserBitPresetReset, "UserBitPresetReset" },
	{ SDK::CrCommandId::CrCommandId_SensorCleaning, "SensorCleaning" },
	{ SDK::CrCommandId::CrCommandId_PictureProfileReset, "PictureProfileReset" },
	{ SDK::CrCommandId::CrCommandId_CreativeLookReset, "CreativeLookReset" },
	{ SDK::CrCommandId::CrCommandId_PowerOff, "PowerOff" },
	{ SDK::CrCommandId::CrCommandId_CancelFocusPosition, "CancelFocusPosition" },
	{ SDK::CrCommandId::CrCommandId_FlickerScan, "FlickerScan" },
};

std::map<PCode, struct PropertyValue> Prop {

	{ PCode::CrDeviceProperty_AEL,	{ -1, "AEL", &map_CrLockIndicator, 0, } },
	{ PCode::CrDeviceProperty_AF_Area_Position,	{ -1, "AF_Area_Position", 0, 0, } },
	{ PCode::CrDeviceProperty_AFAssist,	{ -1, "AFAssist", &map_CrAFAssist, 0, } },
	{ PCode::CrDeviceProperty_AFSubjShiftSens,	{ -1, "AFSubjShiftSens", 0, 0, } },
	{ PCode::CrDeviceProperty_AFTrackingSensitivity,	{ -1, "AFTrackingSensitivity", &map_CrAFTrackingSensitivity, 0, } },
	{ PCode::CrDeviceProperty_AFTransitionSpeed,	{ -1, "AFTransitionSpeed", 0, 0, } },
	{ PCode::CrDeviceProperty_APS_C_or_Full_SwitchingEnableStatus,	{ -1, "APS_C_or_Full_SwitchingEnableStatus", &map_CrAPS_C_or_Full_SwitchingEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_APS_C_or_Full_SwitchingSetting,	{ -1, "APS_C_or_Full_SwitchingSetting", &map_CrAPS_C_or_Full_SwitchingSetting, 0, } },
	{ PCode::CrDeviceProperty_AspectRatio,	{ -1, "AspectRatio", &map_CrAspectRatioIndex, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton1,	{ -1, "AssignableButton1", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton2,	{ -1, "AssignableButton2", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton3,	{ -1, "AssignableButton3", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton4,	{ -1, "AssignableButton4", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton5,	{ -1, "AssignableButton5", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton6,	{ -1, "AssignableButton6", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton7,	{ -1, "AssignableButton7", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton8,	{ -1, "AssignableButton8", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButton9,	{ -1, "AssignableButton9", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator1,	{ -1, "AssignableButtonIndicator1", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator2,	{ -1, "AssignableButtonIndicator2", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator3,	{ -1, "AssignableButtonIndicator3", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator4,	{ -1, "AssignableButtonIndicator4", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator5,	{ -1, "AssignableButtonIndicator5", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator6,	{ -1, "AssignableButtonIndicator6", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator7,	{ -1, "AssignableButtonIndicator7", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator8,	{ -1, "AssignableButtonIndicator8", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AssignableButtonIndicator9,	{ -1, "AssignableButtonIndicator9", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_AudioInputMasterLevel,	{ -1, "AudioInputMasterLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_AudioRecording,	{ -1, "AudioRecording", &map_CrAudioRecording, 0, } },
	{ PCode::CrDeviceProperty_AudioSignals,	{ -1, "AudioSignals", &map_CrAudioSignals, 0, } },
	{ PCode::CrDeviceProperty_AutoPowerOffTemperature,	{ -1, "AutoPowerOffTemperature", &map_CrAutoPowerOffTemperature, 0, } },
	{ PCode::CrDeviceProperty_AutoReview,	{ -1, "AutoReview", 0, 0, } },
	{ PCode::CrDeviceProperty_AutoSlowShutter,	{ -1, "AutoSlowShutter", &map_CrAutoSlowShutter, 0, } },
	{ PCode::CrDeviceProperty_AWB,	{ -1, "AWB", &map_CrAWB, 0, } },
	{ PCode::CrDeviceProperty_AWBL,	{ -1, "AWBL", &map_CrLockIndicator, 0, } },
	{ PCode::CrDeviceProperty_BaseISOSwitchEI,	{ -1, "BaseISOSwitchEI", 0, 0, } },
	{ PCode::CrDeviceProperty_BaseLookImportOperationEnableStatus,	{ -1, "BaseLookImportOperationEnableStatus", &map_CrBaseLookImportOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_BaseLookValue,	{ -1, "BaseLookValue", 0, 0, } },
	{ PCode::CrDeviceProperty_BatteryLevel,	{ -1, "BatteryLevel", &map_CrBatteryLevel, 0, } },
	{ PCode::CrDeviceProperty_BatteryRemain,	{ -1, "BatteryRemain", 0, 0, } },
	{ PCode::CrDeviceProperty_BatteryRemainDisplayUnit,	{ -1, "BatteryRemainDisplayUnit", &map_CrBatteryRemainDisplayUnit, 0, } },
	{ PCode::CrDeviceProperty_BatteryRemainingInMinutes,	{ -1, "BatteryRemainingInMinutes", 0, 0, } },
	{ PCode::CrDeviceProperty_BatteryRemainingInVoltage,	{ -1, "BatteryRemainingInVoltage", 0, 0, } },
	{ PCode::CrDeviceProperty_BodyKeyLock,	{ -1, "BodyKeyLock", &map_CrBodyKeyLock, 0, } },
	{ PCode::CrDeviceProperty_BracketOrder,	{ -1, "BracketOrder", &map_CrBracketOrder, 0, } },
	{ PCode::CrDeviceProperty_BulbExposureTimeSetting,	{ -1, "BulbExposureTimeSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_BulbTimerSetting,	{ -1, "BulbTimerSetting", &map_CrBulbTimerSetting, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable1,	{ -1, "ButtonAssignmentAssignable1", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable2,	{ -1, "ButtonAssignmentAssignable2", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable3,	{ -1, "ButtonAssignmentAssignable3", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable4,	{ -1, "ButtonAssignmentAssignable4", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable5,	{ -1, "ButtonAssignmentAssignable5", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable6,	{ -1, "ButtonAssignmentAssignable6", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable7,	{ -1, "ButtonAssignmentAssignable7", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable8,	{ -1, "ButtonAssignmentAssignable8", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentAssignable9,	{ -1, "ButtonAssignmentAssignable9", 0, 0, } },
	{ PCode::CrDeviceProperty_ButtonAssignmentLensAssignable1,	{ -1, "ButtonAssignmentLensAssignable1", 0, 0, } },
	{ PCode::CrDeviceProperty_CameraEframing,	{ -1, "CameraEframing", &map_CrCameraEframing, 0, } },
	{ PCode::CrDeviceProperty_CameraSetting_ReadOperationEnableStatus,	{ -1, "CameraSetting_ReadOperationEnableStatus", &map_CrCameraSettingReadOperation, 0, } },
	{ PCode::CrDeviceProperty_CameraSetting_SaveOperationEnableStatus,	{ -1, "CameraSetting_SaveOperationEnableStatus", &map_CrCameraSettingSaveOperation, 0, } },
	{ PCode::CrDeviceProperty_CameraSetting_SaveRead_State,	{ -1, "CameraSetting_SaveRead_State", &map_CrCameraSettingSaveReadState, 0, } },
	{ PCode::CrDeviceProperty_CameraSettingsResetEnableStatus,	{ -1, "CameraSettingsResetEnableStatus", &map_CrCameraSettingsResetEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_CameraShakeStatus,	{ -1, "CameraShakeStatus", &map_CrCameraShakeStatus, 0, } },
	{ PCode::CrDeviceProperty_Cancel_Media_FormatEnableStatus,	{ -1, "Cancel_Media_FormatEnableStatus", &map_CrCancelMediaFormat, 0, } },
	{ PCode::CrDeviceProperty_CancelRemoteTouchOperationEnableStatus,	{ -1, "CancelRemoteTouchOperationEnableStatus", &map_CrCancelRemoteTouchOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_ColorSpace,	{ -1, "ColorSpace", &map_CrColorSpace, 0, } },
	{ PCode::CrDeviceProperty_Colortemp,	{ -1, "Colortemp", 0, 0, } },
	{ PCode::CrDeviceProperty_ColortempStep,	{ -1, "ColortempStep", 0, 0, } },
	{ PCode::CrDeviceProperty_ColorTuningAB,	{ -1, "ColorTuningAB", 0, 0, } },
	{ PCode::CrDeviceProperty_ColorTuningGM,	{ -1, "ColorTuningGM", 0, 0, } },
	{ PCode::CrDeviceProperty_CompressionFileFormatStill,	{ -1, "CompressionFileFormatStill", &map_CrCompressionFileFormat, 0, } },
	{ PCode::CrDeviceProperty_ContentsTransferCancelEnableStatus,	{ -1, "ContentsTransferCancelEnableStatus", &map_CrCancelContentsTransferEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_ContentsTransferProgress,	{ -1, "ContentsTransferProgress", 0, 0, } },
	{ PCode::CrDeviceProperty_ContentsTransferStatus,	{ -1, "ContentsTransferStatus", &map_CrContentsTransferStatus, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook,	{ -1, "CreativeLook", &map_CrCreativeLook, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Clarity,	{ -1, "CreativeLook_Clarity", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Contrast,	{ -1, "CreativeLook_Contrast", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_CustomLook,	{ -1, "CreativeLook_CustomLook", &map_CrCreativeLook, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Fade,	{ -1, "CreativeLook_Fade", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Highlights,	{ -1, "CreativeLook_Highlights", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Saturation,	{ -1, "CreativeLook_Saturation", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Shadows,	{ -1, "CreativeLook_Shadows", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_Sharpness,	{ -1, "CreativeLook_Sharpness", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLook_SharpnessRange,	{ -1, "CreativeLook_SharpnessRange", 0, 0, } },
	{ PCode::CrDeviceProperty_CreativeLookResetEnableStatus,	{ -1, "CreativeLookResetEnableStatus", &map_CrCreativeLookResetEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_CurrentSceneFileEdited,	{ -1, "CurrentSceneFileEdited", &map_CrCurrentSceneFileEdited, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capturable_Area,	{ -1, "CustomWB_Capturable_Area", 0, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture,	{ -1, "CustomWB_Capture", 0, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Frame_Size,	{ -1, "CustomWB_Capture_Frame_Size", 0, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Operation,	{ -1, "CustomWB_Capture_Operation", &map_CrPropertyCustomWBOperation, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Standby,	{ -1, "CustomWB_Capture_Standby", &map_CrPropertyCustomWBOperation, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Capture_Standby_Cancel,	{ -1, "CustomWB_Capture_Standby_Cancel", &map_CrPropertyCustomWBOperation, 0, } },
	{ PCode::CrDeviceProperty_CustomWB_Execution_State,	{ -1, "CustomWB_Execution_State", &map_CrPropertyCustomWBExecutionState, 0, } },
	{ PCode::CrDeviceProperty_DateTime_Settings,	{ -1, "DateTime_Settings", 0, 0, } },
	{ PCode::CrDeviceProperty_DCVoltage,	{ -1, "DCVoltage", 0, 0, } },
	{ PCode::CrDeviceProperty_DeleteUserBaseLook,	{ -1, "DeleteUserBaseLook", 0, 0, } },
	{ PCode::CrDeviceProperty_DepthOfFieldAdjustmentInterlockingMode,	{ -1, "DepthOfFieldAdjustmentInterlockingMode", &map_CrDepthOfFieldAdjustmentInterlockingMode, 0, } },
	{ PCode::CrDeviceProperty_DepthOfFieldAdjustmentMode,	{ -1, "DepthOfFieldAdjustmentMode", &map_CrDepthOfFieldAdjustmentMode, 0, } },
	{ PCode::CrDeviceProperty_DeviceOverheatingState,	{ -1, "DeviceOverheatingState", &map_CrDeviceOverheatingState, 0, } },
	{ PCode::CrDeviceProperty_DigitalZoomScale,	{ -1, "DigitalZoomScale", 0, 0, } },
	{ PCode::CrDeviceProperty_DispMode,	{ -1, "DispMode", &map_CrDispMode, 0, } },
	{ PCode::CrDeviceProperty_DispModeCandidate,	{ -1, "DispModeCandidate", 0, 0, } },
	{ PCode::CrDeviceProperty_DispModeSetting,	{ -1, "DispModeSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_DriveMode,	{ -1, "DriveMode", &map_CrDriveMode, 0, } },
	{ PCode::CrDeviceProperty_DRO,	{ -1, "DRO", &map_CrDRangeOptimizer, 0, } },
	{ PCode::CrDeviceProperty_EframingHDMICrop,	{ -1, "EframingHDMICrop", &map_CrEframingHDMICrop, 0, } },
	{ PCode::CrDeviceProperty_EframingModeAuto,	{ -1, "EframingModeAuto", &map_CrEframingModeAuto, 0, } },
	{ PCode::CrDeviceProperty_EframingRecordingImageCrop,	{ -1, "EframingRecordingImageCrop", &map_CrEframingRecordingImageCrop, 0, } },
	{ PCode::CrDeviceProperty_EframingScaleAuto,	{ -1, "EframingScaleAuto", &map_CrEframingScaleAuto, 0, } },
	{ PCode::CrDeviceProperty_EframingSpeedAuto,	{ -1, "EframingSpeedAuto", 0, 0, } },
	{ PCode::CrDeviceProperty_ExposureBiasCompensation,	{ -1, "ExposureBiasCompensation", 0, 0, } },
	{ PCode::CrDeviceProperty_ExposureCtrlType,	{ -1, "ExposureCtrlType", &map_CrExposureCtrlType, 0, } },
	{ PCode::CrDeviceProperty_ExposureIndex,	{ -1, "ExposureIndex", 0, 0, } },
	{ PCode::CrDeviceProperty_ExposureProgramMode,	{ -1, "ExposureProgramMode", &map_CrExposureProgram, 0, } },
	{ PCode::CrDeviceProperty_ExtendedInterfaceMode,	{ -1, "ExtendedInterfaceMode", &map_CrExtendedInterfaceMode, 0, } },
	{ PCode::CrDeviceProperty_FEL,	{ -1, "FEL", &map_CrLockIndicator, 0, } },
	{ PCode::CrDeviceProperty_FileType,	{ -1, "FileType", &map_CrFileType, 0, } },
	{ PCode::CrDeviceProperty_FlashCompensation,	{ -1, "FlashCompensation", 0, 0, } },
	{ PCode::CrDeviceProperty_FlashMode,	{ -1, "FlashMode", &map_CrFlashMode, 0, } },
	{ PCode::CrDeviceProperty_FlickerLessShooting,	{ -1, "FlickerLessShooting", &map_CrFlickerLessShooting, 0, } },
	{ PCode::CrDeviceProperty_FlickerScanEnableStatus,	{ -1, "FlickerScanEnableStatus", &map_CrFlickerScanEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_FlickerScanStatus,	{ -1, "FlickerScanStatus", &map_CrFlickerScanStatus, 0, } },
	{ PCode::CrDeviceProperty_FNumber,	{ -1, "FNumber", 0, format_f_number, } },
	{ PCode::CrDeviceProperty_FocalDistanceInFeet,	{ -1, "FocalDistanceInFeet", 0, 0, } },
	{ PCode::CrDeviceProperty_FocalDistanceInMeter,	{ -1, "FocalDistanceInMeter", 0, 0, } },
	{ PCode::CrDeviceProperty_FocalDistanceUnitSetting,	{ -1, "FocalDistanceUnitSetting", &map_CrFocalDistanceUnitSetting, 0, } },
	{ PCode::CrDeviceProperty_Focus_Magnifier_Setting,	{ -1, "Focus_Magnifier_Setting", 0, 0, } },
	{ PCode::CrDeviceProperty_Focus_Operation,	{ -1, "Focus_Operation", 0, 0, } },
	{ PCode::CrDeviceProperty_Focus_Speed_Range,	{ -1, "Focus_Speed_Range", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusArea,	{ -1, "FocusArea", &map_CrFocusArea, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketExposureLock1stImg,	{ -1, "FocusBracketExposureLock1stImg", &map_CrFocusBracketExposureLock1stImg, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketFocusRange,	{ -1, "FocusBracketFocusRange", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketIntervalUntilNextShot,	{ -1, "FocusBracketIntervalUntilNextShot", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketOrder,	{ -1, "FocusBracketOrder", &map_CrFocusBracketOrder, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketShootingStatus,	{ -1, "FocusBracketShootingStatus", &map_CrFocusBracketShootingStatus, 0, } },
	{ PCode::CrDeviceProperty_FocusBracketShotNumber,	{ -1, "FocusBracketShotNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusDrivingStatus,	{ -1, "FocusDrivingStatus", &map_CrFocusDrivingStatus, 0, } },
	{ PCode::CrDeviceProperty_FocusIndication,	{ -1, "FocusIndication", &map_CrFocusIndicator, 0, } },
	{ PCode::CrDeviceProperty_FocusMagnificationTime,	{ -1, "FocusMagnificationTime", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusMode,	{ -1, "FocusMode", &map_CrFocusMode, 0, } },
	{ PCode::CrDeviceProperty_FocusModeSetting,	{ -1, "FocusModeSetting", &map_CrFocusModeSetting, 0, } },
	{ PCode::CrDeviceProperty_FocusPositionCurrentValue,	{ -1, "FocusPositionCurrentValue", 0, format_focus_position_value, } },
	{ PCode::CrDeviceProperty_FocusPositionSetting,	{ -1, "FocusPositionSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_FocusTouchSpotStatus,	{ -1, "FocusTouchSpotStatus", &map_CrFocusTouchSpotStatus, 0, } },
	{ PCode::CrDeviceProperty_FocusTrackingStatus,	{ -1, "FocusTrackingStatus", &map_CrFocusTrackingStatus, 0, } },
	{ PCode::CrDeviceProperty_FollowFocusPositionCurrentValue,	{ -1, "FollowFocusPositionCurrentValue", 0, 0, } },
	{ PCode::CrDeviceProperty_FollowFocusPositionSetting,	{ -1, "FollowFocusPositionSetting", 0, 0, } },
	{ PCode::CrDeviceProperty_FTP_AutoTransfer,	{ -1, "FTP_AutoTransfer", &map_CrFTPAutoTransfer, 0, } },
	{ PCode::CrDeviceProperty_FTP_AutoTransferTarget,	{ -1, "FTP_AutoTransferTarget", &map_CrFTPAutoTransferTarget, 0, } },
	{ PCode::CrDeviceProperty_FTP_ConnectionErrorInfo,	{ -1, "FTP_ConnectionErrorInfo", &map_CrFTPConnectionErrorInfo, 0, } },
	{ PCode::CrDeviceProperty_FTP_ConnectionStatus,	{ -1, "FTP_ConnectionStatus", &map_CrFTPConnectionStatus, 0, } },
	{ PCode::CrDeviceProperty_FTP_Function,	{ -1, "FTP_Function", &map_CrFTPFunction, 0, } },
	{ PCode::CrDeviceProperty_FTP_PowerSave,	{ -1, "FTP_PowerSave", &map_CrFTPPowerSave, 0, } },
	{ PCode::CrDeviceProperty_FTP_TransferTarget,	{ -1, "FTP_TransferTarget", &map_CrFTPTransferTargetStill, 0, } },
	{ PCode::CrDeviceProperty_FTPJobListDataVersion,	{ -1, "FTPJobListDataVersion", 0, 0, } },
	{ PCode::CrDeviceProperty_FTPServerSettingOperationEnableStatus,	{ -1, "FTPServerSettingOperationEnableStatus", &map_CrFTPServerSettingOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_FTPServerSettingVersion,	{ -1, "FTPServerSettingVersion", 0, 0, } },
	{ PCode::CrDeviceProperty_FTPTransferSetting_ReadOperationEnableStatus,	{ -1, "FTPTransferSetting_ReadOperationEnableStatus", &map_CrFTPTransferSettingReadOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_FTPTransferSetting_SaveOperationEnableStatus,	{ -1, "FTPTransferSetting_SaveOperationEnableStatus", &map_CrFTPTransferSettingSaveOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_FTPTransferSetting_SaveRead_State,	{ -1, "FTPTransferSetting_SaveRead_State", &map_CrFTPTransferSettingSaveReadState, 0, } },
	{ PCode::CrDeviceProperty_FunctionOfRemoteTouchOperation,	{ -1, "FunctionOfRemoteTouchOperation", &map_CrFunctionOfRemoteTouchOperation, 0, } },
	{ PCode::CrDeviceProperty_FunctionOfTouchOperation,	{ -1, "FunctionOfTouchOperation", &map_CrFunctionOfTouchOperation, 0, } },
	{ PCode::CrDeviceProperty_GainBaseIsoSensitivity,	{ -1, "GainBaseIsoSensitivity", &map_CrGainBaseIsoSensitivity, 0, } },
	{ PCode::CrDeviceProperty_GainBaseSensitivity,	{ -1, "GainBaseSensitivity", &map_CrGainBaseSensitivity, 0, } },
	{ PCode::CrDeviceProperty_GainControlSetting,	{ -1, "GainControlSetting", &map_CrGainControlSetting, 0, } },
	{ PCode::CrDeviceProperty_GaindBCurrentValue,	{ -1, "GaindBCurrentValue", 0, 0, } },
	{ PCode::CrDeviceProperty_GaindBValue,	{ -1, "GaindBValue", 0, 0, } },
	{ PCode::CrDeviceProperty_GainUnitSetting,	{ -1, "GainUnitSetting", &map_CrGainUnitSetting, 0, } },
	{ PCode::CrDeviceProperty_HDMIResolutionStillPlay,	{ -1, "HDMIResolutionStillPlay", &map_CrHDMIResolution, 0, } },
	{ PCode::CrDeviceProperty_HighIsoNR,	{ -1, "HighIsoNR", &map_CrHighIsoNR, 0, } },
	{ PCode::CrDeviceProperty_HighResolutionShutterSpeed,	{ -1, "HighResolutionShutterSpeed", 0, 0, } },
	{ PCode::CrDeviceProperty_HighResolutionShutterSpeedSetting,	{ -1, "HighResolutionShutterSpeedSetting", &map_CrHighResolutionShutterSpeedSetting, 0, } },
	{ PCode::CrDeviceProperty_HLGStillImage,	{ -1, "HLGStillImage", &map_CrHLGStillImage, 0, } },
	{ PCode::CrDeviceProperty_ImageID_Num,	{ -1, "ImageID_Num", 0, 0, } },
	{ PCode::CrDeviceProperty_ImageID_Num_Setting,	{ -1, "ImageID_Num_Setting", &map_CrImageIDNumSetting, 0, } },
	{ PCode::CrDeviceProperty_ImageSize,	{ -1, "ImageSize", &map_CrImageSize, 0, } },
	{ PCode::CrDeviceProperty_ImageStabilizationSteadyShot,	{ -1, "ImageStabilizationSteadyShot", &map_CrImageStabilizationSteadyShot, 0, } },
	{ PCode::CrDeviceProperty_Interval_Rec_Mode,	{ -1, "Interval_Rec_Mode", &map_CrIntervalRecMode, 0, } },
	{ PCode::CrDeviceProperty_Interval_Rec_Status,	{ -1, "Interval_Rec_Status", &map_CrIntervalRecStatus, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_AETrackingSensitivity,	{ -1, "IntervalRec_AETrackingSensitivity", &map_CrIntervalRecAETrackingSensitivity, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_NumberOfShots,	{ -1, "IntervalRec_NumberOfShots", 0, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShootingInterval,	{ -1, "IntervalRec_ShootingInterval", 0, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShootingStartTime,	{ -1, "IntervalRec_ShootingStartTime", 0, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShootIntervalPriority,	{ -1, "IntervalRec_ShootIntervalPriority", &map_CrIntervalRecShootIntervalPriority, 0, } },
	{ PCode::CrDeviceProperty_IntervalRec_ShutterType,	{ -1, "IntervalRec_ShutterType", &map_CrIntervalRecShutterType, 0, } },
	{ PCode::CrDeviceProperty_IrisModeSetting,	{ -1, "IrisModeSetting", &map_CrIrisModeSetting, 0, } },
	{ PCode::CrDeviceProperty_IsoAutoMinShutterSpeedManual,	{ -1, "IsoAutoMinShutterSpeedManual", 0, 0, } },
	{ PCode::CrDeviceProperty_IsoAutoMinShutterSpeedMode,	{ -1, "IsoAutoMinShutterSpeedMode", &map_CrIsoAutoMinShutterSpeedMode, 0, } },
	{ PCode::CrDeviceProperty_IsoAutoMinShutterSpeedPreset,	{ -1, "IsoAutoMinShutterSpeedPreset", &map_CrIsoAutoMinShutterSpeedPreset, 0, } },
	{ PCode::CrDeviceProperty_IsoCurrentSensitivity,	{ -1, "IsoCurrentSensitivity", 0, format_iso_sensitivity, } },
	{ PCode::CrDeviceProperty_IsoSensitivity,	{ -1, "IsoSensitivity", 0, format_iso_sensitivity, } },
	{ PCode::CrDeviceProperty_LensAssignableButton1,	{ -1, "LensAssignableButton1", &map_CrAssignableButton, 0, } },
	{ PCode::CrDeviceProperty_LensAssignableButtonIndicator1,	{ -1, "LensAssignableButtonIndicator1", &map_CrAssignableButtonIndicator, 0, } },
	{ PCode::CrDeviceProperty_LensInformationEnableStatus,	{ -1, "LensInformationEnableStatus", &map_CrLensInformationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_LiveView_Image_Quality,	{ -1, "LiveView_Image_Quality", &map_CrPropertyLiveViewImageQuality, 0, } },
	{ PCode::CrDeviceProperty_LiveViewDisplayEffect,	{ -1, "LiveViewDisplayEffect", &map_CrLiveViewDisplayEffect, 0, } },
	{ PCode::CrDeviceProperty_LiveViewStatus,	{ -1, "LiveViewStatus", &map_CrLiveViewStatus, 0, } },
	{ PCode::CrDeviceProperty_LongExposureNR,	{ -1, "LongExposureNR", &map_CrLongExposureNR, 0, } },
	{ PCode::CrDeviceProperty_Media_FormatProgressRate,	{ -1, "Media_FormatProgressRate", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_FileType,	{ -1, "MediaSLOT1_FileType", &map_CrFileType, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_FormatEnableStatus,	{ -1, "MediaSLOT1_FormatEnableStatus", &map_CrMediaFormat, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_ImageQuality,	{ -1, "MediaSLOT1_ImageQuality", &map_CrImageQuality, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_ImageSize,	{ -1, "MediaSLOT1_ImageSize", &map_CrImageSize, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_QuickFormatEnableStatus,	{ -1, "MediaSLOT1_QuickFormatEnableStatus", &map_CrMediaFormat, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RAW_FileCompressionType,	{ -1, "MediaSLOT1_RAW_FileCompressionType", &map_CrRAWFileCompressionType, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RecordingAvailableType,	{ -1, "MediaSLOT1_RecordingAvailableType", &map_CrMediaSlotRecordingAvailableType, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RemainingNumber,	{ -1, "MediaSLOT1_RemainingNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_RemainingTime,	{ -1, "MediaSLOT1_RemainingTime", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_Status,	{ -1, "MediaSLOT1_Status", &map_CrSlotStatus, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1_WritingState,	{ -1, "MediaSLOT1_WritingState", &map_CrMediaSlotWritingState, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT1Player,	{ -1, "MediaSLOT1Player", &map_CrMediaPlayer, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_FileType,	{ -1, "MediaSLOT2_FileType", &map_CrFileType, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_FormatEnableStatus,	{ -1, "MediaSLOT2_FormatEnableStatus", &map_CrMediaFormat, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_ImageQuality,	{ -1, "MediaSLOT2_ImageQuality", &map_CrImageQuality, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_ImageSize,	{ -1, "MediaSLOT2_ImageSize", &map_CrImageSize, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_QuickFormatEnableStatus,	{ -1, "MediaSLOT2_QuickFormatEnableStatus", &map_CrMediaFormat, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RAW_FileCompressionType,	{ -1, "MediaSLOT2_RAW_FileCompressionType", &map_CrRAWFileCompressionType, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RecordingAvailableType,	{ -1, "MediaSLOT2_RecordingAvailableType", &map_CrMediaSlotRecordingAvailableType, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RemainingNumber,	{ -1, "MediaSLOT2_RemainingNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_RemainingTime,	{ -1, "MediaSLOT2_RemainingTime", 0, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_Status,	{ -1, "MediaSLOT2_Status", &map_CrSlotStatus, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2_WritingState,	{ -1, "MediaSLOT2_WritingState", &map_CrMediaSlotWritingState, 0, } },
	{ PCode::CrDeviceProperty_MediaSLOT2Player,	{ -1, "MediaSLOT2Player", &map_CrMediaPlayer, 0, } },
	{ PCode::CrDeviceProperty_MeteringMode,	{ -1, "MeteringMode", &map_CrMeteringMode, 0, } },
	{ PCode::CrDeviceProperty_MonitoringOutputDisplayHDMI,	{ -1, "MonitoringOutputDisplayHDMI", &map_CrMonitoringOutputDisplayHDMI, 0, } },
	{ PCode::CrDeviceProperty_MonitorLUTSetting,	{ -1, "MonitorLUTSetting", &map_CrMonitorLUTSetting, 0, } },
	{ PCode::CrDeviceProperty_Movie_File_Format,	{ -1, "Movie_File_Format", &map_CrFileFormatMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_FTP_AutoTransferTarget,	{ -1, "Movie_FTP_AutoTransferTarget", &map_CrFTPAutoTransferTargetMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_FTP_TransferTarget,	{ -1, "Movie_FTP_TransferTarget", &map_CrFTPTransferTargetMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutput4KSetting,	{ -1, "Movie_HDMIOutput4KSetting", &map_CrHDMIOutput4KSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputAudioCH,	{ -1, "Movie_HDMIOutputAudioCH", &map_CrHDMIOutputAudioCH, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRAW,	{ -1, "Movie_HDMIOutputRAW", &map_CrHDMIOutputRAWMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRawSetting,	{ -1, "Movie_HDMIOutputRawSetting", &map_CrHDMIOutputRawSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRecControl,	{ -1, "Movie_HDMIOutputRecControl", &map_CrHDMIOutputRecControlMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputRecMedia,	{ -1, "Movie_HDMIOutputRecMedia", &map_CrHDMIOutputRecMediaMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputResolution,	{ -1, "Movie_HDMIOutputResolution", &map_CrHDMIResolution, 0, } },
	{ PCode::CrDeviceProperty_Movie_HDMIOutputTimeCode,	{ -1, "Movie_HDMIOutputTimeCode", &map_CrHDMIOutputTimeCodeMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_ImageStabilizationSteadyShot,	{ -1, "Movie_ImageStabilizationSteadyShot", &map_CrImageStabilizationSteadyShotMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_CountDownIntervalTime,	{ -1, "Movie_IntervalRec_CountDownIntervalTime", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_FrameRateSetting,	{ -1, "Movie_IntervalRec_FrameRateSetting", &map_CrRecordingFrameRateSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_IntervalTime,	{ -1, "Movie_IntervalRec_IntervalTime", &map_CrIntervalRecTimeMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_RecordingDuration,	{ -1, "Movie_IntervalRec_RecordingDuration", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_IntervalRec_RecordingSetting,	{ -1, "Movie_IntervalRec_RecordingSetting", &map_CrRecordingSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_ProxyFileFormat,	{ -1, "Movie_ProxyFileFormat", &map_CrFileFormatMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_Recording_FrameRateProxySetting,	{ -1, "Movie_Recording_FrameRateProxySetting", &map_CrRecordingFrameRateSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_Recording_FrameRateSetting,	{ -1, "Movie_Recording_FrameRateSetting", &map_CrRecordingFrameRateSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Movie_Recording_ResolutionForMain,	{ -1, "Movie_Recording_ResolutionForMain", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_Recording_ResolutionForProxy,	{ -1, "Movie_Recording_ResolutionForProxy", 0, 0, } },
	{ PCode::CrDeviceProperty_Movie_Recording_Setting,	{ -1, "Movie_Recording_Setting", &map_CrRecordingSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_MovieForwardButton,	{ -1, "MovieForwardButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MovieNextButton,	{ -1, "MovieNextButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MoviePlayButton,	{ -1, "MoviePlayButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MoviePlayingSpeed,	{ -1, "MoviePlayingSpeed", 0, 0, } },
	{ PCode::CrDeviceProperty_MoviePlayingState,	{ -1, "MoviePlayingState", &map_CrMoviePlayingState, 0, } },
	{ PCode::CrDeviceProperty_MoviePlayPauseButton,	{ -1, "MoviePlayPauseButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MoviePlayStopButton,	{ -1, "MoviePlayStopButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MoviePrevButton,	{ -1, "MoviePrevButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MovieRecButtonToggleEnableStatus,	{ -1, "MovieRecButtonToggleEnableStatus", &map_CrMovieRecButtonToggleEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_MovieRecReviewButton,	{ -1, "MovieRecReviewButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MovieRewindButton,	{ -1, "MovieRewindButton", &map_CrMovieXButton, 0, } },
	{ PCode::CrDeviceProperty_MovieShootingMode,	{ -1, "MovieShootingMode", &map_CrMovieShootingMode, 0, } },
	{ PCode::CrDeviceProperty_MovieShootingModeColorGamut,	{ -1, "MovieShootingModeColorGamut", &map_CrMovieShootingModeColorGamut, 0, } },
	{ PCode::CrDeviceProperty_MovieShootingModeTargetDisplay,	{ -1, "MovieShootingModeTargetDisplay", &map_CrMovieShootingModeTargetDisplay, 0, } },
	{ PCode::CrDeviceProperty_NDFilter,	{ -1, "NDFilter", &map_CrNDFilter, 0, } },
	{ PCode::CrDeviceProperty_NDFilterMode,	{ -1, "NDFilterMode", &map_CrNDFilterMode, 0, } },
	{ PCode::CrDeviceProperty_NDFilterModeSetting,	{ -1, "NDFilterModeSetting", &map_CrNDFilterModeSetting, 0, } },
	{ PCode::CrDeviceProperty_NDFilterSwitchingSetting,	{ -1, "NDFilterSwitchingSetting", &map_CrNDFilterSwitchingSetting, 0, } },
	{ PCode::CrDeviceProperty_NDFilterValue,	{ -1, "NDFilterValue", 0, 0, } },
	{ PCode::CrDeviceProperty_NearFar,	{ -1, "NearFar", 0/*&map_CrNearFarEnableStatus*/, 0, } },
	{ PCode::CrDeviceProperty_PictureEffect,	{ -1, "PictureEffect", &map_CrPictureEffect, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile,	{ -1, "PictureProfile", &map_CrPictureProfile, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_BlackGammaLevel,	{ -1, "PictureProfile_BlackGammaLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_BlackGammaRange,	{ -1, "PictureProfile_BlackGammaRange", &map_CrPictureProfileBlackGammaRange, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_BlackLevel,	{ -1, "PictureProfile_BlackLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthBlue,	{ -1, "PictureProfile_ColorDepthBlue", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthCyan,	{ -1, "PictureProfile_ColorDepthCyan", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthGreen,	{ -1, "PictureProfile_ColorDepthGreen", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthMagenta,	{ -1, "PictureProfile_ColorDepthMagenta", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthRed,	{ -1, "PictureProfile_ColorDepthRed", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorDepthYellow,	{ -1, "PictureProfile_ColorDepthYellow", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorMode,	{ -1, "PictureProfile_ColorMode", &map_CrPictureProfileColorMode, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_ColorPhase,	{ -1, "PictureProfile_ColorPhase", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_Copy,	{ -1, "PictureProfile_Copy", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustBWBalance,	{ -1, "PictureProfile_DetailAdjustBWBalance", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustCrispening,	{ -1, "PictureProfile_DetailAdjustCrispening", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustHiLightDetail,	{ -1, "PictureProfile_DetailAdjustHiLightDetail", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustLimit,	{ -1, "PictureProfile_DetailAdjustLimit", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustMode,	{ -1, "PictureProfile_DetailAdjustMode", &map_CrPictureProfileDetailAdjustMode, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailAdjustVHBalance,	{ -1, "PictureProfile_DetailAdjustVHBalance", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_DetailLevel,	{ -1, "PictureProfile_DetailLevel", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_Gamma,	{ -1, "PictureProfile_Gamma", &map_CrPictureProfileGamma, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeAutoSet_MaxPoint,	{ -1, "PictureProfile_KneeAutoSet_MaxPoint", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeAutoSet_Sensitivity,	{ -1, "PictureProfile_KneeAutoSet_Sensitivity", &map_CrPictureProfileKneeAutoSetSensitivity, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeManualSet_Point,	{ -1, "PictureProfile_KneeManualSet_Point", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeManualSet_Slope,	{ -1, "PictureProfile_KneeManualSet_Slope", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_KneeMode,	{ -1, "PictureProfile_KneeMode", &map_CrPictureProfileKneeMode, 0, } },
	{ PCode::CrDeviceProperty_PictureProfile_Saturation,	{ -1, "PictureProfile_Saturation", 0, 0, } },
	{ PCode::CrDeviceProperty_PictureProfileResetEnableStatus,	{ -1, "PictureProfileResetEnableStatus", &map_CrPictureProfileResetEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_PixelMappingEnableStatus,	{ -1, "PixelMappingEnableStatus", &map_CrPixelMappingEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_PlaybackMedia,	{ -1, "PlaybackMedia", &map_CrPlaybackMedia, 0, } },
	{ PCode::CrDeviceProperty_PlaybackVolumeSettings,	{ -1, "PlaybackVolumeSettings", 0, 0, } },
	{ PCode::CrDeviceProperty_PowerSource,	{ -1, "PowerSource", &map_CrPowerSource, 0, } },
	{ PCode::CrDeviceProperty_PriorityKeySettings,	{ -1, "PriorityKeySettings", &map_CrPriorityKeySettings, 0, } },
	{ PCode::CrDeviceProperty_PrioritySetInAF_C,	{ -1, "PrioritySetInAF_C", &map_CrPrioritySetInAF, 0, } },
	{ PCode::CrDeviceProperty_PrioritySetInAF_S,	{ -1, "PrioritySetInAF_S", &map_CrPrioritySetInAF, 0, } },
	{ PCode::CrDeviceProperty_ProxyRecordingSetting,	{ -1, "ProxyRecordingSetting", &map_CrProxyRecordingSetting, 0, } },
	{ PCode::CrDeviceProperty_RAW_FileCompressionType,	{ -1, "RAW_FileCompressionType", &map_CrRAWFileCompressionType, 0, } },
	{ PCode::CrDeviceProperty_RAW_J_PC_Save_Image,	{ -1, "RAW_J_PC_Save_Image", &map_CrPropertyRAWJPCSaveImage, 0, } },
	{ PCode::CrDeviceProperty_RecognitionTarget,	{ -1, "RecognitionTarget", &map_CrRecognitionTarget, 0, } },
	{ PCode::CrDeviceProperty_RecorderControlMainSetting,	{ -1, "RecorderControlMainSetting", &map_CrRecorderControlSetting, 0, } },
	{ PCode::CrDeviceProperty_RecorderControlProxySetting,	{ -1, "RecorderControlProxySetting", &map_CrRecorderControlSetting, 0, } },
	{ PCode::CrDeviceProperty_RecorderExtRawStatus,	{ -1, "RecorderExtRawStatus", &map_CrRecorderStatus, 0, } },
	{ PCode::CrDeviceProperty_RecorderMainStatus,	{ -1, "RecorderMainStatus", &map_CrRecorderStatus, 0, } },
	{ PCode::CrDeviceProperty_RecorderProxyStatus,	{ -1, "RecorderProxyStatus", &map_CrRecorderStatus, 0, } },
	{ PCode::CrDeviceProperty_RecorderSaveDestination,	{ -1, "RecorderSaveDestination", &map_CrRecorderSaveDestination, 0, } },
	{ PCode::CrDeviceProperty_RecorderStartMain,	{ -1, "RecorderStartMain", &map_CrRecorderStart, 0, } },
	{ PCode::CrDeviceProperty_RecorderStartProxy,	{ -1, "RecorderStartProxy", &map_CrRecorderStart, 0, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimer,	{ -1, "RecordingSelfTimer", &map_CrMovieRecordingSelfTimer, 0, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimerContinuous,	{ -1, "RecordingSelfTimerContinuous", &map_CrMovieRecordingSelfTimerContinuous, 0, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimerCountTime,	{ -1, "RecordingSelfTimerCountTime", 0, 0, } },
	{ PCode::CrDeviceProperty_RecordingSelfTimerStatus,	{ -1, "RecordingSelfTimerStatus", &map_CrMovieRecordingSelfTimerStatus, 0, } },
	{ PCode::CrDeviceProperty_RecordingState,	{ -1, "RecordingState", &map_CrMovie_Recording_State, 0, } },
	{ PCode::CrDeviceProperty_RedEyeReduction,	{ -1, "RedEyeReduction", &map_CrRedEyeReduction, 0, } },
	{ PCode::CrDeviceProperty_Remocon_Zoom_Speed_Type,	{ -1, "Remocon_Zoom_Speed_Type", &map_CrRemoconZoomSpeedType, 0, } },
	{ PCode::CrDeviceProperty_RemoteTouchOperation,	{ -1, "RemoteTouchOperation", 0, 0, } },
	{ PCode::CrDeviceProperty_RemoteTouchOperationEnableStatus,	{ -1, "RemoteTouchOperationEnableStatus", &map_CrRemoteTouchOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_RightLeftEyeSelect,	{ -1, "RightLeftEyeSelect", &map_CrRightLeftEyeSelect, 0, } },
	{ PCode::CrDeviceProperty_S1,	{ -1, "S1", &map_CrLockIndicator, 0, } },
	{ PCode::CrDeviceProperty_SceneFileIndex,	{ -1, "SceneFileIndex", 0, 0, } },
	{ PCode::CrDeviceProperty_SdkControlMode,	{ -1, "SdkControlMode", &map_CrSdkControlMode, 0, } },
	{ PCode::CrDeviceProperty_SelectFinder,	{ -1, "SelectFinder", &map_CrSelectFinder, 0, } },
	{ PCode::CrDeviceProperty_SelectFTPServer,	{ -1, "SelectFTPServer", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectFTPServerID,	{ -1, "SelectFTPServerID", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectUserBaseLookToEdit,	{ -1, "SelectUserBaseLookToEdit", 0, 0, } },
	{ PCode::CrDeviceProperty_SelectUserBaseLookToSetInPPLUT,	{ -1, "SelectUserBaseLookToSetInPPLUT", 0, 0, } },
	{ PCode::CrDeviceProperty_SensorCleaningEnableStatus,	{ -1, "SensorCleaningEnableStatus", &map_CrSensorCleaningEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_ShutterAngle,	{ -1, "ShutterAngle", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSFrequency,	{ -1, "ShutterECSFrequency", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSNumber,	{ -1, "ShutterECSNumber", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSNumberStep,	{ -1, "ShutterECSNumberStep", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterECSSetting,	{ -1, "ShutterECSSetting", &map_CrShutterECSSetting, 0, } },
	{ PCode::CrDeviceProperty_ShutterMode,	{ -1, "ShutterMode", &map_CrShutterModeSetting, 0, } },
	{ PCode::CrDeviceProperty_ShutterModeSetting,	{ -1, "ShutterModeSetting", &map_CrShutterModeSetting, 0, } },
	{ PCode::CrDeviceProperty_ShutterModeStatus,	{ -1, "ShutterModeStatus", &map_CrShutterModeStatus, 0, } },
	{ PCode::CrDeviceProperty_ShutterSetting,	{ -1, "ShutterSetting", &map_CrShutterSetting, 0, } },
	{ PCode::CrDeviceProperty_ShutterSlow,	{ -1, "ShutterSlow", &map_CrShutterSlow, 0, } },
	{ PCode::CrDeviceProperty_ShutterSlowFrames,	{ -1, "ShutterSlowFrames", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterSpeed,	{ -1, "ShutterSpeed", 0, format_shutter_speed, } },
	{ PCode::CrDeviceProperty_ShutterSpeedCurrentValue,	{ -1, "ShutterSpeedCurrentValue", 0, 0, } },
	{ PCode::CrDeviceProperty_ShutterSpeedValue,	{ -1, "ShutterSpeedValue", 0, format_shutter_speed_value, } },
	{ PCode::CrDeviceProperty_ShutterType,	{ -1, "ShutterType", &map_CrShutterType, 0, } },
	{ PCode::CrDeviceProperty_SilentMode,	{ -1, "SilentMode", &map_CrSilentMode, 0, } },
	{ PCode::CrDeviceProperty_SilentModeApertureDriveInAF,	{ -1, "SilentModeApertureDriveInAF", &map_CrSilentModeApertureDriveInAF, 0, } },
	{ PCode::CrDeviceProperty_SilentModeAutoPixelMapping,	{ -1, "SilentModeAutoPixelMapping", &map_CrSilentModeAutoPixelMapping, 0, } },
	{ PCode::CrDeviceProperty_SilentModeShutterWhenPowerOff,	{ -1, "SilentModeShutterWhenPowerOff", &map_CrSilentModeShutterWhenPowerOff, 0, } },
	{ PCode::CrDeviceProperty_SnapshotInfo,	{ -1, "SnapshotInfo", 0, 0, } },
	{ PCode::CrDeviceProperty_SoftSkinEffect,	{ -1, "SoftSkinEffect", &map_CrSoftSkinEffect, 0, } },
	{ PCode::CrDeviceProperty_SQFrameRate,	{ -1, "SQFrameRate", 0, 0, } },
	{ PCode::CrDeviceProperty_SQRecordingFrameRateSetting,	{ -1, "SQRecordingFrameRateSetting", &map_CrRecordingFrameRateSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_SQRecordingSetting,	{ -1, "SQRecordingSetting", &map_CrRecordingSettingMovie, 0, } },
	{ PCode::CrDeviceProperty_Still_Image_Trans_Size,	{ -1, "Still_Image_Trans_Size", &map_CrPropertyStillImageTransSize, 0, } },
	{ PCode::CrDeviceProperty_StillImageQuality,	{ -1, "StillImageQuality", &map_CrImageQuality, 0, } },
	{ PCode::CrDeviceProperty_StillImageStoreDestination,	{ -1, "StillImageStoreDestination", &map_CrStillImageStoreDestination, 0, } },
	{ PCode::CrDeviceProperty_SubjectRecognitionAF,	{ -1, "SubjectRecognitionAF", &map_CrSubjectRecognitionAF, 0, } },
	{ PCode::CrDeviceProperty_SubjectRecognitionInAF,	{ -1, "SubjectRecognitionInAF", &map_CrSubjectRecognitionInAF, 0, } },
	{ PCode::CrDeviceProperty_TimeCodeFormat,	{ -1, "TimeCodeFormat", &map_CrTimeCodeFormat, 0, } },
	{ PCode::CrDeviceProperty_TimeCodeMake,	{ -1, "TimeCodeMake", &map_CrTimeCodeMake, 0, } },
	{ PCode::CrDeviceProperty_TimeCodePreset,	{ -1, "TimeCodePreset", 0, 0, } },
	{ PCode::CrDeviceProperty_TimeCodePresetResetEnableStatus,	{ -1, "TimeCodePresetResetEnableStatus", &map_CrTimeCodePresetResetEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_TimeCodeRun,	{ -1, "TimeCodeRun", &map_CrTimeCodeRun, 0, } },
	{ PCode::CrDeviceProperty_TouchOperation,	{ -1, "TouchOperation", &map_CrTouchOperation, 0, } },
	{ PCode::CrDeviceProperty_UpdateBodyStatus,	{ -1, "UpdateBodyStatus", &map_CrUpdateStatus, 0, } },
	{ PCode::CrDeviceProperty_USBPowerSupply,	{ -1, "USBPowerSupply", &map_CrUSBPowerSupply, 0, } },
	{ PCode::CrDeviceProperty_UserBaseLookAELevelOffset,	{ -1, "UserBaseLookAELevelOffset", 0, 0, } },
	{ PCode::CrDeviceProperty_UserBaseLookInput,	{ -1, "UserBaseLookInput", &map_CrUserBaseLookInput, 0, } },
	{ PCode::CrDeviceProperty_UserBitPreset,	{ -1, "UserBitPreset", 0, 0, } },
	{ PCode::CrDeviceProperty_UserBitPresetResetEnableStatus,	{ -1, "UserBitPresetResetEnableStatus", &map_CrUserBitPresetResetEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_UserBitTimeRec,	{ -1, "UserBitTimeRec", &map_CrUserBitTimeRec, 0, } },
	{ PCode::CrDeviceProperty_WhiteBalance,	{ -1, "WhiteBalance", &map_CrWhiteBalanceSetting, 0, } },
	{ PCode::CrDeviceProperty_WhiteBalanceModeSetting,	{ -1, "WhiteBalanceModeSetting", &map_CrWhiteBalanceModeSetting, 0, } },
	{ PCode::CrDeviceProperty_WhiteBalanceTint,	{ -1, "WhiteBalanceTint", 0, 0, } },
	{ PCode::CrDeviceProperty_WhiteBalanceTintStep,	{ -1, "WhiteBalanceTintStep", 0, 0, } },
	{ PCode::CrDeviceProperty_WindNoiseReduct,	{ -1, "WindNoiseReduct", &map_CrWindNoiseReduction, 0, } },
	{ PCode::CrDeviceProperty_WirelessFlash,	{ -1, "WirelessFlash", &map_CrWirelessFlash, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Bar_Information,	{ -1, "Zoom_Bar_Information", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Operation,	{ -1, "Zoom_Operation", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Operation_Status,	{ -1, "Zoom_Operation_Status", &map_CrZoomOperationEnableStatus, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Scale,	{ -1, "Zoom_Scale", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Setting,	{ -1, "Zoom_Setting", &map_CrZoomSettingType, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Speed_Range,	{ -1, "Zoom_Speed_Range", 0, 0, } },
	{ PCode::CrDeviceProperty_Zoom_Type_Status,	{ -1, "Zoom_Type_Status", &map_CrZoomTypeStatus, 0, } },
	{ PCode::CrDeviceProperty_ZoomAndFocusPosition_Load,	{ -1, "ZoomAndFocusPosition_Load", 0, 0, } },
	{ PCode::CrDeviceProperty_ZoomAndFocusPosition_Save,	{ -1, "ZoomAndFocusPosition_Save", 0, 0, } },
	{ PCode::CrDeviceProperty_ZoomDistance,	{ -1, "ZoomDistance", 0, 0, } },
	{ PCode::CrDeviceProperty_ZoomDistanceUnitSetting,	{ -1, "ZoomDistanceUnitSetting", &map_CrZoomDistanceUnitSetting, 0, } },
};

//	{ PCode::CrDeviceProperty_\1,	{ -1, "\1", } },
std::map<PCode, struct PropertyValueString> PropStr {
	{ PCode::CrDeviceProperty_ImageID_String,	{ -1, "ImageID_String", } },
	{ PCode::CrDeviceProperty_LensModelName,	{ -1, "LensModelName", } },
	{ PCode::CrDeviceProperty_LensVersionNumber,	{ -1, "LensVersionNumber", } },
	{ PCode::CrDeviceProperty_RecorderClipName,	{ -1, "RecorderClipName", } },
	{ PCode::CrDeviceProperty_SoftwareVersion,	{ -1, "SoftwareVersion", } },
};

CameraDevice::CameraDevice(std::int32_t no, SCRSDK::ICrCameraObjectInfo const* camera_info)
    : m_number(no)
    , m_device_handle(0)
    , m_connected(false)
    , m_conn_type(ConnectionType::UNKNOWN)
    , m_net_info()
    , m_usb_info()
    , m_modeSDK(SCRSDK::CrSdkControlMode_Remote)
    , m_spontaneous_disconnection(false)
    , m_fingerprint("")
    , m_userPassword("")
    , m_eventPromise(nullptr)
    , m_cb_respProp(nullptr)
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
#ifndef PORT82
    set_save_info(TEXT("DSC"));
#else
    set_save_info(TEXT("DSC2"));
#endif
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
    } else {
        tout << TEXT("Remote Control Mode\n");
    }
    return m_modeSDK;
}

////////////////

void CameraDevice::s1_shooting()
{
	std::cout << "Shutter Half Press down&up\n";
	setProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Locked);
	int ret = waitProp(PCode::CrDeviceProperty_FocusIndication, 1000);
	setProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Unlocked);
}

void CameraDevice::af_shutter(std::uint32_t delay_ms)
{
	std::cout << "S1 shooting...\n";
	if(GetProp(PCode::CrDeviceProperty_S1)->current != SDK::CrLockIndicator::CrLockIndicator_Locked) {
		setProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Locked);
		int ret = waitProp(PCode::CrDeviceProperty_FocusIndication, 1000);
		if(ret) {
			setProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Unlocked);
			return;
		}
	}

	SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam::CrCommandParam_Down);
	std::this_thread::sleep_for(35ms);
	SDK::SendCommand(m_device_handle, SDK::CrCommandId::CrCommandId_Release, SDK::CrCommandParam::CrCommandParam_Up);

	setProp(PCode::CrDeviceProperty_S1, (uint32_t)SDK::CrLockIndicator::CrLockIndicator_Unlocked);
	std::cout << "Shutter down&up\n";
}

////////////////
#if 0
void CameraDevice::set_af_area_position()
{
    std::int32_t err;
    if(1 == Prop.at(PCode::CrDeviceProperty_PriorityKeySettings).writable) {
        err = setProp(PCode::CrDeviceProperty_PriorityKeySettings, SDK::CrPriorityKeySettings::CrPriorityKey_PCRemote);
        if(CR_FAILED(err)) {
            std::cout << "FAILED(" << __LINE__ << ")\n";
            return;
        }
        std::this_thread::sleep_for(500ms);
        load_properties();
    }

    err = setProp(PCode::CrDeviceProperty_ExposureProgramMode, SDK::CrExposureProgram::CrExposure_P_Auto);
    if(CR_FAILED(err)) {
        std::cout << "FAILED(" << __LINE__ << ")\n";
        return;
    }
    std::this_thread::sleep_for(1000ms);
    load_properties();
    if(Prop.at(PCode::CrDeviceProperty_ExposureProgramMode).current == SDK::CrExposureProgram::CrExposure_P_Auto) {
        std::cout << "FAILED(" << __LINE__ << ")\n";
        return;
    }

    err = setProp(PCode::CrDeviceProperty_FocusArea, SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S);
    if(CR_FAILED(err)) {
        std::cout << "FAILED(" << __LINE__ << ")\n";
        return;
    }
    std::this_thread::sleep_for(1000ms);
    load_properties();
    if(Prop.at(PCode::CrDeviceProperty_FocusArea).current == SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S) {
        std::cout << "FAILED(" << __LINE__ << ")\n";
        return;
    }

    execute_pos_xy(PCode::CrDeviceProperty_AF_Area_Position);
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
#endif
////////////////
#if 0
bool CameraDevice::get_camera_setting_saveread_state()
{
    load_properties();
    if (Prop.at(PCode::CrDeviceProperty_CameraSetting_SaveRead_State).current == SDK::CrCameraSettingSaveReadState::CrCameraSettingSaveReadState_Reading) {
        tout << "Unable to download/upload Camera-Setting file. \n";
        return false;
    }
    std::cout << "Camera-Setting Save/Read State: " << PropCurrentText(PCode::CrDeviceProperty_CameraSetting_SaveRead_State) << '\n';
    return true;
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
#endif
////////////////

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
    if ((false == m_spontaneous_disconnection) && (SDK::CrSdkControlMode_ContentsTransfer == m_modeSDK)) {
        std::cout << "ERROR: Please reboot this command(" << __LINE__ << ").\n";
    }
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
        std::cout << "\nThe camera is in a condition where it cannot transfer content.\n\n";
        std::cout << "Please input '0' to return to the TOP-MENU and connect again.\n";
        break;
    case SDK::CrWarning_ContentsTransferMode_CanceledFromCamera:
        std::cout << "\nContent transfer mode canceled.\n";
        std::cout << "If you want to continue content transfer, input '0' to return to the TOP-MENU and connect again.\n\n";
        break;
    case SDK::CrWarning_CameraSettings_Read_Result_OK:
        std::cout << "\nConfiguration file read successfully.\n\n";
        break;
    case SDK::CrWarning_CameraSettings_Read_Result_NG:
        std::cout << "\nFailed to load configuration file\n\n";
        break;
    case SDK::CrWarning_CameraSettings_Save_Result_NG:
        std::cout << "\nConfiguration file save request failed.\n\n";
        break;
    case SDK::CrWarning_RequestDisplayStringList_Success:
        std::cout << "\nRequest for DisplayStringList  successfully\n\n";        
        break;
    case SDK::CrWarning_RequestDisplayStringList_Error: 
        std::cout << "\nFailed to Request for DisplayStringList\n\n";
        break;
    case SDK::CrWarning_CustomWBCapture_Result_OK:
        std::cout << "\nCustom WB capture successful.\n\n";
        break;
    case SDK::CrWarning_CustomWBCapture_Result_Invalid:
    case SDK::CrWarning_CustomWBCapture_Result_NG:
        std::cout << "\nCustom WB capture failure.\n\n";
        break;
    case SDK::CrWarning_FocusPosition_Result_Invalid:
        std::cout << "\nFocus Position Result Invalid.\n\n";
        break;
    case SDK::CrWarning_FocusPosition_Result_OK:
        std::cout << "\nFocus Position Result OK.\n\n";
        break;
    case SDK::CrWarning_FocusPosition_Result_NG:
        std::cout << "\nFocus Position Result NG.\n\n";
        break;
    default:
        return;
    }
}

void CameraDevice::OnWarningExt(CrInt32u warning, CrInt32 param1, CrInt32 param2, CrInt32 param3)
{
    std::cout << "<Receive>\n";
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
    std::cout << "\n<warning>\n";
    std::cout << " 0x00060001: CrWarningExt_AFStatus\n";
    std::cout << "             <param1> Focus Indication\n";
    std::cout << " 0x00060002: CrWarningExt_OperationResults\n";
    std::cout << "             <param1> enum CrSdkApi\n";
    std::cout << "                      0x00000002: CrSdkApi_SetDeviceProperty\n";
    std::cout << "                      0x00000003: CrSdkApi_SendCommand\n";
    std::cout << "             <param2> CrDevicePropertyCode or CrCommandId\n";
    std::cout << "             <param3> enum CrWarningExt_OperationResultsParam\n";
    std::cout << "                      0x00000000: CrWarningExt_OperationResultsParam_Invalid\n";
    std::cout << "                      0x00000001: CrWarningExt_OperationResultsParam_OK\n";
    std::cout << "                      0x00000002: CrWarningExt_OperationResultsParam_NG\n";
}

void CameraDevice::OnPropertyChanged()
{
    // std::cout << "Property changed.\n";
}

void CameraDevice::OnLvPropertyChanged()
{
    // std::cout << "LvProperty changed.\n";
}
/*
void CameraDevice::OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
    //std::cout << "Property changed.  num = " << std::dec << num;
    //std::cout << std::hex;
    //for (std::int32_t i = 0; i < num; ++i)
    //{
    //    std::cout << ", 0x" << codes[i];
    //}
    //std::cout << std::endl << std::dec;
}

void CameraDevice::OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
    //std::cout << "LvProperty changed.  num = " << std::dec << num;
    //std::cout << std::hex;
    //for (std::int32_t i = 0; i < num; ++i)
    //{
    //    std::cout << ", 0x" << codes[i];
    //}
    //std::cout << std::endl;
}
*/
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
            std::cout << "Please input '0' after Connect camera" << std::endl;
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
        //std::cout << "Please input '0' to return to the TOP-MENU\n";
		if(is_connected())
			disconnect();
        std::cout << "ERROR: Please reboot this command(" << __LINE__ << ").\n";
    }
}
////////////////
#if 0
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
#endif
//////////////// for Nocode SDK

void CameraDevice::OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
	for(std::uint32_t i = 0; i < num; ++i) {
		SDK::CrLiveViewPropertyCode prop = (SDK::CrLiveViewPropertyCode)codes[i];

		switch(prop) {
		case SDK::CrLiveViewPropertyCode::CrLiveViewProperty_AF_Area_Position:			// FocusFrameInfo
		case SDK::CrLiveViewPropertyCode::CrLiveViewProperty_FaceFrame:					// FaceFrameInfo
		case SDK::CrLiveViewPropertyCode::CrLiveViewProperty_Focus_Magnifier_Position:	// Magnifier_Position
		case SDK::CrLiveViewPropertyCode::CrLiveViewProperty_TrackingFrame:				// TrackingFrameInfo
			load_liveview_properties(1, &codes[i]);
			break;
		default:
			break;
		}
	}
}

void CameraDevice::OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes)
{
	for(std::uint32_t i = 0; i < num; ++i) {
		PCode id = (PCode)codes[i];

		auto iter = Prop.find(id);
		if(iter != end(Prop)) {
			iter->second.old = true;
		//	std::cout << iter->second.tag.c_str() << "\n";
		}

		if(m_respPropId == id) {
			m_respPropId = (PCode)0;
			if(m_cb_respProp) {
				(*m_cb_respProp)(id);
				m_cb_respProp = NULL;
			}
			if(m_eventPromise) {
				m_eventPromise->set_value();
				m_eventPromise = NULL;
			}
		} else {
			switch(id) {
			case PCode::CrDeviceProperty_FocusIndication:
			case PCode::CrDeviceProperty_FNumber:
			case PCode::CrDeviceProperty_ShutterSpeed:
			case PCode::CrDeviceProperty_IsoSensitivity:
			case PCode::CrDeviceProperty_FollowFocusPositionCurrentValue:	// FX3
				SendProp(id);
				break;
			default:
				break;
			}
		}
	}
}

bool CameraDevice::set_save_info(text prefix) const
{
#if defined(__APPLE__)
	text_char path[MAC_MAX_PATH]; /*MAX_PATH*/
	memset(path, 0, sizeof(path));
	if(NULL == getcwd(path, sizeof(path) - 1)){
		std::cout << "Folder path is too long.\n";
		return false;
	}
	auto save_status = SDK::SetSaveInfo(m_device_handle, path, 
							const_cast<text_char*>(prefix.data()), -1/*startNo*/);
#else
	text path = fs::current_path().native();
	tout << path.data() << '\n';

	auto save_status = SDK::SetSaveInfo(m_device_handle, const_cast<text_char*>(path.data()), 
							const_cast<text_char*>(prefix.data()), -1/*startNo*/);
#endif
	if (CR_FAILED(save_status)) {
		std::cout << "Failed to set save path.\n";
		return false;
	}
	return true;
}

std::int32_t CameraDevice::get_live_view(std::uint8_t* buf[])
{
	// ?
	CrInt32 num = 0;
	SDK::CrLiveViewProperty* property = nullptr;
	auto err = SDK::GetLiveViewProperties(m_device_handle, &property, &num);
	if(CR_FAILED(err)) {
		std::cout << "GetLiveView FAILED\n";
		return -1;
	}
	SDK::ReleaseLiveViewProperties(m_device_handle, property);

	SDK::CrImageInfo inf;
	err = SDK::GetLiveViewImageInfo(m_device_handle, &inf);
	if(CR_FAILED(err)) {
		std::cout << "GetLiveView FAILED\n";
		return -1;
	}

	CrInt32u bufSize = inf.GetBufferSize();
	if(bufSize < 1) {
		std::cout << "GetLiveView FAILED\n";
		return -1;
	}

	auto* image_data = new SDK::CrImageDataBlock();  // 
	if(!image_data) {
		std::cout << "GetLiveView FAILED\n";
		return -1;
	}

	CrInt8u* image_buff = new CrInt8u[bufSize];  //
	if(!image_buff) {
		delete image_data;
		std::cout << "GetLiveView FAILED\n";
		return -1;
	}

	std::int32_t imageSize = -1;
	image_data->SetSize(bufSize);
	image_data->SetData(image_buff);
	err = SDK::GetLiveViewImage(m_device_handle, image_data);
	if(CR_FAILED(err)) {
		std::cout << "GetLiveView FAILED\n";
	} else if(0 < image_data->GetSize()) {
		imageSize = image_data->GetImageSize();
		*buf = new std::uint8_t[imageSize];  //
		if(!*buf) {
			imageSize = -1;
		} else {
			MemCpyEx(*buf, image_data->GetImageData(), imageSize);
		}
	}
	delete[] image_buff;
	delete image_data;
	return imageSize;
}

void CameraDevice::parse_prop(SDK::CrDeviceProperty& devProp, PCode id)
{
	PropertyValue& prop = Prop.at(id);

	prop.writable = devProp.IsSetEnableCurrentValue();
	prop.readable = devProp.IsGetEnableCurrentValue();
	prop.dataType = devProp.GetValueType();
	prop.current = devProp.GetCurrentValue();

	int dataLen = 1;
	switch(prop.dataType & 0x100F) {
	case SDK::CrDataType::CrDataType_UInt8:		dataLen = sizeof(std::uint8_t); break;
	case SDK::CrDataType::CrDataType_Int8:		dataLen = sizeof(std::int8_t); break;
	case SDK::CrDataType::CrDataType_UInt16:	dataLen = sizeof(std::uint16_t); break;
	case SDK::CrDataType::CrDataType_Int16:		dataLen = sizeof(std::int16_t); break;
	case SDK::CrDataType::CrDataType_UInt32:	dataLen = sizeof(std::uint32_t); break;
	case SDK::CrDataType::CrDataType_UInt64:	dataLen = sizeof(std::uint64_t); break;
	default: return;
	}

	unsigned char const* buf = devProp.GetValues();
	std::uint32_t nval = devProp.GetValueSize() / dataLen;
	prop.possible.resize(nval);
	for (std::uint32_t i = 0; i < nval; ++i) {
		int64_t data = 0;
		switch(prop.dataType & 0x100F) {
		case SDK::CrDataType::CrDataType_UInt8:		data = (reinterpret_cast<std::uint8_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_Int8:		data = (reinterpret_cast<std::int8_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt16:	data = (reinterpret_cast<std::uint16_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_Int16:		data = (reinterpret_cast<std::int16_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt32:	data = (reinterpret_cast<std::uint32_t const*>(buf))[i]; break;
		case SDK::CrDataType::CrDataType_UInt64:	data = (reinterpret_cast<std::uint64_t const*>(buf))[i]; break;
		default: break;
		}
		prop.possible.at(i) = data;
	}
}

void CameraDevice::parse_propStr(SDK::CrDeviceProperty& devProp, PCode id)
{
	PropertyValueString& prop = PropStr.at(id);

	prop.writable = devProp.IsSetEnableCurrentValue();
	prop.readable = devProp.IsGetEnableCurrentValue();
	prop.dataType = devProp.GetValueType();

	uint16_t* dp = devProp.GetCurrentStr();
	if(!dp) return;

//	int length = dp[0];
	prop.current = wstring_convert.to_bytes((wchar_t*)(dp+1));
}

void CameraDevice::load_liveview_properties(std::uint32_t num, std::uint32_t* codes)
{
	std::int32_t nprop = 0;
	SDK::CrLiveViewProperty* prop_list = nullptr;
	SDK::CrError status = SDK::CrError_Generic;
	if (0 == num) {
		status = SDK::GetLiveViewProperties(m_device_handle, &prop_list, &nprop);
	} else {
		status = SDK::GetSelectLiveViewProperties(m_device_handle, num, codes, &prop_list, &nprop);
	}
	if (CR_FAILED(status)) {
		std::cout << "Failed to get liveview properties.\n";
		return;
	}
	if(!prop_list || nprop <= 0) {
		std::cout << "Failed to get liveview properties.\n";
		return;
	}

	for (std::int32_t i = 0; i < nprop; ++i) {
		SDK::CrLiveViewProperty prop = prop_list[i];
		SDK::CrFrameInfoType type = prop.GetFrameInfoType();

		switch(type) {
		case SDK::CrFrameInfoType::CrFrameInfoType_FocusFrameInfo: {
			int count = prop.GetValueSize() / sizeof(SDK::CrFocusFrameInfo);
			SDK::CrFocusFrameInfo* pFrameInfo = (SDK::CrFocusFrameInfo*)prop.GetValue();
			if(0 == count || nullptr == pFrameInfo) {
			#ifdef _DEBUG
				std::cout << "  FocusFrameInfo nothing\n";
			#endif
			} else {
				std::vector<std::vector<uint32_t>> info(count);
				for(int i = 0; i < count; i++) {
					auto& lvprop = pFrameInfo[i];
					info.at(i) = { lvprop.type, lvprop.state,
									lvprop.width, lvprop.height,
									lvprop.xDenominator, lvprop.yDenominator,
									lvprop.xNumerator, lvprop.yNumerator};
				}
				Send2DArray("FocusFrameInfo", info);
			}
			break;
		  }
		case SDK::CrFrameInfoType::CrFrameInfoType_Magnifier_Position: {
			int count = prop.GetValueSize() / sizeof(SDK::CrMagPosInfo);
			SDK::CrMagPosInfo* pFrameInfo = (SDK::CrMagPosInfo*)prop.GetValue();
			if(0 == count || nullptr == pFrameInfo) {
			#ifdef _DEBUG
				std::cout << "  MagPosInfo nothing\n";
			#endif
			} else {
				std::vector<std::vector<uint32_t>> info(count);
				for(int i = 0; i < count; i++) {
					auto& lvprop = pFrameInfo[i];
					info.at(i) = { 0,0,//lvprop.type, lvprop.state,
									lvprop.width, lvprop.height,
									lvprop.xDenominator, lvprop.yDenominator,
									lvprop.xNumerator, lvprop.yNumerator};
				}
				Send2DArray("Magnifier_Position", info);
			}
			break;
		  }
		case SDK::CrFrameInfoType::CrFrameInfoType_FaceFrameInfo: {
			int count = prop.GetValueSize() / sizeof(SDK::CrFaceFrameInfo);
			SDK::CrFaceFrameInfo* pFrameInfo = (SDK::CrFaceFrameInfo*)prop.GetValue();
			if(0 == count || nullptr == pFrameInfo) {
			#ifdef _DEBUG
				std::cout << "  FaceFrameInfo nothing\n";
			#endif
			} else {
				std::vector<std::vector<uint32_t>> info(count);
				for(int i = 0; i < count; i++) {
					auto& lvprop = pFrameInfo[i];
					info.at(i) = { lvprop.type, lvprop.state,
									lvprop.width, lvprop.height,
									lvprop.xDenominator, lvprop.yDenominator,
									lvprop.xNumerator, lvprop.yNumerator};
				}
				Send2DArray("FaceFrameInfo", info);
			}
			break;
		  }
		case SDK::CrFrameInfoType::CrFrameInfoType_TrackingFrameInfo: {
			int count = prop.GetValueSize() / sizeof(SDK::CrTrackingFrameInfo);
			SDK::CrTrackingFrameInfo* pFrameInfo = (SDK::CrTrackingFrameInfo*)prop.GetValue();
			if(0 == count || nullptr == pFrameInfo) {
			#ifdef _DEBUG
				std::cout << "  TrackingFrameInfo nothing\n";
			#endif
			} else {
				std::vector<std::vector<uint32_t>> info(count);
				for(int i = 0; i < count; i++) {
					auto& lvprop = pFrameInfo[i];
					info.at(i) = { lvprop.type, lvprop.state,
									lvprop.width, lvprop.height,
									lvprop.xDenominator, lvprop.yDenominator,
									lvprop.xNumerator, lvprop.yNumerator};
				}
				Send2DArray("TrackingFrameInfo", info);
			}
			break;
		  }
		} //switch(type)
	}
	SDK::ReleaseLiveViewProperties(m_device_handle, prop_list);
}

void CameraDevice::load_properties(std::uint32_t num, std::uint32_t* codes)
{
	std::int32_t nprop = 0;
	SDK::CrDeviceProperty* prop_list = nullptr;
	SDK::CrError status = SDK::CrError_Generic;
	if (0 == num){
		status = SDK::GetDeviceProperties(m_device_handle, &prop_list, &nprop);
	} else {
		status = SDK::GetSelectDeviceProperties(m_device_handle, num, codes, &prop_list, &nprop);
	}
	if(CR_FAILED(status)) {
		std::cout << "Failed to get device properties.\n";
		return;
	}
	if(!prop_list || nprop <= 0) {
		std::cout << "Failed to get device properties.\n";
		return;
	}

	for (std::int32_t i = 0; i < nprop; ++i) {
		auto devProp = prop_list[i];
	//	std::cout << "," << devProp.IsGetEnableCurrentValue() << "," << devProp.IsSetEnableCurrentValue() << "," << (int)devProp.GetPropertyEnableFlag() << "=" << PropCurrentText(id) << "\n";

		PCode id = (PCode)devProp.GetCode();
		SDK::CrDataType type = devProp.GetValueType();
		if(type == SDK::CrDataType::CrDataType_STR) {
			auto iter = PropStr.find(id);
			if(iter != end(PropStr)) {
				parse_propStr(devProp, id);
				iter->second.old = false;
			#ifdef _DEBUG
				std::cout << iter->second.tag.c_str() << "=" << iter->second.current << "\n";	// debug
			#endif
			} else {
			#ifdef _DEBUG
				std::cout << "unknown(" << std::hex << id << ")\n";
			#endif
			}
		} else {
			auto iter = Prop.find(id);
			if(iter != end(Prop)) {
				parse_prop(devProp, id);
				iter->second.old = false;
			#ifdef _DEBUG
				std::cout << iter->second.tag.c_str() << "=" << PropCurrentText(id) << "\n";	// debug
			#endif
				if(id == PCode::CrDeviceProperty_SdkControlMode)
					m_modeSDK = (SDK::CrSdkControlMode)Prop.at(id).current;
			} else {
			#ifdef _DEBUG
				std::cout << "unknown(" << std::hex << id << ")\n";
			#endif
			}
		}
    }
    SDK::ReleaseDeviceProperties(m_device_handle, prop_list);
}

PCode CameraDevice::Prop_tag2id(std::string tag) const
{
	for(const auto& iter : Prop) {
		if(iter.second.tag == tag) {
			return iter.first;
		}
	}
	return (PCode)0;
}

struct PropertyValue* CameraDevice::GetProp(PCode id)
{
	auto prop = &Prop.at(id);
	if(prop->old) {
		CrInt32u code = static_cast<CrInt32u>(id);
		load_properties(1, &code);
	}
	return prop;
}

std::int32_t CameraDevice::setProp(PCode id, std::uint64_t value)
{
	if(1 != Prop.at(id).writable) {
		std::cout << "not writable\n";
		return -1;
	}

	SDK::CrDataType type = Prop.at(id).dataType;
	if(type == SDK::CrDataType::CrDataType_STR) {
		std::cout << "not supported - string\n";
		return -1;
	} else if(type == SDK::CrDataType::CrDataType_Undefined) {
		type = SDK::CrDataType::CrDataType_UInt32;
	}

	type = (SDK::CrDataType)(type & 0x100F);

	SDK::CrDeviceProperty devProp;
	devProp.SetCode(id);
	devProp.SetCurrentValue(value);
	devProp.SetValueType(type);
	return SDK::SetDeviceProperty(m_device_handle, &devProp);
}

std::int32_t CameraDevice::waitProp(PCode id, std::int32_t timeoutMs)
{
	int result = 0;

	std::promise<void> eventPromise;
	m_eventPromise = &eventPromise;
	m_cb_respProp = NULL;
	m_respPropId = id;
	std::future<void> eventFuture = eventPromise.get_future();
	if(eventFuture.wait_for(std::chrono::milliseconds(timeoutMs)) == std::future_status::timeout) {
		std::cout << "Timeout occurred." << std::endl;
		result = -1;
	}
	m_eventPromise = NULL;
	m_respPropId = (PCode)0;
	return result;
}

std::int32_t CameraDevice::SetProp(PCode id, std::uint64_t value)
{
	auto prop = GetProp(id);
	if(prop->readable && id != PCode::CrDeviceProperty_NearFar && prop->current == value) {
		std::cout << "skipped\n";
		SendProp(id);
		return 0;
	}

	int ret = setProp(id, value);
	if(ret) {
		ErrorProp(id, "write error");
		return -1;
	}
	if(!(prop->readable && id != PCode::CrDeviceProperty_NearFar)) {
		ErrorProp(id, "not readable");
		return -1;
	}

	m_cb_respProp = SendProp;
	m_eventPromise = NULL;
	m_respPropId = id;
	return id;
};

std::int32_t CameraDevice::SetProp(PCode id, std::string _text)
{
	PropertyValue& prop = Prop.at(id);
	if(prop.mapEnum) {
		for(const auto& iter : *prop.mapEnum) {
			if(iter.second == _text) {
				return SetProp(id, iter.first);
			}
		}
	} else if(prop.formatFunc && (prop.dataType & SDK::CrDataType::CrDataType_ArrayBit)) {
		for(int i = 0; i < prop.possible.size(); i++) {
			if(_text == prop.formatFunc(prop.possible[i])) {
				return SetProp(id, prop.possible[i]);
			}
		}
	}
	std::cout << "unkknown text-" << _text << "\n";
	return -1;
}

std::string CameraDevice::PropCurrentText(PCode id) const	// convVal2Text
{
	auto iter1 = Prop.find(id);
	if(iter1 == end(Prop))
		return "";

	uint64_t current = iter1->second.current;
	if(iter1->second.mapEnum) {
		auto iter2 = iter1->second.mapEnum->find(current);
		if(iter2 != end(*iter1->second.mapEnum))
			return iter2->second;
	} else if(iter1->second.formatFunc) {
		return iter1->second.formatFunc(current);
	}
	return std::to_string(current);
}

void CameraDevice::GetAvailablePropList(std::vector<std::string>& propList)
{
	propList.resize(0);
	for(const auto& iter : Prop) {
		if(iter.second.writable != -1) {
			propList.push_back(iter.second.tag);
		}
	}
}

std::int32_t CameraDevice::SendCommand(SDK::CrCommandId cmd) const
{
	SDK::SendCommand(m_device_handle, cmd, SDK::CrCommandParam::CrCommandParam_Down);
	std::this_thread::sleep_for(35ms);
	SDK::SendCommand(m_device_handle, cmd, SDK::CrCommandParam::CrCommandParam_Up);
	return 0;
}

std::int32_t CameraDevice::SendCommand(std::string _text) const
{
	for(const auto& iter : Cmds) {
		if(iter.second == _text) {
			return SendCommand(iter.first);
		}
	}
	return -1;
}

}
// namespace cli
