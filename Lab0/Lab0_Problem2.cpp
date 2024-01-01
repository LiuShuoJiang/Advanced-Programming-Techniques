/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Sept 3, 2023
Description:
Lab 0 Problem 2: Check the multiples of 3 or 5
*/

#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

/**
 * @brief takes in a natural number and outputs to the console all the numbers below the entered number 
 * that are multiples of 3 or 5 and then outputs the sum of all the multiples
 * 
 * @param num input number (positive integer)
 */
void checkMultiples(int num)
{
    vector <int> multiples3, multiples5;
    set<int> results;  //we use set to avoid duplicate items
    for (int i = 1; i < num; i++)
    {
        if (i % 3 == 0) 
        {
            multiples3.push_back(i);
            results.insert(i);
        }
        if (i % 5 == 0) 
        {
            multiples5.push_back(i);
            results.insert(i);
        }
    }

    cout << "The multiples of 3 below " << num << " are: ";
    for (auto element: multiples3)
    {
        cout << element << " ";
    }
    cout << endl;

    cout << "The multiples of 5 below " << num << " are: ";
    for (auto element: multiples5)
    {
        cout << element << " ";
    }
    cout << endl;

    cout << "The sum of all multiples is: ";
    
    int sum = 0;
    for (auto it = results.begin(); it != results.end(); it++)
    {
        sum += *it;
    }
    cout << sum << endl;
}

// Take in a positive integer and execute the checkMultiples(int num)
int main()
{
    while (true)
    {
        string userInput;
        cout << "Please enter a natural number (0 to quit): ";
        cin >> userInput;

        // Check if the input is empty or contains non-digit characters
        if (userInput.empty() || userInput.find_first_not_of("0123456789") != string::npos)
        {
            cout << "Invalid input! Try again please." << endl;
        }
        else
        {
            istringstream iss(userInput);
            int val;

            if (iss >> val && val > 0) 
            {
                checkMultiples(val);
            } 
            else if (val == 0) 
            {
                cout << "Program terminated." << endl;
                cout << "Have a nice day!" << endl;
                break;
            } 
            else 
            {
                cout << "Invalid input! Try again please." << endl;
            }
        }
    }
    
    return 0;
}