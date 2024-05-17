#include "hotelconnecting.h"
#include "guest.h"
#include <iostream>
#include <iomanip>
#include "utils.h"

using namespace std;

Guest::Guest() : firstName("none"), lastName("none"), gender("none"), age(0), phoneNumber("none"), guestId(0), roomId(0), roomNumber(0), checkInDate("none"), hotel(*(new HotelConnecting())) {}

Guest::Guest(HotelConnecting& h) : hotel(h) {}

void Guest::displayGuests() {
    sql::Connection& conn = hotel.getConnection(); // Отримання з'єднання з базою даних через об'єкт готелю

    try {
        while (true) {
            cout << "~~ Введіть ідентифікатор гостя або введіть 0, щоб вивести всіх ~~" << endl;
            string guestIdStr;
            cout << "ID гостя: ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, guestIdStr);

            if (isInputEmpty(guestIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор гостя або 0." << endl;
                continue; // Починаємо цикл знову
            }

            if (guestIdStr == "0") {
                // Виведення всіх гостей, якщо введено 0
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Немає записів для відображення." << endl;
                }
                else {
                    // Виведення заголовків таблиці
                    cout << setw(10) << "ID" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(10) << "Стать" << setw(5) << "Вік" << setw(22) << "Номер телефону" << setw(15) << " Дата заїзду" << setw(15) << " Номер кімнати" << endl;
                    // Виведення даних кожного гостя
                    do {
                        cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice(); // Отримання коректного вибору від користувача

                if (choice == 'y' || choice == 'Y') {
                    return; // Повертаємось назад без виведення гостей, якщо користувач вибрав 'n' або 'N'
                }
            }
            else {
                int guestId;
                try {
                    guestId = stoi(guestIdStr); // Конвертація рядка у ціле число
                }
                catch (const std::invalid_argument& ia) {
                    cout << "Помилка: Введений ідентифікатор гостя має бути числом або 0." << endl;
                    continue; // Починаємо цикл знову
                }

                PreparedStatement* pstmt = con->prepareStatement("SELECT GuestId, FirstName, LastName, Gender, Age, PhoneNumber, CheckInDate, RoomNumber FROM guests WHERE GuestID = ?");
                pstmt->setInt(1, guestId);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Помилка: Гостя з введеним ID не знайдено." << endl;
                }
                else {
                    // Виведення заголовків таблиці
                    cout << setw(10) << "ID" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(10) << "Стать" << setw(5) << "Вік" << setw(22) << "Номер телефону" << setw(15) << " Дата заїзду" << setw(15) << " Номер кімнати" << endl;
                    // Виведення даних гостя з введеним ID
                    cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice(); // Отримання коректного вибору від користувача

                if (choice == 'y' || choice == 'Y') {
                    return; // Повертаємось назад без виведення гостей, якщо користувач вибрав 'n' або 'N'
                }
            }
        }
    }
    catch (SQLException& e) {
        // Обробка винятку SQLException
        cout << "Помилка SQL: " << e.what() << endl;
    }
}


bool Guest::isRoomAvailable(int roomNumber) {
    try {
        if (roomNumber >= 101 && roomNumber <= 111) {
            return false;
        }

        PreparedStatement* pstmt = con->prepareStatement("SELECT COUNT(*) AS count FROM rooms WHERE RoomNumber = ?");
        pstmt->setInt(1, roomNumber);
        ResultSet* res = pstmt->executeQuery();

        res->next();
        int roomCount = res->getInt("count");

        delete pstmt;
        delete res;

        return roomCount > 0;
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
        return false;
    }
}

void Guest::addGuest() {
    try {
        while (true) {
            // Запит користувача на введення даних нового гостя
            cout << "~~ Додати нового гостя ~~" << endl;

            // Введення та перевірка імені
            do {
                cin.ignore();
                cout << "Ім'я: ";
                getline(cin, firstName);

                if (isInputEmpty(firstName)) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ім'я." << endl;
                }
                else if (!isLetters(firstName)) {
                    cout << "Помилка: Ім'я повинно містити лише літери." << endl;
                }
            } while (isInputEmpty(firstName) || !isLetters(firstName));

            // Введення та перевірка прізвища
            do {
                cout << "Прізвище: ";
                getline(cin, lastName);
                if (isInputEmpty(lastName)) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть прізвище." << endl;
                }
                else if (!isLetters(lastName)) {
                    cout << "Помилка: Прізвище повинно містити лише літери." << endl;
                }
            } while (isInputEmpty(lastName) || !isLetters(lastName));

            // Введення та перевірка статі
            do {
                cout << "Стать (чоловіча/жіноча): ";
                getline(cin, gender);

                if (isInputEmpty(gender)) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть 'чоловіча' або 'жіноча'." << endl;
                }
                else if (gender != "чоловіча" && gender != "жіноча") {
                    cout << "Помилка: Неправильно введена стать. Будь ласка, введіть 'чоловіча' або 'жіноча'." << endl;
                }
            } while (isInputEmpty(gender) || (gender != "чоловіча" && gender != "жіноча"));

            // Введення та перевірка віку
            string ageStr;
            do {
                cout << "Вік: ";
                getline(cin, ageStr);

                if (isInputEmpty(ageStr)) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть вік." << endl;
                }
                else if (!all_of(ageStr.begin(), ageStr.end(), ::isdigit)) {
                    cout << "Помилка: Вік має бути введений тільки цифрою." << endl;
                }
                else {
                    age = stoi(ageStr);
                    if (!isValidAge(age)) {
                        cout << "Помилка: Неправильно введений вік. Будь ласка, введіть коректний вік від 0 до 125." << endl;
                    }
                }
            } while (isInputEmpty(ageStr) || !all_of(ageStr.begin(), ageStr.end(), ::isdigit) || !isValidAge(age));

            // Введення та перевірка номера телефону
            do {
                cout << "Номер телефону (мінімум 8 символів): ";
                getline(cin, phoneNumber);

                if (isInputEmpty(phoneNumber)) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть номер телефону." << endl;
                }
                else if (phoneNumber.length() < 8) {
                    cout << "Помилка: Номер телефону повинен містити щонайменше 8 цифр." << endl;
                }
                else if (!isValidPhoneNumber(phoneNumber)) {
                    cout << "Помилка: Номер телефону повинен містити тільки цифри." << endl;
                }
            } while (isInputEmpty(phoneNumber) || phoneNumber.length() < 8 || !isValidPhoneNumber(phoneNumber));

            // Введення та перевірка дати заїзду
            bool validDate = false;
            do {
                cout << "Дата заїзду (рік-місяць-день): ";
                getline(cin, checkInDate);
                // Перевірка правильності формату дати
                bool validFormat = true;
                if (checkInDate.length() != 10) {
                    validFormat = false;
                }
                else {
                    for (int i = 0; i < 10; ++i) {
                        if (i == 4 || i == 7) {
                            if (checkInDate[i] != '-') {
                                validFormat = false;
                                break;
                            }
                        }
                        else if (!isdigit(checkInDate[i])) {
                            validFormat = false;
                            break;
                        }
                    }
                }

                if (validFormat) {
                    // Розділення рядка дати на окремі частини (рік, місяць, день)
                    int year = stoi(checkInDate.substr(0, 4));
                    int month = stoi(checkInDate.substr(5, 2));
                    int day = stoi(checkInDate.substr(8, 2));

                    // Перевірка правильності дати
                    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
                        validDate = true;
                    }
                    else {
                        cout << "Помилка: Неправильна дата. Будь ласка, введіть коректну дату." << endl;
                    }
                }
                else {
                    cout << "Помилка: Неправильний формат дати. Будь ласка, введіть дату у форматі 'рік-місяць-день'." << endl;
                }
            } while (isInputEmpty(checkInDate) || !validDate);

            // Введення та перевірка номера кімнати
            string roomNumberStr;
            do {
                cout << "Номер кімнати: ";
                getline(cin, roomNumberStr);

                if (!isNumber(roomNumberStr)) {
                    cout << "Помилка: Неправильний формат. Будь ласка, введіть номер кімнати числом." << endl;
                }
            } while (isInputEmpty(roomNumberStr) || !isNumber(roomNumberStr));

            roomNumber = stoi(roomNumberStr);

            // Перевірка доступності кімнати
            if (!isRoomAvailable(roomNumber)) {
                cout << "Помилка: Кімната з номером " << roomNumber << " зайнята." << endl;
                continue;
            }

            // Додавання гостя до бази даних
            PreparedStatement* pstmt = con->prepareStatement("INSERT INTO guests (FirstName, LastName, Gender, Age, PhoneNumber, CheckInDate, RoomNumber) VALUES (?, ?, ?, ?, ?, ?, ?)");
            pstmt->setString(1, firstName);
            pstmt->setString(2, lastName);
            pstmt->setString(3, gender);
            pstmt->setInt(4, age);
            pstmt->setString(5, phoneNumber);
            pstmt->setString(6, checkInDate);
            pstmt->setInt(7, roomNumber);

            int rowsAffected = pstmt->executeUpdate();

            // Повідомлення про результат додавання гостя
            if (rowsAffected == 0) {
                cout << "Помилка: Нового гостя не було додано до бази даних." << endl;
            }
            else {
                cout << "Нового гостя успішно додано!" << endl;
            }

            delete pstmt;

            // Перевірка бажання користувача продовжити
            char choice = getChoice();

            do {
                cout << "Хочете додати ще? (y/n): ";
                cin >> choice;
                if (choice != 'y' && choice != 'n') {
                    cout << "Помилка: Будь ласка, введіть 'y' або 'n'." << endl;
                }
            } while (choice != 'y' && choice != 'n');

            if (choice == 'n') {
                return;
            }

        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

