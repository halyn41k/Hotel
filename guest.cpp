#include "hotelconnecting.h"
#include "guest.h"
#include <iostream>
#include <iomanip>
#include "utils.h"

using namespace std;

Guest::Guest() : firstName("none"), lastName("none"), gender("none"), age(0), phoneNumber("none"), guestId(0), roomId(0), roomNumber(0), checkInDate("none"), hotel(*(new HotelConnecting())) {}

Guest::Guest(HotelConnecting& h) : hotel(h) {}

void Guest::displayGuests() {
    sql::Connection& conn = hotel.getConnection(); // ��������� �'������� � ����� ����� ����� ��'��� ������

    try {
        while (true) {
            cout << "~~ ������ ������������� ����� ��� ������ 0, ��� ������� ��� ~~" << endl;
            string guestIdStr;
            cout << "ID �����: ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, guestIdStr);

            if (isInputEmpty(guestIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� ����� ��� 0." << endl;
                continue; // �������� ���� �����
            }

            if (guestIdStr == "0") {
                // ��������� ��� ������, ���� ������� 0
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "���� ������ ��� �����������." << endl;
                }
                else {
                    // ��������� ��������� �������
                    cout << setw(10) << "ID" << setw(15) << "��'�" << setw(15) << "�������" << setw(10) << "�����" << setw(5) << "³�" << setw(22) << "����� ��������" << setw(15) << " ���� �����" << setw(15) << " ����� ������" << endl;
                    // ��������� ����� ������� �����
                    do {
                        cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice(); // ��������� ���������� ������ �� �����������

                if (choice == 'y' || choice == 'Y') {
                    return; // ����������� ����� ��� ��������� ������, ���� ���������� ������ 'n' ��� 'N'
                }
            }
            else {
                int guestId;
                try {
                    guestId = stoi(guestIdStr); // ����������� ����� � ���� �����
                }
                catch (const std::invalid_argument& ia) {
                    cout << "�������: �������� ������������� ����� �� ���� ������ ��� 0." << endl;
                    continue; // �������� ���� �����
                }

                PreparedStatement* pstmt = con->prepareStatement("SELECT GuestId, FirstName, LastName, Gender, Age, PhoneNumber, CheckInDate, RoomNumber FROM guests WHERE GuestID = ?");
                pstmt->setInt(1, guestId);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "�������: ����� � �������� ID �� ��������." << endl;
                }
                else {
                    // ��������� ��������� �������
                    cout << setw(10) << "ID" << setw(15) << "��'�" << setw(15) << "�������" << setw(10) << "�����" << setw(5) << "³�" << setw(22) << "����� ��������" << setw(15) << " ���� �����" << setw(15) << " ����� ������" << endl;
                    // ��������� ����� ����� � �������� ID
                    cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice(); // ��������� ���������� ������ �� �����������

                if (choice == 'y' || choice == 'Y') {
                    return; // ����������� ����� ��� ��������� ������, ���� ���������� ������ 'n' ��� 'N'
                }
            }
        }
    }
    catch (SQLException& e) {
        // ������� ������� SQLException
        cout << "������� SQL: " << e.what() << endl;
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
        cout << "������� SQL: " << e.what() << endl;
        return false;
    }
}

void Guest::addGuest() {
    try {
        while (true) {
            // ����� ����������� �� �������� ����� ������ �����
            cout << "~~ ������ ������ ����� ~~" << endl;

            // �������� �� �������� ����
            do {
                cin.ignore();
                cout << "��'�: ";
                getline(cin, firstName);

                if (isInputEmpty(firstName)) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ��'�." << endl;
                }
                else if (!isLetters(firstName)) {
                    cout << "�������: ��'� ������� ������ ���� �����." << endl;
                }
            } while (isInputEmpty(firstName) || !isLetters(firstName));

            // �������� �� �������� �������
            do {
                cout << "�������: ";
                getline(cin, lastName);
                if (isInputEmpty(lastName)) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ �������." << endl;
                }
                else if (!isLetters(lastName)) {
                    cout << "�������: ������� ������� ������ ���� �����." << endl;
                }
            } while (isInputEmpty(lastName) || !isLetters(lastName));

            // �������� �� �������� ����
            do {
                cout << "����� (�������/�����): ";
                getline(cin, gender);

                if (isInputEmpty(gender)) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ '�������' ��� '�����'." << endl;
                }
                else if (gender != "�������" && gender != "�����") {
                    cout << "�������: ����������� ������� �����. ���� �����, ������ '�������' ��� '�����'." << endl;
                }
            } while (isInputEmpty(gender) || (gender != "�������" && gender != "�����"));

            // �������� �� �������� ���
            string ageStr;
            do {
                cout << "³�: ";
                getline(cin, ageStr);

                if (isInputEmpty(ageStr)) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ��." << endl;
                }
                else if (!all_of(ageStr.begin(), ageStr.end(), ::isdigit)) {
                    cout << "�������: ³� �� ���� �������� ����� ������." << endl;
                }
                else {
                    age = stoi(ageStr);
                    if (!isValidAge(age)) {
                        cout << "�������: ����������� �������� ��. ���� �����, ������ ��������� �� �� 0 �� 125." << endl;
                    }
                }
            } while (isInputEmpty(ageStr) || !all_of(ageStr.begin(), ageStr.end(), ::isdigit) || !isValidAge(age));

            // �������� �� �������� ������ ��������
            do {
                cout << "����� �������� (����� 8 �������): ";
                getline(cin, phoneNumber);

                if (isInputEmpty(phoneNumber)) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ����� ��������." << endl;
                }
                else if (phoneNumber.length() < 8) {
                    cout << "�������: ����� �������� ������� ������ ���������� 8 ����." << endl;
                }
                else if (!isValidPhoneNumber(phoneNumber)) {
                    cout << "�������: ����� �������� ������� ������ ����� �����." << endl;
                }
            } while (isInputEmpty(phoneNumber) || phoneNumber.length() < 8 || !isValidPhoneNumber(phoneNumber));

            // �������� �� �������� ���� �����
            bool validDate = false;
            do {
                cout << "���� ����� (��-�����-����): ";
                getline(cin, checkInDate);
                // �������� ����������� ������� ����
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
                    // ��������� ����� ���� �� ����� ������� (��, �����, ����)
                    int year = stoi(checkInDate.substr(0, 4));
                    int month = stoi(checkInDate.substr(5, 2));
                    int day = stoi(checkInDate.substr(8, 2));

                    // �������� ����������� ����
                    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
                        validDate = true;
                    }
                    else {
                        cout << "�������: ����������� ����. ���� �����, ������ �������� ����." << endl;
                    }
                }
                else {
                    cout << "�������: ������������ ������ ����. ���� �����, ������ ���� � ������ '��-�����-����'." << endl;
                }
            } while (isInputEmpty(checkInDate) || !validDate);

            // �������� �� �������� ������ ������
            string roomNumberStr;
            do {
                cout << "����� ������: ";
                getline(cin, roomNumberStr);

                if (!isNumber(roomNumberStr)) {
                    cout << "�������: ������������ ������. ���� �����, ������ ����� ������ ������." << endl;
                }
            } while (isInputEmpty(roomNumberStr) || !isNumber(roomNumberStr));

            roomNumber = stoi(roomNumberStr);

            // �������� ���������� ������
            if (!isRoomAvailable(roomNumber)) {
                cout << "�������: ʳ����� � ������� " << roomNumber << " �������." << endl;
                continue;
            }

            // ��������� ����� �� ���� �����
            PreparedStatement* pstmt = con->prepareStatement("INSERT INTO guests (FirstName, LastName, Gender, Age, PhoneNumber, CheckInDate, RoomNumber) VALUES (?, ?, ?, ?, ?, ?, ?)");
            pstmt->setString(1, firstName);
            pstmt->setString(2, lastName);
            pstmt->setString(3, gender);
            pstmt->setInt(4, age);
            pstmt->setString(5, phoneNumber);
            pstmt->setString(6, checkInDate);
            pstmt->setInt(7, roomNumber);

            int rowsAffected = pstmt->executeUpdate();

            // ����������� ��� ��������� ��������� �����
            if (rowsAffected == 0) {
                cout << "�������: ������ ����� �� ���� ������ �� ���� �����." << endl;
            }
            else {
                cout << "������ ����� ������ ������!" << endl;
            }

            delete pstmt;

            // �������� ������� ����������� ����������
            char choice = getChoice();

            do {
                cout << "������ ������ ��? (y/n): ";
                cin >> choice;
                if (choice != 'y' && choice != 'n') {
                    cout << "�������: ���� �����, ������ 'y' ��� 'n'." << endl;
                }
            } while (choice != 'y' && choice != 'n');

            if (choice == 'n') {
                return;
            }

        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

