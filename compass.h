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
    void stringToHex(QString str, QByteArray &sendData);   //ת��Ϊ16����
    char charToHex(char ch);
    QByteArray sendData;                                   //��Ҫ���͵��ֽ�����
    double stringToAngle(const QString &c);                     //�����յ�����תΪ�Ƕ���Ϣ
    CompassThread compassThread;
};

#endif // COMPASS_H
