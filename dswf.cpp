#include "dswf.h"

DSWF::DSWF(double elevationAngle, MatrixXd azimuthAngle, int losNum,int heightNum, MatrixXd losVelocity)
{
    vectorVelocity = MatrixXd::Zero(heightNum-1,3);

    Eigen::Matrix3d SiSum;
    SiSum << 0,0,0,0,0,0,0,0,0;
    Eigen::MatrixXd Si = MatrixXd::Zero(3,losNum);
    for(int i=0;i<losNum;i++){
        Si(0,i) = sin(elevationAngle/180.0*PI);
        Si(1,i) = cos(elevationAngle/180.0*PI)*cos(azimuthAngle(i)/180.0*PI);
        Si(2,i) = cos(elevationAngle/180.0*PI)*sin(azimuthAngle(i)/180.0*PI);
    }
    SiSum = SiSum + Si*Si.transpose();
    std::cout << "Si=\n" << Si <<endl;

    for(int m=0;m<43;m++) {
        Eigen::Vector3d SiVri(0,0,0);
        Eigen::Vector3d temp(0,0,0);
        for(int n=0;n<4;n++) {
            SiVri = SiVri + losVelocity(m,n)*Si.col(n);
            temp = SiSum.inverse()*SiVri;
            vectorVelocity.row(m) = temp;
        }
    }
}
