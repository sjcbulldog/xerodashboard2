#pragma once

#include <QtCore/QVector>

class LinearRegression
{
    // Dynamic array which is going
    // to contain all (i-th x)
    QVector<double> x;

    // Dynamic array which is going
    // to contain all (i-th y)
    QVector<double> y;

    // Store the coefficient/slope in
    // the best fitting line
    double coeff;

    // Store the constant term in
    // the best fitting line
    double constTerm;

    // Contains sum of product of
    // all (i-th x) and (i-th y)
    double sum_xy;

    // Contains sum of all (i-th x)
    double sum_x;

    // Contains sum of all (i-th y)
    double sum_y;

    // Contains sum of square of
    // all (i-th x)
    double sum_x_square;

    // Contains sum of square of
    // all (i-th y)
    double sum_y_square;

public:
    // Constructor to provide the default
    // values to all the terms in the
    // object of class regression
    LinearRegression()
    {
        reset();
    }

    void reset()
    {
        coeff = 0;
        constTerm = 0;
        sum_y = 0;
        sum_y_square = 0;
        sum_x_square = 0;
        sum_x = 0;
        sum_xy = 0;

        x.clear();
        y.clear();
    }

    // Function that calculate the coefficient/
    // slope of the best fitting line
    void calculateCoefficient()
    {
        double N = x.size();
        double numerator
            = (N * sum_xy - sum_x * sum_y);
        double denominator
            = (N * sum_x_square - sum_x * sum_x);
        coeff = numerator / denominator;
    }

    // Member function that will calculate
    // the constant term of the best
    // fitting line
    void calculateConstantTerm()
    {
        double N = x.size();
        double numerator
            = (sum_y * sum_x_square - sum_x * sum_xy);
        double denominator
            = (N * sum_x_square - sum_x * sum_x);
        constTerm = numerator / denominator;
    }

    // Function that return the number
    // of entries (xi, yi) in the data set
    int sizeOfData()
    {
        return x.size();
    }

    // Function that return the coefficient/
    // slope of the best fitting line
    double coefficient()
    {
        if (coeff == 0)
            calculateCoefficient();
        return coeff;
    }

    // Function that return the constant
    // term of the best fitting line
    double constant()
    {
        if (constTerm == 0)
            calculateConstantTerm();
        return constTerm;
    }

    // Function to take input from the dataset
    void addXY(double dx, double dy)
    {
        sum_xy += dx * dy;
        sum_x += dx;
        sum_y += dy;
        sum_x_square += dx * dx;
        sum_y_square += dy * dy;
        x.push_back(dx);
        y.push_back(dy);
    }

    // Function to predict the value
    // corresponding to some input
    double predict(double x)
    {
        return coeff * x + constTerm;
    }

    // Function that returns overall
    // sum of square of errors
    double errorSquare()
    {
        double ans = 0;
        for (int i = 0;
            i < x.size(); i++) {
            ans += ((predict(x[i]) - y[i])
                * (predict(x[i]) - y[i]));
        }
        return ans;
    }

    double rsquared()
    {
        double ret = 1.0;
        if (x.size() > 2) {
            double num = (sum_xy - sum_x * sum_y / x.size());
            double den = std::sqrt((sum_x_square - sum_x * sum_x / x.size()) * (sum_y_square - sum_y * sum_y / x.size()));
            ret = (num / den) * (num / den);
        }

        return ret;
    }

    // Functions that return the error
    // i.e the difference between the
    // actual value and value predicted
    // by our model
    double errorIn(double num)
    {
        for (int i = 0;
            i < x.size(); i++) {
            if (num == x[i]) {
                return (y[i] - predict(x[i]));
            }
        }
        return 0;
    }
};