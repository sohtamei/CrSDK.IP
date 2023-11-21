#include <map>
#include <cmath>
#include <cstring>
#include "PropertyValueTable.h"


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
std::string format_number(std::uint32_t value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string format_f_number(std::uint32_t f_number)
{
    std::stringstream ss;
    if ((0x0000 == f_number) || (SDK::CrFnumber_Unknown == f_number)) {
        ss << "--";
    }
    else if(SDK::CrFnumber_Nothing == f_number) {
        // Do nothing
    }
    else {
        auto modValue = static_cast<std::uint16_t>(f_number % 100);
        if (modValue > 0) {
            ss << 'F' << impl::Round((f_number / 100.0), 1);
        }
        else {
            ss << 'F' << f_number / 100;
        }
    }

    return ss.str();
}

std::string format_iso_sensitivity(std::uint32_t iso)
{
    std::stringstream ss;

    std::uint32_t iso_ext = (iso >> 24) & 0x000000F0;  // bit 28-31
    std::uint32_t iso_mode = (iso >> 24) & 0x0000000F; // bit 24-27
    std::uint32_t iso_value = (iso & 0x00FFFFFF);      // bit  0-23

    if (iso_mode == SDK::CrISO_MultiFrameNR) {
        ss << "Multi Frame NR ";
    }
    else if (iso_mode == SDK::CrISO_MultiFrameNR_High) {
        ss << "Multi Frame NR High ";
    }

    if (iso_value == SDK::CrISO_AUTO) {
        ss << "ISO AUTO";
    }
    else {
        ss << "ISO " << iso_value;
    }

    //if (iso_ext == SDK::CrISO_Ext) {
    //    ss << " (EXT)";
    //}

    return ss.str();
}

std::string format_shutter_speed(std::uint32_t shutter_speed)
{
    std::stringstream ss;

    CrInt16u numerator = impl::HIWORD(shutter_speed);
    CrInt16u denominator = impl::LOWORD(shutter_speed);

    if (0 == shutter_speed) {
        ss << "Bulb";
    }
    else if (0 == denominator) {
        ss << "error";
    }
    else if (1 == numerator) {
        ss << numerator << '/' << denominator;
    }
    else if (0 == numerator % denominator) {
        ss << (numerator / denominator) << '"';
    }
    else {
        CrInt32 numdivision = numerator / denominator;
        CrInt32 numremainder = numerator % denominator;
        ss << numdivision << '.' << numremainder << '"';
    }
    return ss.str();
}

std::string format_focus_position_value(std::uint32_t value)
{
	std::stringstream ss;
	char dspValue[10];
#if defined (WIN32) || defined(WIN64)
	sprintf_s(dspValue, sizeof(dspValue), "0x%04X", value);
#else
	snprintf(dspValue, sizeof(dspValue), "0x%04X", value);
#endif
	ss << dspValue;
	return ss.str();
}

} // namespace cli
