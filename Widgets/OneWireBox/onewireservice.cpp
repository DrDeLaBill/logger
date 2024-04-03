#include "onewireservice.h"


#define MARGIN_STR "5"


OneWireService::OneWireService(const QWidget* parent): parent(parent)
{
    registrate_box = new QGroupBox(const_cast<QWidget*>(parent));
    registrate_box->setObjectName("registrate_box");
    registrate_box->setGeometry(QRect(0, 0, 351, ONEWIRE_BOX_HEIGHT));
    registrate_box->setStyleSheet(
        QString::fromUtf8(
            "QGroupBox {\n"
            "	margin: " MARGIN_STR "px;\n"
            "	border: 0px;\n"
            "}\n"
            "\n"
            "QLabel {\n"
            "	border: 0px;\n"
            "}"
        )
    );

    QFont font3;

    registerBtn = new QPushButton(registrate_box);
    registerBtn->setObjectName("register_btn");
    registerBtn->setGeometry(QRect(10, 10, 330, 31));
    registerBtn->setFont(font3);
    stop();

    m_connections.push_back(
        QObject::connect(registerBtn, &registerBtn->clicked, this, registerClicked)
    );
}

OneWireService::~OneWireService()
{
    for (auto& connection : m_connections) {
        QObject::disconnect(connection);
    }
    registerBtn->deleteLater();
    registrate_box->deleteLater();
}

int OneWireService::getY()
{
    return registrate_box->geometry().y();
}

void OneWireService::setY(int y)
{
    const QRect& rect = registrate_box->geometry();
    registrate_box->setGeometry(QRect(rect.x(), y, rect.width(), rect.height()));
}

void OneWireService::show()
{
    registrate_box->show();
}

void OneWireService::disable()
{
    registrate_box->setDisabled(true);
}

void OneWireService::enable()
{
    registrate_box->setDisabled(false);
}

unsigned OneWireService::height()
{
    return registrate_box->geometry().height();
}

void OneWireService::start()
{
    registerBtn->setText("Stop registrating");
}

void OneWireService::stop()
{
    registerBtn->setText("Register 1WIRE sensors");
}
