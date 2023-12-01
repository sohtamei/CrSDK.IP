﻿#ifndef CAMERADEVICE_H
#define CAMERADEVICE_H

#if defined(__linux__)
// Use errno_t if available, define it otherwise
#ifndef __STDC_LIB_EXT1__
typedef int errno_t;
#else
#define  __STDC_WANT_LIB_EXT1__ 1
#endif

// End Linux definitions
#endif

#include <atomic>
#include <cstdint>
#include <future>
#include <mutex>
#include "CRSDK/CameraRemote_SDK.h"
#include "CRSDK/IDeviceCallback.h"
#include "ConnectionInfo.h"
#include "PropertyValueTable.h"
#include "Text.h"
#include "MessageDefine.h"

namespace cli
{

class CRFolderInfos
{
public:
    CRFolderInfos(SCRSDK::CrMtpFolderInfo* info, int32_t nums)
        : pFolder(info)
        , numOfContents(nums)
    {};
    ~CRFolderInfos()
    {
        delete[] pFolder->folderName;
        pFolder->folderName = NULL;
        delete pFolder;
        pFolder = NULL;
    };
public:
    SCRSDK::CrMtpFolderInfo* pFolder;
    int32_t numOfContents;
};

typedef std::vector<CRFolderInfos*> MtpFolderList;
typedef std::vector<SCRSDK::CrMtpContentsInfo*> MtpContentsList;
typedef std::vector<SCRSDK::CrMediaProfileInfo*> MediaProfileList;

class CameraDevice : public SCRSDK::IDeviceCallback
{
public:
    CameraDevice() = delete;
    CameraDevice(std::int32_t no, SCRSDK::ICrCameraObjectInfo const* camera_info);
    ~CameraDevice();

    bool getfingerprint();
    bool connect(SCRSDK::CrSdkControlMode openMode, SCRSDK::CrReconnectingSet reconnect);
    bool disconnect();
    bool release();

    CrInt32u get_sshsupport();
    SCRSDK::CrSdkControlMode get_sdkmode();

    /*** Shooting operations ***/

    void s1_shooting();
    void af_shutter(std::uint32_t delay_ms);
    void continuous_shooting();

/*
    void get_select_media_format();
    bool get_custom_wb();
    void get_zoom_operation();
    bool get_aps_c_or_full_switching_setting();
    bool get_camera_setting_saveread_state();
    bool get_baselook_value();
    bool get_white_balance_tint();
    void get_media_slot_status();
    bool get_movie_rec_button_toggle_enable_status();

    void set_af_area_position();
    void set_select_media_format();
    void execute_movie_rec();

    void set_custom_wb();
    void set_zoom_operation();
    void execute_camera_setting_reset();
    void set_baselook_value();
    void execute_pos_xy(CrInt16u code);

    void execute_preset_focus();
    void execute_APS_C_or_Full();
    void execute_movie_rec_toggle();
    void execute_focus_bracket();
    void do_download_camera_setting_file();
    void do_upload_camera_setting_file();
    void getFileNames(std::vector<text> &file_names);

    bool is_live_view_enable() { return m_lvEnbSet; };
    void change_live_view_enable();
*/
    // Check if this device is connected
    bool is_connected() const;
    std::uint32_t ip_address() const;
    text ip_address_fmt() const;
    text mac_address() const;
    std::int16_t pid() const;
    text get_id();

    std::int32_t get_number() { return m_number; }
    text get_model() { return text(m_info->GetModel()); }
    bool is_getfingerprint() { return !m_fingerprint.empty(); };
    bool is_setpassword() { return !m_userPassword.empty(); };

public:
    // Inherited via IDeviceCallback
    virtual void OnConnected(SCRSDK::DeviceConnectionVersioin version) override;
    virtual void OnDisconnected(CrInt32u error) override;
    virtual void OnPropertyChanged() override;
    virtual void OnLvPropertyChanged() override;
    virtual void OnCompleteDownload(CrChar* filename, CrInt32u type) override;
    virtual void OnWarning(CrInt32u warning) override;
    virtual void OnWarningExt(CrInt32u warning, CrInt32 param1, CrInt32 param2, CrInt32 param3) override;
    virtual void OnError(CrInt32u error) override;
    virtual void OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes) override;
    virtual void OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes) override;
    virtual void OnNotifyContentsTransfer(CrInt32u notify, SCRSDK::CrContentHandle contentHandle, CrChar* filename) override;

