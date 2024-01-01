/*
 * Author: Shuojiang Liu
 * Class: ECE 6122
 * Last Date Modified: Sept 24, 2023
 * Description: The main CPP file to calculate electric fields
 * */

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>
#include <regex>
#include <mutex>
#include <vector>
#include "ECE_ElectricField.h"
using namespace std;


/* check if the string is valid or not
 * */
bool is_digit(const string &s, bool positive = false)
{
    if (positive)
    {
        // Matches only strictly positive numbers like "123", "0.123", ".123", "123.", "1e9", "1.23e4", "1.23e-4"
        regex r("^[+]?([0-9]+[.]?[0-9]*|[.][0-9]+)([eE][+-]?[0-9]+)?$");
        return regex_match(s, r);
    }
    else
    {
        // Matches strings including the negatives
        regex r("^[+-]?([0-9]+[.]?[0-9]*|[.][0-9]+)([eE][+-]?[0-9]+)?$");
        return regex_match(s, r);
    }
}

/* check if the string is a natural number
 * */
bool is_natural(const string &s)
{
    regex r("^[1-9][0-9]*$");
    return regex_match(s, r);
}

/* Split the string s with the given delimiter, check if the input is valid,
* store the result in vector results, return true if valid
* */
bool split(const string &s, char delimiter, vector<string> &results, int length, bool positive, bool natural)
{
    bool bRC = true;
    if (s.empty())
    {
        bRC = false;
    }
    else
    {
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter))
        {
            results.push_back(token);
        }
    }

    if (results.size() != length)
    {
        bRC = false;
    }
    else
    {
        // Additional check for each token
        for (const auto &res : results)
        {
            if (natural)
            {
                if (!is_natural(res))
                {
                    bRC = false;
                    break;
                }
            }
            else if (!is_digit(res, positive))
            {
                bRC = false;
                break;
            }
        }
    }

    if (!bRC)
    {
        cout << "Input is invalid! Enter again please." << endl;
    }

    return bRC;
}

/*check if the current point overlaps with the grids
* */
bool check_overlap(const double &x_dist, const double &y_dist,
                   const double &x_coord, const double &y_coord, const double &z_coord,
                   const int &N, const int &M)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            double calc_x = i * x_dist - x_dist * (N - 1) / 2.0;
            double calc_y = j * y_dist - y_dist * (M - 1) / 2.0;
            if (z_coord == 0 && x_coord == calc_x && y_coord == calc_y)
            {
                return false;
            }
        }
    }
    return true;
}

mutex electric_mutex;

/* calculate overall electric electric using multithreading
 * */
void do_calculation(vector<ECE_ElectricField> &electric,
                    double x_c, double y_c, double z_c,
                    int beginning, int ending,
                    double &x_field, double &y_field, double &z_field)
{
    double Ex_val = 0, Ey_val = 0, Ez_val = 0;
    double sum_x = 0, sum_y = 0, sum_z = 0;
    for (int i = beginning; i < ending; i++)
    {
        electric[i].computeFieldAt(x_c, y_c, z_c);
        electric[i].getElectricField(Ex_val, Ey_val, Ez_val);
        sum_x += Ex_val;
        sum_y += Ey_val;
        sum_z += Ez_val;
    }
    lock_guard<mutex> locking(electric_mutex);
    x_field += sum_x;
    y_field += sum_y;
    z_field += sum_z;
}

