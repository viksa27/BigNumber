#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"
#include <iostream>
#include "Calculator.h"


TEST_CASE("Multiplication")
{
    BigNumber m("5");
    BigNumber n("5");
    CHECK(m * n == "25");

    m = BigNumber("1234");
    n = BigNumber("4321");
    CHECK(m * n == "5332114");

    m = BigNumber("12456789031415");
    n = BigNumber("98765432123456789");
    CHECK(m * n == "1230300151558439221348916026435");

}

TEST_CASE("Division") {

    BigNumber m("1230300151558439221348916026435");
    BigNumber n("98765432123456789");

    CHECK(m / n == "12456789031415");
}

TEST_CASE("Addition") {

    BigNumber m("12456789031415");
    BigNumber n("98765432123456789");

    CHECK(m + n == "98777888912488204");
}

TEST_CASE("Substraction") {

    BigNumber m("4321");
    BigNumber n("1234");

    CHECK(m - n == "3087");

}

TEST_CASE("Square root") {

    BigNumber m("12456789031415");
    BigNumber n = m.sqrt();

    CHECK(n == "3529417");

}


//calculator tests
TEST_CASE("2 numbers") {
    std::string input("1234+4321");
    BigNumber expected("5555");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

TEST_CASE("Multiple numbers and every operator") {
    std::string input("1234+4321-9*100/10");
    BigNumber expected("5465");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

TEST_CASE("Every operator and parentheses") {
    std::string input("(1234+(4321-9)*100)/10");
    BigNumber expected("43243");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

TEST_CASE("Every operator and parentheses - 2") {
    std::string input("(1234*10+4321-9*100)/10");
    BigNumber expected("1576");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

TEST_CASE("Hex") {
    std::string input("(0x1232*0xFA23+4321)/0x10");
    BigNumber expected("18642459");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

TEST_CASE("Hex2") {
    std::string input("0x1232*0xFA23+4321/0x10-100/5");
    BigNumber expected("298275280");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

TEST_CASE("Calculator - square root test") {
    std::string input("sqrt15241578750190521");
    BigNumber expected("123456789");
    BigNumber* result = Calculator::calculate(input);
    CHECK(expected == *result);
    delete result;
}

bool confirmCalculation(std::string& input) {
    char expected[] = "calculate";
    size_t size = input.size();
    if (size < 9) {
        return false;
    }

    for (int i = 0; i < 9; i++) {
        if (input[0] != expected[i]) {
            return false;
        }

        input.erase(input.begin());
    }
    return true;
}

NumBase getBase(std::string& input) {
    size_t size = input.size();
    if (size < 2) {
        return NumBase::UNKNOWN;
    }
    if(input[size-1] == 'x' && input[size-2] == 'h') {
        input.pop_back();
        input.pop_back();
        return NumBase::HEXADECIMAL;
    }
    else if (input[size - 1] == 'c' && input[size - 2] == 'd') {
        input.pop_back();
        input.pop_back();
        return NumBase::DECIMAL;
    }
    return NumBase::UNKNOWN;
}

void convert(BigNumber& number,NumBase base) {
    switch (base) {
    case NumBase::HEXADECIMAL: {
        number.convertToHex();
        return;
    }
    case NumBase::DECIMAL: {
        number.convertToDecimal();
        return;
    }
    }
}

int main()
{

    //doctest::Context().run(); std::cout << "\n\n";

    std::cout << "BIG NUMBER CALCULATOR\n";
    while (true) {
        std::string input;
        std::cout << "\n> ";
        getline(std::cin, input);
        std::cout << "\n";
        if (input == "exit") {
            break;
        }
        else if (input == "help") {
            Calculator::help();
        }
        else if (confirmCalculation(input)) {
            NumBase base = getBase(input);
            BigNumber* result = Calculator::calculate(input);
            if (result) {
                convert(*result, base);
                std::cout << "RESULT: " << *result << std::endl;
                delete result;
            }
        }
        else {
            std::cout << "Unsupported command. Type \"help\" for guide on how to use the big number calculator.\n";
        }
    }
}


