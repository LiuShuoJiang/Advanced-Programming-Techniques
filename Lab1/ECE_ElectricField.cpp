/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Sept 24, 2023
 * Description: CPP file for functions to calculate electric fields
 * */

#include "ECE_ElectricField.h"

//a constructor that initializes base class member variables
ECE_ElectricField::ECE_ElectricField(double x, double y, double z, double q) : ECE_PointCharge(x, y, z, q) {}

//calculates the electric field at the point (x, y, z) due to the charge
//and updates the Ex, Ey, Ez member variables
void ECE_ElectricField::computeFieldAt(double x_coord, double y_coord, double z_coord)
{
    double distance = pow(x_coord - this->x, 2) + pow(y_coord - this->y, 2)
            + pow(z_coord - this->z, 2);
    double denominator = pow(distance, 1.5);
    this->Ex = K * (this->q) * (x_coord - this->x) * 1e-6 / denominator;
    this->Ey = K * (this->q) * (y_coord - this->y) * 1e-6 / denominator;
    this->Ez = K * (this->q) * (z_coord - this->z) * 1e-6 / denominator;
}

//get electric field value
void ECE_ElectricField::getElectricField(double &Ex_val, double &Ey_val, double &Ez_val) const
{
    Ex_val = this->Ex;
    Ey_val = this->Ey;
    Ez_val = this->Ez;
}
