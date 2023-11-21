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
/*
enum PossibleType : std::uint32_t {
	PossibleNone = 0,
	PossibleList,
	PossibleRange,
	PossibleString,
};
*/
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
	const std::string tag;
	const std::map<std::uint32_t, std::string>* mapEnum;
	std::string (*const formatFunc)(std::uint32_t val);

	SCRSDK::CrDataType dataType;
	std::int32_t current;
	std::vector<std::int32_t> possible;
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

std::string format_number(std::uint32_t value);

std::string format_f_number(std::uint32_t f_number);
std::string format_iso_sensitivity(std::uint32_t iso);
std::string format_shutter_speed(std::uint32_t shutter_speed);

std::string format_focus_position_value(std::uint32_t value);

} // namespace cli

#endif // !PROPERTYVALUETABLE_H
