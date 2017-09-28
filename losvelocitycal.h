#ifndef LOSVELOCITYCAL_H
#define LOSVELOCITYCAL_H


class LOSVelocityCal
{
public:
    LOSVelocityCal(const int heightNum, const int totalSpecPoints,
                   const int objSpecPoints, const double lambda,
                   const double *freqAxis, const double *specData);
    double *getLOSVelocity();
private:
    double *losVelocity;
};

#endif // LOSVELOCITYCAL_H
