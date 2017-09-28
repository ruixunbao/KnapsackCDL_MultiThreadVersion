#ifndef COMPASS_H
#define COMPASS_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include "compassthread.h"

class Compass : public QObject
{
    Q_OBJECT

public:
    explicit Compass(QObject *parent = nullptr);
    void read();

signals:
    void compassAngleReady(const double &s);

public slots:

private slots:
    void showResponse(const QByteArray &s);
    void processError(const QString &s);


private:
    void stringToHex(QString str, QByteArray &sendData);   //转换为16进制
    char charToHex(char ch);
    QByteArray sendData;                                   //需要发送的字节数组
    double stringToAngle(const QString &c);                     //将接收的数据转为角度信息
    CompassThread compassThread;
};

#endif // COMPASS_H
