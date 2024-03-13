#ifndef SENSORLIST_H
#define SENSORLIST_H


#include <QWidget>
#include <QGroupBox>
#include <QScrollBar>


class SensorList
{
private:
    void retranslateUi();

public:
    QGroupBox*  sensors_group;
    QScrollBar* verticalScrollBar;

    SensorList(QWidget* parent);
    ~SensorList();

    bool isCursorInside();

    void mouseWheelUp();
    void mouseWheelDown();
};

#endif // SENSORLIST_H
