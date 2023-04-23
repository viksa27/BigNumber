#include "BigNumber.h"
#include <queue>


BigNumber::BigNumber(const char c) {
    if (!isDigit(c)) {
        throw std::exception("Invalid number");
    }

    this->base = NumBase::DECIMAL;
    this->number = c;
}

BigNumber::BigNumber(const std::string& newNum) {

    size_t size = newNum.size();
    if (size == 0) {
        throw std::exception("Empty string");
    }

    if (size > 2 && ((newNum[0] == '0' && (newNum[1] == 'x' || newNum[1] == 'X')))) {

        for (size_t i = 2; i < size; ++i) {

            if (!isHexDigit(newNum[i])) {
                throw std::exception("Invalid hexadecimal number");
            }

        }
        this->number = toDecimal(newNum);
        this->base = NumBase::HEXADECIMAL;

    }
    else {

        for (size_t i = 0; i < size; ++i) {
            if (!isDigit(newNum[i])) {
                throw std::exception("Invalid decimal number");
            }
        }

        this->base = NumBase::DECIMAL;
        this->number = newNum;
    }
    clearZeros();
}

bool BigNumber::operator==(const BigNumber& other) const {
    return this->number == other.number;
}

bool BigNumber::operator==(const char* str) const {
    return this->number == str;
}

bool BigNumber::operator!=(const BigNumber& other) const {
    return !(*this == other);
}

bool BigNumber::operator!=(const char* str) const {
    return this->number != str;
}

bool BigNumber::operator<(const BigNumber& other) const {

    size_t size1 = this->number.size(), size2 = other.number.size();

    if (size1 < size2) {
        return true;
    }
    else if (size1 > size2) {
        return false;
    }

    for (size_t i = 0; i < size1; i++) {
        if (this->number[i] < other.number[i]) {
            return true;
        }
        else if (this->number[i] > other.number[i]) {
            return false;
        }
    }

    return false;
}

bool BigNumber::operator<=(const BigNumber& other) const {

    return (*this < other) || (*this == other);

}

bool BigNumber::operator>(const BigNumber& other) const {

    return !(*this <= other);

}

bool BigNumber::operator>=(const BigNumber& other) const {

    return (*this > other) || (*this == other);

}

BigNumber& BigNumber::operator+=(const BigNumber& other) {

    int s1 = this->number.size() - 1, s2 = other.number.size() - 1;
    int carry = 0;

    while (s1 > 3 && s2 > 3) {

        int s1Start = s1 - 4, s2Start = s2 - 4;
        int sum = carry + stringToInt(this->number, s1Start + 1, s1 + 1) + stringToInt(other.number, s2Start + 1, s2 + 1);

        while (s1 != s1Start) {
            this->number[s1] = ((sum % 10) + '0');
            sum /= 10;
            s1--;
            s2--;
        }

        carry = sum;
    }

    while (s1 >= 0 && s2 >= 0) {

        int sum = carry + (this->number[s1] - '0') + (other.number[s2] - '0');
        this->number[s1] = ((sum % 10) + '0');

        s1--;
        s2--;

        carry = sum / 10;
    }

    while (s1 >= 0) {
        int sum = carry + (this->number[s1] - '0');
        this->number[s1] = ((sum % 10) + '0');
        carry = sum / 10;
        s1--;
    }

    while (s2 >= 0) {

        int sum = carry + (other.number[s2] - '0');
        this->number.insert(this->number.begin(), ((sum % 10) + '0'));

        carry = sum / 10;
        s2--;
    }

    if (carry != 0) {
        char* buffer = new char[digitCount(carry) + 1];
        intToCharPtr(carry, buffer);
        this->number.insert(0, buffer);
        delete[] buffer;
    }

    return *this;
}

