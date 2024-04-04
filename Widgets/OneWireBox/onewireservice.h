#ifndef ONEWIRESERVICE_H
#define ONEWIRESERVICE_H


#include <vector>

#include <QObject>
#include <QGroupBox>
#include <QPushButton>


#define ONEWIRE_BOX_HEIGHT (51)


class OneWireService: public QObject
{
    Q_OBJECT

signals:
    void registerClicked();

private:
    const QWidget* parent;

    std::vector<QMetaObject::Connection> m_connections;

    QGroupBox*   registrate_box;
    QPushButton* registerBtn;

    bool registrating;

public:
    OneWireService(const QWidget* parent);
    ~OneWireService();

    int getY();
    void setY(int y);
    void show();
    void disable();
    void enable();
    unsigned height();

    void start();
    void stop();

    void registerBtnClick();

    bool isRegistratinig();
};

#endif // ONEWIRESERVICE_H
