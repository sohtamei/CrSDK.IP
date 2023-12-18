#include "app/CRSDK/CameraRemote_SDK.h"

void uploadFile(const wchar_t* filename);
void SendProp(SCRSDK::CrDevicePropertyCode id);
void ErrorProp(SCRSDK::CrDevicePropertyCode id, std::string message="");
void Send2DArray(std::string tag, std::vector<std::vector<uint32_t>>& info);
