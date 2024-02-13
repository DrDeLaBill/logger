#include "usbhandler.h"

#include <memory>

#include "variables.h"
#include "hidservice.h"
#include "usbdreport.h"
#include "HIDController.h"

#define DEVICE_DELAY_MS (1000)


bool USBHandlerEqual::operator()(const QString& lhs, const QString& rhs) const
{
    return lhs.toStdString() == rhs.toStdString();
}

std::size_t USBHandlerHash::operator()(const QString& key) const
{
    return std::hash<std::string>{}(key.toStdString());
}

USBCStatus USBIdleHandler::operate() const
{
    return USBC_RES_DONE;
}

USBCStatus USBSearchHandler::operate() const
{
    HIDService::init();

    bool status = HIDService::isDeviceConnected(HID_VENDOR_ID, HID_PRODUCT_ID);

    HIDService::deinit();

    return status ? USBC_RES_DONE : USBC_RES_ERROR;
}

USBCStatus USBLoadRecordHandler::operate() const
{
//    HIDService::init();

//    HIDService::loadReport(HID_VENDOR_ID, HID_PRODUCT_ID);

//    HIDService::deinit();
    return USBC_RES_ERROR;
}


HIDController<USBUpdateHandler::hid_settings_table_t> USBUpdateHandler::hid_settings_controller;
fsm::FiniteStateMachine<USBUpdateHandler::fsm_table> USBUpdateHandler::fsm;
uint16_t USBUpdateHandler::characteristic_id = HID_FIRST_KEY;
unsigned USBUpdateHandler::errors_count = 0;
USBCStatus USBUpdateHandler::result = USBC_RES_OK;
uint8_t USBUpdateHandler::index = 0;
USBCStatus USBUpdateHandler::operate() const
{
    HIDService::init();

    fsm.proccess();

    HIDService::deinit();

    return result;
}

void USBUpdateHandler::update()
{
    if (result == USBC_RES_DONE) {
        fsm.push_event(success_e{});
    }
}

void USBUpdateHandler::_init_s::operator ()()
{
    fsm.push_event(success_e{});
    result = USBC_RES_OK;
}

void USBUpdateHandler::_idle_s::operator ()()
{
    fsm.push_event(success_e{});
    result = USBC_RES_DONE;
}

utl::Timer USBUpdateHandler::_request_s::timer(DEVICE_DELAY_MS);
void USBUpdateHandler::_request_s::operator ()()
{
    result = USBC_RES_OK;

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (characteristic_id > hid_settings_controller.maxKey()) {
        fsm.push_event(end_e{});
        result = USBC_RES_ERROR;
        return;
    }

    report_pack_t report = {};
    report.characteristic_id = HID_GETTER_ID;
    report.index = index;
    std::shared_ptr<uint8_t[]> data = utl::serialize<uint16_t>(&characteristic_id);
    report.setData(data.get(), sizeof(uint16_t));

    try {
        HIDService::sendReport(HID_VENDOR_ID, HID_PRODUCT_ID, report);
    } catch (...) {
        result = USBC_RES_ERROR;
        if (errors_count > ERRORS_COUNT_MAX) {
            throw;
        }
        fsm.push_event(timeout_e{});
        return;
    }

    hid_settings_controller.setValue(characteristic_id, USBDReport::getReport().data, index);

    fsm.push_event(success_e{});
}

void USBUpdateHandler::init_characteristics_a::operator ()()
{
    characteristic_id = HID_FIRST_KEY;
    index = 0;
    errors_count = 0;
    _request_s::timer.start();
}

void USBUpdateHandler::iterate_characteristics_a::operator ()()
{
    index++;
    if (index >= hid_settings_controller.characteristicLength(characteristic_id)) {
        characteristic_id++;
        index = 0;
        errors_count = 0;
    }

    _request_s::timer.start();
}

void USBUpdateHandler::start_init_a::operator ()() { }

void USBUpdateHandler::start_idle_a::operator ()() { }

void USBUpdateHandler::count_error_a::operator ()()
{
    errors_count++;
    _request_s::timer.start();
}

void USBUpdateHandler::reset_usb_a::operator ()()
{
    // TODO: reset usb
    throw exceptions::UsbException();
}

USBCStatus USBUpgradeHandler::operate() const
{
    // TODO: save
    return USBC_RES_ERROR;
}
