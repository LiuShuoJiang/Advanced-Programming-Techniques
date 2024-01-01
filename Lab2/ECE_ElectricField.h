/*
* Author: Shuojiang Liu
* Class: ECE 6122
* Last Date Modified: Oct 7, 2023
* Description: Header file for the class to calculate electric fields, inherited from ECE_PointCharge class
* */

#ifndef LAB2_ECE_ELECTRICFIELD_H
#define LAB2_ECE_ELECTRICFIELD_H

#include "ECE_PointCharge.h"
#include <cmath>

const double K = 9.0 * 1e9;  // Coulomb's constant

/**
 * @brief A class to represent an electric field.
 *
 * This class represents an electric field in 3D space.
 * It is derived from the ECE_PointCharge class.
 */
class ECE_ElectricField : public ECE_PointCharge
{
protected:
    double Ex;  // electric field in the x direction
    double Ey;  // electric field in the y direction
    double Ez;  // electric field in the z direction
public:
    /**
     * @brief Constructor for ECE_ElectricField.
     *
     * This constructor takes in the x, y, z, and q values and
     * initializes the corresponding member variables.
     *
     * @param x x coordinate of the charge
     * @param y y coordinate of the charge
     * @param z z coordinate of the charge
     * @param q charge value
     */
    ECE_ElectricField(double x, double y, double z, double q);

    /**
     * @brief electric field computation function.
     *
     * Calculates the electric field at the point (x, y, z) due to the charge using the formula.
     * Updates the Ex, Ey, Ez member variables.
     *
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @param z z coordinate of the point
     */
    void computeFieldAt(double x, double y, double z);

    /**
     * @brief Getter for electric field.
     *
     * This function returns the electric field at the point (x, y, z).
     *
     * @param Ex electric field in the x direction
     * @param Ey electric field in the y direction
     * @param Ez electric field in the z direction
     */
    void getElectricField(double &ExVal, double &EyVal, double &EzVal) const;
};


#endif //LAB2_ECE_ELECTRICFIELD_H
