#ifndef STAFF_H
#define STAFF_H

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <algorithm> // Для функцій all_of і none_of
#include <mysql_connection.h> 
#include <cppconn/prepared_statement.h>
#include "hotelconnecting.h"

class Staff : public HotelConnecting {
private:
    std::string position, firstName, lastName, phoneNumber, hireDate;
    double salary;
    int age; // Додано вік працівника
    HotelConnecting& hotel;

    bool isValidPhoneNumber(const std::string& number);
    bool isValidSalary(double salary);
    bool isValidAge(int age);
    bool isValidPosition(const std::string& position);
public:
    Staff();
    Staff(HotelConnecting& h);
    void displayStaff();
    void addStaff();
    void deleteStaff();
    void updateStaff();
    void sortStaff();
};

#endif // STAFF_H
