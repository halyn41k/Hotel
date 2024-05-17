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
            // ��������� ����������� ��� �������� ID ���������� ��� 0 ��� ��������� ���
            cout << "~~ ������ ID ���������� ��� ������ 0, ��� ������� �� ~~" << endl;

            string bookingIdStr;
            cout << "ID ����������: ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, bookingIdStr);


            // �������� �� ��������� �� ������ ������� isInputEmpty
            if (isInputEmpty(bookingIdStr)) {
                cout << "�������: ID ���������� �� ���� ���� �������." << endl;
                continue;
            }

            // ��������, �� �������� ����� ���������� ���� � ����
            if (bookingIdStr.find_first_not_of("0123456789") != string::npos) {
                cout << "�������: �������� ID ���������� �� ���� ������ ��� 0." << endl;
                continue;
            }

            int bookingId = stoi(bookingIdStr); // ������������ ����� � ���� �����

            // ����� �� ���� ����� �� ����������� ����������
            PreparedStatement* pstmt;
            ResultSet* res;
            if (bookingId == 0) {
                // ��������� ��� ���������
                pstmt = con->prepareStatement("SELECT * FROM bookings");
                res = pstmt->executeQuery();
            }
            else {
                // ��������� ���������� �� ������� ID
                pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
                pstmt->setInt(1, bookingId);
                res = pstmt->executeQuery();
            }

            if (!res->next()) {
                cout << "���� ������ ��� �����������." << endl;
            }
            else {
                // ��������� ��������� �������
                cout << setw(15) << "ID ����������" << setw(15) << "ID �����" << setw(15) << "����� ������" << setw(20) << "���� ����������" << setw(15) << "�����" << endl;

                // ��������� ����� ��� ����������
                do {
                    cout << setw(15) << res->getInt("BookingId") << setw(15) << res->getInt("GuestId") << setw(15) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;
                } while (res->next());
            }

            delete pstmt;
            delete res;

            // ���������� ������ ����������� ��� ����������� ��� ����������
            char choice = getValidChoice();

            if (choice == 'y' || choice == 'Y') {
                return;
            }
        }
    }
    // ������� �������
    catch (const std::invalid_argument& ia) {
        cout << "�������: �������� ID ���������� �� ���� ������ ��� 0." << endl;
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

void Booking::addBooking() {
    try {
        // ��������� ����������� ��� ��������� ������ ����������
        cout << "~~ ������ ���� ���������� ~~" << endl;

        int guestId, roomNumber;
        string bookingDate, term;

        // �������� ID �����
        bool validGuestId = false;
        while (!validGuestId) {
            cout << "ID �����: ";
            cin.clear(); // �������� ����� ������� ��������
            cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
            string guestIdInput;
            getline(cin, guestIdInput);

            stringstream guestIDStream(guestIdInput);
            guestIDStream >> guestId;

            if (cin.fail()) {
                cout << "�������: ������ ����� ��� ID �����." << endl;
                cin.clear(); // �������� ����� ������� ��������
                cin.ignore(INT_MAX, '\n'); // ����������� ������������� �����
            }
            else if (guestId == 0) {
                cout << "�������: ID ����� �� ���� ���� 0. ���� �����, ������ ��������� ID �����." << endl;
            }
            else {
                if (!checkGuestId(guestId)) {
                    cout << "�������: ����� � ����� ID �� ����." << endl;
                }
                else {
                    validGuestId = true;
                }
            }
        }

        // �������� �������� ������ ������
        bool validRoomNumber = false;
        while (!validRoomNumber) {
            cout << "����� ������: ";
            string roomNumberInput;
            cin.clear(); // �������� ����� ������� ��������
            cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
            getline(cin, roomNumberInput);
            stringstream roomNumberStream(roomNumberInput);
            roomNumberStream >> roomNumber;
            if (!checkRoomNumber(roomNumber)) {
                cout << "�������: ʳ����� � ����� ������� �� ����." << endl;
                cin.clear(); // �������� ����� ������� ��������
                cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
            }
            else {
                validRoomNumber = true;
            }
        }

        // �������� ���� ����������
        bool validDate = false;
        do {
            cout << "���� ���������� (��-�����-����): ";
            cin.clear(); // �������� ����� ������� ��������
            cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
            getline(cin,bookingDate);

            if (isInputEmpty(bookingDate)) {
                cout << "�������: ���� ���������� �� ���� ���� ���������." << endl;
            }
            else {
                if (!isValidDateFormat(bookingDate)) {
                    cout << "�������: ������������ ������ ����. ���� �����, ������ ���� � ������ '��-�����-����'." << endl;
                }
                else {
                    validDate = true;
                }
            }
        } while (!validDate);

        // �������� ������
        bool validTerm = false;
        do {
            cout << "�����: ";
            cin.clear(); // �������� ����� ������� ��������
            cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
            getline(cin, term);

            if (isInputEmpty(term)) {
                cout << "�������: ����� �� ���� ���� �������." << endl;
            }
            else {
                validTerm = true;
            }
        } while (!validTerm);

        // ��������� ������ ���������� �� ���� �����
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO bookings (GuestId, RoomNumber, BookingDate, Term) VALUES (?, ?, ?, ?)");
        pstmt->setInt(1, guestId);
        pstmt->setInt(2, roomNumber);
        pstmt->setString(3, bookingDate);
        pstmt->setString(4, term);

        pstmt->executeUpdate();

        cout << "���� ���������� ������ ������!" << endl;

        delete pstmt;

        // ���������� ������ ����������� ��� ����������� ��� ����������
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // ���������� ��� ��������� ������, ���� ���������� ������ 'n' ��� 'N'
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

// �������� ��������� ����� � �������� ID
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
        cout << "������� SQL: " << e.what() << endl;
        return false;
    }
}


// �������� ��������� ������ � �������� �������
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
        cout << "������� SQL: " << e.what() << endl;
        return false;
    }
}
void Booking::deleteBooking() {
    try {
        while (true) {
            string bookingIdStr;
            cout << "������ ID ����������, ��� �� ������ ��������: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, bookingIdStr);

            if (isInputEmpty(bookingIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ �������� ��������." << endl;
                continue; // �������� ���� �����
            }

            if (bookingIdStr == "0") {
                break; // �������� � �����, ���� ���������� ��� 0
            }

            int bookingId;
            try {
                bookingId = stoi(bookingIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ID ���������� �� ���� ������ ��� 0." << endl;
                continue; // �������� ���� �����
            }

            // ϳ�������� �� ��������� ������ �� ���� ����� ��� ��������� ���������� ��� ����������
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
            pstmt->setInt(1, bookingId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ���������� � �������� ID �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� ������ ���������� ��� ������������ ���������
                cout << "������� ���������� ��� ���������:" << endl;
                cout << setw(10) << "ID ����������" << setw(10) << "ID �����" << setw(17) << "����� ������" << setw(20) << "���� ����������" << setw(15) << "�����" << endl;
                cout << setw(10) << res->getInt("BookingId") << setw(10) << res->getInt("GuestId") << setw(17) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;

                char confirmDelete;
                cout << "�� �������, �� ������ �������� �� ����������? (y/n): ";
                cin >> confirmDelete;

                if (isInputEmpty(to_string(confirmDelete))) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ y ��� n." << endl;
                    continue; // �������� ���� �����
                }

                // ϳ����������� ��� ���������� ��������� ����������
                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    pstmt = con->prepareStatement("DELETE FROM bookings WHERE BookingId = ?");
                    pstmt->setInt(1, bookingId);
                    int rowsAffected = pstmt->executeUpdate();

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

            delete pstmt;
            delete res;

            char choice;
            cout << "������ �������� �� ���� ����������? (y/n): ";
            cin >> choice;

            if (isInputEmpty(to_string(choice))) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ y ��� n." << endl;
                continue; // �������� ���� �����
            }

            if (choice == 'n' || choice == 'N') {
                break; // �������� � �����, ���� ���������� �� ���� �������� �� ����������
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� ����������: " << e.what() << endl;
    }
}
void Booking::updateBooking() {
    try {
        while (true) {
            string bookingIdStr;
            cout << "������ ID ����������, ��� �� ������ ������� (��� 0, ��� ����������� �� ��������� ����): ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, bookingIdStr);

            if (isInputEmpty(bookingIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ �������� ��������." << endl;
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
                cout << "�������: �������� ID ���������� �� ���� ������ ��� 0." << endl;
                continue;
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
            pstmt->setInt(1, bookingId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ���������� � �������� ID �� ��������." << endl;
                delete pstmt;
                delete res;
                continue;
            }

            cout << "������ �������� ��� ���������:" << endl;
            cout << "1. ����� ������" << endl;
            cout << "2. ���� ����������" << endl;
            cout << "3. �����" << endl;
            cout << "0. ����������� �� ��������� ����" << endl;
            int updateChoice;
            cout << "��� ����: ";
            cin >> updateChoice;

            switch (updateChoice) {
            case 0:
                delete pstmt;
                delete res;
                return;
            case 1: {
                // ��������� ������ ������
                do {
                    string roomNumberStr;
                    cout << "������ ����� ����� ������: ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, roomNumberStr);
                    if (isInputEmpty(roomNumberStr) || !isNumber(roomNumberStr)) {
                        cout << "�������: ����� ������ �� ���� �������� ����� ������." << endl;
                        continue;
                    }
                    int newRoomNumber = stoi(roomNumberStr);

                    PreparedStatement* checkRoomStmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
                    checkRoomStmt->setInt(1, newRoomNumber);
                    ResultSet* roomRes = checkRoomStmt->executeQuery();
                    if (!roomRes->next()) {
                        cout << "�������: ʳ����� � �������� ������� �� ����." << endl;
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
                // ��������� ���� ����������
                do {
                    string newBookingDate;
                    cout << "������ ���� ���� ���������� (��-�����-����): ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, newBookingDate);
                    if (isInputEmpty(newBookingDate) || !isValidDateFormat(newBookingDate)) {
                        cout << "�������: ����������� ������ ����. ���� �����, �������������� ������ ��-�����-����." << endl;
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
                // ��������� ������
                do {
                    string newTermStr;
                    cout << "������ ����� ����� (�� 1 �� 30 ���): ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, newTermStr);
                    if (isInputEmpty(newTermStr) || !isNumber(newTermStr)) {
                        cout << "�������: ����� �� ���� �������� ����� ������." << endl;
                        continue;
                    }
                    int newTerm = stoi(newTermStr);
                    if (newTerm < 1 || newTerm > 30) {
                        cout << "�������: ����� ������� ���� � ����� �� 1 �� 30." << endl;
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
                cout << "���� �����, ������ ��������� ������ (0-3)." << endl;
                continue;
            }


            char confirmUpdate;
            cout << "�� �������, �� ������ ������� �� ����������? (y/n): ";
            cin >> confirmUpdate;

            if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                int rowsAffected = pstmt->executeUpdate();

                if (rowsAffected > 0) {
                    cout << "���������� ��� ���������� ������ ��������." << endl;
                }
                else {
                    cout << "�������: �� ������� ������� ���������� ��� ����������." << endl;
                }
            }
            else {
                cout << "��������� ���������� ���������." << endl;
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� ���������� ��� ����������: " << e.what() << endl;
    }
}



void Booking::sortBookings() {
    try {
        // ����� ����������� �� ���� ������� ����������
        int sortingCriterion;
        cout << "������ ������� ���������� ���������:" << endl;
        cout << "1. �� ID ����������." << endl;
        cout << "2. �� ID �����." << endl;
        cout << "3. �� ������� ������." << endl;
        cout << "4. �� ����� ����������." << endl;
        cout << "5. �� �������." << endl;
        cout << "0. ����������� �� ������������ ����." << endl;
        cout << "��� ����: ";

        // ��������, �� ���������� ��� �������� ���� �����
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("������� �����. ���� �����, ������ ���� �����.");
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
            return; // ���������� �� ������������ ����
        default:
            cout << "������� ����. ���� �����, ������ ����� �� 0 �� 5." << endl;
            return; // ����� � ������� � ��� �������� ������
        }

        // ��������� ������ �� ���� ����� � ������������� ��������� ������� ����������
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // ��������� ������������� ������ ���������
        cout << setw(15) << "ID ����������" << setw(10) << "ID �����" << setw(17) << "����� ������" << setw(20) << "���� ����������" << setw(15) << "�����" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("BookingId") << setw(10) << res->getInt("GuestId") << setw(17) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;
        }

        delete pstmt;
        delete res;

        // ���������� ���������� ������������ ����-��� ������ ����� ������������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore();
        cin.get();
    }
    catch (const SQLException& e) {
        cout << "������� �� ��� ��������� ������ �� ���� �����: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "������� �������: " << ex.what() << endl;
    }
}
