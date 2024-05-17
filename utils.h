// utils.h
#ifndef UTILS_H
#define UTILS_H
#include <string>

char getValidChoice();
char getChoice();
bool isValidDateFormat(const std::string& date);
bool isValidDate(int year, int month, int day);
bool checkGenderInput(std::string genderInput);
bool isNumber(std::string str);
bool isValidPhoneNumber(const std::string& number);
bool isValidAge(int age);
bool isLetters(const std::string& input);
bool isValidAmount(double amount);
bool isInputEmpty(const std::string& input);

#endif // UTILS_H
