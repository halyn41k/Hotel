#include "payment.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "hotelconnecting.h"
#include "utils.h"
#include <sstream>

using namespace std;

Payment::Payment() : guestId(0), paymentMethod("none"), paymentDate("none"), amount(0), hotel(*(new HotelConnecting())) {}
Payment::Payment(HotelConnecting& h) : hotel(h) {}

void Payment::displayPayments() {
    try {
        while (true) {
            // Запит користувача на введення ID платежу або виведення всіх платежів
            cout << "~~ Введіть ID платежу або введіть 0, щоб вивести всі ~~" << endl;
            string paymentIdStr;
            cout << "ID платежу: ";
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, paymentIdStr);


            // Перевірка на порожній ввід
            if (isInputEmpty(paymentIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу або 0." << endl;
                continue; // Починаємо цикл знову
            }

            if (paymentIdStr == "0") {
                // Виведення всіх платежів
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Немає записів для відображення." << endl;
                }
                else {
                    // Виведення заголовків стовпців
                    cout << setw(10) << "ID платежу" << setw(10) << "ID гостя" << setw(20) << "Спосіб оплати" << setw(10) << "Сума" << setw(20) << "Дата платежу" << endl;

                    // Виведення даних платежів
                    do {
                        cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;
                // Отримання відповіді користувача на продовження
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // Повернення до попереднього меню
                }
            }
            else {
                // Пошук платежу за введеним ID
                int paymentId;
                try {
                    paymentId = stoi(paymentIdStr);
                }
                catch (const std::invalid_argument& ia) {
                    cout << "Помилка: Введений ID платежу має бути числом або 0." << endl;
                    continue; // Починаємо цикл знову
                }

                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments WHERE PaymentId = ?");
                pstmt->setInt(1, paymentId);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "Помилка: Платіж з введеним ID не знайдено." << endl;
                }
                else {
                    // Виведення знайденого платежу
                    cout << setw(10) << "ID платежу" << setw(10) << "ID гостя" << setw(20) << "Спосіб оплати" << setw(10) << "Сума" << setw(20) << "Дата платежу" << endl;
                    cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;
                }

                delete pstmt;
                delete res;
                // Отримання відповіді користувача на продовження
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // Повернення до попереднього меню
                }
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

// Перевірка існування гостя за його ID
bool Payment::isGuestExist(int guestId) {
    try {
        // Підготовка та виконання SQL-запиту
        PreparedStatement* pstmt = con->prepareStatement("SELECT COUNT(*) AS count FROM guests WHERE GuestId = ?");
        pstmt->setInt(1, guestId);
        ResultSet* res = pstmt->executeQuery();

        res->next();
        int guestCount = res->getInt("count");

        delete pstmt;
        delete res;

        return guestCount > 0; // Повернення результату перевірки
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
        return false;
    }
}

// Перевірка правильності способу оплати
bool Payment::isValidPaymentMethod(string method) {
    return method == "Готівка" || method == "Дебютова карта" || method == "Кредитна карта";
}

// Додавання нового платежу
void Payment::addPayment() {
    try {
        cout << "~~ Додати новий платіж ~~" << endl;

        bool validGuestId = false;
        do {
            cout << "ID гостя: ";
            string guestIdInput;
            cin.clear(); // Очищення флага помилки введення
            cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
            getline(cin, guestIdInput);

            stringstream guestIdStream(guestIdInput);
            guestIdStream >> guestId;

            if (!cin || !isGuestExist(guestId)) {
                cout << "Помилка: Гостя з таким ID не існує. Будь ласка, введіть існуючий ID гостя." << endl;
                cin.clear(); // Очистка флага помилки вводу
                cin.ignore(INT_MAX, '\n'); // Очистка буфера введення
            }
            else {
                validGuestId = true;
            }
        } while (isInputEmpty(to_string(guestId)) || !validGuestId);

        // Введення способу оплати
        do {
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            cout << "Спосіб оплати (Готівка/Дебютова карта/Кредитна карта): ";
            getline(cin, paymentMethod);

            if (!isValidPaymentMethod(paymentMethod)) {
                cout << "Помилка: Неправильний спосіб оплати. Будь ласка, введіть 'Готівка', 'Дебютова карта' або 'Кредитна карта'." << endl;
            }
        } while (isInputEmpty(paymentMethod) || !isValidPaymentMethod(paymentMethod));

        // Введення суми платежу
        bool validAmount = false;
        do {
            cout << "Сума: ";
            string amountInput;
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            getline(cin, amountInput);
            stringstream amountStream(amountInput);
            amountStream >> amount;


            if (cin.fail() || amount < 0 || amount > 100000) {
                cin.clear(); // Очистка флага помилки вводу
                cin.ignore(INT_MAX, '\n'); // Видалення неправильного вводу з буфера
                cout << "Помилка: Сума платежу повинна бути числом від 0 до 100000." << endl;
            }
            else {
                validAmount = true; // Сума введена коректно
            }
        } while (isInputEmpty(to_string(amount)) || !validAmount);

        // Введення дати платежу
        bool validDate = false;
        do {
            cin.clear(); // Скидаємо флаг помилки
            cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
            cout << "Дата платежу (рік-місяць-день): ";
            getline(cin, paymentDate);

            // Перевірка правильності формату дати
            bool validFormat = true;
            if (paymentDate.length() != 10) {
                validFormat = false;
            }
            else {
                for (int i = 0; i < 10; ++i) {
                    if (i == 4 || i == 7) {
                        if (paymentDate[i] != '-') {
                            validFormat = false;
                            break;
                        }
                    }
                    else if (!isdigit(paymentDate[i])) {
                        validFormat = false;
                        break;
                    }
                }
            }

            if (validFormat) {
                // Розділення рядка дати на окремі частини (рік, місяць, день)
                int year = stoi(paymentDate.substr(0, 4));
                int month = stoi(paymentDate.substr(5, 2));
                int day = stoi(paymentDate.substr(8, 2));

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
        } while (isInputEmpty(paymentDate) || !validDate);

        // Виконання SQL-запиту для додавання платежу до бази даних
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO payments (GuestId, PaymentMethod, Amount, PaymentDate) VALUES (?, ?, ?, ?)");
        pstmt->setInt(1, guestId);
        pstmt->setString(2, paymentMethod);
        pstmt->setDouble(3, amount);
        pstmt->setString(4, paymentDate);

        pstmt->executeUpdate();

        cout << "Новий платіж успішно додано!" << endl;

        delete pstmt;
        // Отримання відповіді користувача на продовження
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // Повернення до попереднього меню
        }
    }
    catch (SQLException& e) {
        cout << "Помилка SQL: " << e.what() << endl;
    }
}

void Payment::deletePayment() {
    try {
        while (true) {
            string paymentIdStr;
            cout << "Введіть ID платежу, який ви хочете видалити: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, paymentIdStr);

            if (paymentIdStr == "0") {
                break; // Виходимо з циклу, якщо користувач ввів 0
            }

            if (isInputEmpty(paymentIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                continue; // Починаємо цикл знову
            }

            int paymentId;
            try {
                paymentId = stoi(paymentIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ID платежу має бути числом або 0." << endl;
                continue; // Починаємо цикл знову
            }

            // Підготовка та виконання SQL-запиту для отримання платежу за вказаним ID
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments WHERE PaymentId = ?");
            pstmt->setInt(1, paymentId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Платіж з введеним ID не знайдено." << endl;
            }
            else {
                // Виведення інформації про знайдений платіж
                cout << "Вибраний платіж для видалення:" << endl;
                cout << setw(10) << "ID платежу" << setw(10) << "ID гостя" << setw(20) << "Спосіб оплати" << setw(10) << "Сума" << setw(20) << "Дата платежу" << endl;
                cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;

                // Підтвердження видалення платежу від користувача
                char confirmDelete;
                cout << "Ви впевнені, що хочете видалити цей платіж? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // Виконання SQL-запиту для видалення платежу з бази даних
                    pstmt = con->prepareStatement("DELETE FROM payments WHERE PaymentId = ?");
                    pstmt->setInt(1, paymentId);
                    int rowsAffected = pstmt->executeUpdate();

                    if (rowsAffected > 0) {
                        cout << "Платіж успішно видалено." << endl;
                    }
                    else {
                        cout << "Помилка: Платіж з вказаним ID не знайдено." << endl;
                    }
                }
                else {
                    cout << "Видалення платежу скасовано." << endl;
                }
            }

            delete pstmt;
            delete res;

            // Питання користувачу про бажання продовжити видалення
            char choice;
            cout << "Хочете видалити ще один платіж? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // Виходимо з циклу, якщо користувач не хоче видаляти ще платіж
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка видалення платежу: " << e.what() << endl;
    }
}
void Payment::updatePayment() {
    try {
        while (true) {
            string paymentIdStr;
            cout << "Введіть ID платежу, який ви хочете оновити (або 0, щоб повернутися до головного меню): ";
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            getline(cin, paymentIdStr);

            if (paymentIdStr == "0") {
                break;
            }

            if (isInputEmpty(paymentIdStr)) {
                cout << "Помилка: Поле не може бути порожнім. Будь ласка, введіть ідентифікатор платежу." << endl;
                continue;
            }

            int paymentId;
            try {
                paymentId = stoi(paymentIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Помилка: Введений ID платежу має бути числом або 0." << endl;
                continue;
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments WHERE PaymentId = ?");
            pstmt->setInt(1, paymentId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Помилка: Платіж з введеним ID не знайдено." << endl;
            }
            else {
                cout << "Вибраний платіж для оновлення:" << endl;
                cout << setw(10) << "ID платежу" << setw(20) << "Спосіб оплати" << setw(10) << "Сума" << setw(20) << "Дата платежу" << endl;
                cout << setw(10) << res->getInt("PaymentId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;

                cout << "Оберіть параметр для оновлення:" << endl;
                cout << "1. Спосіб оплати" << endl;
                cout << "2. Сума" << endl;
                cout << "3. Дата платежу" << endl;
                cout << "0. Повернутися до головного меню" << endl;

                int updateChoice;
                cout << "Ваш вибір: ";
                cin >> updateChoice;

                if (cin.fail() || updateChoice < 0 || updateChoice > 3) {
                    cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 3." << endl;
                    continue;
                }

                switch (updateChoice) {
                case 0: {
                    break;
                }
                case 1: {
                    string paymentMethod;
                    do {
                        cin.ignore(INT_MAX, '\n');
                        cout << "Новий спосіб оплати (Готівка/Дебютова карта/Кредитна карта): ";
                        getline(cin, paymentMethod);

                        if (paymentMethod != "Готівка" && paymentMethod != "Дебютова карта" && paymentMethod != "Кредитна карта") {
                            cout << "Помилка: Некоректний спосіб оплати." << endl;
                        }
                    } while (paymentMethod != "Готівка" && paymentMethod != "Дебютова карта" && paymentMethod != "Кредитна карта");

                    pstmt = con->prepareStatement("UPDATE payments SET PaymentMethod = ? WHERE PaymentId = ?");
                    pstmt->setString(1, paymentMethod);
                    pstmt->setInt(2, paymentId);
                    pstmt->executeUpdate();
                    cout << "Платіж успішно оновлено." << endl;
                    break;
                }
                case 2: {
                    double amount;
                    string amountInput;
                    bool validAmount = false; // Флаг для перевірки правильності введення суми
                    do {
                        cin.ignore(INT_MAX, '\n');
                        cout << "Нова сума: ";
                        getline(cin, amountInput);

                        // Перевірка, чи введено лише цифри для суми
                        bool isValidAmount = true;
                        for (char c : amountInput) {
                            if (!isdigit(c)) {
                                isValidAmount = false;
                                break;
                            }
                        }

                        if (!isValidAmount || amountInput.empty()) {
                            cout << "Помилка: Сума повинна бути введена тільки цифрами і не може бути порожньою." << endl;
                        }
                        else {
                            validAmount = true; // Сума введена правильно
                            stringstream amountStream(amountInput);
                            amountStream >> amount;

                            if (amount < 0 || amount > 1000000) {
                                cout << "Помилка: Сума повинна бути в межах від 0 до 1000000." << endl;
                                validAmount = false; // Позначити суму як неправильну
                            }
                        }
                    } while (!validAmount);

                    pstmt = con->prepareStatement("UPDATE payments SET Amount = ? WHERE PaymentId = ?");
                    pstmt->setDouble(1, amount);
                    pstmt->setInt(2, paymentId);
                    pstmt->executeUpdate();
                    cout << "Платіж успішно оновлено." << endl;
                    break;
                }

                case 3: {
                    string paymentDate;
                    do {
                        cin.clear();
                        cin.ignore(INT_MAX, '\n');
                        cout << "Нова дата платежу (рік-місяць-день): ";
                        getline(cin, paymentDate);

                        if (!isValidDateFormat(paymentDate)) {
                            cout << "Помилка: Некоректний формат дати. Використовуйте формат рік-місяць-день." << endl;
                        }
                    } while (!isValidDateFormat(paymentDate));

                    pstmt = con->prepareStatement("UPDATE payments SET PaymentDate = ? WHERE PaymentId = ?");
                    pstmt->setString(1, paymentDate);
                    pstmt->setInt(2, paymentId);
                    pstmt->executeUpdate();
                    cout << "Платіж успішно оновлено." << endl;
                    break;
                }
                default: {
                    cout << "Будь ласка, введіть коректний варіант (0-3)." << endl;
                    continue;
                }
                }
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cout << "Помилка оновлення платежу: " << e.what() << endl;
    }
}


void Payment::sortPayments() {
    try {
        int sortingCriterion;
        // Вивід доступних критеріїв сортування
        cout << "Оберіть критерій сортування платежів:" << endl;
        cout << "1. За ID платежу." << endl;
        cout << "2. За ID гостя." << endl;
        cout << "3. За сумою." << endl;
        cout << "4. За датою платежу." << endl;
        cout << "0. Повернутися до попереднього меню." << endl;
        cout << "Ваш вибір: ";

        // Перевірка, чи користувач ввів коректне ціле число
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("Помилка вводу. Будь ласка, введіть ціле число.");
        }

        string orderByClause;
        switch (sortingCriterion) {
        case 1:
            orderByClause = "ORDER BY PaymentId";
            break;
        case 2:
            orderByClause = "ORDER BY GuestId";
            break;
        case 3:
            orderByClause = "ORDER BY Amount";
            break;
        case 4:
            orderByClause = "ORDER BY PaymentDate";
            break;
        case 0:
            return; // Повернутися до попереднього меню
        default:
            cout << "Невірний вибір. Будь ласка, введіть число від 0 до 4." << endl;
            return;
        }

        // Підготовка та виконання SQL-запиту для відсортованих платежів
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // Вивід відсортованого списку платежів
        cout << setw(10) << "ID платежу" << setw(10) << "ID гостя" << setw(20) << "Спосіб оплати" << setw(10) << "Сума" << setw(20) << "Дата платежу" << endl;
        while (res->next()) {
            cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;
        }

        delete pstmt;
        delete res;

        // Очікування натискання будь-якої клавіші перед завершенням програми
        cout << "Натисніть будь-яку клавішу, щоб продовжити...";
        cin.ignore(); // Ігноруємо попередній ввід
        cin.get();    // Очікуємо на новий ввід від користувача
    }
    catch (const SQLException& e) {
        // Обробка помилок SQL
        cout << "Помилка під час виконання запиту до бази даних: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        // Обробка інших виключень
        cout << "Сталася помилка: " << ex.what() << endl;
    }
}