void Guest::deleteGuest() {
    try {
        while (true) {
            // ����� ����������� �� �������� �������������� ����� ��� ���������
            string guestIdStr;
            cout << "������ ������������� �����, ����� �� ������ ��������: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, guestIdStr);

            // �������� �� ������� ������������� �����
            if (isInputEmpty(guestIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �����." << endl;
                continue; // �������� ���� �����
            }

            // ����������� ����� ��������� �������������� � ���� �����
            int guestId;
            try {
                guestId = stoi(guestIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ������������� ����� �� ���� ������." << endl;
                continue; // �������� ���� �����
            }

            // ϳ�������� SQL ������ ��� ��������� ���������� ��� ����� �� ���� ���������������
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests WHERE GuestId = ?");
            pstmt->setInt(1, guestId);
            ResultSet* res = pstmt->executeQuery();

            // �������� �� ���� ���� � �������� ���������������
            if (!res->next()) {
                cout << "�������: ����� � �������� ID �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� �����
                cout << "�������� ���� ��� ���������:" << endl;
                cout << setw(10) << "ID" << setw(15) << "��'�" << setw(15) << "�������" << setw(10) << "�����" << setw(5) << "³�" << setw(22) << "����� ��������" << setw(15) << " ���� �����" << setw(15) << " ����� ������" << endl;
                cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;

                // ϳ����������� ��������� ����� ������������
                char confirmDelete;
                cout << "�� �������, �� ������ �������� ����� �����? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // ��������� ����� � ���� �����
                    pstmt = con->prepareStatement("DELETE FROM guests WHERE GuestId = ?");
                    pstmt->setInt(1, guestId);
                    int rowsAffected = pstmt->executeUpdate();

                    // ����������� ��� ��������� ���������
                    if (rowsAffected > 0) {
                        cout << "����� ������ ��������." << endl;
                    }
                    else {
                        cout << "�������: ����� � �������� ID �� ��������." << endl;
                    }
                }
                else {
                    cout << "��������� ����� ���������." << endl;
                }
            }

            // ��������� �������
            delete pstmt;
            delete res;

            // ����� ����������� ��� ������� �������� �� ������ �����
            char choice;
            cout << "������ �������� �� ������ �����? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // �������� � �����, ���� ���������� �� ���� �������� �� �����
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� �����: " << e.what() << endl;
    }
}

void Guest::updateGuest() {
    try {
        while (true) {
            string guestIdStr;
            // �������� ����������� ��� ������������� ����� ��� ���������
            cout << "������ ������������� �����, ����� ������� ������� ���������� (��� 0, ��� ����������� �� ��������� ����): ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, guestIdStr);

            if (guestIdStr == "0") {
                break; // �������� � �����, ���� ���������� ��� 0
            }

            if (isInputEmpty(guestIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �����." << endl;
                continue; // �������� ���� �����
            }

            int guestId;
            try {
                guestId = stoi(guestIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ������������� ����� �� ���� ������." << endl;
                continue; // �������� ���� �����
            }

            // ϳ�������� SQL-������ ��� ��������� ����� ��� ����� � �������� ���������������
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests WHERE GuestId = ?");
            pstmt->setInt(1, guestId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ����� � �������� ID �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� �������� �����
                cout << "�������� ���� ��� ��������� ����������:" << endl;
                cout << setw(10) << "ID" << setw(15) << "��'�" << setw(15) << "�������" << setw(10) << "�����" << setw(5) << "³�" << setw(22) << "����� ��������" << setw(15) << " ���� �����" << setw(15) << " ����� ������" << endl;
                cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;

                char confirmUpdate;
                cout << "�� �������, �� ������ ������� ���������� ��� ����� �����? (y/n): ";
                cin >> confirmUpdate;

                if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                    // ��������� ���� ��� ������, ��� ���������� �������
                    cout << "��� ���������� �� ������ �������?" << endl;
                    cout << "1. ��'�" << endl;
                    cout << "2. �������" << endl;
                    cout << "3. �����" << endl;
                    cout << "4. ³�" << endl;
                    cout << "5. ����� ��������" << endl;
                    cout << "6. ���� �����" << endl;
                    cout << "7. ����� ������" << endl;
                    cout << "������� ����� (������ ����� �� 1 �� 7): ";

                    int updateOption;
                    cin >> updateOption;

                    if (cin.fail() || updateOption < 0 || updateOption > 7) {
                        system("cls");
                        cout << "������������ ���. ���� �����, ������ ����� �� 0 �� 7." << endl;
                        cin.clear(); // ������� ���� �������
                        cin.ignore(INT_MAX, '\n'); // ������� ������� �����

                        continue; // ����������� �� ������������ ����
                    }

                    switch (updateOption) {
                    case 1: {
                        do {
                            cin.ignore();
                            cout << "���� ��'�: ";
                            getline(cin, firstName);

                            if (!isLetters(firstName)) {
                                cout << "�������: ��'� ������� ������ ���� �����.\n�������� ����-��� ������ ��� �����������..." << endl;
                            }
                        } while (isInputEmpty(firstName) || !isLetters(firstName));

                        // ��������� ���������� ��� ��'� �����
                        pstmt = con->prepareStatement("UPDATE guests SET FirstName = ? WHERE GuestId = ?");
                        pstmt->setString(1, firstName);
                        pstmt->setInt(2, guestId);
                        pstmt->executeUpdate();
                        cout << "��'� ����� �������� ������." << endl;
                        break;
                    }
                    case 2: {
                        do {
                            cin.ignore();
                            cout << "���� �������: ";
                            getline(cin, lastName);

                            if (!isLetters(lastName)) {
                                cout << "�������: ������� ������� ������ ���� �����.\n�������� ����-��� ������ ��� �����������..." << endl;
                            }
                        } while (isInputEmpty(lastName) || !isLetters(lastName));
                        // ��������� ���������� ��� ������� �����
                        pstmt = con->prepareStatement("UPDATE guests SET LastName = ? WHERE GuestId = ?");
                        pstmt->setString(1, lastName);
                        pstmt->setInt(2, guestId);
                        pstmt->executeUpdate();
                        cout << "������� ����� �������� ������." << endl;
                        break;
                    }
                    case 3: {
                        do {
                            cout << "���� ����� (�������/�����): ";
                            cin.clear(); // ������� ���� �������
                            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                            getline(cin, gender);

                            if (isInputEmpty(gender)) {
                                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ �����." << endl;
                                continue;
                            }
                            if (!checkGenderInput(gender)) {
                                cout << "�������: ����������� ������� �����. ���� �����, ������ '�������' ��� '�����'." << endl;
                                continue; // �������� ���� �����, ���� ������� ����������� ��������
                            }
                            // ��������� ���������� ��� ����� �����
                            pstmt = con->prepareStatement("UPDATE guests SET Gender = ? WHERE GuestId = ?");
                            pstmt->setString(1, gender);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "����� ����� �������� ������." << endl;
                            break; // ����� �� �����, ���� ������� ��������� ��������
                        } while (true); // ���������� ����, ���� ���������� �� ����� �������� ��������
                        break;
                    }
                    case 4: {
                        do {
                            string newAgeStr;
                            cout << "����� ��: ";
                            cin.clear(); // ������� ���� �������
                            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                            getline(cin, newAgeStr);
                            // ��������� ���������� ��� �� �����
                            if (isInputEmpty(newAgeStr)) {
                                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ��." << endl;
                                continue; // �������� ���� �����, ���� ������� ������ ��������
                            }
                            if (!isNumber(newAgeStr)) {
                                cout << "�������: ³� �� ���� �������� ����� ������." << endl;
                                continue; // �������� ���� �����, ���� ������� ����������� ��������
                            }
                            long long newAge = stoll(newAgeStr); // ������������� stoll ��� ������� ������� �����
                            if (!isValidAge(newAge)) {
                                cout << "�������: ����������� �������� ��. ���� �����, ������ ��������� �� �� 0 �� 125." << endl;
                                continue; // �������� ���� �����, ���� ������� ����������� ��������
                            }
                            pstmt = con->prepareStatement("UPDATE guests SET Age = ? WHERE GuestId = ?");
                            pstmt->setInt(1, newAge);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "³� ����� �������� ������." << endl;
                            break; // ����� �� �����, ���� ������� ��������� ��������
                        } while (true); // ���������� ����, ���� ���������� �� ����� �������� ��������
                        break;
                    }

                    case 5: {
                        do {
                            string newPhoneNumber;
                            cout << "����� ����� �������� (����� 8 �������): ";
                            cin.clear(); // ������� ���� �������
                            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                            getline(cin, newPhoneNumber);
                            // ��������� ���������� ��� ����� �������� �����
                            if (isInputEmpty(newPhoneNumber)) {
                                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ����� ��������." << endl;
                                continue; // �������� ���� �����, ���� ������� ������ ��������
                            }
                            if (newPhoneNumber.length() < 8 || !isValidPhoneNumber(newPhoneNumber)) {
                                cout << "�������: ����� �������� ������� ������ ���������� 8 ���� � ������ ����� �����." << endl;
                                continue; // �������� ���� �����, ���� ������� ����������� ��������
                            }
                            pstmt = con->prepareStatement("UPDATE guests SET PhoneNumber = ? WHERE GuestId = ?");
                            pstmt->setString(1, newPhoneNumber);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "����� �������� ����� �������� ������." << endl;
                            break; // ����� �� �����, ���� ������� ��������� ��������
                        } while (true); // ���������� ����, ���� ���������� �� ����� �������� ��������
                        break;
                    }
                    case 6: {
                        do {
                            string newCheckInDate;
                            cout << "���� ���� ����� (��-�����-����): ";
                            cin.clear(); // ������� ���� �������
                            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                            getline(cin, newCheckInDate);
                            if (isInputEmpty(newCheckInDate)) {
                                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ���� �����." << endl;
                                continue; // �������� ���� �����, ���� ������� ������ ��������
                            }
                            if (!isValidDateFormat(newCheckInDate)) {
                                cout << "�������: ������������ ������ ����. ������ ���� � ������ '��-�����-����'." << endl;
                                continue; // �������� ���� �����, ���� ������� ������������ ������ ����
                            }
                            // ���� �������� �� �������� ���� (isValidDate) ���

                            // ���� ������� ����� �������, ������� ���� ������� ��������
                            pstmt = con->prepareStatement("UPDATE guests SET CheckInDate = ? WHERE GuestId = ?");
                            pstmt->setString(1, newCheckInDate);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "���� ����� ����� �������� ������." << endl;
                            break; // ����� �� �����, ���� ������� ��������� ��������
                        } while (true); // ���������� ����, ���� ���������� �� ����� �������� ��������
                        break;
                    }
                    case 7: {
                        do {
                            string newRoomNumberStr;
                            cout << "����� ����� ������: ";
                            cin.clear(); // ������� ���� �������
                            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                            getline(cin, newRoomNumberStr);
                            // ��������� ���������� ��� ����� ������ �����
                            if (!isNumber(newRoomNumberStr)) {
                                cout << "�������: ����� ������ �� ���� �������� ����� ������." << endl;
                                continue; // �������� ���� �����, ���� ������� ����������� ��������
                            }
                            if (isInputEmpty(newRoomNumberStr)) {
                                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ����� ������." << endl;
                                continue; // �������� ���� �����, ���� ������� ������ ��������
                            }
                            int newRoomNumber = stoi(newRoomNumberStr);
                            if (!isRoomAvailable(newRoomNumber)) {
                                cout << "�������: ʳ����� � ������� " << newRoomNumber << " ������� ��� �� ����." << endl;
                                continue; // �������� ���� �����, ���� ������� ����������� ��������
                            }
                            pstmt = con->prepareStatement("UPDATE guests SET RoomNumber = ? WHERE GuestId = ?");
                            pstmt->setInt(1, newRoomNumber);
                            pstmt->setInt(2, guestId);
                            pstmt->executeUpdate();
                            cout << "����� ������ ����� �������� ������." << endl;
                            break; // ����� �� �����, ���� ������� ��������� ��������
                        } while (true); // ���������� ����, ���� ���������� �� ����� �������� ��������
                        break;
                    }

                    default: {
                        cout << "�������: ������� ������������ ����� �����." << endl;
                        break;
                    }
                    }
                }
                else {
                    cout << "��������� ���������� ��� ����� ���������." << endl;
                }

                delete pstmt;
                delete res;

                char choice;
                cout << "������ ������� ���������� �� ������ �����? (y/n): ";
                cin >> choice;

                // ��������, �� ���������� ��� ���������� ������
                if (choice != 'y' && choice != 'n') {
                    cout << "�������: ���� �����, ������ 'y' ��� 'n'." << endl;
                    continue; // �������� ���� �����
                }

                if (choice == 'n' || choice == 'N') {
                    break; // �������� � �����, ���� ���������� �� ���� ���������� ���������� �� ������ �����
                }
            }
        }
    }
    catch (SQLException& e) {
        // ������� ������� SQLException
        cout << "������� ��������� ���������� ��� �����: " << e.what() << endl;
    }
}


void Guest::sortGuests() {
    try {
        cout << "������ ������� ����������:" << endl;
        cout << "1. ���������� ���� �� ��������� ��������(�� \"�\" �� \"�\")." << endl;
        cout << "2. ���������� ������ �� ��������� ��������(�� \"�\" �� \"�\")." << endl;
        cout << "3. ³� ������������ �� �����������." << endl;
        cout << "4. ³� ����������� �� ������������." << endl;
        cout << "5. �� ����� �����." << endl;
        cout << "0. ����������� �� ������������ ����." << endl;
        cout << "��� ����: ";

        int sortingCriterion;
        do {
            cin >> sortingCriterion;

            if (isInputEmpty(to_string(sortingCriterion))) {
                cout << "�������: ������� ���������� �� ���� ���� �������. ���� �����, ������ ����� �� 0 �� 5: ";
            }
            else if (cin.fail() || sortingCriterion < 0 || sortingCriterion > 5) {
                system("cls");
                cout << "������������ ���. ���� �����, ������ ����� �� 0 �� 5: ";
                cin.clear(); // ������� ���� �������
                cin.ignore(INT_MAX, '\n'); // ������� ������� �����
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
            return; // ����������� �� ������������ ����
        default:
            cout << "������� ����. ���� �����, ������ ����� �� 0 �� 5." << endl;
            return;
        }

        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM guests " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // ��������� ������������� ������ ������
        cout << setw(10) << "ID" << setw(15) << "��'�" << setw(15) << "�������" << setw(10) << "�����" << setw(5) << "³�" << setw(22) << "����� ��������" << setw(15) << " ���� �����" << setw(15) << " ����� ������" << endl;
        while (res->next()) {
            cout << setw(10) << res->getInt("GuestId") << setw(15) << res->getString("FirstName") << setw(15) << res->getString("LastName") << setw(10) << res->getString("Gender") << setw(5) << res->getInt("Age") << setw(22) << res->getString("PhoneNumber") << setw(15) << res->getString("CheckInDate") << setw(15) << res->getInt("RoomNumber") << endl;
        }

        delete pstmt;
        delete res;

        // ��������, ���� ���������� �� ������� ����-��� ������, ����� ����������� ��������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore(); // �������� ��������� ���
        cin.get();    // ������� �� ����� ��� �� �����������
    }
    catch (const SQLException& e) {
        cout << "������� �� ��� ��������� ������ �� ���� �����: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "������� �������: " << ex.what() << endl;
    }
}



