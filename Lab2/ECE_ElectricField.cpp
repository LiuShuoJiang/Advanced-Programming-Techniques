/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Oct 7, 2023
 * Description: CPP file for functions to calculate electric fields
 * */

#include "ECE_ElectricField.h"

ECE_ElectricField::ECE_ElectricField(double x, double y, double z, double q) : ECE_PointCharge(x, y, z, q)
{
    Ex = 0;
    Ey = 0;
    Ez = 0;
}

void ECE_ElectricField::computeFieldAt(double x, double y, double z)
{
    double r = sqrt(pow(x - this->x, 2) + pow(y - this->y, 2) + pow(z - this->z, 2));
    Ex = K * q * (x - this->x) / (r * r * r);
    Ey = K * q * (y - this->y) / (r * r * r);
    Ez = K * q * (z - this->z) / (r * r * r);
}

void ECE_ElectricField::getElectricField(double &ExVal, double &EyVal, double &EzVal) const
{
    ExVal = this->Ex;
    EyVal = this->Ey;
    EzVal = this->Ez;
}

