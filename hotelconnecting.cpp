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

// ����������� �������� ����� con
unique_ptr<Connection> HotelConnecting::con = nullptr;

// ����� ��� ���������� �� ���� �����
void HotelConnecting::connectToDatabase() {
    try {
        // ��������� �������� ��� MySQL
        sql::mysql::MySQL_Driver* driver;
        driver = sql::mysql::get_mysql_driver_instance();

        // ������ ���������� �� ���� �����
        con.reset(driver->connect("127.0.0.1", "root", "12345678"));

        // �������� �������� ����������
        if (con) {
            cout << "ϳ��������� �� ���� ����� ������." << endl;

            // ������������ ����� ���� �����
            con->setSchema("hotel");

            // ������������ ��������� ��� �������� ������� ���������� �������
            if (con->isValid()) {
                string query = "SET NAMES 'cp1251' COLLATE 'cp1251_general_ci'";
                PreparedStatement* pstmt = con->prepareStatement(query);
                pstmt->executeQuery();
                delete pstmt;
            }
            else {
                cout << "�������: ��������� ����������� �� ���� �����." << endl;
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� ���������� �� ���� �����: " << e.what() << endl;
    }
}

// ����� ��� ��������� ���������� �� ���� �����
sql::Connection& HotelConnecting::getConnection() {
    // �������� �������� ����������
    if (con) {
        return *con;
    }
    else {
        // ������� �������, ���� ���������� ������
        throw runtime_error("�������: ������ ���������� �� ���� �����.");
    }
}
