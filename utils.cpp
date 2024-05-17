#include "utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <regex>

using namespace std;
// ������� ��� ��������� ���������� ������ (y ��� n) �� �����������
char getValidChoice() {
    char choice;
    bool validChoice = false;
    bool errorDisplayed = false;

    while (!validChoice) {
        if (!errorDisplayed) {
            cout << "����������� � ����? (y/n): ";
        }
        cin >> choice;

        // �������� �� ���������� ��������� ������
        if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N') {
            validChoice = true; // ���������, �� ���� ������� ��������� �����
        }
        else {
            if (!errorDisplayed) {
                cout << "�������: ������� ����������� �����. ���� �����, ������ 'y' ��� 'n'." << endl;
                errorDisplayed = true; // �������, �� ������� ��� ������������
            }
        }
    }

    return choice;
};

// ������� ��� �������� ���������� ������� ���� (��-�����-����)
bool isValidDateFormat(const string& date) {
    if (date.length() != 10) {
        return false;
    }

    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') {
                return false;
            }
        }
        else {
            if (!isdigit(date[i])) {
                return false;
            }
        }
    }
    return true;
};

// ������� ��� �������� ���������� �������� ���
bool isValidDate(int year, int month, int day) {
    // �������� �� ���������� ����, ����� �� ���
    if (year < 0 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    // �������� �� ���������� ������� ��� � �����
    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return (day <= 29);
        }
        else {
            return (day <= 28);
        }
    }

    // �������� �� ���������� ������� ��� � ������, �� ����� 30 ���
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return (day <= 30);
    }

    return (day <= 31);
};

// ������� ��� ��������� ���������� ������ (y ��� n) �� �����������
char getChoice() {
    char choice;
    bool validChoice = false;
    bool errorDisplayed = false;
    while (!validChoice) {
        if (!errorDisplayed) {
            cout << "������ ������ ��? (y/n): ";
        }
        cin >> choice;

        // �������� �� ���������� ��������� ������
        if (isInputEmpty(to_string(choice))) {
            cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ y ��� n." << endl;
            continue; // �������� ���� �����
        }
        else if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N') {
            validChoice = true; // ���������, �� ���� ������� ��������� �����
        }
        else {
            if (!errorDisplayed) {
                cout << "�������: ������� ����������� �����. ���� �����, ������ 'y' ��� 'n'." << endl;
                errorDisplayed = true; // �������, �� ������� ��� ������������
            }
        }
    }

    return choice;
};

// ������� ��� �������� ���������� ��������� ����� ���� ���� (������� ��� �����)
bool checkGenderInput(string genderInput) {
    return genderInput == "�������" || genderInput == "�����";
};

// ������� ��� ��������, �� �������� ����� ������ ���� �����
bool isNumber(string str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

// ������� ��� �������� ���������� ��������� ������ ��������
bool isValidPhoneNumber(const string& number) {
    int minLength = 8;
    int maxLength = 13;

    // ��������, �� ������� ���� � ����� �������� � ����� ������������� ��������
    if (number.length() < minLength || number.length() > maxLength) {
        return false;
    }

    // ��������, �� �� ������� � ����� � �������
    return all_of(number.begin(), number.end(), ::isdigit);
}


// ������� ��� �������� ���������� ��������� ���
bool isValidAge(int age) {
    return age >= 0 && age <= 125;
}

// ������� ��� ��������, �� �������� ����� ������ ���� �����
bool isLetters(const string& input) {
    // ���������� ����� ��� �������� �������� ���������� ����
    regex pattern("[�-ߥ����-�����']+");

    // �������� �� ���������� ����������� ������
    return regex_match(input, pattern);
}

// ������� ��� �������� ���������� ��������� �������� ����
bool isValidAmount(double amount) {
    return amount >= 1 && amount <= 100000;
}

// ������� ��� ��������, �� �������� ����� � �������
bool isInputEmpty(const std::string& input) {
    return input.empty();
}
