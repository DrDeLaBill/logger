#ifndef SENSORBOX_H
#define SENSORBOX_H


#include <vector>
#include <string>

#include <QLabel>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>

#include "sensordata.h"


#define SENSOR_BOX_HEIGHT (51)


class SensorBox: public QObject
{
    Q_OBJECT

private slots:
    void onIdChanged();
    void onIdRegChanged();
    void onValueRegChanged();
    void onButtonClicked();

signals:
    void save(const SensorData& sensorData);

private:
    std::vector<QMetaObject::Connection> m_connections;

    QWidget* parent;
    SensorData data;

    QGroupBox *sensor_box;
    QPushButton *sensor_add_btn;
    QLabel *sensor_value_label;
    QTextEdit *sensor_id_reg_text_edit;
    QTextEdit *sensor_value_reg_text_edit;
    QTextEdit *sensor_id_text_edit;

    void retranslateUi();

    void init();
    void destroy();

    void textEditProccess(QTextEdit* edit, unsigned& lastVal);

public:
    SensorBox(const QWidget* parent, const SensorData& data);

    SensorBox(const SensorBox& other);
    SensorBox& operator=(const SensorBox& other);

    ~SensorBox();

    int getY();
    void setY(int y);

    void show();

};

#endif // SENSORBOX_H
