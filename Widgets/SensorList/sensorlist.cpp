#include "sensorlist.h"


SensorList::SensorList(QWidget* parent)
{
    sensors_group = new QGroupBox(parent);
    sensors_group->setObjectName("sensors_group");
    sensors_group->setGeometry(QRect(10, 90, 371, 311));
    sensors_group->setStyleSheet(
        QString::fromUtf8(
            "QGroupBox {\n"
           "	border: 1px solid #17202a;\n"
           "	border-radius: 2px;\n"
           "}"
        )
    );

    verticalScrollBar = new QScrollBar(sensors_group);
    verticalScrollBar->setObjectName("verticalScrollBar");
    verticalScrollBar->setGeometry(QRect(350, 0, 20, 311));
    verticalScrollBar->setMaximum(100);
    verticalScrollBar->setSingleStep(15);
    verticalScrollBar->setPageStep(30);
    verticalScrollBar->setOrientation(Qt::Vertical);

    this->retranslateUi();
}

bool SensorList::isCursorInside()
{
    const QPoint& point = sensors_group->mapFromGlobal(QCursor::pos());
    const QRect& group = sensors_group->geometry();
    if (point.x() < 0 || point.x() > group.width()) {
        return false;
    }
    if (point.y() < 0 || point.y() > group.height()) {
        return false;
    }
    return true;
}

void SensorList::retranslateUi()
{
    sensors_group->setTitle(QString());
}

void SensorList::mouseWheelUp()
{
    int newValue = verticalScrollBar->value() - verticalScrollBar->singleStep();
    verticalScrollBar->setValue(newValue);
    emit verticalScrollBar->valueChanged(newValue);
}

void SensorList::mouseWheelDown()
{
    int newValue = verticalScrollBar->value() + verticalScrollBar->singleStep();
    verticalScrollBar->setValue(newValue);
    emit verticalScrollBar->valueChanged(newValue);
}