public:
/*
    void getContentsList();
    void pullContents(SCRSDK::CrContentHandle content);
    void getScreennail(SCRSDK::CrContentHandle content);
    void getThumbnail(SCRSDK::CrContentHandle content);

    text format_display_string_type(SCRSDK::CrDisplayStringType type);
    void execute_request_displaystringlist();
    void execute_get_displaystringtypes();
    void execute_get_displaystringlist();
    text format_dispstrlist(SCRSDK::CrDisplayStringListInfo list);

    void get_mediaprofile();
    bool get_focus_position_setting();
    void set_focus_position_setting();
    bool execute_focus_position_cancel();
*/
private:
    std::int32_t m_number;
    SCRSDK::ICrCameraObjectInfo* m_info;
    std::int64_t m_device_handle;
    std::atomic<bool> m_connected;
    ConnectionType m_conn_type;
    NetworkInfo m_net_info;
    UsbInfo m_usb_info;
    bool m_lvEnbSet;
    SCRSDK::CrSdkControlMode m_modeSDK;
    MtpFolderList   m_foldList;
    MtpContentsList m_contentList;
    bool m_spontaneous_disconnection;
    // DispStrList
    std::vector<SCRSDK::CrDisplayStringType> m_dispStrTypeList; // Information returned as a result of GetDisplayStringTypes
    MediaProfileList m_mediaprofileList;
    std::string m_fingerprint;
    std::string m_userPassword;
    
	SCRSDK::CrDevicePropertyCode m_respPropId;
	void (*m_cb_respProp)(SCRSDK::CrDevicePropertyCode id);
	std::promise<void>* m_eventPromise;

public:
	bool set_save_info(text prefix) const;
	std::int32_t get_live_view(uint8_t* buf[]);

	void load_liveview_properties(std::uint32_t num = 0, std::uint32_t* codes = nullptr);
	void load_properties(std::uint32_t num = 0, std::uint32_t* codes = nullptr);

	SCRSDK::CrDevicePropertyCode Prop_tag2id(std::string tag) const;

	struct PropertyValue* GetProp(SCRSDK::CrDevicePropertyCode id);
	std::int32_t SetProp(SCRSDK::CrDevicePropertyCode id, std::uint64_t value);
	std::int32_t SetProp(SCRSDK::CrDevicePropertyCode id, std::string _text);
	std::int32_t setProp(SCRSDK::CrDevicePropertyCode id, std::uint64_t value);
	std::int32_t waitProp(SCRSDK::CrDevicePropertyCode id, std::int32_t timeoutMs);

	void GetAvailablePropList(std::vector<std::string>& propList);

	std::int32_t SendCommand(SCRSDK::CrCommandId cmd) const;
	std::int32_t SendCommand(std::string _text) const;

private:
	void parse_prop(SCRSDK::CrDeviceProperty& devProp, SCRSDK::CrDevicePropertyCode id);
	void parse_propStr(SCRSDK::CrDeviceProperty& devProp, SCRSDK::CrDevicePropertyCode id);

	std::string PropCurrentText(SCRSDK::CrDevicePropertyCode id) const;

	void notifyEvent(SCRSDK::CrDevicePropertyCode id);
	void sendProp(SCRSDK::CrDevicePropertyCode id);


};
} // namespace cli


inline errno_t MemCpyEx(void* result, const void* source, size_t type_size)
{
#if (defined(_WIN32) || defined(_WIN64))
    return memcpy_s(result, type_size, source, type_size);
#else
    std::memcpy(result, source, type_size);
    return 0;
#endif
}

#define MAC_MAX_PATH 255

#endif // !CAMERADEVICE_H
