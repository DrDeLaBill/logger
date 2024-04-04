#include "onewirebox.h"

#include <QRect>
#include <QString>
#include <QCoreApplication>

#include <string>

#include "onewireservice.h"


#define MARGIN_STR "5"
#define MARGIN_INT 5


OneWireBox::OneWireBox(const QWidget* parent, const OneWireData& data, const int offset):
    offset(offset), parent(const_cast<QWidget*>(parent)), data(data)
{
    this->init();
    this->retranslateUi();
}

OneWireBox::OneWireBox(const OneWireBox& other): OneWireBox(other.parent, other.data, other.offset) { }

OneWireBox& OneWireBox::operator=(const OneWireBox& other)
{
    destroy();
    offset = other.offset;
    parent = other.parent;
    data   = other.data;
    init();
    return *this;
}

OneWireBox::~OneWireBox()
{
    destroy();
}

void OneWireBox::retranslateUi()
{
    sensor_box->setTitle(QString());
    sensor_number->setText(QCoreApplication::translate("MainWindow", std::to_string(data.number).c_str(), nullptr));
    char value[20] = "";
    snprintf(value, sizeof(value), "0x%08X%08X", (uint32_t)(data.address >> 32), (uint32_t)data.address);
    sensor_address->setText(QCoreApplication::translate("MainWindow", value, nullptr));
    sensor_value->setText(QCoreApplication::translate("MainWindow", std::to_string(data.value).c_str(), nullptr));
}

void OneWireBox::init()
{
    sensor_box = new QGroupBox(const_cast<QWidget*>(parent));
    sensor_box->setObjectName(("sensor_box_" + std::to_string(data.number)).c_str());
    sensor_box->setGeometry(QRect(0, offset, 351, ONEWIRE_BOX_HEIGHT));
    sensor_box->setGeometry(QRect(0, offset + getY(), 351, ONEWIRE_BOX_HEIGHT));
    sensor_box->setStyleSheet(
        QString::fromUtf8(
            "QGroupBox {\n"
            "	margin: " MARGIN_STR "px;\n"
            "}\n"
            "\n"
            "QLabel {\n"
            "	border: 0px;\n"
            "}"
        )
    );

    horizontalLayoutWidget = new QWidget(sensor_box);
    horizontalLayoutWidget->setObjectName("sensor_horizontalLayoutWidget_" + data.number);
    horizontalLayoutWidget->setGeometry(QRect(0, 0, 351, ONEWIRE_BOX_HEIGHT));

    horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
    horizontalLayout->setObjectName("sensor_horizontalLayout_" + data.number);
    horizontalLayout->setContentsMargins(MARGIN_INT, MARGIN_INT, MARGIN_INT, MARGIN_INT);

    sensor_number = new QLabel(horizontalLayoutWidget);
    sensor_number->setObjectName("sensor_number_" + data.number);
    horizontalLayout->addWidget(sensor_number);
    sensor_number->setMargin(MARGIN_INT);
    sensor_number->setText(std::to_string(data.number).c_str());

    sensor_address = new QLabel(horizontalLayoutWidget);
    sensor_address->setObjectName("sensor_address_" + data.number);
    horizontalLayout->addWidget(sensor_address);
    sensor_address->setMargin(MARGIN_INT);
    sensor_address->setText(std::to_string(data.address).c_str());

    sensor_value = new QLabel(horizontalLayoutWidget);
    sensor_value->setObjectName("sensor_value_" + data.number);
    horizontalLayout->addWidget(sensor_value);
    sensor_value->setMargin(MARGIN_INT);
    sensor_value->setText(std::to_string(data.value).c_str());
}

void OneWireBox::destroy()
{
    sensor_number->deleteLater();
    sensor_address->deleteLater();
    sensor_value->deleteLater();
    horizontalLayout->deleteLater();
    horizontalLayoutWidget->deleteLater();
    sensor_box->deleteLater();
}

int OneWireBox::getY()
{
    return ONEWIRE_BOX_HEIGHT * data.number;
}

void OneWireBox::setY(int y)
{
    const QRect& rect = sensor_box->geometry();
    this->sensor_box->setGeometry(QRect(rect.x(), y, rect.width(), rect.height()));
}

void OneWireBox::setValue(const QString& value)
{
    this->sensor_value->setText(value);
}

uint8_t OneWireBox::getNumber()
{
    return data.number;
}

void OneWireBox::show()
{
    sensor_box->show();
}

void OneWireBox::clear()
{
    sensor_number->setText("0");
    sensor_address->setText("0");
    sensor_value->setText("0");
}

void OneWireBox::disable()
{
    sensor_number->setDisabled(true);
    sensor_address->setDisabled(true);
    sensor_value->setDisabled(true);
}

void OneWireBox::enable()
{
    sensor_number->setDisabled(false);
    sensor_address->setDisabled(false);
    sensor_value->setDisabled(false);
}

unsigned OneWireBox::height()
{
    return sensor_box->height();
}
