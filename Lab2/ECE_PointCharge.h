/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Oct 7, 2023
 * Description: Header file for the electric point charge
 * */

#ifndef LAB2_ECE_POINTCHARGE_H
#define LAB2_ECE_POINTCHARGE_H

/**
 * @brief A class to represent a point charge.
 *
 * This class is used to represent a point charge in 3D space.
 * It contains the charge value and the position of the charge.
 */
class ECE_PointCharge
{
protected:
    double x; // x coordinate of the charge
    double y; // y coordinate of the charge
    double z; // z coordinate of the charge
    double q; // charge value
public:
    /**
     * @brief Constructor for ECE_PointCharge.
     *
     * This constructor takes in the x, y, z, and q values and
     * initializes the corresponding member variables.
     *
     * @param x x coordinate of the charge
     * @param y y coordinate of the charge
     * @param z z coordinate of the charge
     * @param q charge value
     */
    ECE_PointCharge(double x, double y, double z, double q);

    /**
     * @brief Setter for x, y, z coordinate.
     *
     * This function sets the x, y, z of the charge.
     *
     * @param xCoord x coordinate of the charge
     * @param yCoord y coordinate of the charge
     * @param zCoord z coordinate of the charge
     */
    void setLocation(double xCoord, double yCoord, double zCoord);

    /**
     * @brief Setter for charge value.
     *
     * This function sets the charge value of the charge.
     *
     * @param qValue charge value
     */
    void setCharge(double qValue);
};


#endif //LAB2_ECE_POINTCHARGE_H