BigNumber& BigNumber::operator*=(const BigNumber& other) {
    if (this->number == "0") {
        return *this;
    }

    if (other.number == "0") {
        this->number = "0";
        return *this;
    }

    if (other.number == "1") {
        return *this;
    }

    if (this->number == "1") {
        this->number = other.number;
        return *this;
    }

    std::stack<BigNumber> stack;

    for (int index = other.number.size() - 1, base = 0; index >= 0; index--, base++) {
        stack.push(multiplyByDigit(other.number[index] - '0', base));
    }

    this->number = "0";
    while (!stack.empty()) {
        *this += stack.top();
        stack.pop();
    }

    return *this;
}

BigNumber& BigNumber::operator-=(const BigNumber& other) {

    if (*this < other) {
        throw std::exception("Negative numbers are not supported.");
    }

    int s1 = this->number.size() - 1, s2 = other.number.size() - 1;
    int carry = 0;

    while (s2 >= 0) {
        int num1 = this->number[s1] - '0';
        int num2 = other.number[s2] - '0';

        int added = 0;
        while (num1 < num2 + carry) {
            num1 += 10;
            added++;
        }
        this->number[s1] = (num1 - num2 - carry) + '0';

        carry = added;
        s1--;
        s2--;
    }

    while (s1 >= 0 && carry != 0) {
        int num = this->number[s1] - '0';
        int added = 0;

        while (num < carry) {
            num += 10;
            added++;
        }

        this->number[s1] = (num - carry) + '0';

        carry = added;
        s1--;
    }
    clearZeros();
    return *this;
}

BigNumber& BigNumber::operator/=(const BigNumber& other) {

    if (other == "0") {
        throw std::exception("Can't divide by 0");
    }

    if (*this < other) {
        this->number = "0";
        return *this;
    }

    if (*this == other) {
        this->number = "1";
        return *this;
    }

    std::string result;

    int s1 = 0, s2 = other.number.size(), s1End = this->number.size();
    BigNumber dividend(this->number.substr(s1, s2));
    s1 = s2;
    
    while (s1 < s1End) {
        bool firstDigit = true;

        while (dividend < other) {
            if (s1 >= s1End) {
                break;
            }
            if (!firstDigit) {
                result += '0';
            }

            if (dividend != "0") {
                dividend.number += this->number[s1];
            }
            else {
                dividend.number = this->number[s1];
            }
            s1++;
            firstDigit = false;
        }

        int quotient = 0;
        while (dividend >= other) {
            dividend -= other;
            quotient++;
        }

        result += (quotient + '0');
    }

    if (dividend >= other) {
        int quotient = 0;
        while (dividend >= other) {
            dividend -= other;
            quotient++;
        }

        result += (quotient + '0');
    }

    this->number = result;
    return *this;
}

BigNumber& BigNumber::operator%=(const BigNumber& other) {

    if (other == "0") {
        throw std::exception("Can't divide by 0");
    }

    if (*this < other) {
        return *this;
    }

    if (*this == other) {
        this->number = "0";
        return *this;
    }

    int s1 = 0, s2 = other.number.size(), s1End = this->number.size();
    BigNumber dividend(this->number.substr(s1, s2));
    s1 = s2;
    while (s1 < s1End) {

        while (dividend < other) {
            if (s1 >= s1End) {
                break;
            }

            if (dividend != "0") {
                dividend.number += this->number[s1];
            }
            else {
                dividend.number = this->number[s1];
            }
            s1++;
        }

        while (dividend >= other) {
            dividend -= other;

        }
    }

    if (dividend >= other) {
        while (dividend >= other) {
            dividend -= other;
        }
    }

    *this = dividend;
    return *this;
}

BigNumber BigNumber::operator+(const BigNumber& other) const {
    BigNumber result(this->number);
    result += other;
    if (this->base == NumBase::HEXADECIMAL && other.base == NumBase::HEXADECIMAL) {
        result.convertToHex();
    }
    return result;
}

BigNumber BigNumber::operator*(const BigNumber& other) const {
    BigNumber result(this->number);
    result *= other;
    if (this->base == NumBase::HEXADECIMAL && other.base == NumBase::HEXADECIMAL) {
        result.convertToHex();
    }
    return result;
}

