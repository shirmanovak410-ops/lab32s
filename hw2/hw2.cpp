//
// Created by Ксения on 25.11.2025.
//
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <stack>
#include <cmath>
int main() {
    std::string line;
    std::getline(std::cin, line);
    std::vector<std::string> result;
    std::vector<std::string> numbers;
    std::unordered_map<std::string, int> priority = {
        {"+",1}, {"-",1}, {"*",2}, {"/",2}, {"^",3},
        {"@",4}, {"~",4}, {"`",4}, {"$",4}, {"!",4}
    };
    std::unordered_map<std::string, std::string> complex_functions = {
        {"cos","@"}, {"sin","~"}, {"tg","`"}, {"ctg","$"}, {"exp","!"}
    };
    if (!line.empty() && line[0] == '-') { //унарный минус
        line[0] = '#';
    }
    size_t pos = 0;
    while ((pos = line.find("(-", pos)) != std::string::npos) {
        line.replace(pos, 2, "(#");
        pos += 2;
    }
    for (auto& func_pair : complex_functions) {
        std::string func_name = func_pair.first;
        std::string func_symbol = func_pair.second;
        pos = 0;
        while ((pos = line.find(func_name, pos)) != std::string::npos) {

            line.replace(pos, func_name.length(), func_symbol);
            pos += func_symbol.length();
        }
    }
    std::string temp = line;

    std::string operators = "+-*/^()@~`$!#";
    for (char& c : temp) {
        if (operators.find(c) != std::string::npos) {
            c = ' ';
        }
    }

    temp.erase(std::unique(temp.begin(), temp.end(),
        [](char a, char b) { return a == ' ' && b == ' '; }), temp.end());

    std::stringstream ss(temp);
    std::string token;
    while (ss >> token) {

        if (token[0] == '#') {
            token[0] = '-';
        }
        numbers.push_back(token);
    }

    std::stack<std::string> memory;
    int number_index = 0;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == ' ') {
            continue;
        }

        std::string op_str(1, c);
        bool is_operator = (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' ||
                           c == '@' || c == '~' || c == '`' || c == '$' || c == '!');

        if (is_operator) {

            while (!memory.empty() && memory.top() != "(" &&
                   priority[memory.top()] >= priority[op_str]) {
                result.push_back(memory.top());
                memory.pop();
            }
            memory.push(op_str);
        }
        else if (c == '(') {
            memory.push("(");
        }
        else if (c == ')') {
            while (!memory.empty() && memory.top() != "(") {
                result.push_back(memory.top());
                memory.pop();
            }
            if (!memory.empty() && memory.top() == "(") {
                memory.pop();
            }
        }
        else {

            if (number_index < numbers.size()) {
                result.push_back(numbers[number_index]);


                std::string current_num = numbers[number_index];
                if (current_num[0] == '-') {
                    // Для отрицательных чисел пропускаем на 1 символ меньше
                    i += current_num.length() - 2;
                } else {
                    i += current_num.length() - 1;
                }
                number_index++;
            }
        }
    }

    while (!memory.empty()) {
        result.push_back(memory.top());
        memory.pop();
    }

    for (auto& token : result) {
        for (const auto& func_pair : complex_functions) {
            if (token == func_pair.second) {
                token = func_pair.first;
                break;
            }
        }
    }

    std::cout << "Postfix notation: ";
    for (const auto& t : result) {
        std::cout << t << " ";
    }
    std::cout << std::endl;

    bool has_x = false;
    for (const auto& num : numbers) {
        if (num == "x") {
            has_x = true;
            break;
        }
    }

    if (has_x) {
        std::string x_val;
        std::cout << "Enter value of x: ";
        std::cin >> x_val;

        for (auto& token : result) {
            if (token == "x") {
                token = x_val;
            }
        }
    }

    std::stack<double> calc_stack;

    for (const auto& token : result) {
        try {
            if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
                if (calc_stack.size() < 2) {
                    std::cout << "Error: not enough operands for operator " << token << std::endl;
                    return 1;
                }
                double b = calc_stack.top(); calc_stack.pop();
                double a = calc_stack.top(); calc_stack.pop();

                if (token == "+") calc_stack.push(a + b);
                else if (token == "-") calc_stack.push(a - b);
                else if (token == "*") calc_stack.push(a * b);
                else if (token == "/") {
                    if (b == 0) {
                        std::cout << "Error: division by zero" << std::endl;
                        return 1;
                    }
                    calc_stack.push(a / b);
                }
                else if (token == "^") calc_stack.push(pow(a, b));
            }
            else if (token == "sin" || token == "cos" || token == "tg" || token == "ctg" || token == "exp") {
                if (calc_stack.empty()) {
                    std::cout << "Error: not enough operands for function " << token << std::endl;
                    return 1;
                }
                double a = calc_stack.top(); calc_stack.pop();

                if (token == "sin") calc_stack.push(sin(a));
                else if (token == "cos") calc_stack.push(cos(a));
                else if (token == "tg") calc_stack.push(tan(a));
                else if (token == "ctg") {
                    if (tan(a) == 0) {
                        std::cout << "Error: ctg undefined" << std::endl;
                        return 1;
                    }
                    calc_stack.push(1.0 / tan(a));
                }
                else if (token == "exp") calc_stack.push(exp(a));
            }
            else {
                // Это число
                calc_stack.push(std::stod(token));
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error processing token '" << token << "': " << e.what() << std::endl;
            return 1;
        }
    }

    if (calc_stack.size() != 1) {
        std::cout << "Error: invalid expression" << std::endl;
        return 1;
    }

    std::cout << "Result: " << calc_stack.top() << std::endl;

    return 0;
}