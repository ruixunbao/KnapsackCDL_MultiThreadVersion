#ifndef LASERSEED_H
#define LASERSEED_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include "laserseedthread.h"
#include "laserpulse.h"

class LaserSeed : public QObject
{
    Q_OBJECT
public:
    explicit LaserSeed(QObject *parent = nullptr);
    void beginSeedLaser();
    void setSeedPower(const int &s);
    void closeSeedLaser();
    void checkLaser();
private slots:
    void receive_response(const QString &temp);
    void portError();
    void timeout();


signals:
    void laserSeedError(QString &s);
    void seedOpenReady();
    void laserColseRight();
private:
    QByteArray senddata;
    QString laserPort,errorCode;
    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
    bool powerSet,fire,close,openPulse;
    LaserSeedThread Laserseedthread;
};

#endif // LASERSEED_H
