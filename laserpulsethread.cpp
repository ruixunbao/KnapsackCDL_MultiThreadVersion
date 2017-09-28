#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include "laserpulsethread.h"

LaserPulseThread::LaserPulseThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

LaserPulseThread::~LaserPulseThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void LaserPulseThread::transaction(const QString &portName, const QByteArray &request)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = 3000;
    this->request = request;
    if(!isRunning())
        start();
    else
        cond.wakeOne();
}

void LaserPulseThread::run()
{
    bool currentPortNameChanged = false;

    mutex.lock();
    QString currentPortName;
    if(currentPortName != portName)
    {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QByteArray currentRequest = request;
    mutex.unlock();
    QSerialPort serial;
    while(!quit)
    {
        if(currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);
            if(!serial.open(QIODevice::ReadWrite))
            {
                emit this->Pulse_PortNotOpen();						//����δ�ɹ��򿪣��źŷ��ص�������
                return;
            }
            serial.setBaudRate(QSerialPort::Baud9600);			//������
            serial.setDataBits(QSerialPort::Data8);				//����λ
            serial.setParity(QSerialPort::NoParity);			//У��λ
            serial.setStopBits(QSerialPort::OneStop);			//ֹͣλ
            serial.setFlowControl(QSerialPort::NoFlowControl);	//������
        }
        serial.write(currentRequest);
        if(serial.waitForBytesWritten(waitTimeout))
        {
            if(serial.waitForReadyRead(currentWaitTimeout))
            {
                QByteArray responseData = serial.readAll();
                while(serial.waitForReadyRead(30))
                    responseData += serial.readAll();
                QString response=responseData.toHex();
                emit this->responsePulse(response);
            }
            else
            {
                emit this->timeoutPulse();
                return;
            }
        }
        else
        {
            emit this->timeoutPulse();
            return;
        }

        mutex.lock();
        cond.wait(&mutex);
        if(currentPortName != portName)
        {
            currentPortName = portName;
            currentPortNameChanged = true;
        }
        else
            currentPortNameChanged = false;

        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
