#include "Staff.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <algorithm> // ��� ������� all_of � none_of
#include <mysql_connection.h> // ����������, �� �� ������������� MySQL Connector/C++
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
    // ��������, �� �������� � ������ �� �� ���� � ����� �� 10 �� 100000
    return salary >= 10 && salary <= 100000;
}

bool Staff::isValidAge(int age) {
    // ��������, �� �� � ������ �� �� �� � ����� �� 18 �� 70
    return age >= 18 && age <= 70;
}

bool Staff::isValidPosition(const string& position) {
    // ��������, �� ������ �� ������ ����
    return none_of(position.begin(), position.end(), ::isdigit);
}
void Staff::displayStaff() {
    try {
        while (true) {
            // �������� ID ����������� ��� 0 ��� ��������� ���
            cout << "~~ ������ ID ����������� ��� ������ 0, ��� ������� ��� ~~" << endl;
            string staffIdStr;
            cout << "ID �����������: ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, staffIdStr);

            // �������� �� ������� ���
            if (isInputEmpty(staffIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ID ����������� ��� 0." << endl;
                continue; // �������� ���� �����
            }

            if (staffIdStr == "0") {
                // ��������� ��� �����������
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "���� ������ ��� �����������." << endl;
                }
                else {
                    // ��������� ����� ��� ����������� � ������ �������
                    cout << setw(15) << "ID �����������" << setw(19) << "������" << setw(15) << "��'�" << setw(15) << "�������" << setw(20) << "����� ��������" << setw(20) << "��������" << setw(10) << "³�" << setw(30) << "���� ����������������" << endl;
                    do {
                        cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // ����������� ����� ��� ��������� ������, ���� ���������� ������ 'n' ��� 'N'
                }
            }
            else {
                int staffId;
                try {
                    staffId = stoi(staffIdStr);
                }
                catch (const std::invalid_argument& ia) {
                    cout << "�������: �������� ID ����������� �� ���� ������ ��� 0." << endl;
                    continue; // �������� ���� �����
                }

                // ��������� ����� ����������� ����������� �� ���� ID
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff WHERE StaffId = ?");
                pstmt->setInt(1, staffId);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "�������: ����������� � �������� ID �� ��������." << endl;
                }
                else {
                    // ��������� ����� ��� ����������� � ������ �������
                    cout << setw(15) << "ID �����������" << setw(19) << "������" << setw(15) << "��'�" << setw(15) << "�������" << setw(20) << "����� ��������" << setw(20) << "��������" << setw(10) << "³�" << setw(30) << "���� ����������������" << endl;
                    cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;
                }

                delete pstmt;
                delete res;
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // ����������� ����� ��� ��������� ������, ���� ���������� ������ 'n' ��� 'N'
                }
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

void Staff::addStaff() {
    try {
        cout << "~~ ������ ������ ���������� ~~" << endl;

        // �������� ������ ����������
        do {
            cout << "������: ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, position);

            if (position.empty() || !isValidPosition(position)) {
                cout << "�������: ������ �� ���� ���� ��������� � ������� ������ ���� �����. ���� �����, ������ ������ ��� ����." << endl;
            }
        } while (position.empty() || !isValidPosition(position));

        // �������� ���� ����������
        do {
            cout << "��'�: ";
            getline(cin, firstName);

            if (firstName.empty() || !isLetters(firstName)) {
                cout << "�������: ��'� �� ���� ���� ������� � ������� ������ ���� �����." << endl;
            }
        } while (firstName.empty() || !isLetters(firstName));

        // �������� ������� ����������
        do {
            cout << "�������: ";
            getline(cin, lastName);

            if (lastName.empty() || !isLetters(lastName)) {
                cout << "�������: ������� �� ���� ���� ������� � ������� ������ ���� �����." << endl;
            }
        } while (lastName.empty() || !isLetters(lastName));

        // �������� ������ �������� ����������
        do {
            cout << "����� �������� (����� 8 �������): ";
            getline(cin,phoneNumber);

            if (phoneNumber.empty() || phoneNumber.length() < 8) {
                cout << "�������: ����� �������� �� ���� ���� ������� � ������� ������ ���������� 8 ����." << endl;
            }
            else if (!isValidPhoneNumber(phoneNumber)) {
                cout << "�������: ����� �������� ������� ������ ����� �����." << endl;
            }
        } while (phoneNumber.empty() || phoneNumber.length() < 8 || !isValidPhoneNumber(phoneNumber));

        // �������� �������� ����������
        do {
            cout << "��������: ";
            string salaryInput;
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, salaryInput);
            stringstream salaryStream(salaryInput);
            salaryStream >> salary;

            if (cin.fail() || !isValidSalary(salary)) {
                cout << "�������: �������� �� ���� ������ � ����� �� 10 �� 100000." << endl;
                cin.clear();
                cin.ignore(INT_MAX, '\n');
            }
        } while (cin.fail() || !isValidSalary(salary));

        // �������� ���� ���������������� ����������
        bool validDate = false;
        do {
            cout << "���� ���������������� (��-�����-����): ";
            getline(cin,hireDate);

            if (hireDate.empty() || hireDate.length() != 10) {
                cout << "�������: ������������ ������ ����. ���� �����, ������ ���� � ������ '��-�����-����'." << endl;
            }
            else {
                // �������� ����������� ������� ����
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
                    cout << "�������: ������������ ������ ����. ���� �����, ������ ���� � ������ '��-�����-����'." << endl;
                }
                else {
                    // ��������� ����� ���� �� ����� ������� (��, �����, ����)
                    int year = stoi(hireDate.substr(0, 4));
                    int month = stoi(hireDate.substr(5, 2));
                    int day = stoi(hireDate.substr(8, 2));

                    // �������� ����������� ����
                    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
                        validDate = true;
                    }
                    else {
                        cout << "�������: ����������� ����. ���� �����, ������ �������� ����." << endl;
                    }
                }
            }
        } while (!validDate);

        // ������� ������ ������ ��� ���������� � ���� �����
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO staff (Position, FirstName, LastName, PhoneNumber, Salary, Age, HireDate) VALUES (?, ?, ?, ?, ?, ?, ?)");
        pstmt->setString(1, position);
        pstmt->setString(2, firstName);
        pstmt->setString(3, lastName);
        pstmt->setString(4, phoneNumber);
        pstmt->setDouble(5, salary);
        pstmt->setInt(6, age); // age �� ���������� � ������� addStaff
        pstmt->setString(7, hireDate);
        pstmt->executeUpdate();

        cout << "������ ���������� ������ ������!" << endl;

        delete pstmt;

        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return;
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

void Staff::deleteStaff() {
    try {
        while (true) {
            string staffIdStr;
            cout << "������ ID ����������, ����� �� ������ ��������: ";
            cin.ignore(INT_MAX, '\n'); // �������� ������ �����
            getline(cin, staffIdStr); // ��������� ��������� ID ����������

            if (staffIdStr == "0") {
                break; // ����� � �����, ���� ���������� ��� 0
            }

            if (staffIdStr.empty()) {
                cout << "�������: ������ ID ����������." << endl;
                continue; // ������ ���� �����, ���� ������� ������� �����
            }

            int staffId;
            try {
                staffId = stoi(staffIdStr); // ����������� ����� ��������� ID � ���� �����
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ID ���������� �� ���� ������ ��� 0." << endl;
                continue; // ������ ���� �����, ���� ������� ���������� �������� ID
            }

            // ϳ�������� SQL-������ ��� ��������� ���������� ��� ���������� �� �������� ID
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff WHERE StaffId = ?");
            pstmt->setInt(1, staffId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ���������� � �������� ID �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� ���������� ����������
                cout << "�������� ��������� ��� ���������:" << endl;
                cout << setw(15) << "ID �����������" << setw(19) << "������" << setw(15) << "��'�" << setw(15) << "�������" << setw(20) << "����� ��������" << setw(20) << "��������" << setw(10) << "³�" << setw(30) << "���� ����������������" << endl;
                cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;

                // ϳ����������� ��������� ����������
                char confirmDelete;
                cout << "�� �������, �� ������ �������� ����� ����������? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // ϳ�������� SQL-������ ��� ��������� ���������� � ���� �����
                    pstmt = con->prepareStatement("DELETE FROM staff WHERE StaffId = ?");
                    pstmt->setInt(1, staffId);
                    int rowsAffected = pstmt->executeUpdate();

                    // ����������� ��� ��������� ��������� ����������
                    if (rowsAffected > 0) {
                        cout << "���������� ������ ��������." << endl;
                    }
                    else {
                        cout << "�������: ���������� � �������� ID �� ��������." << endl;
                    }
                }
                else {
                    cout << "��������� ���������� ���������." << endl;
                }
            }

            // ��������� ������� � ��������� �� �������� �������� �����
            delete pstmt;
            delete res;

            // �������� �� ���������� ���� ���������� ��������� ����������
            char choice;
            cout << "������ �������� �� ������ ����������? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // ����� � �����, ���� ���������� ����� �� ���� �������� ����������
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� ����������: " << e.what() << endl;
    }
}

// ����� ��� ��������� ���������� ��� ����������
void Staff::updateStaff() {
    try {
        // ���������� ���� ��� �����䳿 � ������������
        while (true) {
            string staffIdStr;
            // ����� �� �������� ID ����������, ����� ������� �������
            cout << "������ ID ����������, ����� �� ������ ������� (��� 0, ��� ����������� �� ��������� ����): ";
            cin.ignore(INT_MAX, '\n'); // �������� ������ �����
            getline(cin, staffIdStr); // ��������� ��������� ID ����������

            // ��������, �� ���������� ��� 0 ��� ������
            if (staffIdStr == "0") {
                break; // ����� � �����, ���� ���������� ��� 0
            }

            int staffId;
            try {
                staffId = stoi(staffIdStr); // ����������� ����� ��������� ID � ���� �����
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ID ���������� �� ���� ������ ��� 0." << endl;
                continue; // ������� ����� �����, ���� ������� ���������� �������� ID
            }

            // ϳ�������� SQL-������ ��� ��������� ���������� ��� ���������� �� �������� ID
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff WHERE StaffId = ?");
            pstmt->setInt(1, staffId);
            ResultSet* res = pstmt->executeQuery();

            // ��������, �� �������� ���������� � �������� ID
            if (!res->next()) {
                cout << "�������: ������� ���������� � �������� ID �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� ���������� ����������
                cout << "�������� ��������� ��� ���������:" << endl;
                cout << setw(15) << "ID �����������" << setw(19) << "������" << setw(15) << "��'�" << setw(15) << "�������" << setw(20) << "����� ��������" << setw(20) << "��������" << setw(10) << "³�" << setw(30) << "���� ����������������" << endl;
                cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;

                // ��������� ����� ����������
                cout << "������ �������� ��� ���������:" << endl;
                cout << "1. ������" << endl;
                cout << "2. ��'�" << endl;
                cout << "3. �������" << endl;
                cout << "4. ����� ��������" << endl;
                cout << "5. ��������" << endl;
                cout << "6. ³�" << endl;
                cout << "7. ���� ����������������" << endl;
                cout << "0. ����������� �� ��������� ����" << endl;

                // �������� ������ �����������
                int updateChoice;
                cout << "��� ����: ";
                cin >> updateChoice;

                // ������� ������ ����������� �� ��������� ���������� ���� � ��� �����
                switch (updateChoice) {
                    // ��������� ������
                case 1: {
                    string newPosition;
                    do {
                        cin.ignore();
                        cout << "���� ������: ";
                        getline(cin, newPosition);
                        cin.ignore(INT_MAX, '\n');
                        // �������� �� ������� ��� �� ����������� �������
                        if (isInputEmpty(newPosition)) {
                            cout << "�������: ���� '������' �� ���� ���� �������." << endl;
                        }
                        else if (!isLetters(newPosition)) {
                            cout << "�������: ������ ������� ������ ���� �����." << endl;
                        }
                    } while (isInputEmpty(newPosition) || !isLetters(newPosition));

                    // ��������� ���� '������' � ��� �����
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
                        cout << "���� ��'�: ";
                        getline(cin, newFirstName);
                        cin.ignore(INT_MAX, '\n');
                        if (isInputEmpty(newFirstName)) {
                            cout << "�������: ���� '��'�' �� ���� ���� �������." << endl;
                        }
                        else if (!isLetters(newFirstName)) {
                            cout << "�������: ��'� ������� ������ ���� �����." << endl;
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
                        cout << "���� �������: ";
                        getline(cin, newLastName);
                        cin.ignore(INT_MAX, '\n');
                        if (isInputEmpty(newLastName)) {
                            cout << "�������: ���� '�������' �� ���� ���� �������." << endl;
                        }
                        else if (!isLetters(newLastName)) {
                            cout << "�������: ������� ������� ������ ���� �����." << endl;
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
                        cout << "����� ����� ��������: ";
                        getline(cin,newPhoneNumber);
                        cin.ignore(INT_MAX, '\n');
                        if (isInputEmpty(newPhoneNumber)) {
                            cout << "�������: ���� '����� ��������' �� ���� ���� �������." << endl;
                        }
                        else if (!isValidPhoneNumber(newPhoneNumber)) {
                            cout << "�������: ����������� ������ ������ ��������." << endl;
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
                        cout << "���� ��������: ";
                        getline(cin, newSalaryStr);
                        cin.ignore(); // ������� ����� �����

                        if (isInputEmpty(newSalaryStr)) {
                            cout << "�������: �������� �� ���� ���� ���������." << endl;
                            continue;
                        }

                        if (!isNumber(newSalaryStr)) {
                            cout << "�������: �������� ������� ���� ������." << endl;
                            continue;
                        }

                        newSalary = stod(newSalaryStr);
                        if (!staff.isValidSalary(newSalary)) {
                            cout << "�������: �������� ������� ���� ����� 10 �� ����� 100000." << endl;
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
                        cout << "³�: ";
                        string newAgeInput;
                        getline(cin, newAgeInput);
                        cin.ignore(INT_MAX, '\n');
                        stringstream newAgeStream(newAgeInput);
                        newAgeStream >> newAge;
                        if (isInputEmpty(to_string(newAge))) {
                            cout << "�������: �������� ����� �� ���� ���� �������." << endl;
                        }
                        else if (!isValidAge(newAge)) {
                            cout << "�������: ³� �� ���� � ����� �� 18 �� 70." << endl;
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
                        cout << "���� ���� ���������������� (��-�����-����): ";
                        cin.ignore(INT_MAX, '\n');
                        getline(cin,newHireDate);
                        if (isInputEmpty(newHireDate)) {
                            cout << "�������: �������� ����� �� ���� ���� �������." << endl;
                        }
                        else if (!isValidDateFormat(newHireDate)) {
                            cout << "�������: ����������� ������ ����. �������� ������: ��-�����-����" << endl;
                        }
                    } while (isInputEmpty(newHireDate) || !isValidDateFormat(newHireDate));

                    pstmt = con->prepareStatement("UPDATE staff SET HireDate = ? WHERE StaffId = ?");
                    pstmt->setString(1, newHireDate);
                    pstmt->setInt(2, staffId);
                    pstmt->executeUpdate();
                    break;
                }
                      // ����������� ��� ����������� ��� ������
                default: {
                    cout << "���� �����, ������ ��������� ������ (0-7)." << endl;
                    continue; // �������� ���� �����
                }
                }

                cout << "���������� ��� ���������� � ID " << staffId << " ������ ��������." << endl;
            }

            // ��������� �������
            delete pstmt;
            delete res;
        }
    }
    // ������� ������� ���� �����
    catch (SQLException& e) {
        cout << "������� ��������� ����� ��� ����������: " << e.what() << endl;
    }
}
void Staff::sortStaff() {
    try {
        int sortingCriterion;
        // ��������� ���� ��� ������ ������� ����������
        cout << "������ ������� ���������� ���������:" << endl;
        cout << "1. �� ID �����������." << endl;
        cout << "2. �� �������." << endl;
        cout << "3. �� ������." << endl;
        cout << "4. �� ��������." << endl;
        cout << "5. �� ���������." << endl;
        cout << "6. �� ����." << endl;
        cout << "7. �� ����� ����������������." << endl;
        cout << "0. ����������� �� ������������ ����." << endl;
        cout << "��� ����: ";

        // ��������� ������ ����������� �� �������� �� ����������
        string sortingCriterionStr;
        cin >> sortingCriterionStr;
        if (isInputEmpty(sortingCriterionStr)) {
            cout << "�������: �������� ����� �� ���� ���� �������." << endl;
            return;
        }

        // ����������� ����� ��������� ������ � ���� �����
        try {
            sortingCriterion = std::stoi(sortingCriterionStr);
        }
        catch (const std::invalid_argument& e) {
            throw std::runtime_error("������� �����. ���� �����, ������ ���� �����.");
        }

        // ���������� SQL-������ � ����������� ��������� ������� ����������
        string orderByClause;
        switch (sortingCriterion) {
            // ���������� �� ID �����������
        case 1:
            orderByClause = "ORDER BY StaffId";
            break;
            // ���������� �� �������
        case 2:
            orderByClause = "ORDER BY Position";
            break;
            // ���������� �� ������
        case 3:
            orderByClause = "ORDER BY FirstName";
            break;
            // ���������� �� ��������
        case 4:
            orderByClause = "ORDER BY LastName";
            break;
            // ���������� �� ���������
        case 5:
            orderByClause = "ORDER BY Salary";
            break;
            // ���������� �� ����
        case 6:
            orderByClause = "ORDER BY Age";
            break;
            // ���������� �� ����� ����������������
        case 7:
            orderByClause = "ORDER BY HireDate";
            break;
            // ���������� �� ������������ ����
        case 0:
            return;
            // ��������� ����������� ��� ����������� ����
        default:
            cout << "������� ����. ���� �����, ������ ����� �� 0 �� 7." << endl;
            return;
        }

        // ��������� SQL-������ ��� ������������� ������ ���������
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM staff " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // ��������� ������������� ������ ���������
        cout << setw(15) << "ID �����������" << setw(19) << "������" << setw(15) << "��'�" << setw(15) << "�������" << setw(20) << "����� ��������" << setw(20) << "��������" << setw(10) << "³�" << setw(30) << "���� ����������������" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("StaffId") << setw(20) << res->getString("Position") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(20) << res->getString("PhoneNumber") << setw(20) << fixed << setprecision(2) << res->getDouble("Salary") << setw(10) << res->getInt("Age") << setw(25) << res->getString("HireDate") << endl;
        }

        // ��������� �������
        delete pstmt;
        delete res;

        // ���������� �������� ������������ ����-��� ������ ����� ����������� ��������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore(); // �������� ��������� ���
        cin.get();    // ���������� �� ����� ��� �� �����������
    }
    // ������� ������� ���� �����
    catch (const SQLException& e) {
        cout << "������� �� ��� ��������� ������ �� ���� �����: " << e.what() << endl;
    }
    // ������� ����� �������
    catch (const std::exception& ex) {
        cout << "������� �������: " << ex.what() << endl;
    }
}
