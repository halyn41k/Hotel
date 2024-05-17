#include "utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <regex>

using namespace std;
// Функція для отримання коректного вибору (y або n) від користувача
char getValidChoice() {
    char choice;
    bool validChoice = false;
    bool errorDisplayed = false;

    while (!validChoice) {
        if (!errorDisplayed) {
            cout << "Повернутись в меню? (y/n): ";
        }
        cin >> choice;

        // Перевірка на коректність введеного вибору
        if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N') {
            validChoice = true; // Позначаємо, що було введено правильну опцію
        }
        else {
            if (!errorDisplayed) {
                cout << "Помилка: Введено неправильну опцію. Будь ласка, введіть 'y' або 'n'." << endl;
                errorDisplayed = true; // Помічаємо, що помилка вже відображалася
            }
        }
    }

    return choice;
};

// Функція для перевірки коректності формату дати (рік-місяць-день)
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

// Функція для перевірки коректності введених дат
bool isValidDate(int year, int month, int day) {
    // Перевірка на коректність року, місяця та дня
    if (year < 0 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    // Перевірка на коректність кількості днів у місяці
    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return (day <= 29);
        }
        else {
            return (day <= 28);
        }
    }

    // Перевірка на коректність кількості днів у місяцях, які мають 30 днів
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return (day <= 30);
    }

    return (day <= 31);
};

// Функція для отримання коректного вибору (y або n) від користувача
char getChoice() {
    char choice;
    bool validChoice = false;
    bool errorDisplayed = false;
    while (!validChoice) {
        if (!errorDisplayed) {
            cout << "Хочете додати ще? (y/n): ";
        }
        cin >> choice;

        // Перевірка на коректність введеного вибору
        if (isInputEmpty(to_string(choice))) {
            cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть y або n." << endl;
            continue; // Починаємо цикл знову
        }
        else if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N') {
            validChoice = true; // Позначаємо, що було введено правильну опцію
        }
        else {
            if (!errorDisplayed) {
                cout << "Помилка: Введено неправильну опцію. Будь ласка, введіть 'y' або 'n'." << endl;
                errorDisplayed = true; // Помічаємо, що помилка вже відображалася
            }
        }
    }

    return choice;
};

// Функція для перевірки коректності введеного рядка щодо статі (чоловіча або жіноча)
bool checkGenderInput(string genderInput) {
    return genderInput == "чоловіча" || genderInput == "жіноча";
};

// Функція для перевірки, чи введений рядок містить лише цифри
bool isNumber(string str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Функція для перевірки коректності введеного номеру телефону
bool isValidPhoneNumber(const string& number) {
    int minLength = 8;
    int maxLength = 13;

    // Перевірка, чи кількість цифр у номері телефону в межах встановленого діапазону
    if (number.length() < minLength || number.length() > maxLength) {
        return false;
    }

    // Перевірка, чи всі символи у номері є цифрами
    return all_of(number.begin(), number.end(), ::isdigit);
}


// Функція для перевірки коректності введеного віку
bool isValidAge(int age) {
    return age >= 0 && age <= 125;
}

// Функція для перевірки, чи введений рядок містить лише літери
bool isLetters(const string& input) {
    // Регулярний вираз для перевірки наявності українських літер
    regex pattern("[А-ЯҐЄІЇа-яґєії']+");

    // Перевірка на відповідність регулярному виразу
    return regex_match(input, pattern);
}

// Функція для перевірки коректності введеного значення суми
bool isValidAmount(double amount) {
    return amount >= 1 && amount <= 100000;
}

// Функція для перевірки, чи введений рядок є порожнім
bool isInputEmpty(const std::string& input) {
    return input.empty();
}
