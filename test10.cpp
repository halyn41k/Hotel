#include <iostream>
#include <Windows.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
/*
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
*/
#include "hotelconnecting.h"
#include "guest.h"
#include "room.h"
#include "booking.h"
#include "payment.h"
#include "staff.h"
#include "utils.h"

using namespace std;
using namespace sql;

unique_ptr<Connection> con;

Guest guest;
Room room;
Booking booking;
Payment payment;
Staff staff;

class ConsoleIO : public HotelConnecting {
private:
    HotelConnecting& hotel;
public:
    ConsoleIO() : hotel(*(new HotelConnecting())) {}
    ConsoleIO(HotelConnecting& h) : hotel(h) {}

    void userLogin() {
    string login, password;
    bool loggedIn = false;

    do {
        // Відображення заголовка вікна входу
        cout << "===================" << endl;
        cout << "  ВХІД В СИСТЕМУ" << endl;
        cout << "===================" << endl;
        cout << "Логін: ";
        cin >> login;
        cout << "Пароль: ";
        cin >> password;

        try {
            sql::Statement* stmt;
            sql::ResultSet* res;

            // Створення об'єкту для виконання SQL-запиту та результату
            stmt = con->createStatement();
            // Виконання запиту на вибірку користувача з бази даних
            res = stmt->executeQuery("SELECT * FROM userlogin WHERE login='" + login + "' AND password='" + password + "'");

            // Перевірка наявності результату
            if (res->next()) {
                // Отримання типу користувача
                string userType = res->getString("login");
                cout << "Ви успішно увійшли в систему як ";

                // Виведення повідомлення відповідно до типу користувача
                if (userType == "admin") {
                    cout << "адміністратор." << endl;
                    DoAdminMenu(0); // Виклик функції адміністратора
                }
                else if (userType == "manager") {
                    cout << "менеджер." << endl;
                    DoManagerMenu(0); // Виклик функції менеджера
                }
                else if (userType == "cleaner") {
                    cout << "покоївка." << endl;
                    DoCleanerMenu(0); // Виклик функції покоївки
                }
                else {
                    cout << "користувач з невідомим типом." << endl;
                }
                loggedIn = true; // Встановлення прапорця відправленості в систему
            }
            else {
                cout << "Неправильний логін або пароль. Спробуйте ще раз." << endl;
            }

            // Звільнення пам'яті від результату та запиту
            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) {
            // Обробка винятку, пов'язаного з SQL-запитом
            cerr << "SQL Error: " << e.what() << endl;
            cerr << "SQL State: " << e.getSQLState() << endl;
        }
    } while (!loggedIn); // Повторення циклу до входу користувача в систему
}

    static void ShowManagerMenu(int menuLevel) {
        system("cls"); // Очистка екрану консолі

        // Відображення меню відповідно до рівня меню
        switch (menuLevel) {
        case 0:
            // Відображення заголовка головного меню менеджера
            cout << "===========================" << endl;
            cout << "       МЕНЮ МЕНЕДЖЕРА" << endl;
            cout << "===========================" << endl;
            // Виведення пунктів головного меню
            cout << "1. Переглянути інформацію." << endl;
            cout << "2. Додати інформацію." << endl;
            cout << "3. Оновити інформацію." << endl;
            cout << "4. Видалити інформацію." << endl;
            cout << "5. Сортувати інформацію." << endl;
            cout << "0. Вийти." << endl;
            break;
        case 1:
            // Відображення меню перегляду інформації
            cout << "1. Переглянути інформацію про гостя." << endl;
            cout << "2. Переглянути інформацію про кімнату." << endl;
            cout << "3. Переглянути інформацію про бронювання." << endl;
            cout << "4. Переглянути інформацію про оплату." << endl;
            cout << "5. Переглянути інформацію про співробітників." << endl;
            cout << "0. Повернутися до попереднього меню." << endl;
            break;
        case 2:
            // Відображення меню додавання інформації
            cout << "1. Додати нового гостя." << endl;
            cout << "2. Додати нову кімнату." << endl;
            cout << "3. Додати нове бронювання." << endl;
            cout << "4. Додати новий платіж." << endl;
            cout << "5. Додати нового співробітника." << endl;
            cout << "0. Повернутися до попереднього меню." << endl;
            break;
        case 3:
            // Відображення меню оновлення інформації
            cout << "1. Оновити гостя." << endl;
            cout << "2. Оновити кімнату." << endl;
            cout << "3. Оновити бронювання." << endl;
            cout << "4. Оновити оплату." << endl;
            cout << "5. Оновити співробітника." << endl;
            cout << "0. Повернутися до попереднього меню." << endl;
            break;
        case 4:
            // Відображення меню видалення інформації
            cout << "1. Видалити гостя." << endl;
            cout << "2. Видалити кімнату." << endl;
            cout << "3. Видалити бронювання." << endl;
            cout << "4. Видалити оплату." << endl;
            cout << "5. Видалити співробітника." << endl;
            cout << "0. Повернутися до попереднього меню." << endl;
            break;
        case 5:
            // Відображення меню сортування інформації
            cout << "1. Сортувати гостів." << endl;
            cout << "2. Сортувати кімнати." << endl;
            cout << "3. Сортувати бронювання." << endl;
            cout << "4. Сортувати оплати." << endl;
            cout << "5. Сортувати працівників." << endl;
            cout << "0. Повернутися до попереднього меню." << endl;
            break;
        default:
            // Виведення повідомлення про неправильний рівень меню
            cout << "Неправильний рівень меню." << endl;
            break; // Переміщення цього оператора "break" всередину випадку за умови default
        }
    }