void Guest::deleteGuest() {
    try {
        while (true) {
            // Запит користувача на введення ідентифікатора гостя для видалення
            string guestIdStr;
            cout << "Введіть ідентифікатор гостя, якого ви хочете видалити: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, guestIdStr);

            // Перевірка чи введено ідентифікатор гостя
            if (isInputEmpty(guestIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор гостя." << endl;
                continue; // Починаємо цикл знову
            }

            // Конвертація рядка введеного ідентифікатора в ціле число
            int guestId;
            try {
                guestId = stoi(guestIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ідентифікатор гостя має бути числом." << endl;
                continue; // Починаємо цикл знову
            }

            // Підготовка SQL запиту для отримання інформації про гостя за його ідентифікатором
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests WHERE GuestId = ?");
            pstmt->setInt(1, guestId);
            ResultSet* res = pstmt->executeQuery();

            // Перевірка чи існує гість з введеним ідентифікатором
            if (!res->next()) {
                cout << "Помилка: Гостя з введеним ID не знайдено." << endl;
            }
            else {
                // Виведення інформації про гостя
                cout << "Вибраний гість для видалення:" << endl;
                cout << setw(10) << "ID" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(10) << "Стать" << setw(5) << "Вік" << setw(22) << "Номер телефону" << setw(15) << " Дата заїзду" << setw(15) << " Номер кімнати" << endl;
                cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;

                // Підтвердження видалення гостя користувачем
                char confirmDelete;
                cout << "Ви впевнені, що хочете видалити цього гостя? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // Видалення гостя з бази даних
                    pstmt = con->prepareStatement("DELETE FROM guests WHERE GuestId = ?");
                    pstmt->setInt(1, guestId);
                    int rowsAffected = pstmt->executeUpdate();

                    // Повідомлення про результат видалення
                    if (rowsAffected > 0) {
                        cout << "Гостя успішно видалено." << endl;
                    }
                    else {
                        cout << "Помилка: Гостя з вказаним ID не знайдено." << endl;
                    }
                }
                else {
                    cout << "Видалення гостя скасовано." << endl;
                }
            }

            // Звільнення ресурсів
            delete pstmt;
            delete res;

            // Запит користувача про бажання видалити ще одного гостя
            char choice;
            cout << "Хочете видалити ще одного гостя? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // Виходимо з циклу, якщо користувач не хоче видаляти ще гостя
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка видалення гостя: " << e.what() << endl;
    }
}

void Guest::updateGuest() {
    try {
        while (true) {
            string guestIdStr;
            // Запитати користувача про ідентифікатор гостя для оновлення
            cout << "Введіть ідентифікатор гостя, якому потрібно оновити інформацію (або 0, щоб повернутися до головного меню): ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, guestIdStr);

            if (guestIdStr == "0") {
                break; // Виходимо з циклу, якщо користувач ввів 0
            }

            if (isInputEmpty(guestIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор гостя." << endl;
                continue; // Починаємо цикл знову
            }

            int guestId;
            try {
                guestId = stoi(guestIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ідентифікатор гостя має бути числом." << endl;
                continue; // Починаємо цикл знову
            }

            // Підготовка SQL-запиту для отримання даних про гостя з введеним ідентифікатором
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests WHERE GuestId = ?");
            pstmt->setInt(1, guestId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Гостя з введеним ID не знайдено." << endl;
            }
            else {
                // Виведення інформації про обраного гостя
                cout << "Вибраний гість для оновлення інформації:" << endl;
                cout << setw(10) << "ID" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(10) << "Стать" << setw(5) << "Вік" << setw(22) << "Номер телефону" << setw(15) << " Дата заїзду" << setw(15) << " Номер кімнати" << endl;
                cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;

                char confirmUpdate;
                cout << "Ви впевнені, що хочете оновити інформацію про цього гостя? (y/n): ";
                cin >> confirmUpdate;

                if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                    // Виведення меню для вибору, яку інформацію оновити
                    cout << "Яку інформацію ви хочете оновити?" << endl;
                    cout << "1. Ім'я" << endl;
                    cout << "2. Прізвище" << endl;
                    cout << "3. Стать" << endl;
                    cout << "4. Вік" << endl;
                    cout << "5. Номер телефону" << endl;
                    cout << "6. Дата заїзду" << endl;
                    cout << "7. Номер кімнати" << endl;
                    cout << "Виберіть опцію (введіть номер від 1 до 7): ";

                    int updateOption;
                    cin >> updateOption;

                    if (cin.fail() || updateOption < 0 || updateOption > 7) {
                        system("cls");
                        cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 7." << endl;
                        cin.clear(); // Скидаємо флаг помилки
                        cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер

                        continue; // Повертаємося до попереднього меню
                    }

                    switch (updateOption) {
                    case 1: {
                        do {
                            cin.ignore();
                            cout << "Нове ім'я: ";
                            getline(cin, firstName);

                            if (!isLetters(firstName)) {
                                cout << "Помилка: Ім'я повинно містити лише літери.\nНатисніть будь-яку клавішу для продовження..." << endl;
                            }
                        } while (isInputEmpty(firstName) || !isLetters(firstName));

                        // Оновлення інформації про ім'я гостя
                        pstmt = con->prepareStatement("UPDATE guests SET FirstName = ? WHERE GuestId = ?");
                        pstmt->setString(1, firstName);
                        pstmt->setInt(2, guestId);
                        pstmt->executeUpdate();
                        cout << "Ім'я гостя оновлено успішно." << endl;
                        break;
                    }
                    case 2: {
                        do {
                            cin.ignore();
                            cout << "Нове прізвище: ";
                            getline(cin, lastName);

                            if (!isLetters(lastName)) {
                                cout << "Помилка: Прізвище повинно містити лише літери.\nНатисніть будь-яку клавішу для продовження..." << endl;
                            }
                        } while (isInputEmpty(lastName) || !isLetters(lastName));
                        // Оновлення інформації про прізвище гостя
                        pstmt = con->prepareStatement("UPDATE guests SET LastName = ? WHERE GuestId = ?");
                        pstmt->setString(1, lastName);
                        pstmt->setInt(2, guestId);
                        pstmt->executeUpdate();
                        cout << "Прізвище гостя оновлено успішно." << endl;
                        break;
                    }
                    case 3: {
                        do {
                            cout << "Нова стать (чоловіча/жіноча): ";
                            cin.clear(); // Скидаємо флаг помилки
                            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                            getline(cin, gender);

                            if (isInputEmpty(gender)) {
                                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть стать." << endl;
                                continue;
                            }
                            if (!checkGenderInput(gender)) {
                                cout << "Помилка: Неправильно введена стать. Будь ласка, введіть 'чоловіча' або 'жіноча'." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильне значення
                            }
                            // Оновлення інформації про стать гостя
                            pstmt = con->prepareStatement("UPDATE guests SET Gender = ? WHERE GuestId = ?");
                            pstmt->setString(1, gender);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "Стать гостя оновлено успішно." << endl;
                            break; // Вихід із циклу, якщо введено правильне значення
                        } while (true); // Повторюємо цикл, доки користувач не введе коректне значення
                        break;
                    }
                    case 4: {
                        do {
                            string newAgeStr;
                            cout << "Новий вік: ";
                            cin.clear(); // Скидаємо флаг помилки
                            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                            getline(cin, newAgeStr);
                            // Оновлення інформації про вік гостя
                            if (isInputEmpty(newAgeStr)) {
                                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть вік." << endl;
                                continue; // Починаємо цикл знову, якщо введено порожнє значення
                            }
                            if (!isNumber(newAgeStr)) {
                                cout << "Помилка: Вік має бути введений тільки цифрою." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильне значення
                            }
                            long long newAge = stoll(newAgeStr); // Використовуємо stoll для обробки великих чисел
                            if (!isValidAge(newAge)) {
                                cout << "Помилка: Неправильно введений вік. Будь ласка, введіть коректний вік від 0 до 125." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильне значення
                            }
                            pstmt = con->prepareStatement("UPDATE guests SET Age = ? WHERE GuestId = ?");
                            pstmt->setInt(1, newAge);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "Вік гостя оновлено успішно." << endl;
                            break; // Вихід із циклу, якщо введено правильне значення
                        } while (true); // Повторюємо цикл, доки користувач не введе коректне значення
                        break;
                    }

                    case 5: {
                        do {
                            string newPhoneNumber;
                            cout << "Новий номер телефону (мінімум 8 символів): ";
                            cin.clear(); // Скидаємо флаг помилки
                            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                            getline(cin, newPhoneNumber);
                            // Оновлення інформації про номер телефону гостя
                            if (isInputEmpty(newPhoneNumber)) {
                                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть номер телефону." << endl;
                                continue; // Починаємо цикл знову, якщо введено порожнє значення
                            }
                            if (newPhoneNumber.length() < 8 || !isValidPhoneNumber(newPhoneNumber)) {
                                cout << "Помилка: Номер телефону повинен містити щонайменше 8 цифр і містити тільки цифри." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильне значення
                            }
                            pstmt = con->prepareStatement("UPDATE guests SET PhoneNumber = ? WHERE GuestId = ?");
                            pstmt->setString(1, newPhoneNumber);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "Номер телефону гостя оновлено успішно." << endl;
                            break; // Вихід із циклу, якщо введено правильне значення
                        } while (true); // Повторюємо цикл, доки користувач не введе коректне значення
                        break;
                    }
                    case 6: {
                        do {
                            string newCheckInDate;
                            cout << "Нова дата заїзду (рік-місяць-день): ";
                            cin.clear(); // Скидаємо флаг помилки
                            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                            getline(cin, newCheckInDate);
                            if (isInputEmpty(newCheckInDate)) {
                                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть дату заїзду." << endl;
                                continue; // Починаємо цикл знову, якщо введено порожнє значення
                            }
                            if (!isValidDateFormat(newCheckInDate)) {
                                cout << "Помилка: Неправильний формат дати. Введіть дату у форматі 'рік-місяць-день'." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильний формат дати
                            }
                            // Ваша перевірка на валідність дати (isValidDate) тут

                            // Якщо досягли цього моменту, значить дата введена коректно
                            pstmt = con->prepareStatement("UPDATE guests SET CheckInDate = ? WHERE GuestId = ?");
                            pstmt->setString(1, newCheckInDate);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "Дата заїзду гостя оновлено успішно." << endl;
                            break; // Вихід із циклу, якщо введено правильне значення
                        } while (true); // Повторюємо цикл, доки користувач не введе коректне значення
                        break;
                    }
                    case 7: {
                        do {
                            string newRoomNumberStr;
                            cout << "Новий номер кімнати: ";
                            cin.clear(); // Скидаємо флаг помилки
                            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                            getline(cin, newRoomNumberStr);
                            // Оновлення інформації про номер кімнати гостя
                            if (!isNumber(newRoomNumberStr)) {
                                cout << "Помилка: Номер кімнати має бути введений тільки цифрою." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильне значення
                            }
                            if (isInputEmpty(newRoomNumberStr)) {
                                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть номер кімнати." << endl;
                                continue; // Починаємо цикл знову, якщо введено порожнє значення
                            }
                            int newRoomNumber = stoi(newRoomNumberStr);
                            if (!isRoomAvailable(newRoomNumber)) {
                                cout << "Помилка: Кімната з номером " << newRoomNumber << " зайнята або не існує." << endl;
                                continue; // Починаємо цикл знову, якщо введено неправильне значення
                            }
                            pstmt = con->prepareStatement("UPDATE guests SET RoomNumber = ? WHERE GuestId = ?");
                            pstmt->setInt(1, newRoomNumber);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "Номер кімнати гостя оновлено успішно." << endl;
                            break; // Вихід із циклу, якщо введено правильне значення
                        } while (true); // Повторюємо цикл, доки користувач не введе коректне значення
                        break;
                    }

                    default: {
                        cout << "Помилка: Введено неправильний номер опції." << endl;
                        break;
                    }
                    }
                }
                else {
                    cout << "Оновлення інформації про гостя скасовано." << endl;
                }

                delete pstmt;
                delete res;

                char choice;
                cout << "Хочете оновити інформацію ще одного гостя? (y/n): ";
                cin >> choice;

                // Перевірка, чи користувач ввів правильний символ
                if (choice != 'y' && choice != 'n') {
                    cout << "Помилка: Будь ласка, введіть 'y' або 'n'." << endl;
                    continue; // Починаємо цикл знову
                }

                if (choice == 'n' || choice == 'N') {
                    break; // Виходимо з циклу, якщо користувач не хоче оновлювати інформацію ще одного гостя
                }
            }
        }
    }
    catch (SQLException& e) {
        // Обробка винятку SQLException
        cout << "Помилка оновлення інформації про гостя: " << e.what() << endl;
    }
}


void Guest::sortGuests() {
    try {
        cout << "Оберіть критерій сортування:" << endl;
        cout << "1. Сортування імен за алфавітним порядком(від \"А\" до \"Я\")." << endl;
        cout << "2. Сортування прізвищ за алфавітним порядком(від \"А\" до \"Я\")." << endl;
        cout << "3. Від наймолодшого до найстаршого." << endl;
        cout << "4. Від найстаршого до наймолодшого." << endl;
        cout << "5. За датою заїзду." << endl;
        cout << "0. Повернутися до попереднього меню." << endl;
        cout << "Ваш вибір: ";

        int sortingCriterion;
        do {
            cin >> sortingCriterion;

            if (isInputEmpty(to_string(sortingCriterion))) {
                cout << "Помилка: Критерій сортування не може бути порожнім. Будь ласка, введіть цифру від 0 до 5: ";
            }
            else if (cin.fail() || sortingCriterion < 0 || sortingCriterion > 5) {
                system("cls");
                cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 5: ";
                cin.clear(); // Скидаємо флаг помилки
                cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            }
        } while (isInputEmpty(to_string(sortingCriterion)) || cin.fail() || sortingCriterion < 0 || sortingCriterion > 5);


        string orderByClause;
        switch (sortingCriterion) {
        case 1:
            orderByClause = "ORDER BY FirstName";
            break;
        case 2:
            orderByClause = "ORDER BY LastName";
            break;
        case 3:
            orderByClause = "ORDER BY Age";
            break;
        case 4:
            orderByClause = "ORDER BY Age DESC";
            break;
        case 5:
            orderByClause = "ORDER BY CheckInDate";
            break;
        case 0:
            return; // Повернутися до попереднього меню
        default:
            cout << "Невірний вибір. Будь ласка, введіть число від 0 до 5." << endl;
            return;
        }

        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // Виведення відсортованого списку гостей
        cout << setw(10) << "ID" << setw(15) << "Ім'я" << setw(15) << "Прізвище" << setw(10) << "Стать" << setw(5) << "Вік" << setw(22) << "Номер телефону" << setw(15) << " Дата заїзду" << setw(15) << " Номер кімнати" << endl;
        while (res->next()) {
            cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;
        }

        delete pstmt;
        delete res;

        // Зачекати, доки користувач не натисне будь-яку клавішу, перед завершенням програми
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore(); // Ігноруємо попередній ввід
        cin.get();    // Очікуємо на новий ввід від користувача
    }
    catch (const SQLException& e) {
        cout << "Помилка під час виконання запиту до бази даних: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "Сталася помилка: " << ex.what() << endl;
    }
}



