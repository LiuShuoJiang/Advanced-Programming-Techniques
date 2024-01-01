/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Sept 24, 2023
 * Description: CPP file for the basic electric point charge
 * */

#include "ECE_PointCharge.h"

//constructor
ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q) : x(x), y(y), z(z), q(q) {}

//set x, y, z values
void ECE_PointCharge::setLocation(double x_coord, double y_coord, double z_coord)
{
    this->x = x_coord;
    this->y = y_coord;
    this->z = z_coord;
}

//set q values
void ECE_PointCharge::setCharge(double q_value)
{
    ECE_PointCharge::q = q_value;
}
