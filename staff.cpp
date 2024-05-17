#include "Staff.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <algorithm> // Для функцій all_of і none_of
#include <mysql_connection.h> // Припустимо, що ви використовуєте MySQL Connector/C++
#include <cppconn/prepared_statement.h>
#include <sstream>
#include "utils.h"

using namespace std;

Staff::Staff() : position("none"), firstName("none"), lastName("none"), phoneNumber("none"), hireDate("none"), salary(0), age(0), hotel(*(new HotelConnecting())) {}
Staff::Staff(HotelConnecting& h) : hotel(h) {}


bool Staff::isValidPhoneNumber(const string& number) {
    return all_of(number.begin(), number.end(), ::isdigit);
}

bool Staff::isValidSalary(double salary) {
    // Перевірка, чи зарплата є числом та чи вона в межах від 10 до 100000
    return salary >= 10 && salary <= 100000;
}

bool Staff::isValidAge(int age) {
    // Перевірка, чи вік є числом та чи він в межах від 18 до 70
    return age >= 18 && age <= 70;
}

bool Staff::isValidPosition(const string& position) {
    // Перевірка, чи посада не містить цифр
    return none_of(position.begin(), position.end(), ::isdigit);
}
void Staff::displayStaff() {
    try {
        while (true) {
            // Введення ID співробітника або 0 для виведення всіх
            cout << "~~ Введіть ID співробітника або введіть 0, щоб вивести всіх ~~" << endl;
            string staffIdStr;
            cout << "ID співробітника: ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, staffIdStr);

            // Перевірка на порожній ввід
            if (isInputEmpty(staffIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ID співробітника або 0." << endl;
                continue; // Починаємо цикл знову
            }

            if (staffIdStr == "0") {
                // Виведення всіх співробітників
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Немає записів для відображення." << endl;
                }
                else {
                    // Виведення даних про співробітників у форматі таблиці
                    cout << setw(15) << "ID співробітника" << setw(19) << "Посада" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(20) << "Номер телефону" << setw(20) << "Зарплата" << setw(10) << "Вік" << setw(30) << "Дата працевлаштування" << endl;
                    do {
                        cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // Повертаємось назад без виведення гостей, якщо користувач вибрав 'n' або 'N'
                }
            }
            else {
                int staffId;
                try {
                    staffId = stoi(staffIdStr);
                }
                catch (const std::invalid_argument& ia) {
                    cout << "Помилка: Введений ID співробітника має бути числом або 0." << endl;
                    continue; // Починаємо цикл знову
                }

                // Виведення даних конкретного співробітника за його ID
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff WHERE StaffId = ?");
                pstmt->setInt(1, staffId);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Помилка: Співробітника з введеним ID не знайдено." << endl;
                }
                else {
                    // Виведення даних про співробітника у форматі таблиці
                    cout << setw(15) << "ID співробітника" << setw(19) << "Посада" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(20) << "Номер телефону" << setw(20) << "Зарплата" << setw(10) << "Вік" << setw(30) << "Дата працевлаштування" << endl;
                    cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;
                }

                delete pstmt;
                delete res;
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // Повертаємось назад без виведення гостей, якщо користувач вибрав 'n' або 'N'
                }
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

void Staff::addStaff() {
    try {
        cout << "~~ Додати нового працівника ~~" << endl;

        // Введення посади працівника
        do {
            cout << "Посада: ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, position);

            if (position.empty() || !isValidPosition(position)) {
                cout << "Помилка: Посада не може бути порожньою і повинна містити лише букви. Будь ласка, введіть посаду без цифр." << endl;
            }
        } while (position.empty() || !isValidPosition(position));

        // Введення імені працівника
        do {
            cout << "Ім'я: ";
            getline(cin, firstName);

            if (firstName.empty() || !isLetters(firstName)) {
                cout << "Помилка: Ім'я не може бути порожнім і повинно містити лише літери." << endl;
            }
        } while (firstName.empty() || !isLetters(firstName));

        // Введення прізвища працівника
        do {
            cout << "Прізвище: ";
            getline(cin, lastName);

            if (lastName.empty() || !isLetters(lastName)) {
                cout << "Помилка: Прізвище не може бути порожнім і повинно містити лише літери." << endl;
            }
        } while (lastName.empty() || !isLetters(lastName));

        // Введення номера телефону працівника
        do {
            cout << "Номер телефону (мінімум 8 символів): ";
            getline(cin,phoneNumber);

            if (phoneNumber.empty() || phoneNumber.length() < 8) {
                cout << "Помилка: Номер телефону не може бути порожнім і повинен містити щонайменше 8 цифр." << endl;
            }
            else if (!isValidPhoneNumber(phoneNumber)) {
                cout << "Помилка: Номер телефону повинен містити тільки цифри." << endl;
            }
        } while (phoneNumber.empty() || phoneNumber.length() < 8 || !isValidPhoneNumber(phoneNumber));

        // Введення зарплати працівника
        do {
            cout << "Зарплата: ";
            string salaryInput;
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, salaryInput);
            stringstream salaryStream(salaryInput);
            salaryStream >> salary;

            if (cin.fail() || !isValidSalary(salary)) {
                cout << "Помилка: Зарплата має бути числом у межах від 10 до 100000." << endl;
                cin.clear();
                cin.ignore(INT_MAX, '\n');
            }
        } while (cin.fail() || !isValidSalary(salary));

        // Введення дати працевлаштування працівника
        bool validDate = false;
        do {
            cout << "Дата працевлаштування (рік-місяць-день): ";
            getline(cin,hireDate);

            if (hireDate.empty() || hireDate.length() != 10) {
                cout << "Помилка: Неправильний формат дати. Будь ласка, введіть дату у форматі 'рік-місяць-день'." << endl;
            }
            else {
                // Перевірка правильності формату дати
                bool validFormat = true;
                for (int i = 0; i < 10; ++i) {
                    if (i == 4 || i == 7) {
                        if (hireDate[i] != '-') {
                            validFormat = false;
                            break;
                        }
                    }
                    else if (!isdigit(hireDate[i])) {
                        validFormat = false;
                        break;
                    }
                }

                if (!validFormat) {
                    cout << "Помилка: Неправильний формат дати. Будь ласка, введіть дату у форматі 'рік-місяць-день'." << endl;
                }
                else {
                    // Розділення рядка дати на окремі частини (рік, місяць, день)
                    int year = stoi(hireDate.substr(0, 4));
                    int month = stoi(hireDate.substr(5, 2));
                    int day = stoi(hireDate.substr(8, 2));

                    // Перевірка правильності дати
                    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
                        validDate = true;
                    }
                    else {
                        cout << "Помилка: Неправильна дата. Будь ласка, введіть коректну дату." << endl;
                    }
                }
            }
        } while (!validDate);

        // Вставка нового запису про працівника у базу даних
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO staff (Position, FirstName, LastName, PhoneNumber, Salary, Age, HireDate) VALUES (?, ?, ?, ?, ?, ?, ?)");
        pstmt->setString(1, position);
        pstmt->setString(2, firstName);
        pstmt->setString(3, lastName);
        pstmt->setString(4, phoneNumber);
        pstmt->setDouble(5, salary);
        pstmt->setInt(6, age); // age не визначений у функції addStaff
        pstmt->setString(7, hireDate);
        pstmt->executeUpdate();

        cout << "Нового працівника успішно додано!" << endl;

        delete pstmt;

        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return;
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

void Staff::deleteStaff() {
    try {
        while (true) {
            string staffIdStr;
            cout << "Введіть ID працівника, якого ви хочете видалити: ";
            cin.ignore(INT_MAX, '\n'); // Очищення буфера вводу
            getline(cin, staffIdStr); // Отримання введеного ID працівника

            if (staffIdStr == "0") {
                break; // Вихід з циклу, якщо користувач ввів 0
            }

            if (staffIdStr.empty()) {
                cout << "Помилка: Введіть ID працівника." << endl;
                continue; // Почати цикл знову, якщо введено порожній рядок
            }

            int staffId;
            try {
                staffId = stoi(staffIdStr); // Конвертація рядка введеного ID в ціле число
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ID працівника має бути числом або 0." << endl;
                continue; // Почати цикл знову, якщо введено некоректне значення ID
            }

            // Підготовка SQL-запиту для отримання інформації про працівника за вказаним ID
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff WHERE StaffId = ?");
            pstmt->setInt(1, staffId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Працівника з введеним ID не знайдено." << endl;
            }
            else {
                // Виведення інформації про знайденого працівника
                cout << "Вибраний працівник для видалення:" << endl;
                cout << setw(15) << "ID співробітника" << setw(19) << "Посада" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(20) << "Номер телефону" << setw(20) << "Зарплата" << setw(10) << "Вік" << setw(30) << "Дата працевлаштування" << endl;
                cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;

                // Підтвердження видалення працівника
                char confirmDelete;
                cout << "Ви впевнені, що хочете видалити цього працівника? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // Підготовка SQL-запиту для видалення працівника з бази даних
                    pstmt = con->prepareStatement("DELETE FROM staff WHERE StaffId = ?");
                    pstmt->setInt(1, staffId);
                    int rowsAffected = pstmt->executeUpdate();

                    // Повідомлення про результат видалення працівника
                    if (rowsAffected > 0) {
                        cout << "Працівника успішно видалено." << endl;
                    }
                    else {
                        cout << "Помилка: Працівника з вказаним ID не знайдено." << endl;
                    }
                }
                else {
                    cout << "Видалення працівника скасовано." << endl;
                }
            }

            // Звільнення ресурсів і підготовка до наступної ітерації циклу
            delete pstmt;
            delete res;

            // Перевірка чи користувач хоче продовжити видалення працівників
            char choice;
            cout << "Хочете видалити ще одного працівника? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // Вихід з циклу, якщо користувач більше не хоче видаляти працівників
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка видалення працівника: " << e.what() << endl;
    }
}

// Метод для оновлення інформації про працівників
void Staff::updateStaff() {
    try {
        // Безкінечний цикл для взаємодії з користувачем
        while (true) {
            string staffIdStr;
            // Запит на введення ID працівника, якого потрібно оновити
            cout << "Введіть ID працівника, якого ви хочете оновити (або 0, щоб повернутися до головного меню): ";
            cin.ignore(INT_MAX, '\n'); // Очищення буфера вводу
            getline(cin, staffIdStr); // Отримання введеного ID працівника

            // Перевірка, чи користувач ввів 0 для виходу
            if (staffIdStr == "0") {
                break; // Вихід з циклу, якщо користувач ввів 0
            }

            int staffId;
            try {
                staffId = stoi(staffIdStr); // Конвертація рядка введеного ID в ціле число
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ID працівника має бути числом або 0." << endl;
                continue; // Початок циклу знову, якщо введено некоректне значення ID
            }

            // Підготовка SQL-запиту для отримання інформації про працівника за вказаним ID
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff WHERE StaffId = ?");
            pstmt->setInt(1, staffId);
            ResultSet* res = pstmt->executeQuery();

            // Перевірка, чи знайдено працівника з вказаним ID
            if (!res->next()) {
                cout << "Помилка: Прізвища працівника з введеним ID не знайдено." << endl;
            }
            else {
                // Виведення інформації про знайденого працівника
                cout << "Вибраний працівник для оновлення:" << endl;
                cout << setw(15) << "ID співробітника" << setw(19) << "Посада" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(20) << "Номер телефону" << setw(20) << "Зарплата" << setw(10) << "Вік" << setw(30) << "Дата працевлаштування" << endl;
                cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;

                // Оновлення даних працівника
                cout << "Оберіть параметр для оновлення:" << endl;
                cout << "1. Посада" << endl;
                cout << "2. Ім'я" << endl;
                cout << "3. Прізвище" << endl;
                cout << "4. Номер телефону" << endl;
                cout << "5. Зарплата" << endl;
                cout << "6. Вік" << endl;
                cout << "7. Дата працевлаштування" << endl;
                cout << "0. Повернутися до головного меню" << endl;

                // Введення вибору користувача
                int updateChoice;
                cout << "Ваш вибір: ";
                cin >> updateChoice;

                // Обробка вибору користувача та оновлення відповідного поля в базі даних
                switch (updateChoice) {
                    // Оновлення посади
                case 1: {
                    string newPosition;
                    do {
                        cin.ignore();
                        cout << "Нова посада: ";
                        getline(cin, newPosition);
                        cin.ignore(INT_MAX, '\n');
                        // Перевірка на порожній ввід та правильність символів
                        if (isInputEmpty(newPosition)) {
                            cout << "Помилка: Поле 'Посада' не може бути порожнім." << endl;
                        }
                        else if (!isLetters(newPosition)) {
                            cout << "Помилка: Посада повинна містити лише літери." << endl;
                        }
                    } while (isInputEmpty(newPosition) || !isLetters(newPosition));

                    // Оновлення поля 'Посада' у базі даних
                    pstmt = con->prepareStatement("UPDATE staff SET Position = ? WHERE StaffId = ?");
                    pstmt->setString(1, newPosition);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                case 2: {
                    string newFirstName;
                    do {
                        cin.ignore();
                        cout << "Нове ім'я: ";
                        getline(cin, newFirstName);
                        cin.ignore(INT_MAX, '\n');
                        if (isInputEmpty(newFirstName)) {
                            cout << "Помилка: Поле 'Ім'я' не може бути порожнім." << endl;
                        }
                        else if (!isLetters(newFirstName)) {
                            cout << "Помилка: Ім'я повинно містити лише літери." << endl;
                        }
                    } while (isInputEmpty(newFirstName) || !isLetters(newFirstName));

                    pstmt = con->prepareStatement("UPDATE staff SET FirstName = ? WHERE StaffId = ?");
                    pstmt->setString(1, newFirstName);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                case 3: {
                    string newLastName;
                    do {
                        cin.ignore();
                        cout << "Нове прізвище: ";
                        getline(cin, newLastName);
                        cin.ignore(INT_MAX, '\n');
                        if (isInputEmpty(newLastName)) {
                            cout << "Помилка: Поле 'Прізвище' не може бути порожнім." << endl;
                        }
                        else if (!isLetters(newLastName)) {
                            cout << "Помилка: Прізвище повинно містити лише літери." << endl;
                        }
                    } while (isInputEmpty(newLastName) || !isLetters(newLastName));

                    pstmt = con->prepareStatement("UPDATE staff SET LastName = ? WHERE StaffId = ?");
                    pstmt->setString(1, newLastName);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                case 4: {
                    string newPhoneNumber;
                    do {
                        cout << "Новий номер телефону: ";
                        getline(cin,newPhoneNumber);
                        cin.ignore(INT_MAX, '\n');
                        if (isInputEmpty(newPhoneNumber)) {
                            cout << "Помилка: Поле 'Номер телефону' не може бути порожнім." << endl;
                        }
                        else if (!isValidPhoneNumber(newPhoneNumber)) {
                            cout << "Помилка: Некоректний формат номеру телефону." << endl;
                        }
                    } while (isInputEmpty(newPhoneNumber) || !isValidPhoneNumber(newPhoneNumber));

                    pstmt = con->prepareStatement("UPDATE staff SET PhoneNumber = ? WHERE StaffId = ?");
                    pstmt->setString(1, newPhoneNumber);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                case 5: {
                    double newSalary;
                    string newSalaryStr;
                    Staff staff;
                    bool isValidSalary = false;
                    do {
                        cout << "Нова зарплата: ";
                        getline(cin, newSalaryStr);
                        cin.ignore(); // Очищаємо буфер вводу

                        if (isInputEmpty(newSalaryStr)) {
                            cout << "Помилка: Зарплата не може бути порожньою." << endl;
                            continue;
                        }

                        if (!isNumber(newSalaryStr)) {
                            cout << "Помилка: Зарплата повинна бути числом." << endl;
                            continue;
                        }

                        newSalary = stod(newSalaryStr);
                        if (!staff.isValidSalary(newSalary)) {
                            cout << "Помилка: Зарплата повинна бути більше 10 та менше 100000." << endl;
                        }
                        else {
                            isValidSalary = true;
                        }
                    } while (!isValidSalary);


                    pstmt = con->prepareStatement("UPDATE staff SET Salary = ? WHERE StaffId = ?");
                    pstmt->setDouble(1, newSalary);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                case 6: {
                    int newAge;
                    do {
                        cout << "Вік: ";
                        string newAgeInput;
                        getline(cin, newAgeInput);
                        cin.ignore(INT_MAX, '\n');
                        stringstream newAgeStream(newAgeInput);
                        newAgeStream >> newAge;
                        if (isInputEmpty(to_string(newAge))) {
                            cout << "Помилка: Введений рядок не може бути порожнім." << endl;
                        }
                        else if (!isValidAge(newAge)) {
                            cout << "Помилка: Вік має бути в межах від 18 до 70." << endl;
                        }
                    } while (isInputEmpty(to_string(newAge)) || !isValidAge(newAge));

                    pstmt = con->prepareStatement("UPDATE staff SET Age = ? WHERE StaffId = ?");
                    pstmt->setInt(1, newAge);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                case 7: {
                    string newHireDate;
                    do {
                        cout << "Нова дата працевлаштування (рік-місяць-день): ";
                        cin.ignore(INT_MAX, '\n');
                        getline(cin,newHireDate);
                        if (isInputEmpty(newHireDate)) {
                            cout << "Помилка: Введений рядок не може бути порожнім." << endl;
                        }
                        else if (!isValidDateFormat(newHireDate)) {
                            cout << "Помилка: Некоректний формат дати. Потрібний формат: рік-місяць-день" << endl;
                        }
                    } while (isInputEmpty(newHireDate) || !isValidDateFormat(newHireDate));

                    pstmt = con->prepareStatement("UPDATE staff SET HireDate = ? WHERE StaffId = ?");
                    pstmt->setString(1, newHireDate);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                      // Повідомлення про некоректний ввід вибору
                default: {
                    cout << "Будь ласка, введіть коректний варіант (0-7)." << endl;
                    continue; // Починаємо цикл знову
                }
                }

                cout << "Інформацію про працівника з ID " << staffId << " успішно оновлено." << endl;
            }

            // Звільнення ресурсів
            delete pstmt;
            delete res;
        }
    }
    // Обробка помилок бази даних
    catch (SQLException& e) {
        cout << "Помилка оновлення даних про працівника: " << e.what() << endl;
    }
}
void Staff::sortStaff() {
    try {
        int sortingCriterion;
        // Виведення меню для вибору критерію сортування
        cout << "Оберіть критерій сортування персоналу:" << endl;
        cout << "1. За ID співробітника." << endl;
        cout << "2. За посадою." << endl;
        cout << "3. За іменем." << endl;
        cout << "4. За прізвищем." << endl;
        cout << "5. За зарплатою." << endl;
        cout << "6. За віком." << endl;
        cout << "7. За датою працевлаштування." << endl;
        cout << "0. Повернутися до попереднього меню." << endl;
        cout << "Ваш вибір: ";

        // Отримання вибору користувача та перевірка на коректність
        string sortingCriterionStr;
        cin >> sortingCriterionStr;
        if (isInputEmpty(sortingCriterionStr)) {
            cout << "Помилка: Введений рядок не може бути порожнім." << endl;
            return;
        }

        // Конвертація рядка введеного вибору в ціле число
        try {
            sortingCriterion = std::stoi(sortingCriterionStr);
        }
        catch (const std::invalid_argument& e) {
            throw std::runtime_error("Помилка вводу. Будь ласка, введіть ціле число.");
        }

        // Формування SQL-запиту з урахуванням вибраного критерію сортування
        string orderByClause;
        switch (sortingCriterion) {
            // Сортування за ID співробітника
        case 1:
            orderByClause = "ORDER BY StaffId";
            break;
            // Сортування за посадою
        case 2:
            orderByClause = "ORDER BY Position";
            break;
            // Сортування за іменем
        case 3:
            orderByClause = "ORDER BY FirstName";
            break;
            // Сортування за прізвищем
        case 4:
            orderByClause = "ORDER BY LastName";
            break;
            // Сортування за зарплатою
        case 5:
            orderByClause = "ORDER BY Salary";
            break;
            // Сортування за віком
        case 6:
            orderByClause = "ORDER BY Age";
            break;
            // Сортування за датою працевлаштування
        case 7:
            orderByClause = "ORDER BY HireDate";
            break;
            // Повернення до попереднього меню
        case 0:
            return;
            // Виведення повідомлення про некоректний вибір
        default:
            cout << "Невірний вибір. Будь ласка, введіть число від 0 до 7." << endl;
            return;
        }

        // Виконання SQL-запиту для відсортованого списку персоналу
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // Виведення відсортованого списку персоналу
        cout << setw(15) << "ID співробітника" << setw(19) << "Посада" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(20) << "Номер телефону" << setw(20) << "Зарплата" << setw(10) << "Вік" << setw(30) << "Дата працевлаштування" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;
        }

        // Звільнення ресурсів
        delete pstmt;
        delete res;

        // Очікування введення користувачем будь-якої клавіші перед завершенням програми
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore(); // Ігноруємо попередній ввід
        cin.get();    // Очікування на новий ввід від користувача
    }
    // Обробка помилок бази даних
    catch (const SQLException& e) {
        cout << "Помилка під час виконання запиту до бази даних: " << e.what() << endl;
    }
    // Обробка інших винятків
    catch (const std::exception& ex) {
        cout << "Сталася помилка: " << ex.what() << endl;
    }
}
