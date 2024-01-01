/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Oct 7, 2023
 * Description: The main CPP file to calculate electric fields using multithreading
 * */

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iomanip>
#include <omp.h>
#include "ECE_ElectricField.h"

using namespace std;

/**
 * @brief Checks if the input string is a positive integer.
 *
 * This function checks if the input string is a positive integer.
 *
 * @param input the input string
 * @return true if the input string is a positive integer, false otherwise
 */
bool isPositiveInteger(const string &input)
{
    regex positiveInteger("^[1-9]\\d*$");
    return regex_match(input, positiveInteger);
}

/**
 * @brief Checks if the input string is a double number.
 *
 * This function checks if the input string is a double.
 *
 * @param input the input string
 * @return true if the input string is a double, false otherwise
 */
bool isDouble(const string &input)
{
    regex doubleNumber("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
    return regex_match(input, doubleNumber);
}

/**
 * @brief Checks if the input string is valid.
 *
 * Split the string input with the given delimiter, check if the input is valid,
 * store the result in vector results, return true if valid
 *
 * @param input the input string
 * @param output the vector to store the results
 * @param delimiter the delimiter to split the string, such as ' '
 * @param length the length of the vector to store the results
 * @param checkForWhat true if checking for double, false if checking for positive integer
 * @return true if the input string is a valid coordinate, false otherwise
 */
bool splitString(const string &input, vector<string> &output, char delimiter, size_t length, bool checkForWhat)
{
    bool result = true;
    if (input.empty())
    {
        result = false;
    }
    else
    {
        string token;
        istringstream tokenStream(input);
        while (getline(tokenStream, token, delimiter))
        {
            if (checkForWhat)
            {
                if (!isDouble(token))
                {
                    result = false;
                    break;
                }
            }
            else
            {
                if (!isPositiveInteger(token))
                {
                    result = false;
                    break;
                }
            }
            output.push_back(token);
        }
    }
    if (output.size() != length)
    {
        result = false;
    }
    return result;
}

/**
 * @brief check if the current point overlaps with the electric grids
 *
 * This function checks if the point overlaps with the field grid
 *
 * @param xDist the x distance between two adjacent points
 * @param yDist the y distance between two adjacent points
 * @param xTarget the x coordinate of the target point
 * @param yTarget the y coordinate of the target point
 * @param zTarget the z coordinate of the target point
 * @param N the number of rows in the field grid
 * @param M the number of columns in the field grid
 * */
bool checkOverlap(const double &xDist, const double &yDist, const double &xTarget, const double &yTarget,
                  const double &zTarget, const int &N, const int &M)
{
    for (int i = 0; i < N; i++)
    {
        double x = xDist * (i - (N - 1) / 2.0);
        for (int j = 0; j < M; j++)
        {
            double y = yDist * (j - (M - 1) / 2.0);
            if (x == xTarget && y == yTarget && zTarget == 0)
            {
                return true;
            }
        }
    }
    return false;
}

int main()
{
    //query the user for how many threads to run concurrently when doing the calculation
    cout << "Please enter the number of concurrent threads to use: ";
    string numThreads;
    getline(cin, numThreads);
    while (!isPositiveInteger(numThreads))
    {
        cout << "Invalid input. Please enter a positive integer: ";
        getline(cin, numThreads);
    }
    int numThreadsInt = stoi(numThreads);

    // Prompt the user for the size of the array and make sure it is valid
    cout << "Please enter the number of rows and columns in the N x M array: ";
    string inputNM;
    vector<string> inputNMVector;
    getline(cin, inputNM);
    while (!splitString(inputNM, inputNMVector, ' ', 2, false))
    {
        cout << "Invalid input. Please enter two positive integers separated by a space: ";
        getline(cin, inputNM);
        inputNMVector.clear();
    }
    int N = stoi(inputNMVector[0]);
    int M = stoi(inputNMVector[1]);

    // Prompt the user for the separation distances and make sure it is valid
    // the x and y value must also be positive doubles
    cout << "Please enter the x and y separation distances in meters: ";
    string inputXY;
    bool isPositive;  // check if x and y are positive doubles
    vector<string> inputXYVector;
    getline(cin, inputXY);
    do
    {
        isPositive = true;
        while (!splitString(inputXY, inputXYVector, ' ', 2, true))
        {
            cout << "Invalid input. Please enter two positive doubles separated by a space: ";
            getline(cin, inputXY);
            inputXYVector.clear();
        }
        for (const auto &i: inputXYVector)
        {
            if (stod(i) <= 0)
            {
                isPositive = false;
                break;
            }
        }
        if (!isPositive)
        {
            cout << "Invalid input. Please enter two positive doubles separated by a space: ";
            getline(cin, inputXY);
            inputXYVector.clear();
        }
    } while (!isPositive);
    double xDistance = stod(inputXYVector[0]) * 1.0;
    double yDistance = stod(inputXYVector[1]) * 1.0;

    // Prompt the user for the electric charge and make sure it is valid
    cout << "Please enter the common charge_user on the points in micro C: ";
    string inputQ;
    getline(cin, inputQ);
    while (!isDouble(inputQ))
    {
        cout << "Invalid input. Please enter a double: ";
        getline(cin, inputQ);
    }
    double q = stod(inputQ) * 1.0 * 1e-6;

    bool finish = false;
    while (!finish)
    {
        // Prompt the user for the target coordinates of the point and make sure it is valid
        // Also check if the point overlaps with the electric grids
        string inputTarget;
        vector<string> inputTargetVector;
        bool isOverlap;
        double x, y, z;
        cout << "Please enter the location in space to determine the electric field x y z in meters: ";
        getline(cin, inputTarget);
        do
        {
            isOverlap = false;
            while (!splitString(inputTarget, inputTargetVector, ' ', 3, true))
            {
                cout << "Invalid input. Please enter three doubles separated by a space: ";
                getline(cin, inputTarget);
                inputTargetVector.clear();
            }
            x = stod(inputTargetVector[0]);
            y = stod(inputTargetVector[1]);
            z = stod(inputTargetVector[2]);
            if (checkOverlap(xDistance, yDistance, x, y, z, N, M))
            {
                isOverlap = true;
                cout << "The point overlaps with the electric grids. Please enter another point: ";
                getline(cin, inputTarget);
                inputTargetVector.clear();
            }
        } while (isOverlap);

        // Create instances of ECE_ElectricField
        vector<ECE_ElectricField> electricFields;
        for (int i = 0; i < N; i++)
        {
            double xC = xDistance * (i - (N - 1) / 2.0);
            for (int j = 0; j < M; j++)
            {
                double yC = yDistance * (j - (M - 1) / 2.0);
                electricFields.emplace_back(xC, yC, 0, q);
            }
        }

        // Calculate overall electric field at the target point using OpenMP multithreading
        double Ex = 0;
        double Ey = 0;
        double Ez = 0;
        //legacy code (not efficient)
//        double ExVal, EyVal, EzVal;
//#pragma omp parallel for reduction(+:Ex, Ey, Ez) num_threads(numThreadsInt)
//        for (int i = 0; i < N * M; i++) {
//            electricFields[i].computeFieldAt(x, y, z);
//            electricFields[i].getElectricField(ExVal, EyVal, EzVal);
//            Ex += ExVal;
//            Ey += EyVal;
//            Ez += EzVal;
//        }

        double start = omp_get_wtime();
#pragma omp parallel reduction(+:Ex, Ey, Ez) num_threads(numThreadsInt)
        {
            double localEx = 0, localEy = 0, localEz = 0;
            double ExVal, EyVal, EzVal;

#pragma omp for
            for (int i = 0; i < N * M; i++)
            {
                electricFields[i].computeFieldAt(x, y, z);
                electricFields[i].getElectricField(ExVal, EyVal, EzVal);
                localEx += ExVal;
                localEy += EyVal;
                localEz += EzVal;
            }

            Ex += localEx;
            Ey += localEy;
            Ez += localEz;
        }
        double end = omp_get_wtime();

        cout << "The electric field at (" << x << ", " << y << ", " << z << ") in V/m is: " << endl;

        int signEx = (Ex < 0) ? -1 : 1;
        double absEx = abs(Ex);
        int exponentEx = static_cast<int>(floor(log10(absEx)));
        double mantissaEx = absEx / pow(10.0, exponentEx);

        int signEy = (Ey < 0) ? -1 : 1;
        double absEy = abs(Ey);
        int exponentEy = static_cast<int>(floor(log10(absEy)));
        double mantissaEy = absEy / pow(10.0, exponentEy);

        int signEz = (Ez < 0) ? -1 : 1;
        double absEz = abs(Ez);
        int exponentEz = static_cast<int>(floor(log10(absEz)));
        double mantissaEz = absEz / pow(10.0, exponentEz);

        double absE = sqrt(pow(Ex, 2) + pow(Ey, 2) + pow(Ez, 2));
        int exponentE = static_cast<int>(floor(log10(absE)));
        double mantissaE = absE / pow(10.0, exponentE);

        cout << "Ex = " << (signEx < 0 ? "-" : "")
             << fixed << setprecision(4) << mantissaEx << " * 10^" << exponentEx << endl;
        cout << "Ey = " << (signEy < 0 ? "-" : "")
             << fixed << setprecision(4) << mantissaEy << " * 10^" << exponentEy << endl;
        cout << "Ez = " << (signEz < 0 ? "-" : "")
             << fixed << setprecision(4) << mantissaEz << " * 10^" << exponentEz << endl;
        cout << "|E| = " << fixed << setprecision(4) << mantissaE << " * 10^" << exponentE << endl;

        cout << "The calculation took " << setprecision(4) << (end - start) * 1000000 << " microsec!" << endl;

        // Ask the user if they want to continue
        cout << "Do you want to enter a new location (Y/N)? ";
        string inputContinue;
        getline(cin, inputContinue);
        while (inputContinue != "Y" && inputContinue != "N" && inputContinue != "y" && inputContinue != "n")
        {
            cout << "Invalid input. Please enter Y or N: ";
            getline(cin, inputContinue);
        }
        if (inputContinue == "N" || inputContinue == "n")
        {
            finish = true;
            cout << "Bye!" << endl;
        }
    }

    return 0;
}
