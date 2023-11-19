#ifndef PROPERTYVALUETABLE_H
#define PROPERTYVALUETABLE_H

#include <cstdint>
#include <vector>
#include "CameraRemote_SDK.h"
#include "Text.h"
#include "CRSDK/CrDeviceProperty.h"

namespace cli
{
static const int MAX_CURRENT_STR = 255;

enum PossibleType : std::uint32_t {
	PossibleNone = 0,
	PossibleList,
	PossibleRange,
};

template <typename T>
struct PropertyValueEntry
{
    int writable; // -1:Initial, 0:false, 1:true
    T current;
    std::vector<T> possible;
    PropertyValueEntry()
    {
        writable = -1;
    }
};

struct PropertyValue
{
	int writable; // -1:Initial, 0:false, 1:true
	const SCRSDK::CrDataType dataType;
	PossibleType possibleType;
	const std::string tag;
	text (*const formatFunc)(std::uint32_t val);

	std::uint32_t current;
	std::vector<std::uint32_t> possible;
};

struct PropertyStringValueEntry
{
    int writable; // -1:Iniitial, 0:false, 1:true
    int  length;
    text current;
    CrInt16u* currentStr;
    PropertyStringValueEntry()
    {
        writable = -1;
    }
};

struct PropertyValueTable
{
    PropertyStringValueEntry lensModelNameStr;
    PropertyValueEntry<std::uint64_t> shutter_speed_value;
};

text format_number(std::uint32_t value);

text format_f_number(std::uint32_t f_number);
text format_iso_sensitivity(std::uint32_t iso);
text format_shutter_speed(std::uint32_t shutter_speed);
text format_position_key_setting(std::uint32_t value);
text format_exposure_program_mode(std::uint32_t value);
text format_still_capture_mode(std::uint32_t value);
text format_focus_mode(std::uint32_t value);
text format_focus_area(std::uint32_t value);
text format_live_view_image_quality(std::uint32_t value);
text format_media_slotx_format_enable_status(std::uint32_t value);
text format_white_balance(std::uint32_t value);
text format_customwb_capture_standby(std::uint32_t value);
text format_customwb_capture_standby_cancel(std::uint32_t value);
text format_customwb_capture_operation(std::uint32_t value);
text format_customwb_capture_execution_state(std::uint32_t value);
text format_zoom_operation_status(std::uint32_t value);
text format_zoom_setting_type(std::uint32_t value);
text format_zoom_types_status(std::uint32_t value);
text format_remocon_zoom_speed_type(std::uint32_t value);
text format_aps_c_or_full_switching_setting(std::uint32_t value);
text format_aps_c_or_full_switching_enable_status(std::uint32_t value);
text format_camera_setting_save_operation(std::uint32_t value);
text format_camera_setting_read_operation(std::uint32_t value);
text format_camera_setting_save_read_state(std::uint32_t value);
text format_camera_setting_reset_enable_status(std::uint32_t value);
text format_gain_base_sensitivity(std::uint32_t value);
text format_gain_base_iso_sensitivity(std::uint32_t value);
text format_monitor_lut_setting(std::uint32_t value);
text format_baselook_value(std::uint32_t value);
text format_playback_media(std::uint32_t value);
text format_shutter_mode_setting(std::uint32_t value);
text format_iris_mode_setting(std::uint32_t value);
text format_exposure_control_type(std::uint32_t value);
text format_gain_control_setting(std::uint32_t value);
text format_recording_setting(std::uint32_t value);
text format_dispmode(std::uint32_t value);
text format_movie_rec_button_toggle_enable_status(std::uint32_t value);
text format_shutter_speed_value(std::uint64_t shutter_speed_value);
text format_media_slotx_status(std::uint32_t value);
text format_image_stabilization_steady_shot(std::uint32_t value);
text format_movie_image_stabilization_steady_shot(std::uint32_t value);
text format_silent_mode(std::uint32_t value);
text format_silent_mode_aperture_drive_in_af(std::uint32_t value);
text format_silent_mode_shutter_when_power_off(std::uint32_t value);
text format_silent_mode_auto_pixel_mapping(std::uint32_t value);
text format_shutter_type(std::uint32_t value);
text format_movie_shooting_mode(std::uint32_t value);
text format_focus_driving_status(std::uint32_t value);
text format_media_slotx_rec_available(std::uint32_t value);

text format_focus_indication(std::uint32_t value);
text format_lock_indicator(std::uint32_t value);
text format_SdkControlMode(std::uint32_t value);
text format_focus_position_value(std::uint32_t value);

} // namespace cli

#endif // !PROPERTYVALUETABLE_H
