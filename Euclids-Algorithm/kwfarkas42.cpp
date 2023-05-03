/*
    Title:    kwfarkas42.cpp
    Author:   Keegan Farkas
    Date:     August 24, 2022
    Purpose:  To find the greatest common divisor of 2 numbers
*/
#include <iostream>
#include <sstream>
#include <exception>
#include <cmath>

void calculate_gcd(int m, int n)
{
    // Int to hold remainder, set equal m to make swapping easy if m < n
    int r = m;

    // Printing the original numbers early since m and n might be changed
    std::cout << "gcd(" << m << "," << n << ") ";

    // To make sure neither number is negative
    m = abs(m);
    n = abs(n);

    // To swap m and n
    if (m < n)
    {
        m = n;
        n = r;
    }

    // Euclid's algorithm
    while (n != 0)
    {
        r = m % n;
        m = n;
        n = r;
    }

    if (m == 0)
    {
        std::cout << "is undefined" << std::endl;
    }
    else
    {
        std::cout << "= " << m << std::endl;
    }
}

void usage(char *name)
{
    std::cerr << "Calculates the greatest common divisor of two numbers." << std::endl;
    std::cerr << "usage:" << name << " [m] [n]" << std::endl;
    std::cerr << "m,n integers, not both zero" << std::endl;
}

int main(int argc, char *argv[])
{
    int m = 0;
    int n = 0;

    if (argc != 3)
    {
        std::cerr << "Invalid number of arguments." << std::endl
                  << std::endl;
        usage(argv[0]);

        return 0;
    }

    std::istringstream iss(argv[1]);
    if ((iss >> m).fail() || !iss.eof())
    {
        std::cerr << argv[1] << " is not a valid integer." << std::endl
                  << std::endl;
        usage(argv[0]);

        return 0;
    }

    iss.str(argv[2]);
    iss.clear();
    if ((iss >> n).fail() || !iss.eof())
    {
        std::cerr << argv[2] << " is not a valid integer." << std::endl
                  << std::endl;
        usage(argv[0]);

        return 0;
    }

    calculate_gcd(m, n);

    return 0;
}
