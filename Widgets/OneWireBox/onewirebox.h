#ifndef ONEWIREBOX_H
#define ONEWIREBOX_H


#include <QLabel>
#include <QWidget>
#include <QObject>
#include <QGroupBox>
#include <QHBoxLayout>

#include "onewiredata.h"


class OneWireBox: public QObject
{
    Q_OBJECT

private:
    int offset;

    QWidget* parent;
    OneWireData data;

    QGroupBox*   sensor_box;
    QWidget*     horizontalLayoutWidget;
    QHBoxLayout* horizontalLayout;
    QLabel*      sensor_number;
    QLabel*      sensor_address;
    QLabel*      sensor_value;

    void retranslateUi();

    void init();
    void destroy();

public:
    OneWireBox(const QWidget* parent, const OneWireData& data, const int offset = 0);

    OneWireBox(const OneWireBox& other);
    OneWireBox& operator=(const OneWireBox& other);

    ~OneWireBox();

    int getY();
    void setY(int y);

    void setValue(const QString& value);

    uint8_t getNumber();

    void show();
    void clear();
    void disable();
    void enable();
    unsigned height();

};

#endif // ONEWIREBOX_H
