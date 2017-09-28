#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include"motorthread.h"

class motor : public QObject
{
    Q_OBJECT
public:
    explicit motor(QObject *parent = nullptr);
    void prepare();
    void moveAbsolute(const double &a);
    void moveRelative(const double &a);
    void position();
    void checkMove();
    void motorQuit();
signals:
    void motorAngle(const double &s);
    void motorPrepareOk();
    void beginMove();
    void moveReady();
    void motorError();
public slots:
private slots:
    void timeout();
    void portError();
    void receive_response(const QString &s);
private:
    QString portname,Order_str,AC,DC,PR,PA,SP;
    //       ������   �������� �Ӽ��ٶ� ���/�����ƶ� ���ԭʼλ��  ���ת����ʼλ�� ��������
    motorthread thread_port;
    //    bool defaultset,direction,handle_PX,check_PX,PR_move,first_move;

    //                     Ĭ������    ת��      �ȴ���ȡλ�� �ȴ����λ��  ȷ��PR��PAת��  �жϳ���ת��
};

#endif // MOTOR_H



