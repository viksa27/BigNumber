#pragma once
#include "BigNumber.h"

class Calculator {
public:
    //Prints guide for using the calculator in the console
    static void help();

    //Calculates the value of a std::string mathematical expression
    //Returns dynamic allocated memory on success and nullptr on failure
    static BigNumber* calculate(std::string&);

    static bool isWhiteSpace(char);

private:

    static BigNumber doOperation(const BigNumber&, const BigNumber&, char);
    static BigNumber calculateExpression(bool&, const std::string&, size_t&, char, char currentOperation);
    static int operatorPriority(char);
    static BigNumber parseNumber(bool&, const std::string&, size_t&);
};
