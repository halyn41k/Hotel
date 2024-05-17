#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <iostream>
#include <memory>
#include <stdexcept>

#include "hotelconnecting.h"

using namespace std;
using namespace sql;

// Ініціалізація статичної змінної con
unique_ptr<Connection> HotelConnecting::con = nullptr;

// Метод для підключення до бази даних
void HotelConnecting::connectToDatabase() {
    try {
        // Отримання драйвера для MySQL
        sql::mysql::MySQL_Driver* driver;
        driver = sql::mysql::get_mysql_driver_instance();

        // Спроба підключення до бази даних
        con.reset(driver->connect("127.0.0.1", "root", "12345678"));

        // Перевірка успішності підключення
        if (con) {
            cout << "Підключення до бази даних успішне." << endl;

            // Встановлення схеми бази даних
            con->setSchema("hotel");

            // Встановлення кодування для коректної обробки українських символів
            if (con->isValid()) {
                string query = "SET NAMES 'cp1251' COLLATE 'cp1251_general_ci'";
                PreparedStatement* pstmt = con->prepareStatement(query);
                pstmt->executeQuery();
                delete pstmt;
            }
            else {
                cout << "Помилка: неможливо підключитися до бази даних." << endl;
            }
        }
    }
    catch (SQLException& e) {
        cout << "Помилка підключення до бази даних: " << e.what() << endl;
    }
}

// Метод для отримання підключення до бази даних
sql::Connection& HotelConnecting::getConnection() {
    // Перевірка наявності підключення
    if (con) {
        return *con;
    }
    else {
        // Обробка випадку, коли підключення відсутнє
        throw runtime_error("Помилка: відсутнє підключення до бази даних.");
    }
}
