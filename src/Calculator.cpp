#include "Calculator.h"

bool Calculator::isWhiteSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

BigNumber Calculator::calculateExpression(bool& allHex, const std::string& input, size_t& start, char end = '\0', char currentOperation = '+') {
    BigNumber result('0');
    size_t size = input.size();
    bool numExpected = true;
    while (start < size && input[start] != end) {
        if (input[start] == '(') {
            start++;
            result = doOperation(result, calculateExpression(allHex, input, start, ')'), currentOperation);
        }
        else if (numExpected) {
            BigNumber lhs = parseNumber(allHex, input, start);
            result = doOperation(result, lhs, currentOperation);
        }

        if (start >= size) {
            break;
        }

        if (!BigNumber::isOperator(input[start])) {
            throw std::exception("Invalid operator.");
        }
        numExpected = false;
        currentOperation = input[start];
        start++;

        if (start >= size) {
            break;
        }

        BigNumber rhs = '0';
        if (input[start] == '(') {
            start++;
            rhs = calculateExpression(allHex, input, start, ')');
        }
        else {
            rhs = parseNumber(allHex, input, start);
        }
        BigNumber lhs = result;
        if (operatorPriority(input[start]) > operatorPriority(currentOperation)) {

            while (operatorPriority(input[start]) > operatorPriority(currentOperation)) {
                char higherOperator = input[start];
                start++;
                BigNumber nextExpression = '0';
                if (input[start] == '(') {
                    nextExpression = calculateExpression(allHex, input, start, ')', '+');
                }
                else {
                    nextExpression = parseNumber(allHex, input, start);
                }
                rhs = doOperation(rhs, nextExpression, higherOperator);
                if (start >= size) {
                    break;
                }
            }
            result = doOperation(lhs, rhs, currentOperation);

        }
        else {
            result = doOperation(result, rhs, currentOperation);
        }
    }
    start++;
    return result;
}


BigNumber* Calculator::calculate(std::string& input) {

    //Deleting white spaces from the string
    for (size_t i = 0; i < input.size(); i++) {
        if (isWhiteSpace(input[i])) {
            input.erase(i, 1);
        }
    }

    size_t start = 0;
    try {
        bool allHex = true;
        BigNumber result = calculateExpression(allHex, input, start);
        if (allHex) {
            result.convertToHex();
        }
        return new BigNumber(result);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return nullptr;
    }
}

void Calculator::help() {
    std::cout << "Big number calculator guide:\n"
        << "- You can use both decimal and hexadecimal numbers\n"
        << "- You can use parentheses - '(' and ')'\n"
        << "- Available operators are +, -, *, /, % and sqrt(square root)\n"
        << "- The calculator works with integers only, any input which contains double values will result in an error\n"
        << "- Use the command \"calculate\" followed by an expression and the calculator will print the result\n"
        << "- For hexadecimal result, write the suffix -hx after the expression\n"
        << "- For decimal result, write the suffix -dc after the expression\n"
        << "- To close the program write \"exit\"\n"
        << "\n- Example: \n"
        << "> calculate 10+20*2 hx\n"
        << "\nRESULT: 0x32\n";
}

BigNumber Calculator::doOperation(const BigNumber& lhs, const BigNumber& rhs, char operation) {

    switch (operation) {
    case '+': return lhs + rhs;
    case '*': return lhs * rhs;
    case '-': return lhs - rhs;
    case '/': return lhs / rhs;
    case '%': return lhs % rhs;
    }

}



int Calculator::operatorPriority(char operation) {

    switch (operation) {
    case '+': return 0;
    case '-': return 0;
    case '*': return 1;
    case '/': return 1;
    case '%': return 1;
    default: return -1;
    }

}


BigNumber Calculator::parseNumber(bool& allHex, const std::string& input, size_t& start) {

    size_t size = input.size();
    if (start >= size || input.empty()) {
        throw std::exception("Invalid string index as argument.");
    }

    if (start < size - 2 && input[start + 1] == 'x' && BigNumber::isHexDigit(input[start + 2])) {
        if (input[start] != '0') {
            throw std::exception("Invalid hex number input.");
        }
        start += 2;
        std::string num = "0x";
        while (start < size && BigNumber::isHexDigit(input[start])) {
            num += input[start];
            start++;
        }
        return BigNumber(num);
    }
    else if (start < size - 4 && input[start] == 's' && input[start + 1] == 'q' && input[start + 2] == 'r' && input[start + 3] == 't') {
        start += 4;
        if (BigNumber::isDigit(input[start])) {
            BigNumber number = parseNumber(allHex, input, start);
            return number.sqrt();
        }
        else if (input[start] == '(') {
            start++;
            BigNumber number = calculateExpression(allHex, input, start, ')');
            return number.sqrt();
        }
    }
    std::string num;
    allHex = false;
    while (start < size && BigNumber::isDigit(input[start])) {
        num += input[start];
        start++;
    }
    return BigNumber(num);

}