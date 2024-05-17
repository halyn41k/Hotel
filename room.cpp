#include "room.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "hotelconnecting.h"
#include "utils.h"
#include <sstream>

Room::Room() : roomNumber(0), roomType("none"), status("none"), capacity(0), price(0.0), hotel(*(new HotelConnecting())) {}
Room::Room(HotelConnecting& h) : hotel(h) {}

// Метод виведення інформації про кімнати
void Room::displayRooms() {
    try {
        while (true) {
            cout << "~~ Введіть номер кімнати або введіть 0, щоб вивести всі ~~" << endl;
            string roomNumberStr;
            cout << "Номер кімнати: ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, roomNumberStr);

            if (isInputEmpty(roomNumberStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть номер кімнати або 0." << endl;
                continue; // Починаємо цикл знову
            }

            if (roomNumberStr == "0") {
                // Виведення інформації про всі кімнати, якщо введено 0
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Немає записів для відображення." << endl;
                }
                else {
                    // Виведення заголовків таблиці
                    cout << setw(10) << "Номер кімнати" << setw(17) << "Тип" << setw(15) << "Статус" << setw(15) << "Вмістимість" << setw(10) << "Ціна" << endl;
                    // Виведення даних кожної кімнати
                    do {
                        cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // Повертаємось назад без виведення кімнат, якщо користувач вибрав 'n' або 'N'
                }
            }
            else {
                int roomNumber;
                try {
                    roomNumber = stoi(roomNumberStr); // Конвертація рядка у ціле число
                }
                catch (const std::invalid_argument& ia) {
                    cout << "Помилка: Введений номер кімнати має бути числом або 0." << endl;
                    continue; // Починаємо цикл знову
                }

                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
                pstmt->setInt(1, roomNumber);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Помилка: Кімнати з введеним номером не знайдено." << endl;
                }
                else {
                    // Виведення заголовків таблиці
                    cout << setw(10) << "Номер кімнати" << setw(17) << "Тип" << setw(15) << "Статус" << setw(15) << "Вмістимість" << setw(10) << "Ціна" << endl;
                    // Виведення даних кімнати з введеним номером
                    cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice(); // Отримання коректного вибору від користувача

                if (choice == 'y' || choice == 'Y') {
                    return; // Повертаємось назад без виведення кімнат, якщо користувач вибрав 'n' або 'N'
                }
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl; // Обробка винятку SQLException
    }
}


bool Room::isNumber(string str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }

}
// Метод додавання нової кімнати
void Room::addRoom() {
    try {
        cout << "~~ Додати нову кімнату ~~" << endl;

        // Перевірка типу кімнати
        do {
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            cout << "Тип кімнати (Стандартна/Люкс/Президентська): ";
            getline(cin,roomType);

            if (roomType != "Стандартна" && roomType != "Люкс" && roomType != "Президентська") {
                cout << "Помилка: Неправильно введений тип кімнати. Будь ласка, введіть 'Стандартна', 'Люкс' або 'Президентська'." << endl;
            }
        } while (isInputEmpty(roomType) || roomType != "Стандартна" && roomType != "Люкс" && roomType != "Президентська");

        status = "Доступна";

        // Перевірка вмістимості кімнати
        do {
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            cout << "Вмістимість (від 1 до 4): ";
            string capacityInput;
            getline(cin, capacityInput);
            stringstream capacityStream(capacityInput);
            capacityStream >> capacity;

            if (capacity < 1 || capacity > 4) {
                cout << "Помилка: Вмістимість кімнати повинна бути від 1 до 4." << endl;
            }
        } while (isInputEmpty(to_string(capacity)) || capacity < 1 || capacity > 4);

        bool validPrice = false; // Флаг для перевірки коректності введення ціни

        do {
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            cout << "Ціна за ніч (від 10 до 100000): ";
            string priceInput;
            getline(cin, priceInput);
            stringstream priceStream(priceInput);
            priceStream >> price;

            if (cin.fail() || price < 10 || price > 100000) {
                cin.clear(); // Очистка флага помилки вводу
                cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
                cout << "Помилка: Ціна за ніч повинна бути числом від 10 до 100000." << endl;
            }
            else {
                validPrice = true;
            }
        } while (isInputEmpty(to_string(price)) || !validPrice);

        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO rooms (RoomType, Status, Capacity, Price) VALUES (?, ?, ?, ?)");
        pstmt->setString(1, roomType);
        pstmt->setString(2, status);
        pstmt->setInt(3, capacity);
        pstmt->setDouble(4, price);

        pstmt->executeUpdate();

        cout << "Нову кімнату успішно додано!" << endl;

        delete pstmt;
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // Повертаємось назад без виведення гостей, якщо користувач вибрав 'n' або 'N'
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

void Room::deleteRoom() {
    try {
        while (true) {
            string roomNumberStr;
            cout << "Введіть номер кімнати, яку ви хочете видалити: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, roomNumberStr);

            if (roomNumberStr == "0") {
                break; // Виходимо з циклу, якщо користувач ввів 0
            }
            if (isInputEmpty(roomNumberStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть номер кімнати." << endl;
                continue; // Починаємо цикл знову
            }

            int roomNumber;
            try {
                roomNumber = stoi(roomNumberStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений номер кімнати має бути числом або 0." << endl;
                continue; // Починаємо цикл знову
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
            pstmt->setInt(1, roomNumber);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Кімнати з введеним номером не знайдено." << endl;
            }
            else {
                cout << "Вибрана кімната для видалення:" << endl;
                cout << setw(10) << "Номер кімнати" << setw(17) << "Тип" << setw(15) << "Статус" << setw(15) << "Вмістимість" << setw(10) << "Ціна" << endl;
                cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;

                char confirmDelete;
                cout << "Ви впевнені, що хочете видалити цю кімнату? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // Видалення з таблиці RoomCleaningStatus
                    pstmt = con->prepareStatement("DELETE FROM RoomCleaningStatus WHERE RoomNumber = ?");
                    pstmt->setInt(1, roomNumber);
                    pstmt->executeUpdate();
                    delete pstmt;

                    // Видалення з таблиці rooms
                    pstmt = con->prepareStatement("DELETE FROM rooms WHERE RoomNumber = ?");
                    pstmt->setInt(1, roomNumber);
                    int rowsAffected = pstmt->executeUpdate();
                    delete pstmt;

                    if (rowsAffected > 0) {
                        cout << "Кімнату успішно видалено." << endl;
                    }
                    else {
                        cout << "Помилка: Кімнату з вказаним номером не знайдено." << endl;
                    }
                }
                else {
                    cout << "Видалення кімнати скасовано." << endl;
                }
            }

            delete res;

            char choice;
            cout << "Хочете видалити ще одну кімнату? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // Виходимо з циклу, якщо користувач не хоче видаляти ще кімнату
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка видалення кімнати: " << e.what() << endl;
    }
}


// Метод оновлення інформації про кімнату
void Room::updateRoom() {
    try {
        while (true) {
            string roomNumberStr;
            // Просимо користувача ввести номер кімнати для оновлення
            cout << "Введіть номер кімнати, яку ви хочете оновити (або 0, щоб повернутися до головного меню): ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, roomNumberStr);

            // Перевіряємо, чи користувач не вибрав повернення до головного меню
            if (roomNumberStr == "0") {
                break; // Виходимо з циклу, якщо користувач ввів 0
            }
            // Перевіряємо, чи користувач ввів порожній рядок
            if (isInputEmpty(roomNumberStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть номер кімнати." << endl;
                continue; // Починаємо цикл знову
            }

            int roomNumber;
            try {
                roomNumber = stoi(roomNumberStr);
            }
            // Обробка винятків, якщо введено некоректне число
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений номер кімнати має бути числом або 0." << endl;
                continue; // Починаємо цикл знову
            }

            // Підготовка запиту до бази даних для отримання інформації про кімнату за введеним номером
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
            pstmt->setInt(1, roomNumber);
            ResultSet* res = pstmt->executeQuery();

            // Перевірка, чи існує кімната з введеним номером
            if (!res->next()) {
                cout << "Помилка: Кімнати з введеним номером не знайдено." << endl;
            }
            else {
                // Виведення інформації про кімнату для оновлення
                cout << "Вибрана кімната для оновлення:" << endl;
                cout << setw(10) << "Номер кімнати" << setw(17) << "Тип" << setw(15) << "Статус" << setw(15) << "Вмістимість" << setw(10) << "Ціна" << endl;
                cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;

                // Вибір параметра для оновлення
                cout << "Оберіть параметр для оновлення:" << endl;
                cout << "1. Тип кімнати" << endl;
                cout << "2. Статус кімнати" << endl;
                cout << "3. Вмістимість кімнати" << endl;
                cout << "4. Ціна за ніч" << endl;
                cout << "0. Повернутися до головного меню" << endl;
                int updateChoice;
                cout << "Ваш вибір: ";
                cin >> updateChoice;

                // Перевірка вибору користувача
                if (isInputEmpty(to_string(updateChoice))) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                    continue; // Починаємо цикл знову
                }

                // Обробка вибору користувача для оновлення
                switch (updateChoice) {
                case 0:
                    break; // Повертаємося до головного меню
                case 1:
                    // Оновлення типу кімнати
                    do {
                        cout << "Введіть новий тип кімнати (Стандартна/Люкс/Президентська): ";
                        cin.clear(); // Скидаємо флаг помилки
                        cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                        getline(cin, roomType);
                        // Перевірка коректності введеного типу кімнати
                        if (roomType != "Стандартна" && roomType != "Люкс" && roomType != "Президентська") {
                            cout << "Помилка: Введено неправильний тип кімнати. Виберіть зі списку." << endl;
                            continue; // Починаємо цикл знову
                        }
                        // Перевірка на порожність поля
                        if (isInputEmpty(roomType)) {
                            cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                            continue; // Починаємо цикл знову
                        }
                        break; // Вихід із циклу, якщо введено правильне значення
                    } while (true);
                    break;
                case 2:
                    // Оновлення статусу кімнати
                    do {
                        cout << "Введіть новий статус кімнати (Зайнята/Доступна): ";
                        cin.clear(); // Скидаємо флаг помилки
                        cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                        getline(cin, status);
                        // Перевірка коректності введеного статусу кімнати
                        if (status != "Зайнята" && status != "Доступна") {
                            cout << "Помилка: Введено неправильний статус кімнати. Виберіть зі списку." << endl;
                            continue; // Починаємо цикл знову
                        }
                        // Перевірка на порожність поля
                        if (isInputEmpty(roomType)) {
                            cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                            continue; // Починаємо цикл знову
                        }
                        break; // Вихід із циклу, якщо введено правильне значення
                    } while (true);
                    break;
                case 3:
                    // Оновлення вмістимості кімнати
                    do {
                        cout << "Введіть нову вмістимість кімнати (1-4): ";
                        string capacityInput;
                        cin.clear(); // Скидаємо флаг помилки
                        cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                        getline(cin, capacityInput);
                        stringstream capacityStream(capacityInput);
                        capacityStream >> capacity;
                        // Перевірка коректності введеної вмістимості кімнати
                        if (capacity < 1 || capacity > 4) {
                            cout << "Помилка: Введено неправильну вмістимість кімнати. Введіть число від 1 до 4." << endl;
                            continue; // Починаємо цикл знову
                        }
                        // Перевірка на порожність поля
                        if (isInputEmpty(roomType)) {
                            cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                            continue; // Починаємо цикл знову
                        }
                        break; // Вихід із циклу, якщо введено правильне значення
                    } while (true);
                    break;
                case 4:
                    // Оновлення ціни за ніч
                    do {
                        cin.clear(); // Скидаємо флаг помилки
                        cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                        cout << "Введіть нову ціну за ніч (від 10 до 100000): ";
                        string priceInput;
                        getline(cin, priceInput);
                        stringstream priceStream(priceInput);
                        priceStream >> price;
                        // Перевірка коректності введеної ціни за ніч
                        if (price < 10 || price > 100000) {
                            cout << "Помилка: Введено неправильну ціну за ніч. Введіть число від 10 до 100000." << endl;
                            continue; // Починаємо цикл знову
                        }
                        // Перевірка на порожність поля
                        if (isInputEmpty(roomType)) {
                            cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                            continue; // Починаємо цикл знову
                        }
                        break; // Вихід із циклу, якщо введено правильне значення
                    } while (true);
                    break;
                default:
                    // Виведення повідомлення про некоректний вибір
                    cout << "Будь ласка, введіть коректний варіант (0-4)." << endl;
                    continue; // Починаємо цикл знову
                }

                // Підтвердження оновлення
                char confirmUpdate;
                cout << "Ви впевнені, що хочете оновити цю кімнату? (y/n): ";
                cin >> confirmUpdate;

                if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                    // Виконання запиту оновлення інформації про кімнату
                    pstmt = con->prepareStatement("UPDATE rooms SET RoomType = ?, Status = ?, Capacity = ?, Price = ? WHERE RoomNumber = ?");
                    pstmt->setString(1, roomType);
                    pstmt->setString(2, status);
                    pstmt->setInt(3, capacity);
                    pstmt->setDouble(4, price);
                    pstmt->setInt(5, roomNumber);

                    int rowsAffected = pstmt->executeUpdate();

                    if (rowsAffected > 0) {
                        cout << "Інформацію про кімнату успішно оновлено." << endl;
                    }
                    else {
                        cout << "Помилка: Не вдалося оновити інформацію про кімнату." << endl;
                    }
                }
                else {
                    cout << "Оновлення кімнати скасовано." << endl;
                }
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cerr << "Помилка SQL: " << e.what() << endl;
    }
}


void Room::sortRooms() {
    try {
        int sortingCriterion;
        // Виводимо меню для вибору критерію сортування кімнат
        cout << "Оберіть критерій сортування кімнат:" << endl;
        cout << "1. За номером кімнати." << endl;
        cout << "2. За типом кімнати." << endl;
        cout << "3. За статусом кімнати." << endl;
        cout << "4. За вмістимістю кімнати." << endl;
        cout << "5. За ціною кімнати." << endl;
        cout << "0. Повернутися до попереднього меню." << endl;
        cout << "Ваш вибір: ";

        // Перевіряємо, чи коректно введений критерій сортування
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("Помилка вводу. Будь ласка, введіть ціле число.");
        }

        // Перевірка на порожній ввід
        if (isInputEmpty(to_string(sortingCriterion))) {
            cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
            return;
        }

        string orderByClause;
        switch (sortingCriterion) {
        case 1:
            orderByClause = "ORDER BY RoomNumber";
            break;
        case 2:
            orderByClause = "ORDER BY RoomType";
            break;
        case 3:
            orderByClause = "ORDER BY Status";
            break;
        case 4:
            orderByClause = "ORDER BY Capacity";
            break;
        case 5:
            orderByClause = "ORDER BY Price";
            break;
        case 0:
            return; // Повертаємося до попереднього меню
        default:
            cout << "Невірний вибір. Будь ласка, введіть число від 0 до 5." << endl;
            return;
        }

        // Підготовка та виконання запиту до бази даних для відсортованого списку кімнат
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // Виведення відсортованого списку кімнат
        cout << setw(15) << "Номер кімнати" << setw(17) << "Тип" << setw(15) << "Статус" << setw(15) << "Вмістимість" << setw(10) << "Ціна" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;
        }

        delete pstmt;
        delete res;

        // Зачекати, доки користувач не натисне будь-яку клавішу, перед завершенням програми
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore(); // Ігноруємо попередній ввід
        cin.get();    // Очікуємо на новий ввід від користувача
    }
    // Обробка винятків
    catch (const SQLException& e) {
        cout << "Помилка під час виконання запиту до бази даних: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "Сталася помилка: " << ex.what() << endl;
    }
}

void Room::updateCleaningStatus() {
    int roomNumber;
    string cleaned;
    try {
        // Введення номера кімнати та перевірка на коректність вводу
        do {
            cout << "Введіть номер кімнати: ";
            cin >> roomNumber;

            // Перевірка, чи введено ціле число
            if (cin.fail()) {
                cout << "Помилка: Введений номер кімнати має бути числом або 0." << endl;
                cin.clear(); // Скидаємо прапорець помилки введення
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ігноруємо введений рядок
                continue;
            }

            // Перевірка, чи існує кімната з таким номером
            Statement* stmt = con->createStatement();
            ResultSet* res = stmt->executeQuery("SELECT * FROM RoomCleaningStatus WHERE RoomNumber = " + to_string(roomNumber));
            if (!res->next()) {
                cout << "Кімнати з номером " << roomNumber << " не існує." << endl;
                delete res;
                delete stmt;
                cout << "Спробуйте ще раз." << endl;
                continue;
            }
            delete res;
            delete stmt;

            break; // Вийти з циклу, якщо введено коректний номер кімнати

        } while (true);

        // Отримання статусу прибирання від користувача
        int choice;
        cout << "Введіть статус прибирання (0 - не прибрана, 1 - прибрана): ";
        cin >> choice;

        // Встановлення значення для поля "Cleaned" залежно від вибору користувача
        if (choice == 0) {
            cleaned = "не прибрана";
        }
        else if (choice == 1) {
            cleaned = "прибрана";
        }
        else {
            cout << "Неправильний ввід. Статус прибирання кімнати не оновлено." << endl;
            return;
        }

        // Оновлення статусу прибирання в базі даних
        PreparedStatement* pstmt = con->prepareStatement("UPDATE RoomCleaningStatus SET Cleaned=? WHERE RoomNumber=?");
        pstmt->setString(1, cleaned);
        pstmt->setInt(2, roomNumber);
        pstmt->executeUpdate();
        delete pstmt;
        cout << "Статус прибирання кімнати " << roomNumber << " оновлено." << endl;

        // Зачекати, доки користувач не натисне будь-яку клавішу, перед завершенням програми
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore(); // Ігноруємо попередній ввід
        cin.get();    // Очікуємо на новий ввід від користувача
    }
    // Обробка винятків
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        cerr << "SQL State: " << e.getSQLState() << endl;
    }
}



void Room::viewCleaningStatus() {
    try {
        // Створення звіту зі статусом прибирання кімнат
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery("SELECT * FROM RoomCleaningStatus");

        // Виведення заголовка звіту
        cout << "=============================" << endl;
        cout << " Статус прибирання кімнат:" << endl;
        cout << "=============================" << endl;
        cout << "Кімната\t\tСтатус" << endl;
        cout << "-----------------------------" << endl;

        // Виведення даних про статус прибирання кімнат
        while (res->next()) {
            int roomNum = res->getInt("RoomNumber");
            string cleaned = res->getString("Cleaned");
            cout << roomNum << "\t\t" << cleaned << endl;
        }

        delete res;
        delete stmt;

        // Зачекати, доки користувач не натисне будь-яку клавішу, перед завершенням програми
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore(); // Ігноруємо попередній ввід
        cin.get();    // Очікуємо на новий ввід від користувача
    }
    // Обробка винятків
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        cerr << "SQL State: " << e.getSQLState() << endl;
    }
}



