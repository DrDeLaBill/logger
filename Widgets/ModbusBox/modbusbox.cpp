#include "modbusbox.h"

#include <QCoreApplication>


#define MARGIN_STR "5"
#define MARGIN_INT (5)


ModbusBox::ModbusBox(const ModbusBox &other): ModbusBox(other.parent, other.data) {}

ModbusBox& ModbusBox::operator=(const ModbusBox& other)
{
    destroy();
    parent = other.parent;
    data   = other.data;
    init();
    return *this;
}

ModbusBox::ModbusBox(const QWidget* parent, const ModbusData& data): parent(const_cast<QWidget*>(parent)), data(data)
{
    init();
}

ModbusBox::~ModbusBox()
{
    for (auto& connection : m_connections) {
        QObject::disconnect(connection);
    }
    destroy();
}

void ModbusBox::init()
{
    this->sensor_box = new QGroupBox(parent);
    this->sensor_box->setObjectName(("sensor_box_" + std::to_string(data.sensorID)).c_str());
    this->sensor_box->setGeometry(QRect(0, 0, 351, MODBUS_BOX_HEIGHT));
    this->sensor_box->setGeometry(QRect(0, getY(), 351, MODBUS_BOX_HEIGHT));
    this->sensor_box->setStyleSheet(
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

    QFont font3;

    sensor_value_label = new QLabel(this->sensor_box);
    sensor_value_label->setObjectName("sensor_value_label");
    sensor_value_label->setGeometry(QRect(230, 10, 51, 31));
    sensor_value_label->setText(std::to_string(data.sensorID).c_str());

    QFont font4;
    font4.setPointSize(12);

    sensor_value_label->setFont(font4);

    sensor_id_text_edit = new QTextEdit(this->sensor_box);
    sensor_id_text_edit->setObjectName("sensor_id_edit");
    sensor_id_text_edit->setGeometry(QRect(10, 10, 51, 31));
    sensor_id_text_edit->setFont(font3);
    sensor_id_text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sensor_id_text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sensor_id_text_edit->setTabChangesFocus(true);

    sensor_id_reg_text_edit = new QTextEdit(this->sensor_box);
    sensor_id_reg_text_edit->setObjectName("sensorid_id_reg_text_edit");
    sensor_id_reg_text_edit->setGeometry(QRect(70, 10, 61, 31));
    sensor_id_reg_text_edit->setFont(font3);
    sensor_id_reg_text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sensor_id_reg_text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sensor_id_reg_text_edit->setTabChangesFocus(true);

    sensor_value_reg_text_edit = new QTextEdit(this->sensor_box);
    sensor_value_reg_text_edit->setObjectName("sensor_value_reg_text_edit");
    sensor_value_reg_text_edit->setGeometry(QRect(140, 10, 81, 31));
    sensor_value_reg_text_edit->setFont(font3);
    sensor_value_reg_text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sensor_value_reg_text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sensor_value_reg_text_edit->setTabChangesFocus(true);

    sensor_remove_btn = new QPushButton(this->sensor_box);
    sensor_remove_btn->setObjectName(("sensor_remove_btn_" + std::to_string(data.sensorID)).c_str());
    sensor_remove_btn->setGeometry(QRect(270, 10, 31, 31));
    sensor_remove_btn->setFont(font3);
    sensor_remove_btn->setText("D");

    sensor_add_btn = new QPushButton(this->sensor_box);
    sensor_add_btn->setObjectName(("sensor_add_btn_" + std::to_string(data.sensorID)).c_str());
    sensor_add_btn->setGeometry(QRect(311, 10, 31, 31));
    sensor_add_btn->setFont(font3);

    this->retranslateUi();

    m_connections.push_back(
        QObject::connect(sensor_id_text_edit, &sensor_id_text_edit->textChanged, this, onIdChanged)
    );
    m_connections.push_back(
        QObject::connect(sensor_id_reg_text_edit, &sensor_id_reg_text_edit->textChanged, this, onIdRegChanged)
    );
    m_connections.push_back(
        QObject::connect(sensor_value_reg_text_edit, &sensor_value_reg_text_edit->textChanged, this, onValueRegChanged)
    );
    m_connections.push_back(
        QObject::connect(sensor_add_btn, &sensor_add_btn->clicked, this, onAddButtonClicked)
    );
    m_connections.push_back(
        QObject::connect(sensor_remove_btn, &sensor_remove_btn->clicked, this, onRemoveButtonClicked)
    );
}

void ModbusBox::onAddButtonClicked()
{
    emit save(data);
    // TODO: data.lastID = data.sensorID;
}

void ModbusBox::onRemoveButtonClicked()
{
    data.sensorID = 0;
    data.idReg = 0;
    data.valueReg = 0;

    emit save(data);
    // TODO: data.lastID = data.sensorID;
}

void ModbusBox::textEditProccess(QTextEdit* edit, unsigned& target)
{
    if (!edit) {
        return;
    }
    const QTextCursor cursor = edit->textCursor();
    const std::string stdString = edit->toPlainText().toStdString();
    std::string::const_iterator it = stdString.begin();
    while (it != stdString.end() && std::isdigit(*it)) ++it;
    if (it  == stdString.end()) {
        target = edit->toPlainText().toUInt();
    }
    edit->blockSignals(true);
    edit->setText(std::to_string(target).c_str());
    edit->blockSignals(false);
    edit->setTextCursor(cursor);
}

void ModbusBox::onIdChanged()
{
    QTextEdit* edit = sensor_box->findChild<QTextEdit*>("sensor_id_edit");
    textEditProccess(edit, data.sensorID);
}

void ModbusBox::onIdRegChanged()
{
    QTextEdit* edit = sensor_box->findChild<QTextEdit*>("sensorid_id_reg_text_edit");
    textEditProccess(edit, data.idReg);
}

void ModbusBox::onValueRegChanged()
{
    QTextEdit* edit = sensor_box->findChild<QTextEdit*>("sensor_value_reg_text_edit");
    textEditProccess(edit, data.valueReg);
}

void ModbusBox::destroy()
{
    if (sensor_add_btn) {
        sensor_add_btn->deleteLater();
    }

    if (sensor_value_label) {
        sensor_value_label->deleteLater();
    }

    if (sensor_id_reg_text_edit) {
        sensor_id_reg_text_edit->deleteLater();
    }

    if (sensor_value_reg_text_edit) {
        sensor_value_reg_text_edit->deleteLater();
    }

    if (sensor_id_text_edit) {
        sensor_id_text_edit->deleteLater();
    }

    if (sensor_box) {
        sensor_box->deleteLater();
    }
}

void ModbusBox::retranslateUi()
{
    sensor_box->setTitle(QString());
    sensor_add_btn->setText(QCoreApplication::translate("MainWindow", data.buttonLabel.c_str(), nullptr));
    sensor_value_label->setText(QCoreApplication::translate("MainWindow", std::to_string(data.value).c_str(), nullptr));
    sensor_id_reg_text_edit->setHtml(
        QCoreApplication::translate(
            "MainWindow",
            std::string("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
            "<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
            "p, li { white-space: pre-wrap; }\n"
            "hr { height: 1px; border-width: 0; }\n"
            "li.unchecked::marker { content: \"\\2610\"; }\n"
            "li.checked::marker { content: \"\\2612\"; }\n"
            "</style></head><body style=\" font-family:'Segoe UI'; font-size:10px; font-weight:400; font-style:normal;\">\n"
            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10px;\">"
            + std::to_string(data.idReg) +
            "</span></p></body></html>").c_str(),
            nullptr
        )
    );
    sensor_value_reg_text_edit->setHtml(
        QCoreApplication::translate(
            "MainWindow",
            std::string("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
            "<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
            "p, li { white-space: pre-wrap; }\n"
            "hr { height: 1px; border-width: 0; }\n"
            "li.unchecked::marker { content: \"\\2610\"; }\n"
            "li.checked::marker { content: \"\\2612\"; }\n"
            "</style></head><body style=\" font-family:'Segoe UI'; font-size:10px; font-weight:400; font-style:normal;\">\n"
            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10px;\">"
            + std::to_string(data.valueReg) +
            "</span></p>\n"
            "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>").c_str(),
            nullptr
        )
    );
    sensor_id_text_edit->setHtml(
        QCoreApplication::translate(
            "MainWindow",
            std::string("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
            "<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
            "p, li { white-space: pre-wrap; }\n"
            "hr { height: 1px; border-width: 0; }\n"
            "li.unchecked::marker { content: \"\\2610\"; }\n"
            "li.checked::marker { content: \"\\2612\"; }\n"
            "</style></head><body style=\" font-family:'Segoe UI'; font-size:10px; font-weight:400; font-style:normal;\">\n"
            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10px;\">"
            + std::to_string(data.sensorID) +
            "</span></p>\n"
            "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>").c_str(),
            nullptr
        )
    );
}

int ModbusBox::getY()
{
    return MODBUS_BOX_HEIGHT * data.number;
}

void ModbusBox::setY(int y)
{
    const QRect& rect = sensor_box->geometry();
    this->sensor_box->setGeometry(QRect(rect.x(), y, rect.width(), rect.height()));
}

uint16_t ModbusBox::getID()
{
    return data.sensorID;
}

void ModbusBox::setValue(const QString& value)
{
    this->sensor_value_label->setText(value);
}

void ModbusBox::show()
{
    sensor_box->show();
}

void ModbusBox::clear()
{
    sensor_id_text_edit->setText(std::to_string(data.lastID).c_str());
    sensor_id_reg_text_edit->setText("0");
    sensor_value_reg_text_edit->setText("0");
}

void ModbusBox::disable()
{
    sensor_id_text_edit->setDisabled(true);
    sensor_id_reg_text_edit->setDisabled(true);
    sensor_value_reg_text_edit->setDisabled(true);
    sensor_add_btn->setDisabled(true);
    sensor_remove_btn->setDisabled(true);
    sensor_value_label->setDisabled(true);
}

void ModbusBox::enable()
{
    sensor_id_text_edit->setDisabled(false);
    sensor_id_reg_text_edit->setDisabled(false);
    sensor_value_reg_text_edit->setDisabled(false);
    sensor_add_btn->setDisabled(false);
    sensor_remove_btn->setDisabled(false);
    sensor_value_label->setDisabled(false);
}

unsigned ModbusBox::height()
{
    return sensor_box->height();
}
