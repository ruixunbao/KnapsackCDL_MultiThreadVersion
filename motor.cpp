#include "motor.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

motor::motor(QObject *parent) : QObject(parent)
{
    connect(&thread_port,SIGNAL(response2(QString)),this,SLOT(receive_response(QString)));
    connect(&thread_port,SIGNAL(S_PortNotOpen()),this,SLOT(portError()));
    connect(&thread_port,SIGNAL(timeout2()),this,SLOT(timeout()));
}

void motor::prepare()
{
    portname = "COM8";
    Order_str = "VR;";
    thread_port.transaction(portname,Order_str);   //��ȡ�汾�������ϵ�--���üӼ��ٶ�--�����ٶ�--�����ϵ�����ź�

}

void motor::moveAbsolute(const double &a)
{   QString anglePA = QString::number(a*524000/360,'f',2);
    qDebug()<<"a="<<anglePA;
    Order_str = "PA="+anglePA+";";
    thread_port.transaction(portname,Order_str);
}

void motor::moveRelative(const double &a)
{
    QString anglePR=QString::number(a*524000/360,'f',2);
    Order_str = "PR="+anglePR+";";
    thread_port.transaction(portname,Order_str);
}

void motor::position()
{
    Order_str = "PX;";
    thread_port.transaction(portname,Order_str);
}

void motor::checkMove()
{
    Order_str = "MS;";
    thread_port.transaction(portname,Order_str);
}

void motor::portError()
{
    qDebug()<<"motor no open";
}

void motor::timeout()
{
    qDebug()<<"motor timeout";
}

void motor::motorQuit()
{
    Order_str = "MO=0;";
    thread_port.transaction(portname,Order_str);
}

void motor::receive_response(const QString &s)
{
    if(s.left(2) == "MO")                       //����Ƿ�ر�
    {
        if(s.left(4) == "MO=1")
        {
            Order_str = "AC=1800;";
            thread_port.transaction(portname,Order_str);
            qDebug()<<"�����";
        }
        else
            qDebug()<<"����ر�";
    }
    if(s.left(2) == "AC")
    {
        Order_str = "DC=1800;";
        thread_port.transaction(portname,Order_str);
    }
    if(s.left(2) == "DC")
    {
        Order_str = "SP=1310000;";
        thread_port.transaction(portname,Order_str);
    }
    if(s.left(2) == "MS")
    {
        if(s.left(4)=="MS;0")
        {
            emit this->moveReady();
            qDebug()<<"moveready";
        }
        else
        {
            if(s.left(4)=="MS;3")
            {
                emit this->motorError();
                qDebug()<<"��������쳣";
            }
            else
            {qDebug()<<"moveon";}
        }
    }
    if(s.left(2) == "PA")
    {
        Order_str = "BG;";
        thread_port.transaction(portname,Order_str);
        emit this->beginMove();
    }
    if(s.left(2) == "PR")
    {
        Order_str = "BG;";
        thread_port.transaction(portname,Order_str);
        emit this->beginMove();
    }
    if(s.left(2) == "PX")
    {
        QString a=s.split(";").at(1).toLocal8Bit().data();
        double angle=a.toInt()*360/524000;
        qDebug()<<"PX"<<angle;

        if(angle<0)
        {
            while(angle<0)
                angle = angle + 360;
        }
        while(angle>360)
            angle = angle - 360;
        emit this->motorAngle(angle);

    }
    if(s.left(2) == "SP")
    {
        emit this->motorPrepareOk();
    }
    if(s.left(10) == "VR;Whistle")
    {
        Order_str = ";";
        thread_port.transaction(portname,Order_str);
    }
    if(s.left(1) == ";")
    {
        Order_str = "MO=1;";
        thread_port.transaction(portname,Order_str);
    }
}
