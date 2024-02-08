#include "usbhandler.h"

#include "hidservice.h"
#include "HIDController.h"

#define DDEVICE_DELAY_MS (1000)


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

utl::Timer USBCGetCharacteristicHandler::_idle_s::timer(DDEVICE_DELAY_MS);
void USBCGetCharacteristicHandler::_idle_s::operator ()()
{
    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
    }
}

utl::Timer USBCGetCharacteristicHandler::_request_s::timer(DDEVICE_DELAY_MS);
void USBCGetCharacteristicHandler::_request_s::operator ()()
{
    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
    }
}

void USBCGetCharacteristicHandler::init_characteristics_a::operator ()()
{
    characteristic_id = HID_FIRST_KEY;
}

void USBCGetCharacteristicHandler::iterate_characteristics_a::operator ()()
{
    characteristic_id++;
    if (characteristic_id > hid_settings_controller.maxKey()) {
        fsm.push_event(end_e{});
    } else {
        // TODO: send report & get report
    }
}
