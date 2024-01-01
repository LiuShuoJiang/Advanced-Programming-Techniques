/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Sept 24, 2023
 * Description: Header file for the electric point charge
 * */

#ifndef ECE_POINTCHARGE_H
#define ECE_POINTCHARGE_H

class ECE_PointCharge
{
public:
    ECE_PointCharge(double x, double y, double z, double q);
    void setLocation(double x_coord, double y_coord, double z_coord);
    void setCharge(double q_value);

protected:
    double x;  //x coordinate
    double y;  //y coordinate
    double z;  //z coordinate
    double q;  //charge of point
};

#endif //ECE_POINTCHARGE_H