BigNumber BigNumber::operator-(const BigNumber& other) const {
    BigNumber result(this->number);
    result -= other;
    if (this->base == NumBase::HEXADECIMAL && other.base == NumBase::HEXADECIMAL) {
        result.convertToHex();
    }
    return result;
}

BigNumber BigNumber::operator/(const BigNumber& other) const {
    BigNumber result(this->number);
    result /= other;
    if (this->base == NumBase::HEXADECIMAL && other.base == NumBase::HEXADECIMAL) {
        result.convertToHex();
    }
    return result;
}

BigNumber BigNumber::operator%(const BigNumber& other) const {
    BigNumber result(this->number);
    result %= other;
    if (this->base == NumBase::HEXADECIMAL && other.base == NumBase::HEXADECIMAL) {
        result.convertToHex();
    }
    return result;
}

BigNumber& BigNumber::operator=(const BigNumber& other) {
    this->number = other.number;
    this->base = other.base;
    return *this;
}

BigNumber BigNumber::sqrt() const {
    int i = 0;
    std::queue<std::string> divided;
    if (this->number.size() % 2 != 0) {
        std::string str;
        str += this->number[0];
        divided.push(str);
        i++;
    }
    int size = this->number.size();

    while (i + 1 < size) {
        std::string num;
        num += this->number[i];
        num += this->number[i + 1];
        divided.push(num);
        i += 2;
    }

    BigNumber start('0');
    BigNumber target('0');
    BigNumber remainder('0');

    std::string result = "0";
    bool shouldBreak = false;
    while (!divided.empty()) {

        start = (BigNumber(result) * BigNumber('2'));
        if (start != "0") {
            start.number += '0';
        }

        target.number += divided.front();
        divided.pop();

        while (start > target) {

            result += '0';
            if (divided.empty()) {
                shouldBreak = true;
                break;
            }

            start = BigNumber(result) * BigNumber("2");
            if (start != "0") {
                start.number += '0';
            }
            target.number += divided.front();
            divided.pop();
        }

        if (shouldBreak) {
            break;
        }

        target.clearZeros();

        char multiplier = '-';
        BigNumber iterationResult = findClosestSquare(start, target, multiplier);
        remainder = target - iterationResult;
        target -= iterationResult;


        result += multiplier;

    }

    return BigNumber(result);
}

void BigNumber::convertToHex() {
    this->base = NumBase::HEXADECIMAL;
}

void BigNumber::convertToDecimal() {
    this->base = NumBase::DECIMAL;
}

std::string BigNumber::toHex() const {

    if (this->number == "0") {
        return "0x0";
    }

    BigNumber tempBase("16");
    BigNumber temp(this->number);
    std::stack<char> digits;

    while (temp != "0") {
        digits.push(digitToHex(stringToInt((temp % tempBase).number, 0, -1)));
        temp /= tempBase;
    }

    std::string result = "0x";
    while (!digits.empty()) {
        result += digits.top();
        digits.pop();
    }

    return result;

}

void BigNumber::toDecimalHelper(const BigNumber& base, BigNumber& result, std::stack<BigNumber>& s, int exp) const {

    if (exp <= 0) {
        return;
    }
    if (exp == 1) {
        s.push(BigNumber(result.number));
        return;
    }

    s.push(result);
    result *= base;
    exp--;
    toDecimalHelper(base, result, s, exp);

}

std::string BigNumber::toDecimal(const std::string& num) const {
    BigNumber result("0");
    size_t size = num.size();
    int exp = size - 3;
    BigNumber base("16");

    std::stack<BigNumber> s;
    s.push(BigNumber("1"));
    toDecimalHelper(static_cast<const BigNumber>(base), base, s, exp);

    for (size_t i = 2; i < size; i++) {
        result += BigNumber(hexToDigit(num[i])) * s.top();
        s.pop();
    }

    return result.number;
}