    // Функція для виконання меню менеджера
    static void DoManagerMenu(int menuLevel) {
        system("cls"); // Очищення екрану консолі
        int choice; // Змінна для зберігання вибору користувача
        string orderBy = "параметр сортування"; // Змінна для зберігання параметра сортування (поки що залишена як рядок)

        do {
            // Відображення меню
            ShowManagerMenu(menuLevel);
            cout << "Оберіть опцію: ";
            cin >> choice;

            // Перевірка на правильність введеного значення
            if (cin.fail() || choice < 0 || choice > 5) {
                system("cls");
                cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 5." << endl;
                cin.clear();
                cin.ignore(INT_MAX, '\n');
                continue; // Перехід на початок циклу
            }

            // Перевірка на пустий ввід
            if (isInputEmpty(to_string(choice))) {
                system("cls");
                cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 5." << endl;
                continue; // Перехід на початок циклу
            }

            // Обробка вибору користувача залежно від поточного рівня меню
            switch (menuLevel) {
            case 0:
                switch (choice) {
                case 1:
                    menuLevel = 1;
                    break;
                case 2:
                    menuLevel = 2;
                    break;
                case 3:
                    menuLevel = 3;
                    break;
                case 4:
                    menuLevel = 4;
                    break;
                case 5:
                    menuLevel = 5;
                    break;
                case 0:
                    cout << "Завершення роботи..." << endl;
                    return; // Вихід з функції
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            case 1:
                switch (choice) {
                case 0:
                    menuLevel = 0;
                    break;
                case 1:
                    guest.displayGuests();
                    break;
                case 2:
                    room.displayRooms();
                    break;
                case 3:
                    booking.displayBookings();
                    break;
                case 4:
                    payment.displayPayments();
                    break;
                case 5:
                    staff.displayStaff();
                    break;
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            case 2:
                switch (choice) {
                case 0:
                    menuLevel = 0;
                    break;
                case 1:
                    guest.addGuest();
                    break;
                case 2:
                    room.addRoom();
                    break;
                case 3:
                    booking.addBooking();
                    break;
                case 4:
                    payment.addPayment();
                    break;
                case 5:
                    staff.addStaff();
                    break;
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            case 3:
                switch (choice) {
                case 0:
                    menuLevel = 0;
                    break;
                case 1:
                    guest.updateGuest();
                    break;
                case 2:
                    room.updateRoom();
                    break;
                case 3:
                    booking.updateBooking();
                    break;
                case 4:
                    payment.updatePayment();
                    break;
                case 5:
                    staff.updateStaff();
                    break;
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            case 4:
                switch (choice) {
                case 0:
                    menuLevel = 0;
                    break;
                case 1:
                    guest.deleteGuest();
                    break;
                case 2:
                    room.deleteRoom();
                    break;
                case 3:
                    booking.deleteBooking();
                    break;
                case 4:
                    payment.deletePayment();
                    break;
                case 5:
                    staff.deleteStaff();
                    break;
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            case 5:
                switch (choice) {
                case 0:
                    menuLevel = 0;
                    break;
                case 1:
                    guest.sortGuests();
                    break;
                case 2:
                    room.sortRooms();
                    break;
                case 3:
                    booking.sortBookings();
                    break;
                case 4:
                    payment.sortPayments();
                    break;
                case 5:
                    staff.sortStaff();
                    break;
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            default:
                cout << "Невідомий рівень меню" << endl;
            }
        } while (true); // Нескінченний цикл для відображення меню та обробки вибору користувача
    }

    // Функція для виконання меню адміністратора
    static void DoAdminMenu(int menuLevel) {
        system("cls"); // Очищення екрану консолі
        int choice; // Змінна для зберігання вибору користувача

        do {
            // Відображення меню залежно від рівня меню
            switch (menuLevel) {
            case 0:
                cout << "===============================================" << endl;
                cout << "             МЕНЮ АДМІНІСТРАТОРА" << endl;
                cout << "===============================================" << endl;
                cout << "1. Додати нового гостя." << endl;
                cout << "2. Додати оплату." << endl;
                cout << "3. Додати бронювання кімнати." << endl;
                cout << "4. Переглянути інформацію про гостей." << endl;
                cout << "5. Переглянути інформацію про оплати гостей." << endl;
                cout << "6. Переглянути інформацію про бронювання кімнат." << endl;
                cout << "0. Вийти." << endl;
                break;
            default:
                cout << "Неправильний рівень меню." << endl;
                break; // Перехід на початок циклу
            }

            cout << "Оберіть опцію: ";
            cin >> choice;

            // Перевірка на правильність введеного значення
            if (cin.fail() || choice < 0 || choice > 6) {
                cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 6." << endl;
                system("cls");
                cin.clear(); // Скидаємо флаг помилки
                cin.ignore(INT_MAX, '\n'); // Очищаємо вхідний буфер
                continue; // Повертаємося до початку циклу
            }

            // Обробка вибору користувача залежно від поточного рівня меню
            switch (menuLevel) {
            case 0:
                switch (choice) {
                case 1:
                    guest.addGuest(); // Додавання нового гостя
                    break;
                case 2:
                    room.addRoom(); // Додавання нової кімнати
                    break;
                case 3:
                    booking.addBooking(); // Додавання нового бронювання кімнати
                    break;
                case 4:
                    guest.displayGuests(); // Перегляд інформації про гостей
                    break;
                case 5:
                    payment.displayPayments(); // Перегляд інформації про оплати гостей
                    break;
                case 6:
                    booking.displayBookings(); // Перегляд інформації про бронювання кімнат
                    break;
                case 0:
                    cout << "Завершення роботи..." << endl;
                    return; // Вихід з функції
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-5)." << endl;
                }
                break;
            default:
                cout << "Невідомий рівень меню" << endl;
            }
        } while (true); // Нескінченний цикл для відображення меню та обробки вибору користувача
    }

    // Функція DoCleanerMenu відповідає за відображення та обробку меню для покоївок.
    static void DoCleanerMenu(int menuLevel) {
        system("cls"); // Очищення консольного вікна

        int choice; // Змінна для вибору опції користувачем

        do {
            switch (menuLevel) {
            case 0:
                system("cls"); // Очищення консольного вікна
                cout << "===========================================" << endl;
                cout << "             МЕНЮ ПОКОЇВКИ" << endl;
                cout << "===========================================" << endl;
                cout << "1. Переглянути зайнятість кімнат." << endl;
                cout << "2. Переглянути статус прибирання кімнат." << endl;
                cout << "3. Змінити статус прибирання кімнат." << endl;
                cout << "0. Вийти." << endl;
                break;
            default:
                cout << "Неправильний рівень меню." << endl;
                break;
            }

            cout << "Оберіть опцію: ";
            cin >> choice;

            if (cin.fail() || choice < 0 || choice > 3) {
                // Перевірка на правильність введених даних
                system("cls"); // Очищення консольного вікна
                cout << "Неправильний ввід. Будь ласка, введіть цифру від 0 до 3." << endl;
                cin.clear(); // Скидання флага помилки
                cin.ignore(INT_MAX, '\n'); // Очищення вхідного буфера
                continue; // Почати цикл знову
            }

            switch (menuLevel) {
            case 0:
                switch (choice) {
                case 1:
                    booking.displayBookings(); // Відображення заброньованих кімнат
                    break;
                case 2:
                    room.viewCleaningStatus(); // Відображення статусу прибирання кімнат
                    break;
                case 3:
                    room.updateCleaningStatus(); // Оновлення статусу прибирання кімнат
                    break;
                case 0:
                    cout << "Завершення роботи..." << endl; // Вихід з програми
                    return;
                default:
                    cout << "Будь ласка, введіть коректний варіант (0-3)." << endl;
                }
                break;
            default:
                cout << "Невідомий рівень меню" << endl;
            }
        } while (true); // Безкінечний цикл для взаємодії з меню
    }

};


// Основна функція програми
int main() {
    SetConsoleOutputCP(1251); // Встановлення кодової сторінки виводу консолі
    SetConsoleCP(1251); // Встановлення кодової сторінки вводу консолі

    HotelConnecting con; // Створення об'єкту для підключення до бази даних
    con.connectToDatabase(); // Виклик методу для підключення до бази даних

    ConsoleIO console; // Створення об'єкту для взаємодії з консоллю
    console.userLogin(); // Виклик методу для авторизації користувача

    return 0;
}
