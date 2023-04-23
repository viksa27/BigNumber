#pragma once
#include <iostream>
#include <stack>

enum class NumBase {
    DECIMAL,
    HEXADECIMAL,
    UNKNOWN
};

class BigNumber {
public:

    BigNumber(const std::string&);
    BigNumber(const char);

    BigNumber& operator+=(const BigNumber&);
    BigNumber operator+(const BigNumber&) const;

    BigNumber& operator*=(const BigNumber&);
    BigNumber operator*(const BigNumber&) const;

    BigNumber& operator-=(const BigNumber&);
    BigNumber operator-(const BigNumber&) const;

    BigNumber& operator/=(const BigNumber&);
    BigNumber operator/(const BigNumber&) const;

    BigNumber& operator%=(const BigNumber&);
    BigNumber operator%(const BigNumber&) const;

    BigNumber sqrt() const;

    BigNumber& operator=(const BigNumber&);

    bool operator==(const BigNumber&) const;
    bool operator==(const char*) const;
    bool operator!=(const BigNumber&) const;
    bool operator!=(const char*) const;

    bool operator<(const BigNumber&) const;
    bool operator<=(const BigNumber&) const;

    bool operator>(const BigNumber&) const;
    bool operator>=(const BigNumber&) const;
   
    //Converts the number to hexadecimal base
    void convertToHex();

    //Converts the number to decimal base
    void convertToDecimal();

    friend std::ostream& operator<<(std::ostream&, const BigNumber&);


    static void intToCharPtr(int, char*);
    static int stringToInt(const std::string&, int, int);
    static int power(int, int);
    static BigNumber power(const BigNumber&, int);

    static bool isOperator(char);
    static bool isDigit(char);
    static bool isHexDigit(char);
    static int digitCount(int);
    static char digitToHex(int);
    static const char* hexToDigit(char);


private:
    std::string number;
    NumBase base;
 
    BigNumber findClosestSquare(const BigNumber&, const BigNumber&, char&) const;
    
    //Deletes all zeros from the beggining of the number
    //Example - 00123 => 123
    void clearZeros();

    BigNumber multiplyByDigit(int, int) const;

    std::string toHex() const;
    std::string toDecimal(const std::string&) const;

    void toDecimalHelper(const BigNumber&, BigNumber&, std::stack<BigNumber>&, int) const;

};