BigNumber BigNumber::multiplyByDigit(int digit, int base) const {
    std::string result;
    int carry = 0;

    for (int i = this->number.size() - 1; i >= 0; i--) {

        int product = carry + ((this->number[i] - '0') * digit);
        result.insert(result.begin(), (product % 10 + '0'));
        product /= 10;

        carry = product;
    }

    if (carry != 0) {
        char* buffer = new char[digitCount(carry) + 1];
        intToCharPtr(carry, buffer);
        result.insert(0, buffer);
        delete[] buffer;
    }

    for (int i = 0; i < base; i++) {
        result.push_back('0');
    }

    return BigNumber(result);
}

void BigNumber::clearZeros() {

    for (std::string::iterator i = this->number.begin();
        i != this->number.end();
        ) {
        if (*i != '0' || this->number.size() == 1) {
            break;
        }

        this->number.erase(i);
    }

}

void BigNumber::intToCharPtr(int number, char* buffer) {

    if (number == 0) {
        *buffer = '0';
        buffer++;
        *buffer = '\0';
        return;
    }

    int digits = digitCount(number);
    *(buffer + digits) = '\0';
    digits--;
    while (number > 0) {
        *(buffer + digits) = (number % 10) + '0';
        number /= 10;
        digits--;
    }
}

int BigNumber::power(int number, int exp) {
    if (exp < 0) {
        return 0;
    }

    if (exp == 0) {
        return 1;
    }
    if (exp == 1) {
        return number;
    }

    return number * power(number, exp - 1);
}

BigNumber BigNumber::power(const BigNumber& base, int exp) {
    if (exp < 0) {
        return BigNumber("0");
    }
    if (exp == 0) {
        return BigNumber("1");
    }
    if (exp == 1) {
        return BigNumber(base.number);
    }

    return base * power(base, exp - 1);
}

int BigNumber::stringToInt(const std::string& str, int start = 0, int end = -1) {
    if (str.empty()) {
        return -1;
    }
    if (end == -1) {
        end = str.size();
    }

    int multiplier = power(10, end - start - 1);
    int result = 0;

    for (; start != end; start++) {
        if (!isDigit(str[start])) {
            return -1;
        }
        result += (str[start] - '0') * multiplier;
        multiplier /= 10;
    }

    return result;
}


std::ostream& operator<<(std::ostream& outputStream, const BigNumber& num) {
    if (num.base == NumBase::DECIMAL) {
        outputStream << num.number;
    }
    else {
        outputStream << num.toHex();
    }
    return outputStream;
}

BigNumber BigNumber::findClosestSquare(const BigNumber& bigNum, const BigNumber& target, char& result) const {

    for (char i = '1'; i <= '8'; i++) {

        BigNumber product = (bigNum + BigNumber(i + 1));
        product *= BigNumber(i + 1);

        if (product > target) {
            result = i;
            return BigNumber(bigNum + BigNumber(i)) * BigNumber(i);
        }

    }
    result = '9';
    return BigNumber(bigNum + BigNumber('9')) * BigNumber('9');

}


bool BigNumber::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

bool BigNumber::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool BigNumber::isHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

int BigNumber::digitCount(int num) {
    int result = 0;
    while (num != 0) {
        result++;
        num /= 10;
    }
    return result;
}

char BigNumber::digitToHex(int num) {
    switch (num) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'A';
    case 11: return 'B';
    case 12: return 'C';
    case 13: return 'D';
    case 14: return 'E';
    case 15: return 'F';
    default: return '-';
    }
}

const char* BigNumber::hexToDigit(char num) {
    switch (num) {
    case '0': return "0";
    case '1': return "1";
    case '2': return "2";
    case '3': return "3";
    case '4': return "4";
    case '5': return "5";
    case '6': return "6";
    case '7': return "7";
    case '8': return "8";
    case '9': return "9";
    case 'A': return "10";
    case 'B': return "11";
    case 'C': return "12";
    case 'D': return "13";
    case 'E': return "14";
    case 'F': return "15";
    case 'a': return "10";
    case 'b': return "11";
    case 'c': return "12";
    case 'd': return "13";
    case 'e': return "14";
    case 'f': return "15";
    default: return "-";
    }
}