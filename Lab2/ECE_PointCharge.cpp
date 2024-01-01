/*
* Author: Shuojiang Liu
* Class: ECE 6122
* Last Date Modified: Oct 7, 2023
* Description: Header file for the class to calculate electric fields, inherited from ECE_PointCharge class
* */

#include "ECE_PointCharge.h"

ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q) : x(x), y(y), z(z), q(q) {}

void ECE_PointCharge::setLocation(double xCoord, double yCoord, double zCoord)
{
    this->x = xCoord;
    this->y = yCoord;
    this->z = zCoord;
}

void ECE_PointCharge::setCharge(double qValue)
{
    this->q = qValue;
}