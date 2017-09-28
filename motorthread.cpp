#include "motorthread.h"
#include <QtSerialPort/QSerialPort>
#include <QDebug>

motorthread::motorthread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

motorthread::~motorthread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void motorthread::transaction(const QString &portName, const QString &request)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = 300;
    this->request = request;
    qDebug()<<"request="<<request;
    if(!isRunning())
        start();
    else
        cond.wakeOne();
}

void motorthread::run()
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
    QString currentRequest = request;
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
                emit this->S_PortNotOpen();						//����δ�ɹ��򿪣��źŷ��ص�������
                return;
            }
            serial.setBaudRate(QSerialPort::Baud19200);			//������
            serial.setDataBits(QSerialPort::Data8);				//����λ
            serial.setParity(QSerialPort::NoParity);			//У��λ
            serial.setStopBits(QSerialPort::OneStop);			//ֹͣλ
            serial.setFlowControl(QSerialPort::NoFlowControl);	//������
        }
        QByteArray requestData = currentRequest.toLocal8Bit();
        serial.write(requestData);
        if(serial.waitForBytesWritten(waitTimeout))
        {
            if(serial.waitForReadyRead(currentWaitTimeout))
            {
                QByteArray responseData = serial.readAll();
                while(serial.waitForReadyRead(80))
                    responseData += serial.readAll();
                qDebug()<<"response="<<responseData;
                QString response(responseData);
                emit this->response2(response);
            }
            else
            {
                emit this->timeout2();
                return;
            }
        }
        else
        {
            emit this->timeout2();
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

