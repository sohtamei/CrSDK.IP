#include "PropertyValueTable.h"
#include <map>
#include <cmath>
#include <cstring>
#include "CameraDevice.h"


namespace SDK = SCRSDK;

namespace impl
{
inline double Round(double value, int figure)
{
    bool isNegative = ( value < 0 );
    if (isNegative == true) value = -value;

    double rate = std::pow(10.0, figure);
    //double value1 = value * rate + 0.5;

    long tmp = (long)(value * rate + 0.5);
    value = tmp/rate;

    if (isNegative == true) value = -value;

    return value;
}

// Retrieve the hi-order word (16-bit value) of a dword (32-bit value)
constexpr std::uint16_t HIWORD(std::uint32_t dword)
{
    return static_cast<std::uint16_t>((dword >> 16) & 0xFFFF);
}

// Retrieve the low-order word (16-bit value) of a dword (32-bit value)
constexpr std::uint16_t LOWORD(std::uint32_t dword)
{
    return static_cast<std::uint16_t>(dword & 0xFFFF);
}
} // namespace impl

namespace cli
{
text format_number(std::uint32_t value)
{
	text_stringstream ts;
	ts << value;
	return ts.str();
}

text format_f_number(std::uint32_t f_number)
{
    text_stringstream ts;
    if ((0x0000 == f_number) || (SDK::CrFnumber_Unknown == f_number)) {
        ts << TEXT("--");
    }
    else if(SDK::CrFnumber_Nothing == f_number) {
        // Do nothing
    }
    else {
        auto modValue = static_cast<std::uint16_t>(f_number % 100);
        if (modValue > 0) {
            ts << TEXT('F') << impl::Round((f_number / 100.0), 1);
        }
        else {
            ts << TEXT('F') << f_number / 100;
        }
    }

    return ts.str();
}

text format_iso_sensitivity(std::uint32_t iso)
{
    text_stringstream ts;

    std::uint32_t iso_ext = (iso >> 24) & 0x000000F0;  // bit 28-31
    std::uint32_t iso_mode = (iso >> 24) & 0x0000000F; // bit 24-27
    std::uint32_t iso_value = (iso & 0x00FFFFFF);      // bit  0-23

    if (iso_mode == SDK::CrISO_MultiFrameNR) {
        ts << TEXT("Multi Frame NR ");
    }
    else if (iso_mode == SDK::CrISO_MultiFrameNR_High) {
        ts << TEXT("Multi Frame NR High ");
    }

    if (iso_value == SDK::CrISO_AUTO) {
        ts << TEXT("ISO AUTO");
    }
    else {
        ts << TEXT("ISO ") << iso_value;
    }

    //if (iso_ext == SDK::CrISO_Ext) {
    //    ts << TEXT(" (EXT)");
    //}

    return ts.str();
}

text format_shutter_speed(std::uint32_t shutter_speed)
{
    text_stringstream ts;

    CrInt16u numerator = impl::HIWORD(shutter_speed);
    CrInt16u denominator = impl::LOWORD(shutter_speed);

    if (0 == shutter_speed) {
        ts << TEXT("Bulb");
    }
    else if (1 == numerator) {
        ts << numerator << '/' << denominator;
    }
    else if (0 == numerator % denominator) {
        ts << (numerator / denominator) << '"';
    }
    else {
        CrInt32 numdivision = numerator / denominator;
        CrInt32 numremainder = numerator % denominator;
        ts << numdivision << '.' << numremainder << '"';
    }
    return ts.str();
}

text format_position_key_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPriorityKeySettings::CrPriorityKey_CameraPosition,				TEXT("Camera Position") },
		{ SDK::CrPriorityKeySettings::CrPriorityKey_PCRemote,				TEXT("PC Remote Setting") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_exposure_program_mode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrExposureProgram::CrExposure_M_Manual,				TEXT("M_Manual") },
		{ SDK::CrExposureProgram::CrExposure_P_Auto,				TEXT("P_Auto") },
		{ SDK::CrExposureProgram::CrExposure_A_AperturePriority,				TEXT("A_AperturePriority") },
		{ SDK::CrExposureProgram::CrExposure_S_ShutterSpeedPriority,				TEXT("S_ShutterSpeedPriority") },
 		{ SDK::CrExposureProgram::CrExposure_Program_Creative,				TEXT("ProgramCreative") },
  		{ SDK::CrExposureProgram::CrExposure_Program_Action,				TEXT("ProgramAction") },
 		{ SDK::CrExposureProgram::CrExposure_Portrait,				TEXT("Portrait") },
		{ SDK::CrExposureProgram::CrExposure_Auto,				TEXT("Auto") },
		{ SDK::CrExposureProgram::CrExposure_Auto_Plus,				TEXT("Auto_Plus") },
		{ SDK::CrExposureProgram::CrExposure_P_A,				TEXT("P_A") },
		{ SDK::CrExposureProgram::CrExposure_P_S,				TEXT("P_S") },
		{ SDK::CrExposureProgram::CrExposure_Sports_Action,				TEXT("Sports_Action") },
		{ SDK::CrExposureProgram::CrExposure_Sunset,				TEXT("Sunset") },
		{ SDK::CrExposureProgram::CrExposure_Night,				TEXT("Night") },
		{ SDK::CrExposureProgram::CrExposure_Landscape,				TEXT("Landscape") },
		{ SDK::CrExposureProgram::CrExposure_Macro,				TEXT("Macro") },
		{ SDK::CrExposureProgram::CrExposure_HandheldTwilight,				TEXT("HandheldTwilight") },
		{ SDK::CrExposureProgram::CrExposure_NightPortrait,				TEXT("NightPortrait") },
		{ SDK::CrExposureProgram::CrExposure_AntiMotionBlur,				TEXT("AntiMotionBlur") },
		{ SDK::CrExposureProgram::CrExposure_Pet,				TEXT("Pet") },
		{ SDK::CrExposureProgram::CrExposure_Gourmet,				TEXT("Gourmet") },
		{ SDK::CrExposureProgram::CrExposure_Fireworks,				TEXT("Fireworks") },
		{ SDK::CrExposureProgram::CrExposure_HighSensitivity,				TEXT("HighSensitivity") },
		{ SDK::CrExposureProgram::CrExposure_MemoryRecall,				TEXT("MemoryRecall") },
		{ SDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_8pics,				TEXT("ContinuousPriority_AE_8pics") },
		{ SDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_10pics,				TEXT("ContinuousPriority_AE_10pics") },
		{ SDK::CrExposureProgram::CrExposure_ContinuousPriority_AE_12pics,				TEXT("ContinuousPriority_AE_12pics") },
		{ SDK::CrExposureProgram::CrExposure_3D_SweepPanorama,				TEXT("3D_SweepPanorama") },
		{ SDK::CrExposureProgram::CrExposure_SweepPanorama,				TEXT("SweepPanorama") },
		{ SDK::CrExposureProgram::CrExposure_Movie_P,				TEXT("Movie_P") },
		{ SDK::CrExposureProgram::CrExposure_Movie_A,				TEXT("Movie_A") },
		{ SDK::CrExposureProgram::CrExposure_Movie_S,				TEXT("Movie_S") },
		{ SDK::CrExposureProgram::CrExposure_Movie_M,				TEXT("Movie_M") },
		{ SDK::CrExposureProgram::CrExposure_Movie_Auto,				TEXT("Movie_Auto") },
		{ SDK::CrExposureProgram::CrExposure_Movie_F,				TEXT("Movie_F") },
		{ SDK::CrExposureProgram::CrExposure_Movie_SQMotion_P,				TEXT("Movie_SQMotion_P") },
		{ SDK::CrExposureProgram::CrExposure_Movie_SQMotion_A,				TEXT("Movie_SQMotion_A") },
		{ SDK::CrExposureProgram::CrExposure_Movie_SQMotion_S,				TEXT("Movie_SQMotion_S") },
		{ SDK::CrExposureProgram::CrExposure_Movie_SQMotion_M,				TEXT("Movie_SQMotion_M") },
		{ SDK::CrExposureProgram::CrExposure_Movie_SQMotion_AUTO,				TEXT("Movie_SQMotion_AUTO") },
		{ SDK::CrExposureProgram::CrExposure_Movie_SQMotion_F,				TEXT("Movie_SQMotion_F") },
		{ SDK::CrExposureProgram::CrExposure_Flash_Off,				TEXT("FlashOff") },
  		{ SDK::CrExposureProgram::CrExposure_PictureEffect,				TEXT("PictureEffect") },
 		{ SDK::CrExposureProgram::CrExposure_HiFrameRate_P,				TEXT("HiFrameRate_P") },
		{ SDK::CrExposureProgram::CrExposure_HiFrameRate_A,				TEXT("HiFrameRate_A") },
		{ SDK::CrExposureProgram::CrExposure_HiFrameRate_S,				TEXT("HiFrameRate_S") },
		{ SDK::CrExposureProgram::CrExposure_HiFrameRate_M,				TEXT("HiFrameRate_M") },
		{ SDK::CrExposureProgram::CrExposure_SQMotion_P,				TEXT("SQMotion_P") },
		{ SDK::CrExposureProgram::CrExposure_SQMotion_A,				TEXT("SQMotion_A") },
		{ SDK::CrExposureProgram::CrExposure_SQMotion_S,				TEXT("SQMotion_S") },
		{ SDK::CrExposureProgram::CrExposure_SQMotion_M,				TEXT("SQMotion_M") },
		{ SDK::CrExposureProgram::CrExposure_MOVIE,				TEXT("MOVIE") },
		{ SDK::CrExposureProgram::CrExposure_STILL,				TEXT("STILL") },
		{ SDK::CrExposureProgram::CrExposure_F_MovieOrSQMotion,				TEXT("F(Movie or S&Q)") },
		{ SDK::CrExposureProgram::CrExposure_Movie_IntervalRec_F,				TEXT("Movie_IntervalRec_F") },
		{ SDK::CrExposureProgram::CrExposure_Movie_IntervalRec_P,				TEXT("Movie_IntervalRec_P") },
		{ SDK::CrExposureProgram::CrExposure_Movie_IntervalRec_A,				TEXT("Movie_IntervalRec_A") },
		{ SDK::CrExposureProgram::CrExposure_Movie_IntervalRec_S,				TEXT("Movie_IntervalRec_S") },
		{ SDK::CrExposureProgram::CrExposure_Movie_IntervalRec_M,				TEXT("Movie_IntervalRec_M") },
		{ SDK::CrExposureProgram::CrExposure_Movie_IntervalRec_AUTO,				TEXT("Movie_IntervalRec_AUTO") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_still_capture_mode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrDriveMode::CrDrive_Single,				TEXT("CrDrive_Single") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Hi,				TEXT("CrDrive_Continuous_Hi") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Hi_Plus,				TEXT("CrDrive_Continuous_Hi_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Hi_Live,				TEXT("CrDrive_Continuous_Hi_Live") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Lo,				TEXT("CrDrive_Continuous_Lo") },
		{ SDK::CrDriveMode::CrDrive_Continuous,				TEXT("CrDrive_Continuous") },
		{ SDK::CrDriveMode::CrDrive_Continuous_SpeedPriority,				TEXT("CrDrive_Continuous_SpeedPriority") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Mid,				TEXT("CrDrive_Continuous_Mid") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Mid_Live,				TEXT("CrDrive_Continuous_Mid_Live") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Lo_Live,				TEXT("CrDrive_Continuous_Lo_Live") },
		{ SDK::CrDriveMode::CrDrive_SingleBurstShooting_lo,				TEXT("CrDrive_SingleBurstShooting_lo") },
		{ SDK::CrDriveMode::CrDrive_SingleBurstShooting_mid,				TEXT("CrDrive_SingleBurstShooting_mid") },
		{ SDK::CrDriveMode::CrDrive_SingleBurstShooting_hi,				TEXT("CrDrive_SingleBurstShooting_hi") },
		{ SDK::CrDriveMode::CrDrive_Timelapse,				TEXT("CrDrive_Timelapse") },
		{ SDK::CrDriveMode::CrDrive_Timer_2s,				TEXT("CrDrive_Timer_2s") },
		{ SDK::CrDriveMode::CrDrive_Timer_5s,				TEXT("CrDrive_Timer_5s") },
		{ SDK::CrDriveMode::CrDrive_Timer_10s,				TEXT("CrDrive_Timer_10s") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_03Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_03Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_03Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_03Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_03Ev_9pics,				TEXT("CrDrive_Continuous_Bracket_03Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_05Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_05Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_05Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_05Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_05Ev_9pics,				TEXT("CrDrive_Continuous_Bracket_05Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_07Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_07Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_07Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_07Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_07Ev_9pics,				TEXT("CrDrive_Continuous_Bracket_07Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_10Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_10Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_10Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_10Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_10Ev_9pics,				TEXT("CrDrive_Continuous_Bracket_10Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_20Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_20Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_20Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_20Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_30Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_30Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_30Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_30Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_03Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_03Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_03Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_03Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_03Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_03Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_05Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_05Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_05Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_05Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_05Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_05Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_07Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_07Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_07Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_07Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_07Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_07Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_10Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_10Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_10Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_10Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_10Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_10Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_13Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_13Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_13Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_13Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_13Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_13Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_13Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_13Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_13Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_13Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_15Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_15Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_15Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_15Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_15Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_15Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_15Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_15Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_15Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_15Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_17Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_17Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_17Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_17Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_17Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_17Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_17Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_17Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_17Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_17Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_20Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_20Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_20Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_20Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_20Ev_7pics,				TEXT("CrDrive_Continuous_Bracket_20Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_23Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_23Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_23Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_23Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_23Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_23Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_23Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_23Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_25Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_25Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_25Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_25Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_25Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_25Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_25Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_25Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_27Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_27Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_27Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_27Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_27Ev_3pics,				TEXT("CrDrive_Continuous_Bracket_27Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_27Ev_5pics,				TEXT("CrDrive_Continuous_Bracket_27Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_30Ev_2pics_Plus,				TEXT("CrDrive_Continuous_Bracket_30Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Bracket_30Ev_2pics_Minus,				TEXT("CrDrive_Continuous_Bracket_30Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_03Ev_3pics,				TEXT("CrDrive_Single_Bracket_03Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_03Ev_5pics,				TEXT("CrDrive_Single_Bracket_03Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_03Ev_9pics,				TEXT("CrDrive_Single_Bracket_03Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_05Ev_3pics,				TEXT("CrDrive_Single_Bracket_05Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_05Ev_5pics,				TEXT("CrDrive_Single_Bracket_05Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_05Ev_9pics,				TEXT("CrDrive_Single_Bracket_05Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_07Ev_3pics,				TEXT("CrDrive_Single_Bracket_07Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_07Ev_5pics,				TEXT("CrDrive_Single_Bracket_07Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_07Ev_9pics,				TEXT("CrDrive_Single_Bracket_07Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_10Ev_3pics,				TEXT("CrDrive_Single_Bracket_10Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_10Ev_5pics,				TEXT("CrDrive_Single_Bracket_10Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_10Ev_9pics,				TEXT("CrDrive_Single_Bracket_10Ev_9pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_20Ev_3pics,				TEXT("CrDrive_Single_Bracket_20Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_20Ev_5pics,				TEXT("CrDrive_Single_Bracket_20Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_30Ev_3pics,				TEXT("CrDrive_Single_Bracket_30Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_30Ev_5pics,				TEXT("CrDrive_Single_Bracket_30Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_03Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_03Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_03Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_03Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_03Ev_7pics,				TEXT("CrDrive_Single_Bracket_03Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_05Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_05Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_05Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_05Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_05Ev_7pics,				TEXT("CrDrive_Single_Bracket_05Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_07Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_07Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_07Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_07Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_07Ev_7pics,				TEXT("CrDrive_Single_Bracket_07Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_10Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_10Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_10Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_10Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_10Ev_7pics,				TEXT("CrDrive_Single_Bracket_10Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_13Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_13Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_13Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_13Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_13Ev_3pics,				TEXT("CrDrive_Single_Bracket_13Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_13Ev_5pics,				TEXT("CrDrive_Single_Bracket_13Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_13Ev_7pics,				TEXT("CrDrive_Single_Bracket_13Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_15Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_15Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_15Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_15Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_15Ev_3pics,				TEXT("CrDrive_Single_Bracket_15Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_15Ev_5pics,				TEXT("CrDrive_Single_Bracket_15Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_15Ev_7pics,				TEXT("CrDrive_Single_Bracket_15Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_17Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_17Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_17Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_17Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_17Ev_3pics,				TEXT("CrDrive_Single_Bracket_17Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_17Ev_5pics,				TEXT("CrDrive_Single_Bracket_17Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_17Ev_7pics,				TEXT("CrDrive_Single_Bracket_17Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_20Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_20Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_20Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_20Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_20Ev_7pics,				TEXT("CrDrive_Single_Bracket_20Ev_7pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_23Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_23Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_23Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_23Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_23Ev_3pics,				TEXT("CrDrive_Single_Bracket_23Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_23Ev_5pics,				TEXT("CrDrive_Single_Bracket_23Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_25Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_25Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_25Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_25Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_25Ev_3pics,				TEXT("CrDrive_Single_Bracket_25Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_25Ev_5pics,				TEXT("CrDrive_Single_Bracket_25Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_27Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_27Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_27Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_27Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_27Ev_3pics,				TEXT("CrDrive_Single_Bracket_27Ev_3pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_27Ev_5pics,				TEXT("CrDrive_Single_Bracket_27Ev_5pics") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_30Ev_2pics_Plus,				TEXT("CrDrive_Single_Bracket_30Ev_2pics_Plus") },
		{ SDK::CrDriveMode::CrDrive_Single_Bracket_30Ev_2pics_Minus,				TEXT("CrDrive_Single_Bracket_30Ev_2pics_Minus") },
		{ SDK::CrDriveMode::CrDrive_WB_Bracket_Lo,				TEXT("CrDrive_WB_Bracket_Lo") },
		{ SDK::CrDriveMode::CrDrive_WB_Bracket_Hi,				TEXT("CrDrive_WB_Bracket_Hi") },
		{ SDK::CrDriveMode::CrDrive_DRO_Bracket_Lo,				TEXT("CrDrive_DRO_Bracket_Lo") },
		{ SDK::CrDriveMode::CrDrive_DRO_Bracket_Hi,				TEXT("CrDrive_DRO_Bracket_Hi") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Timer_3pics,				TEXT("CrDrive_Continuous_Timer_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Timer_5pics,				TEXT("CrDrive_Continuous_Timer_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Timer_2s_3pics,				TEXT("CrDrive_Continuous_Timer_2s_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Timer_2s_5pics,				TEXT("CrDrive_Continuous_Timer_2s_5pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Timer_5s_3pics,				TEXT("CrDrive_Continuous_Timer_5s_3pics") },
		{ SDK::CrDriveMode::CrDrive_Continuous_Timer_5s_5pics,				TEXT("CrDrive_Continuous_Timer_5s_5pics") },
		{ SDK::CrDriveMode::CrDrive_LPF_Bracket,				TEXT("CrDrive_LPF_Bracket") },
		{ SDK::CrDriveMode::CrDrive_RemoteCommander,				TEXT("CrDrive_RemoteCommander") },
		{ SDK::CrDriveMode::CrDrive_MirrorUp,				TEXT("CrDrive_MirrorUp") },
		{ SDK::CrDriveMode::CrDrive_SelfPortrait_1,				TEXT("CrDrive_SelfPortrait_1") },
		{ SDK::CrDriveMode::CrDrive_SelfPortrait_2,				TEXT("CrDrive_SelfPortrait_2") },
		{ SDK::CrDriveMode::CrDrive_FocusBracket,				TEXT("CrDrive_FocusBracket") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_focus_mode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrFocusMode::CrFocus_MF,				TEXT("MF") },
		{ SDK::CrFocusMode::CrFocus_AF_S,				TEXT("AF_S") },
		{ SDK::CrFocusMode::CrFocus_AF_C,				TEXT("AF_C") },
		{ SDK::CrFocusMode::CrFocus_AF_A,				TEXT("AF_A") },
		{ SDK::CrFocusMode::CrFocus_DMF,				TEXT("DMF") },
		{ SDK::CrFocusMode::CrFocus_AF_D,				TEXT("AF_D") },
		{ SDK::CrFocusMode::CrFocus_PF,				TEXT("PF") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_focus_area(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrFocusArea::CrFocusArea_Unknown,				TEXT("Unknown") },
		{ SDK::CrFocusArea::CrFocusArea_Wide,				TEXT("Wide") },
		{ SDK::CrFocusArea::CrFocusArea_Zone,				TEXT("Zone") },
		{ SDK::CrFocusArea::CrFocusArea_Center,				TEXT("Center") },
		{ SDK::CrFocusArea::CrFocusArea_Flexible_Spot_S,				TEXT("Flexible Spot S") },
		{ SDK::CrFocusArea::CrFocusArea_Flexible_Spot_M,				TEXT("Flexible Spot M") },
		{ SDK::CrFocusArea::CrFocusArea_Flexible_Spot_L,				TEXT("Flexible Spot L") },
		{ SDK::CrFocusArea::CrFocusArea_Expand_Flexible_Spot,				TEXT("Expand Flexible Spot") },
		{ SDK::CrFocusArea::CrFocusArea_Flexible_Spot,				TEXT("Flexible Spot") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Wide,				TEXT("Tracking Wide") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Zone,				TEXT("Tracking Zone") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Center,				TEXT("Tracking Center") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot_S,				TEXT("Tracking Flexible Spot S") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot_M,				TEXT("Tracking Flexible Spot M") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot_L,				TEXT("Tracking Flexible Spot L") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Expand_Flexible_Spot,				TEXT("Tracking Expand Flexible Spot") },
		{ SDK::CrFocusArea::CrFocusArea_Tracking_Flexible_Spot,				TEXT("Tracking Flexible Spot") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_live_view_image_quality(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_High,				TEXT("High") },
		{ SDK::CrPropertyLiveViewImageQuality::CrPropertyLiveViewImageQuality_Low,				TEXT("Low") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_media_slotx_format_enable_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrMediaFormat::CrMediaFormat_Disable,				TEXT("Disable") },
		{ SDK::CrMediaFormat::CrMediaFormat_Enable,				TEXT("Enabled") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}
text format_white_balance(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_AWB,				TEXT("AWB") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Underwater_Auto,				TEXT("Underwater_Auto") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Daylight,				TEXT("Daylight") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Shadow,				TEXT("Shadow") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Cloudy,				TEXT("Cloudy") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Tungsten,				TEXT("Tungsten") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent,				TEXT("Fluorescent") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_WarmWhite,				TEXT("Fluorescent_WarmWhite") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_CoolWhite,				TEXT("Fluorescent_CoolWhite") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_DayWhite,				TEXT("Fluorescent_DayWhite") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Fluorescent_Daylight,				TEXT("Fluorescent_Daylight") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Flush,				TEXT("Flush") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_ColorTemp,				TEXT("ColorTemp") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_1,				TEXT("Custom_1") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_2,				TEXT("Custom_2") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom_3,				TEXT("Custom_3") },
		{ SDK::CrWhiteBalanceSetting::CrWhiteBalance_Custom,				TEXT("Custom") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_customwb_capture_standby(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable,				TEXT("Disable") },
		{ SDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable,				TEXT("Enable") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_customwb_capture_standby_cancel(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable,				TEXT("Disable") },
		{ SDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable,				TEXT("Enable") },

	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_customwb_capture_operation(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Disable,				TEXT("Disable") },
		{ SDK::CrPropertyCustomWBOperation::CrPropertyCustomWBOperation_Enable,				TEXT("Enabled") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_customwb_capture_execution_state(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_Invalid,				TEXT("Invalid") },
		{ SDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_Standby,				TEXT("Standby") },
		{ SDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_Capturing,				TEXT("Capturing") },
		{ SDK::CrPropertyCustomWBExecutionState::CrPropertyCustomWBExecutionState_OperatingCamera,				TEXT("OperatingCamera") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_zoom_operation_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Disable,				TEXT("Disable") },
		{ SDK::CrZoomOperationEnableStatus::CrZoomOperationEnableStatus_Enable,				TEXT("Enable") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_zoom_setting_type(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrZoomSettingType::CrZoomSetting_OpticalZoomOnly,				TEXT("OpticalZoom") },
		{ SDK::CrZoomSettingType::CrZoomSetting_SmartZoomOnly,				TEXT("SmartZoom") },
		{ SDK::CrZoomSettingType::CrZoomSetting_On_ClearImageZoom,				TEXT("ClearImageZoom") },
		{ SDK::CrZoomSettingType::CrZoomSetting_On_DigitalZoom,				TEXT("DigitalZoom") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_zoom_types_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrZoomTypeStatus::CrZoomTypeStatus_OpticalZoom,				TEXT("OpticalZoom") },
		{ SDK::CrZoomTypeStatus::CrZoomTypeStatus_SmartZoom,				TEXT("SmartZoom") },
		{ SDK::CrZoomTypeStatus::CrZoomTypeStatus_ClearImageZoom,				TEXT("ClearImageZoom") },
		{ SDK::CrZoomTypeStatus::CrZoomTypeStatus_DigitalZoom,				TEXT("DigitalZoom") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_remocon_zoom_speed_type(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrRemoconZoomSpeedType::CrRemoconZoomSpeedType_Invalid,				TEXT("Invalid") },
		{ SDK::CrRemoconZoomSpeedType::CrRemoconZoomSpeedType_Variable,				TEXT("Variable") },
		{ SDK::CrRemoconZoomSpeedType::CrRemoconZoomSpeedType_Fixed,				TEXT("Fixed") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_camera_setting_save_operation(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrCameraSettingSaveOperation::CrCameraSettingSaveOperation_Disable,				TEXT("Disable") },
		{ SDK::CrCameraSettingSaveOperation::CrCameraSettingSaveOperation_Enable,				TEXT("Enabled") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_camera_setting_read_operation(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrCameraSettingReadOperation::CrCameraSettingReadOperation_Disable,				TEXT("Disable") },
		{ SDK::CrCameraSettingReadOperation::CrCameraSettingReadOperation_Enable,				TEXT("Enabled") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_camera_setting_save_read_state(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrCameraSettingSaveReadState::CrCameraSettingSaveReadState_Idle,				TEXT("Idle") },
		{ SDK::CrCameraSettingSaveReadState::CrCameraSettingSaveReadState_Reading,				TEXT("Reading") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_aps_c_or_full_switching_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrAPS_C_or_Full_SwitchingSetting::CrAPS_C_or_Full_SwitchingSetting_Full,				TEXT("Full") },
		{ SDK::CrAPS_C_or_Full_SwitchingSetting::CrAPS_C_or_Full_SwitchingSetting_APS_C,				TEXT("APS-C") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_aps_c_or_full_switching_enable_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrAPS_C_or_Full_SwitchingEnableStatus::CrAPS_C_or_Full_Switching_Disable,				TEXT("Disable") },
		{ SDK::CrAPS_C_or_Full_SwitchingEnableStatus::CrAPS_C_or_Full_Switching_Enable,				TEXT("Enable") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_playback_media(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrPlaybackMedia::CrPlaybackMedia_Slot1,				TEXT("Slot1") },
		{ SDK::CrPlaybackMedia::CrPlaybackMedia_Slot2,				TEXT("Slot2") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_camera_setting_reset_enable_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrCameraSettingsResetEnableStatus::CrCameraSettingsReset_Disable,				TEXT("Disable") },
		{ SDK::CrCameraSettingsResetEnableStatus::CrCameraSettingsReset_Enable,				TEXT("Enable") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_gain_base_sensitivity(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrGainBaseSensitivity::CrGainBaseSensitivity_High,				TEXT("High Level") },
		{ SDK::CrGainBaseSensitivity::CrGainBaseSensitivity_Low,				TEXT("Low Level") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}
text format_gain_base_iso_sensitivity(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrGainBaseIsoSensitivity::CrGainBaseIsoSensitivity_High,				TEXT("High Level") },
		{ SDK::CrGainBaseIsoSensitivity::CrGainBaseIsoSensitivity_Low,				TEXT("Low Level") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_monitor_lut_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrMonitorLUTSetting::CrMonitorLUT_OFF,				TEXT("OFF") },
		{ SDK::CrMonitorLUTSetting::CrMonitorLUT_ON,				TEXT("ON") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_baselook_value(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrBaseLookValueSetter::CrBaseLookValue_Preset,				TEXT("(Preset)") },
		{ SDK::CrBaseLookValueSetter::CrBaseLookValue_User,				TEXT("(User)") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_shutter_mode_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrShutterModeSetting::CrShutterMode_Automatic,				TEXT("Automatic") },
		{ SDK::CrShutterModeSetting::CrShutterMode_Manual,				TEXT("Manual") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_iris_mode_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrIrisModeSetting::CrIrisMode_Automatic,				TEXT("Automatic") },
		{ SDK::CrIrisModeSetting::CrIrisMode_Manual,				TEXT("Manual") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_exposure_control_type(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrExposureCtrlType::CrExposureCtrlType_PASMMode,				TEXT("P/A/S/M Mode") },
		{ SDK::CrExposureCtrlType::CrExposureCtrlType_FlexibleExposureMode,				TEXT("Flexible Exposure Mode") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_gain_control_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrGainControlSetting::CrGainControl_Automatic,				TEXT("Automatic") },
		{ SDK::CrGainControlSetting::CrGainControl_Manual,				TEXT("Manual") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_recording_setting(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_60p_50M,				TEXT("60p 50M / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_30p_50M,				TEXT("30p 50M / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_24p_50M,				TEXT("24p 50M / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50p_50M,				TEXT("50p 50M / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25p_50M,				TEXT("25p 50M / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_60i_24M,				TEXT("60i 24M(FX) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50i_24M_FX,				TEXT("50i 24M(FX) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_60i_17M_FH,				TEXT("60i 17M(FH) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50i_17M_FH,				TEXT("50i 17M(FH) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_60p_28M_PS,				TEXT("60p 28M(PS) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50p_28M_PS,				TEXT("50p 28M(PS) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_24p_24M_FX,				TEXT("24p 24M(FX) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25p_24M_FX,				TEXT("25p 24M(FX) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_24p_17M_FH,				TEXT("24p 17M(FH) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25p_17M_FH,				TEXT("25p 17M(FH) / AVCHD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_120p_50M_1280x720,				TEXT("120p 50M (1280x720) / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_100p_50M_1280x720,				TEXT("100p 50M (1280x720) / XAVC S") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_1920x1080_30p_16M,				TEXT("1920x1080 30p 16M / MP4") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_1920x1080_25p_16M,				TEXT("1920x1080 25p 16M / MP4") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_1280x720_30p_6M,				TEXT("1280x720 30p 6M / MP4") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_1280x720_25p_6M,				TEXT("1280x720 25p 6M / MP4") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_1920x1080_60p_28M,				TEXT("1920x1080 60p 28M / MP4") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_1920x1080_50p_28M,				TEXT("1920x1080 50p 28M / MP4") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_60p_25M_XAVC_S_HD,				TEXT("60p 25M / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50p_25M_XAVC_S_HD,				TEXT("50p 25M / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_30p_16M_XAVC_S_HD,				TEXT("30p 16M / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25p_16M_XAVC_S_HD,				TEXT("25p 16M / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_120p_100M_1920x1080_XAVC_S_HD,				TEXT("120p 100M (1920x1080)  / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_100p_100M_1920x1080_XAVC_S_HD,				TEXT("100p 100M (1920x1080)  / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_120p_60M_1920x1080_XAVC_S_HD,				TEXT("120p 60M (1920x1080) / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_100p_60M_1920x1080_XAVC_S_HD,				TEXT("100p 60M (1920x1080) / XAVC S HD") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_30p_100M_XAVC_S_4K,				TEXT("30p 100M / XAVC S 4K") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25p_100M_XAVC_S_4K,				TEXT("25p 100M / XAVC S 4K") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_24p_100M_XAVC_S_4K,				TEXT("24p 100M / XAVC S 4K") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_30p_60M_XAVC_S_4K,				TEXT("30p 60M / XAVC S 4K") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25p_60M_XAVC_S_4K,				TEXT("25p 60M / XAVC S 4K") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_24p_60M_XAVC_S_4K,				TEXT("24p 60M / XAVC S 4K") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_600M_422_10bit,				TEXT("600M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_500M_422_10bit,				TEXT("500M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_400M_420_10bit,				TEXT("400M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_300M_422_10bit,				TEXT("300M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_280M_422_10bit,				TEXT("280M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_250M_422_10bit,				TEXT("250M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_240M_422_10bit,				TEXT("240M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_222M_422_10bit,				TEXT("222M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_200M_422_10bit,				TEXT("200M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_200M_420_10bit,				TEXT("200M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_200M_420_8bit,				TEXT("200M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_185M_422_10bit,				TEXT("185M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_150M_420_10bit,				TEXT("150M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_150M_420_8bit,				TEXT("150M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_140M_422_10bit,				TEXT("140M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_111M_422_10bit,				TEXT("111M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_100M_422_10bit,				TEXT("100M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_100M_420_10bit,				TEXT("100M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_100M_420_8bit,				TEXT("100M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_93M_422_10bit,				TEXT("93M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_89M_422_10bit,				TEXT("89M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_75M_420_10bit,				TEXT("75M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_60M_420_8bit,				TEXT("60M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50M_422_10bit,				TEXT("50M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50M_420_10bit,				TEXT("50M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_50M_420_8bit,				TEXT("50M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_45M_420_10bit,				TEXT("45M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_30M_420_10bit,				TEXT("30M 420 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_25M_420_8bit,				TEXT("25M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_16M_420_8bit,				TEXT("16M 420 8bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_520M_422_10bit,				TEXT("520M 422 10bit") },
		{ SDK::CrRecordingSettingMovie::CrRecordingSettingMovie_260M_422_10bit,				TEXT("260M 422 10bit") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_dispmode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrDispMode::CrDispMode_GraphicDisplay,				TEXT("Graphic Display") },
		{ SDK::CrDispMode::CrDispMode_DisplayAllInfo,				TEXT("Display All Information") },
		{ SDK::CrDispMode::CrDispMode_NoDispInfo,				TEXT("No Display Information") },
		{ SDK::CrDispMode::CrDispMode_Histogram,				TEXT("Histogram") },
		{ SDK::CrDispMode::CrDispMode_Level,				TEXT("Level") },
		{ SDK::CrDispMode::CrDispMode_ForViewFinder,				TEXT("For viewfinder") },
		{ SDK::CrDispMode::CrDispMode_MonitorOff,				TEXT("Monitor Off") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_movie_rec_button_toggle_enable_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrMovieRecButtonToggleEnableStatus::CrMovieRecButtonToggle_Disable,				TEXT("Disable") },
		{ SDK::CrMovieRecButtonToggleEnableStatus::CrMovieRecButtonToggle_Enable,				TEXT("Enable") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_shutter_speed_value(std::uint64_t shutter_speed_value)
{
	text_stringstream ts;
	CrInt32u numerator = (CrInt32u)(shutter_speed_value >> 32);
	CrInt32u denominator = (CrInt32u)(shutter_speed_value & 0x0000FFFF);
	ts << numerator << '/' << denominator;
	return ts.str();
}

text format_media_slotx_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrSlotStatus::CrSlotStatus_OK,				TEXT("OK") },
		{ SDK::CrSlotStatus::CrSlotStatus_NoCard,				TEXT("No Card") },
		{ SDK::CrSlotStatus::CrSlotStatus_CardError,				TEXT("Card Error") },
		{ SDK::CrSlotStatus::CrSlotStatus_RecognizingOrLockedError,				TEXT("Recognizing Or LockedError") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_image_stabilization_steady_shot(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrImageStabilizationSteadyShot::CrImageStabilizationSteadyShot_Off,				TEXT("OFF") },
		{ SDK::CrImageStabilizationSteadyShot::CrImageStabilizationSteadyShot_On,				TEXT("ON") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_movie_image_stabilization_steady_shot(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrImageStabilizationSteadyShotMovie::CrImageStabilizationSteadyShotMovie_Off,				TEXT("OFF") },
		{ SDK::CrImageStabilizationSteadyShotMovie::CrImageStabilizationSteadyShotMovie_Standard,				TEXT("Standard") },
		{ SDK::CrImageStabilizationSteadyShotMovie::CrImageStabilizationSteadyShotMovie_Active,				TEXT("Active") },
		{ SDK::CrImageStabilizationSteadyShotMovie::CrImageStabilizationSteadyShotMovie_DynamicActive,				TEXT("Dynamic active") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_silent_mode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrSilentMode::CrSilentMode_Off,				TEXT("OFF") },
		{ SDK::CrSilentMode::CrSilentMode_On,				TEXT("ON") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_silent_mode_aperture_drive_in_af(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrSilentModeApertureDriveInAF::CrSilentModeApertureDriveInAF_NotTarget,				TEXT("Not Target") },
		{ SDK::CrSilentModeApertureDriveInAF::CrSilentModeApertureDriveInAF_Standard,				TEXT("Standard") },
		{ SDK::CrSilentModeApertureDriveInAF::CrSilentModeApertureDriveInAF_SilentPriority,				TEXT("Silent Priority") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_silent_mode_shutter_when_power_off(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrSilentModeShutterWhenPowerOff::CrSilentModeShutterWhenPowerOff_NotTarget,				TEXT("Not Target") },
		{ SDK::CrSilentModeShutterWhenPowerOff::CrSilentModeShutterWhenPowerOff_Off,				TEXT("OFF") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}
text format_silent_mode_auto_pixel_mapping(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrSilentModeAutoPixelMapping::CrSilentModeAutoPixelMapping_NotTarget,	TEXT("Not Target") },
		{ SDK::CrSilentModeAutoPixelMapping::CrSilentModeAutoPixelMapping_Off,			TEXT("OFF") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_shutter_type(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrShutterType::CrShutterType_Auto,							TEXT("Auto") },
		{ SDK::CrShutterType::CrShutterType_MechanicalShutter,				TEXT("Mechanical Shutter") },
		{ SDK::CrShutterType::CrShutterType_ElectronicShutter,				TEXT("Electronic Shutter") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_movie_shooting_mode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrMovieShootingMode::CrMovieShootingMode_Off,				TEXT("OFF") },
		{ SDK::CrMovieShootingMode::CrMovieShootingMode_CineEI,				TEXT("Cine EI") },
		{ SDK::CrMovieShootingMode::CrMovieShootingMode_CineEIQuick,		TEXT("Cine EI Quick") },
		{ SDK::CrMovieShootingMode::CrMovieShootingMode_Custom,				TEXT("Custom") },
		{ SDK::CrMovieShootingMode::CrMovieShootingMode_FlexibleISO,		TEXT("Flexible ISO") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_focus_driving_status(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrFocusDrivingStatus::CrFocusDrivingStatus_NotDriving,		TEXT("Not Driving") },
		{ SDK::CrFocusDrivingStatus::CrFocusDrivingStatus_Driving,			TEXT("Driving") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_media_slotx_rec_available(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrMediaSlotRecordingAvailableType::CrMediaSlotRecordingAvailableType_None,			TEXT("None") },
		{ SDK::CrMediaSlotRecordingAvailableType::CrMediaSlotRecordingAvailableType_Main,			TEXT("Main") },
		{ SDK::CrMediaSlotRecordingAvailableType::CrMediaSlotRecordingAvailableType_Proxy,			TEXT("Proxy") },
		{ SDK::CrMediaSlotRecordingAvailableType::CrMediaSlotRecordingAvailableType_MainAndProxy,	TEXT("Main and Proxy") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_focus_indication(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrFocusIndicator::CrFocusIndicator_Unlocked,				TEXT("Unlocked") },
		{ SDK::CrFocusIndicator::CrFocusIndicator_Focused_AF_S,			TEXT("Focused_AF_S") },
		{ SDK::CrFocusIndicator::CrFocusIndicator_NotFocused_AF_S,		TEXT("NotFocused_AF_S") },
		{ SDK::CrFocusIndicator::CrFocusIndicator_Focused_AF_C,			TEXT("Focused_AF_C") },
		{ SDK::CrFocusIndicator::CrFocusIndicator_NotFocused_AF_C,		TEXT("NotFocused_AF_C") },
		{ SDK::CrFocusIndicator::CrFocusIndicator_TrackingSubject_AF_C,	TEXT("TrackingSubject_AF_C") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_lock_indicator(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrLockIndicator::CrLockIndicator_Unknown,				TEXT("Unknown") },
		{ SDK::CrLockIndicator::CrLockIndicator_Unlocked,				TEXT("Unlocked") },
		{ SDK::CrLockIndicator::CrLockIndicator_Locked,					TEXT("Locked") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_SdkControlMode(std::uint32_t value)
{
	const std::map<std::uint32_t, text> table{
		{ SDK::CrSdkControlMode::CrSdkControlMode_Remote,				TEXT("Remote") },
		{ SDK::CrSdkControlMode::CrSdkControlMode_ContentsTransfer,		TEXT("ContentsTransfer") },
	};
	auto iter = table.find(value);
	if ( iter != end(table) )	return iter->second;
	else						return TEXT("");
}

text format_focus_position_value(std::uint32_t value)
{
	text_stringstream ts;
	char dspValue[10];
#if defined (WIN32) || defined(WIN64)
	sprintf_s(dspValue, sizeof(dspValue), "0x%04X", value);
#else
	snprintf(dspValue, sizeof(dspValue), "0x%04X", value);
#endif
	ts << dspValue;
	return ts.str();
}

} // namespace cli