int main() {
    bool finish = false;

    // Determine the number of threads running concurrently
    unsigned int max_threads = thread::hardware_concurrency();
    cout << endl << "Your computer supports " << max_threads << " concurrent threads" << endl;

    char delimiter = ' ';
    // Prompt the user for the size of the array and make sure it is valid
    string user_get;
    vector<string> n_rows_and_columns;
    do
    {
        n_rows_and_columns.clear();
        cout << "Please enter the number of rows and columns in the N_row x_sep M_column array: ";
        getline(cin, user_get);
    } while (!split(user_get, delimiter, n_rows_and_columns, 2, true, true));

    int N_row = stoi(n_rows_and_columns[0]), M_column = stoi(n_rows_and_columns[1]);

    // Prompt the user for the separation distances and make sure it is valid
    string user_distances;
    vector<string> sep_x_and_y;

    bool is_in_range;
    double x_sep, y_sep;
    do
    {
        is_in_range = true;
        do
        {
            sep_x_and_y.clear();
            cout << "Please enter the x and y separation distances in meters: ";
            getline(cin, user_distances);
        } while (!split(user_distances, delimiter, sep_x_and_y, 2, true, false));

        x_sep = stod(sep_x_and_y[0]), y_sep = stod(sep_x_and_y[1]);

        if (x_sep <= 0 || y_sep <= 0)
        {
            cout << "Input value range is invalid! Enter again please." << endl;
            is_in_range = false;
        }
    } while (!is_in_range);

    // Prompt the user for the charge_user and make sure it is valid
    string charge_user;
    do
    {
        charge_user.clear();
        cout << "Please enter the common charge_user on the points in micro C: ";
        getline(cin, charge_user);

    } while (!is_digit(charge_user));
    double q = stod(charge_user);

    while (!finish)
    {
        // Input coordinates
        string user_coordinate;
        vector<string> coordinate_results;
        do
        {
            coordinate_results.clear();
            cout << "Please enter the location in space to determine the electric field (x,y,z) in meters: ";
            getline(cin, user_coordinate);
        } while (!split(user_coordinate, delimiter, coordinate_results, 3, false, false));

        double x_target = stod(coordinate_results[0]);
        double y_target = stod(coordinate_results[1]);
        double z_target = stod(coordinate_results[2]);

        // Check if the point overlaps with the field grid
        if (!check_overlap(x_sep, y_sep,
                           x_target, y_target, z_target,
                           N_row, M_column))
        {
            cout << "Input value range is invalid! Enter again please." << endl;
            continue;
        }

        vector<ECE_ElectricField> electric_field;
        for (int i = 0; i < N_row; ++i)
            for (int j = 0; j < M_column; ++j)
                electric_field.emplace_back(x_sep * i - (N_row - 1) * x_sep / 2,
                                                           y_sep * j - (M_column - 1) * y_sep / 2, 0, q);

        vector<thread> calculation_threads;
        double x_field = 0, y_field = 0, z_field = 0;
        int beginning = 0;
        int ending = 0;
        int blocks = M_column * N_row / max_threads;
        int x_power = 0, y_power = 0, z_power = 0, electric_power = 0;

        auto startTimePoint = chrono::high_resolution_clock::now();

        for (int i = 0; i < max_threads - 1; ++i)
        {
            ending = beginning + blocks;
            calculation_threads.emplace_back(do_calculation, ref(electric_field),
                                             x_target, y_target, z_target,
                                             beginning, ending,
                                             ref(x_field), ref(y_field), ref(z_field));
            beginning = ending;
        }
        do_calculation(electric_field,
                       x_target, y_target, z_target,
                       beginning, electric_field.size(),
                       x_field, y_field, z_field);

        for (thread &item: calculation_threads) item.join();

        auto stopTimePoint = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stopTimePoint - startTimePoint);

        cout << "The electric field at (" << x_target << ", "
        << y_target << ", " << z_target << ") in V/m is" << endl;

        double abs_electric = sqrt(pow(x_field, 2) + pow(y_field, 2) + pow(z_field, 2));
        if (abs(x_field) > 1e-11)
            x_power = static_cast<int>(floor(log10(abs(x_field))));
        if (abs(y_field) > 1e-11)
            y_power = static_cast<int>(floor(log10(abs(y_field))));
        if (abs(z_field) > 1e-11)
            z_power = static_cast<int>(floor(log10(abs(z_field))));
        if (abs(abs_electric) > 1e-11)
            electric_power = static_cast<int>(floor(log10(abs(abs_electric))));

        cout << "Ex = " << fixed << setprecision(4) << x_field / pow(10, x_power)
        << " * 10^" << x_power << endl;
        cout << "Ey = " << fixed << setprecision(4) << y_field / pow(10, y_power)
        << " * 10^" << y_power << endl;
        cout << "Ez = " << fixed << setprecision(4) << z_field / pow(10, z_power)
        << " * 10^" << z_power << endl;
        cout << "|E| = " << fixed << setprecision(4) << abs_electric / pow(10, electric_power)
        << " * 10^" << electric_power << endl;

        cout << "The calculation took " << duration.count() << " microsec!" << endl;

        do
        {
            string yes_or_no;
            cout << "Do you want to enter a new location (Y/N)?";
            getline(cin, yes_or_no);
            if (yes_or_no == "n" || yes_or_no == "N")
            {
                finish = true;
                cout << "Bye!" << endl;
                break;
            }
            else if (yes_or_no == "y" || yes_or_no == "Y")
                break;
            else
                cout << "Input is invalid! Enter again please." << endl;
        } while (true);

    }

    return 0;
}
