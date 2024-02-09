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

void USBSearchHandler::operate() const
{
    HIDService::init();

    while (!HIDService::isDeviceConnected(HID_VENDOR_ID, HID_PRODUCT_ID));

    HIDService::deinit();
}

void USBLoadRecordHandler::operate() const
{
    HIDService::init();

    HIDService::loadReport(HID_VENDOR_ID, HID_PRODUCT_ID);

    HIDService::deinit();
}


HIDController<USBCGetCharacteristicHandler::hid_settings_table_t> USBCGetCharacteristicHandler::hid_settings_controller;
fsm::FiniteStateMachine<USBCGetCharacteristicHandler::fsm_table> USBCGetCharacteristicHandler::fsm;
uint16_t USBCGetCharacteristicHandler::characteristic_id = HID_FIRST_KEY;
unsigned USBCGetCharacteristicHandler::errors_count = 0;
uint8_t USBCGetCharacteristicHandler::index = 0;
void USBCGetCharacteristicHandler::operate() const
{
    HIDService::init();

    fsm.proccess();

    HIDService::deinit();
}

void USBCGetCharacteristicHandler::_init_s::operator ()()
{
    fsm.push_event(success_e{});
}

utl::Timer USBCGetCharacteristicHandler::_idle_s::timer(DEVICE_DELAY_MS);
void USBCGetCharacteristicHandler::_idle_s::operator ()()
{
    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
    }
}

utl::Timer USBCGetCharacteristicHandler::_request_s::timer(DEVICE_DELAY_MS);
void USBCGetCharacteristicHandler::_request_s::operator ()()
{
    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
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
        if (errors_count > ERRORS_COUNT_MAX) {
            throw;
        }
        fsm.push_event(timeout_e{});
        return;
    }

    hid_settings_controller.setValue(characteristic_id, USBDReport::getReport().data, index);

    fsm.push_event(success_e{});
}

void USBCGetCharacteristicHandler::init_characteristics_a::operator ()()
{
    characteristic_id = HID_FIRST_KEY;
    index = 0;
    errors_count = 0;
    _request_s::timer.start();
}

void USBCGetCharacteristicHandler::iterate_characteristics_a::operator ()()
{
    if (index < hid_settings_controller.characteristicLength(characteristic_id)) {
        index++;
    } else {
        characteristic_id++;
        index = 0;
        errors_count = 0;
    }

    if (characteristic_id > hid_settings_controller.maxKey()) {
        fsm.push_event(end_e{});
    }

    _request_s::timer.start();
}

void USBCGetCharacteristicHandler::start_idle_timer_a::operator ()()
{
    _idle_s::timer.start();
}

void USBCGetCharacteristicHandler::count_error_a::operator ()()
{
    errors_count++;
    _request_s::timer.start();
}

void USBCGetCharacteristicHandler::reset_usb_a::operator ()()
{
    // TODO: reset usb
    throw exceptions::UsbException();
}
