#include "booking.h"
#include "hotelconnecting.h"
#include "guest.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "utils.h"
#include <sstream>

Booking::Booking() : bookingId(0), roomNumber(0), guestId(0), bookingDate("none"), term("none") {}

void Booking::displayBookings() {
    try {
        while (true) {
            // Виведення повідомлення про введення ID бронювання або 0 для виведення всіх
            cout << "~~ Введіть ID бронювання або введіть 0, щоб вивести всі ~~" << endl;

            string bookingIdStr;
            cout << "ID бронювання: ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, bookingIdStr);


            // Перевірка на порожність та виклик функції isInputEmpty
            if (isInputEmpty(bookingIdStr)) {
                cout << "Помилка: ID бронювання не може бути порожнім." << endl;
                continue;
            }

            // Перевірка, чи введений рядок складається лише з цифр
            if (bookingIdStr.find_first_not_of("0123456789") != string::npos) {
                cout << "Помилка: Введений ID бронювання має бути числом або 0." << endl;
                continue;
            }

            int bookingId = stoi(bookingIdStr); // Перетворення рядка у ціле число

            // Запит до бази даних та відображення результатів
            PreparedStatement* pstmt;
            ResultSet* res;
            if (bookingId == 0) {
                // Виведення всіх бронювань
                pstmt = con->prepareStatement("SELECT * FROM bookings");
                res = pstmt->executeQuery();
            }
            else {
                // Виведення бронювання за заданим ID
                pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
                pstmt->setInt(1, bookingId);
                res = pstmt->executeQuery();
            }

            if (!res->next()) {
                cout << "Немає записів для відображення." << endl;
            }
            else {
                // Виведення заголовка таблиці
                cout << setw(15) << "ID бронювання" << setw(15) << "ID гостя" << setw(15) << "Номер кімнати" << setw(20) << "Дата бронювання" << setw(15) << "Термін" << endl;

                // Виведення даних про бронювання
                do {
                    cout << setw(15) << res->getInt("BookingId") << setw(15) << res->getInt("GuestId") << setw(15) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;
                } while (res->next());
            }

            delete pstmt;
            delete res;

            // Очікування вибору користувача для продовження або завершення
            char choice = getValidChoice();

            if (choice == 'y' || choice == 'Y') {
                return;
            }
        }
    }
    // Обробка винятків
    catch (const std::invalid_argument& ia) {
        cout << "Помилка: Введений ID бронювання має бути числом або 0." << endl;
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

void Booking::addBooking() {
    try {
        // Виведення повідомлення про додавання нового бронювання
        cout << "~~ Додати нове бронювання ~~" << endl;

        int guestId, roomNumber;
        string bookingDate, term;

        // Перевірка ID гостя
        bool validGuestId = false;
        while (!validGuestId) {
            cout << "ID гостя: ";
            cin.clear(); // Очищення флага помилки введення
            cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
            string guestIdInput;
            getline(cin, guestIdInput);

            stringstream guestIDStream(guestIdInput);
            guestIDStream >> guestId;

            if (cin.fail()) {
                cout << "Помилка: Введіть число для ID гостя." << endl;
                cin.clear(); // Очищення флага помилки введення
                cin.ignore(INT_MAX, '\n'); // Ігнорування неправильного вводу
            }
            else if (guestId == 0) {
                cout << "Помилка: ID гостя не може бути 0. Будь ласка, введіть коректний ID гостя." << endl;
            }
            else {
                if (!checkGuestId(guestId)) {
                    cout << "Помилка: Гостя з таким ID не існує." << endl;
                }
                else {
                    validGuestId = true;
                }
            }
        }

        // Перевірка наявності номера кімнати
        bool validRoomNumber = false;
        while (!validRoomNumber) {
            cout << "Номер кімнати: ";
            string roomNumberInput;
            cin.clear(); // Очищення флага помилки введення
            cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
            getline(cin, roomNumberInput);
            stringstream roomNumberStream(roomNumberInput);
            roomNumberStream >> roomNumber;
            if (!checkRoomNumber(roomNumber)) {
                cout << "Помилка: Кімната з таким номером не існує." << endl;
                cin.clear(); // Очищення флага помилки введення
                cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
            }
            else {
                validRoomNumber = true;
            }
        }

        // Перевірка дати бронювання
        bool validDate = false;
        do {
            cout << "Дата бронювання (рік-місяць-день): ";
            cin.clear(); // Очищення флага помилки введення
            cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
            getline(cin,bookingDate);

            if (isInputEmpty(bookingDate)) {
                cout << "Помилка: Дата бронювання не може бути порожньою." << endl;
            }
            else {
                if (!isValidDateFormat(bookingDate)) {
                    cout << "Помилка: Неправильний формат дати. Будь ласка, введіть дату у форматі 'рік-місяць-день'." << endl;
                }
                else {
                    validDate = true;
                }
            }
        } while (!validDate);

        // Перевірка терміну
        bool validTerm = false;
        do {
            cout << "Термін: ";
            cin.clear(); // Очищення флага помилки введення
            cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
            getline(cin, term);

            if (isInputEmpty(term)) {
                cout << "Помилка: Термін не може бути порожнім." << endl;
            }
            else {
                validTerm = true;
            }
        } while (!validTerm);

        // Додавання нового бронювання до бази даних
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO bookings (GuestId, RoomNumber, BookingDate, Term) VALUES (?, ?, ?, ?)");
        pstmt->setInt(1, guestId);
        pstmt->setInt(2, roomNumber);
        pstmt->setString(3, bookingDate);
        pstmt->setString(4, term);

        pstmt->executeUpdate();

        cout << "Нове бронювання успішно додано!" << endl;

        delete pstmt;

        // Очікування вибору користувача для продовження або завершення
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // Повернення без виведення гостей, якщо користувач вибрав 'n' або 'N'
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

// Перевірка існування гостя з введеним ID
bool Booking::checkGuestId(int guestId) {
    try {
        PreparedStatement* pstmt = con->prepareStatement("SELECT COUNT(*) AS count FROM guests WHERE GuestId = ?");
        pstmt->setInt(1, guestId);
        ResultSet* res = pstmt->executeQuery();

        res->next();
        int guestCount = res->getInt("count");

        delete pstmt;
        delete res;

        return guestCount > 0;
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
        return false;
    }
}


// Перевірка існування кімнати з введеним номером
bool Booking::checkRoomNumber(int roomNumber) {
    try {
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
void Booking::deleteBooking() {
    try {
        while (true) {
            string bookingIdStr;
            cout << "Введіть ID бронювання, яке ви хочете видалити: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, bookingIdStr);

            if (isInputEmpty(bookingIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть коректне значення." << endl;
                continue; // Починаємо цикл знову
            }

            if (bookingIdStr == "0") {
                break; // Виходимо з циклу, якщо користувач ввів 0
            }

            int bookingId;
            try {
                bookingId = stoi(bookingIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ID бронювання має бути числом або 0." << endl;
                continue; // Починаємо цикл знову
            }

            // Підготовка та виконання запиту до бази даних для отримання інформації про бронювання
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
            pstmt->setInt(1, bookingId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Бронювання з введеним ID не знайдено." << endl;
            }
            else {
                // Виведення інформації про обране бронювання для підтвердження видалення
                cout << "Вибране бронювання для видалення:" << endl;
                cout << setw(10) << "ID бронювання" << setw(10) << "ID гостя" << setw(17) << "Номер кімнати" << setw(20) << "Дата бронювання" << setw(15) << "Термін" << endl;
                cout << setw(10) << res->getInt("BookingId") << setw(10) << res->getInt("GuestId") << setw(17) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;

                char confirmDelete;
                cout << "Ви впевнені, що хочете видалити це бронювання? (y/n): ";
                cin >> confirmDelete;

                if (isInputEmpty(to_string(confirmDelete))) {
                    cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть y або n." << endl;
                    continue; // Починаємо цикл знову
                }

                // Підтвердження або скасування видалення бронювання
                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    pstmt = con->prepareStatement("DELETE FROM bookings WHERE BookingId = ?");
                    pstmt->setInt(1, bookingId);
                    int rowsAffected = pstmt->executeUpdate();

                    if (rowsAffected > 0) {
                        cout << "Бронювання успішно видалено." << endl;
                    }
                    else {
                        cout << "Помилка: Бронювання з вказаним ID не знайдено." << endl;
                    }
                }
                else {
                    cout << "Видалення бронювання скасовано." << endl;
                }
            }

            delete pstmt;
            delete res;

            char choice;
            cout << "Хочете видалити ще одне бронювання? (y/n): ";
            cin >> choice;

            if (isInputEmpty(to_string(choice))) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть y або n." << endl;
                continue; // Починаємо цикл знову
            }

            if (choice == 'n' || choice == 'N') {
                break; // Виходимо з циклу, якщо користувач не хоче видаляти ще бронювання
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка видалення бронювання: " << e.what() << endl;
    }
}
void Booking::updateBooking() {
    try {
        while (true) {
            string bookingIdStr;
            cout << "Введіть ID бронювання, яке ви хочете оновити (або 0, щоб повернутися до головного меню): ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, bookingIdStr);

            if (isInputEmpty(bookingIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть коректне значення." << endl;
                continue;
            }

            if (bookingIdStr == "0") {
                break;
            }

            int bookingId;
            try {
                bookingId = stoi(bookingIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ID бронювання має бути числом або 0." << endl;
                continue;
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
            pstmt->setInt(1, bookingId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Бронювання з введеним ID не знайдено." << endl;
                delete pstmt;
                delete res;
                continue;
            }

            cout << "Оберіть параметр для оновлення:" << endl;
            cout << "1. Номер кімнати" << endl;
            cout << "2. Дату бронювання" << endl;
            cout << "3. Термін" << endl;
            cout << "0. Повернутися до головного меню" << endl;
            int updateChoice;
            cout << "Ваш вибір: ";
            cin >> updateChoice;

            switch (updateChoice) {
            case 0:
                delete pstmt;
                delete res;
                return;
            case 1: {
                // Оновлення номеру кімнати
                do {
                    string roomNumberStr;
                    cout << "Введіть новий номер кімнати: ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, roomNumberStr);
                    if (isInputEmpty(roomNumberStr) || !isNumber(roomNumberStr)) {
                        cout << "Помилка: Номер кімнати має бути введений тільки цифрою." << endl;
                        continue;
                    }
                    int newRoomNumber = stoi(roomNumberStr);

                    PreparedStatement* checkRoomStmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
                    checkRoomStmt->setInt(1, newRoomNumber);
                    ResultSet* roomRes = checkRoomStmt->executeQuery();
                    if (!roomRes->next()) {
                        cout << "Помилка: Кімнати з введеним номером не існує." << endl;
                        delete checkRoomStmt;
                        delete roomRes;
                        continue;
                    }

                    pstmt = con->prepareStatement("UPDATE bookings SET RoomNumber = ? WHERE BookingId = ?");
                    pstmt->setInt(1, newRoomNumber);
                    pstmt->setInt(2, bookingId);

                    break;
                } while (true);
                break;
            }
            case 2: {
                // Оновлення дати бронювання
                do {
                    string newBookingDate;
                    cout << "Введіть нову дату бронювання (рік-місяць-день): ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, newBookingDate);
                    if (isInputEmpty(newBookingDate) || !isValidDateFormat(newBookingDate)) {
                        cout << "Помилка: Некоректний формат дати. Будь ласка, використовуйте формат рік-місяць-день." << endl;
                        continue;
                    }

                    pstmt = con->prepareStatement("UPDATE bookings SET BookingDate = ? WHERE BookingId = ?");
                    pstmt->setString(1, newBookingDate);
                    pstmt->setInt(2, bookingId);

                    break;
                } while (true);
                break;
            }
            case 3: {
                // Оновлення терміну
                do {
                    string newTermStr;
                    cout << "Введіть новий термін (від 1 до 30 днів): ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, newTermStr);
                    if (isInputEmpty(newTermStr) || !isNumber(newTermStr)) {
                        cout << "Помилка: Термін має бути введений тільки цифрою." << endl;
                        continue;
                    }
                    int newTerm = stoi(newTermStr);
                    if (newTerm < 1 || newTerm > 30) {
                        cout << "Помилка: Термін повинен бути в межах від 1 до 30." << endl;
                        continue;
                    }

                    pstmt = con->prepareStatement("UPDATE bookings SET Term = ? WHERE BookingId = ?");
                    pstmt->setInt(1, newTerm);
                    pstmt->setInt(2, bookingId);

                    break;
                } while (true);
                break;
            }
            default:
                cout << "Будь ласка, введіть коректний варіант (0-3)." << endl;
                continue;
            }


            char confirmUpdate;
            cout << "Ви впевнені, що хочете оновити це бронювання? (y/n): ";
            cin >> confirmUpdate;

            if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                int rowsAffected = pstmt->executeUpdate();

                if (rowsAffected > 0) {
                    cout << "Інформацію про бронювання успішно оновлено." << endl;
                }
                else {
                    cout << "Помилка: Не вдалося оновити інформацію про бронювання." << endl;
                }
            }
            else {
                cout << "Оновлення бронювання скасовано." << endl;
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cout << "Помилка оновлення інформації про бронювання: " << e.what() << endl;
    }
}



void Booking::sortBookings() {
    try {
        // Запит користувача на вибір критерію сортування
        int sortingCriterion;
        cout << "Оберіть критерій сортування бронювань:" << endl;
        cout << "1. За ID бронювання." << endl;
        cout << "2. За ID гостя." << endl;
        cout << "3. За номером кімнати." << endl;
        cout << "4. За датою бронювання." << endl;
        cout << "5. За терміном." << endl;
        cout << "0. Повернутися до попереднього меню." << endl;
        cout << "Ваш вибір: ";

        // Перевірка, чи користувач ввів коректне ціле число
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("Помилка вводу. Будь ласка, введіть ціле число.");
        }

        string orderByClause;
        switch (sortingCriterion) {
        case 1:
            orderByClause = "ORDER BY BookingId";
            break;
        case 2:
            orderByClause = "ORDER BY GuestId";
            break;
        case 3:
            orderByClause = "ORDER BY RoomNumber";
            break;
        case 4:
            orderByClause = "ORDER BY BookingDate";
            break;
        case 5:
            orderByClause = "ORDER BY Term";
            break;
        case 0:
            return; // Повернення до попереднього меню
        default:
            cout << "Невірний вибір. Будь ласка, введіть число від 0 до 5." << endl;
            return; // Вихід з функції у разі невірного вибору
        }

        // Виконання запиту до бази даних з використанням вибраного критерію сортування
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // Виведення відсортованого списку бронювань
        cout << setw(15) << "ID бронювання" << setw(10) << "ID гостя" << setw(17) << "Номер кімнати" << setw(20) << "Дата бронювання" << setw(15) << "Термін" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("BookingId") << setw(10) << res->getInt("GuestId") << setw(17) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;
        }

        delete pstmt;
        delete res;

        // Очікування натискання користувачем будь-якої клавіші перед продовженням
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore();
        cin.get();
    }
    catch (const SQLException& e) {
        cout << "Помилка під час виконання запиту до бази даних: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "Сталася помилка: " << ex.what() << endl;
    }
}
