#ifndef PROPERTYVALUETABLE_H
#define PROPERTYVALUETABLE_H

#include <cstdint>
#include <vector>
#include <map>
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
	PossibleString,
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
	SCRSDK::CrDataType dataType;
	const std::string tag;
	text (*const formatFunc)(std::uint32_t val);
	const std::map<std::uint32_t, std::string>* mapEnum;

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

text format_focus_position_value(std::uint32_t value);

} // namespace cli

#endif // !PROPERTYVALUETABLE_H
