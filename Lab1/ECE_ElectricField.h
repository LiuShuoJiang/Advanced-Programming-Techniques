/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Sept 24, 2023
 * Description: Header file for the class to calculate electric fields, inherited from ECE_PointCharge class
 * */

#ifndef ECE_ELECTRICFIELD_H
#define ECE_ELECTRICFIELD_H

#include "ECE_PointCharge.h"
#include <cmath>

const double K = 9.0 * 1e9;   //Coulomb's constant

class ECE_ElectricField : public ECE_PointCharge
{
public:
    ECE_ElectricField(double x, double y, double z, double q);
    void computeFieldAt(double x_coord, double y_coord, double z_coord);
    void getElectricField(double &Ex_val, double &Ey_val, double &Ez_val) const;

protected:
    double Ex;  // Electric field in the x-direction
    double Ey;  // Electric field in the y-direction
    double Ez;  // Electric field in the z-direction
};

#endif //ECE_ELECTRICFIELD_H
