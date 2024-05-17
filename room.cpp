#include "room.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "hotelconnecting.h"
#include "utils.h"
#include <sstream>

Room::Room() : roomNumber(0), roomType("none"), status("none"), capacity(0), price(0.0), hotel(*(new HotelConnecting())) {}
Room::Room(HotelConnecting& h) : hotel(h) {}

// ����� ��������� ���������� ��� ������
void Room::displayRooms() {
    try {
        while (true) {
            cout << "~~ ������ ����� ������ ��� ������ 0, ��� ������� �� ~~" << endl;
            string roomNumberStr;
            cout << "����� ������: ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, roomNumberStr);

            if (isInputEmpty(roomNumberStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ����� ������ ��� 0." << endl;
                continue; // �������� ���� �����
            }

            if (roomNumberStr == "0") {
                // ��������� ���������� ��� �� ������, ���� ������� 0
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "���� ������ ��� �����������." << endl;
                }
                else {
                    // ��������� ��������� �������
                    cout << setw(10) << "����� ������" << setw(17) << "���" << setw(15) << "������" << setw(15) << "���������" << setw(10) << "ֳ��" << endl;
                    // ��������� ����� ����� ������
                    do {
                        cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // ����������� ����� ��� ��������� �����, ���� ���������� ������ 'n' ��� 'N'
                }
            }
            else {
                int roomNumber;
                try {
                    roomNumber = stoi(roomNumberStr); // ����������� ����� � ���� �����
                }
                catch (const std::invalid_argument& ia) {
                    cout << "�������: �������� ����� ������ �� ���� ������ ��� 0." << endl;
                    continue; // �������� ���� �����
                }

                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
                pstmt->setInt(1, roomNumber);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "�������: ʳ����� � �������� ������� �� ��������." << endl;
                }
                else {
                    // ��������� ��������� �������
                    cout << setw(10) << "����� ������" << setw(17) << "���" << setw(15) << "������" << setw(15) << "���������" << setw(10) << "ֳ��" << endl;
                    // ��������� ����� ������ � �������� �������
                    cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;
                }

                delete pstmt;
                delete res;

                char choice = getValidChoice(); // ��������� ���������� ������ �� �����������

                if (choice == 'y' || choice == 'Y') {
                    return; // ����������� ����� ��� ��������� �����, ���� ���������� ������ 'n' ��� 'N'
                }
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl; // ������� ������� SQLException
    }
}


bool Room::isNumber(string str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }

}
// ����� ��������� ���� ������
void Room::addRoom() {
    try {
        cout << "~~ ������ ���� ������ ~~" << endl;

        // �������� ���� ������
        do {
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            cout << "��� ������ (����������/����/�������������): ";
            getline(cin,roomType);

            if (roomType != "����������" && roomType != "����" && roomType != "�������������") {
                cout << "�������: ����������� �������� ��� ������. ���� �����, ������ '����������', '����' ��� '�������������'." << endl;
            }
        } while (isInputEmpty(roomType) || roomType != "����������" && roomType != "����" && roomType != "�������������");

        status = "��������";

        // �������� ��������� ������
        do {
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            cout << "��������� (�� 1 �� 4): ";
            string capacityInput;
            getline(cin, capacityInput);
            stringstream capacityStream(capacityInput);
            capacityStream >> capacity;

            if (capacity < 1 || capacity > 4) {
                cout << "�������: ��������� ������ ������� ���� �� 1 �� 4." << endl;
            }
        } while (isInputEmpty(to_string(capacity)) || capacity < 1 || capacity > 4);

        bool validPrice = false; // ���� ��� �������� ���������� �������� ����

        do {
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            cout << "ֳ�� �� �� (�� 10 �� 100000): ";
            string priceInput;
            getline(cin, priceInput);
            stringstream priceStream(priceInput);
            priceStream >> price;

            if (cin.fail() || price < 10 || price > 100000) {
                cin.clear(); // ������� ����� ������� �����
                cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
                cout << "�������: ֳ�� �� �� ������� ���� ������ �� 10 �� 100000." << endl;
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

        cout << "���� ������ ������ ������!" << endl;

        delete pstmt;
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // ����������� ����� ��� ��������� ������, ���� ���������� ������ 'n' ��� 'N'
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

void Room::deleteRoom() {
    try {
        while (true) {
            string roomNumberStr;
            cout << "������ ����� ������, ��� �� ������ ��������: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, roomNumberStr);

            if (roomNumberStr == "0") {
                break; // �������� � �����, ���� ���������� ��� 0
            }
            if (isInputEmpty(roomNumberStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ����� ������." << endl;
                continue; // �������� ���� �����
            }

            int roomNumber;
            try {
                roomNumber = stoi(roomNumberStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ����� ������ �� ���� ������ ��� 0." << endl;
                continue; // �������� ���� �����
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
            pstmt->setInt(1, roomNumber);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ʳ����� � �������� ������� �� ��������." << endl;
            }
            else {
                cout << "������� ������ ��� ���������:" << endl;
                cout << setw(10) << "����� ������" << setw(17) << "���" << setw(15) << "������" << setw(15) << "���������" << setw(10) << "ֳ��" << endl;
                cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;

                char confirmDelete;
                cout << "�� �������, �� ������ �������� �� ������? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // ��������� � ������� RoomCleaningStatus
                    pstmt = con->prepareStatement("DELETE FROM RoomCleaningStatus WHERE RoomNumber = ?");
                    pstmt->setInt(1, roomNumber);
                    pstmt->executeUpdate();
                    delete pstmt;

                    // ��������� � ������� rooms
                    pstmt = con->prepareStatement("DELETE FROM rooms WHERE RoomNumber = ?");
                    pstmt->setInt(1, roomNumber);
                    int rowsAffected = pstmt->executeUpdate();
                    delete pstmt;

                    if (rowsAffected > 0) {
                        cout << "ʳ����� ������ ��������." << endl;
                    }
                    else {
                        cout << "�������: ʳ����� � �������� ������� �� ��������." << endl;
                    }
                }
                else {
                    cout << "��������� ������ ���������." << endl;
                }
            }

            delete res;

            char choice;
            cout << "������ �������� �� ���� ������? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // �������� � �����, ���� ���������� �� ���� �������� �� ������
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� ������: " << e.what() << endl;
    }
}


// ����� ��������� ���������� ��� ������
void Room::updateRoom() {
    try {
        while (true) {
            string roomNumberStr;
            // ������� ����������� ������ ����� ������ ��� ���������
            cout << "������ ����� ������, ��� �� ������ ������� (��� 0, ��� ����������� �� ��������� ����): ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, roomNumberStr);

            // ����������, �� ���������� �� ������ ���������� �� ��������� ����
            if (roomNumberStr == "0") {
                break; // �������� � �����, ���� ���������� ��� 0
            }
            // ����������, �� ���������� ��� ������� �����
            if (isInputEmpty(roomNumberStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ����� ������." << endl;
                continue; // �������� ���� �����
            }

            int roomNumber;
            try {
                roomNumber = stoi(roomNumberStr);
            }
            // ������� �������, ���� ������� ���������� �����
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ����� ������ �� ���� ������ ��� 0." << endl;
                continue; // �������� ���� �����
            }

            // ϳ�������� ������ �� ���� ����� ��� ��������� ���������� ��� ������ �� �������� �������
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
            pstmt->setInt(1, roomNumber);
            ResultSet* res = pstmt->executeQuery();

            // ��������, �� ���� ������ � �������� �������
            if (!res->next()) {
                cout << "�������: ʳ����� � �������� ������� �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� ������ ��� ���������
                cout << "������� ������ ��� ���������:" << endl;
                cout << setw(10) << "����� ������" << setw(17) << "���" << setw(15) << "������" << setw(15) << "���������" << setw(10) << "ֳ��" << endl;
                cout << setw(10) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;

                // ���� ��������� ��� ���������
                cout << "������ �������� ��� ���������:" << endl;
                cout << "1. ��� ������" << endl;
                cout << "2. ������ ������" << endl;
                cout << "3. ��������� ������" << endl;
                cout << "4. ֳ�� �� ��" << endl;
                cout << "0. ����������� �� ��������� ����" << endl;
                int updateChoice;
                cout << "��� ����: ";
                cin >> updateChoice;

                // �������� ������ �����������
                if (isInputEmpty(to_string(updateChoice))) {
                    cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                    continue; // �������� ���� �����
                }

                // ������� ������ ����������� ��� ���������
                switch (updateChoice) {
                case 0:
                    break; // ����������� �� ��������� ����
                case 1:
                    // ��������� ���� ������
                    do {
                        cout << "������ ����� ��� ������ (����������/����/�������������): ";
                        cin.clear(); // ������� ���� �������
                        cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                        getline(cin, roomType);
                        // �������� ���������� ��������� ���� ������
                        if (roomType != "����������" && roomType != "����" && roomType != "�������������") {
                            cout << "�������: ������� ������������ ��� ������. ������� � ������." << endl;
                            continue; // �������� ���� �����
                        }
                        // �������� �� ��������� ����
                        if (isInputEmpty(roomType)) {
                            cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                            continue; // �������� ���� �����
                        }
                        break; // ����� �� �����, ���� ������� ��������� ��������
                    } while (true);
                    break;
                case 2:
                    // ��������� ������� ������
                    do {
                        cout << "������ ����� ������ ������ (�������/��������): ";
                        cin.clear(); // ������� ���� �������
                        cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                        getline(cin, status);
                        // �������� ���������� ��������� ������� ������
                        if (status != "�������" && status != "��������") {
                            cout << "�������: ������� ������������ ������ ������. ������� � ������." << endl;
                            continue; // �������� ���� �����
                        }
                        // �������� �� ��������� ����
                        if (isInputEmpty(roomType)) {
                            cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                            continue; // �������� ���� �����
                        }
                        break; // ����� �� �����, ���� ������� ��������� ��������
                    } while (true);
                    break;
                case 3:
                    // ��������� ��������� ������
                    do {
                        cout << "������ ���� ��������� ������ (1-4): ";
                        string capacityInput;
                        cin.clear(); // ������� ���� �������
                        cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                        getline(cin, capacityInput);
                        stringstream capacityStream(capacityInput);
                        capacityStream >> capacity;
                        // �������� ���������� ������� ��������� ������
                        if (capacity < 1 || capacity > 4) {
                            cout << "�������: ������� ����������� ��������� ������. ������ ����� �� 1 �� 4." << endl;
                            continue; // �������� ���� �����
                        }
                        // �������� �� ��������� ����
                        if (isInputEmpty(roomType)) {
                            cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                            continue; // �������� ���� �����
                        }
                        break; // ����� �� �����, ���� ������� ��������� ��������
                    } while (true);
                    break;
                case 4:
                    // ��������� ���� �� ��
                    do {
                        cin.clear(); // ������� ���� �������
                        cin.ignore(INT_MAX, '\n'); // ������� ������� �����
                        cout << "������ ���� ���� �� �� (�� 10 �� 100000): ";
                        string priceInput;
                        getline(cin, priceInput);
                        stringstream priceStream(priceInput);
                        priceStream >> price;
                        // �������� ���������� ������� ���� �� ��
                        if (price < 10 || price > 100000) {
                            cout << "�������: ������� ����������� ���� �� ��. ������ ����� �� 10 �� 100000." << endl;
                            continue; // �������� ���� �����
                        }
                        // �������� �� ��������� ����
                        if (isInputEmpty(roomType)) {
                            cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                            continue; // �������� ���� �����
                        }
                        break; // ����� �� �����, ���� ������� ��������� ��������
                    } while (true);
                    break;
                default:
                    // ��������� ����������� ��� ����������� ����
                    cout << "���� �����, ������ ��������� ������ (0-4)." << endl;
                    continue; // �������� ���� �����
                }

                // ϳ����������� ���������
                char confirmUpdate;
                cout << "�� �������, �� ������ ������� �� ������? (y/n): ";
                cin >> confirmUpdate;

                if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                    // ��������� ������ ��������� ���������� ��� ������
                    pstmt = con->prepareStatement("UPDATE rooms SET RoomType = ?, Status = ?, Capacity = ?, Price = ? WHERE RoomNumber = ?");
                    pstmt->setString(1, roomType);
                    pstmt->setString(2, status);
                    pstmt->setInt(3, capacity);
                    pstmt->setDouble(4, price);
                    pstmt->setInt(5, roomNumber);

                    int rowsAffected = pstmt->executeUpdate();

                    if (rowsAffected > 0) {
                        cout << "���������� ��� ������ ������ ��������." << endl;
                    }
                    else {
                        cout << "�������: �� ������� ������� ���������� ��� ������." << endl;
                    }
                }
                else {
                    cout << "��������� ������ ���������." << endl;
                }
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cerr << "������� SQL: " << e.what() << endl;
    }
}


void Room::sortRooms() {
    try {
        int sortingCriterion;
        // �������� ���� ��� ������ ������� ���������� �����
        cout << "������ ������� ���������� �����:" << endl;
        cout << "1. �� ������� ������." << endl;
        cout << "2. �� ����� ������." << endl;
        cout << "3. �� �������� ������." << endl;
        cout << "4. �� ��������� ������." << endl;
        cout << "5. �� ����� ������." << endl;
        cout << "0. ����������� �� ������������ ����." << endl;
        cout << "��� ����: ";

        // ����������, �� �������� �������� ������� ����������
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("������� �����. ���� �����, ������ ���� �����.");
        }

        // �������� �� ������� ���
        if (isInputEmpty(to_string(sortingCriterion))) {
            cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
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
            return; // ����������� �� ������������ ����
        default:
            cout << "������� ����. ���� �����, ������ ����� �� 0 �� 5." << endl;
            return;
        }

        // ϳ�������� �� ��������� ������ �� ���� ����� ��� ������������� ������ �����
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM rooms " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // ��������� ������������� ������ �����
        cout << setw(15) << "����� ������" << setw(17) << "���" << setw(15) << "������" << setw(15) << "���������" << setw(10) << "ֳ��" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("RoomNumber") << setw(20) << res->getString("RoomType") << setw(15) << res->getString("Status") << setw(15) << res->getInt("Capacity") << setw(10) << fixed << setprecision(2) << res->getDouble("Price") << endl;
        }

        delete pstmt;
        delete res;

        // ��������, ���� ���������� �� ������� ����-��� ������, ����� ����������� ��������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore(); // �������� ��������� ���
        cin.get();    // ������� �� ����� ��� �� �����������
    }
    // ������� �������
    catch (const SQLException& e) {
        cout << "������� �� ��� ��������� ������ �� ���� �����: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "������� �������: " << ex.what() << endl;
    }
}

void Room::updateCleaningStatus() {
    int roomNumber;
    string cleaned;
    try {
        // �������� ������ ������ �� �������� �� ���������� �����
        do {
            cout << "������ ����� ������: ";
            cin >> roomNumber;

            // ��������, �� ������� ���� �����
            if (cin.fail()) {
                cout << "�������: �������� ����� ������ �� ���� ������ ��� 0." << endl;
                cin.clear(); // ������� ��������� ������� ��������
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // �������� �������� �����
                continue;
            }

            // ��������, �� ���� ������ � ����� �������
            Statement* stmt = con->createStatement();
            ResultSet* res = stmt->executeQuery("SELECT * FROM RoomCleaningStatus WHERE RoomNumber = " + to_string(roomNumber));
            if (!res->next()) {
                cout << "ʳ����� � ������� " << roomNumber << " �� ����." << endl;
                delete res;
                delete stmt;
                cout << "��������� �� ���." << endl;
                continue;
            }
            delete res;
            delete stmt;

            break; // ����� � �����, ���� ������� ��������� ����� ������

        } while (true);

        // ��������� ������� ���������� �� �����������
        int choice;
        cout << "������ ������ ���������� (0 - �� ��������, 1 - ��������): ";
        cin >> choice;

        // ������������ �������� ��� ���� "Cleaned" ������� �� ������ �����������
        if (choice == 0) {
            cleaned = "�� ��������";
        }
        else if (choice == 1) {
            cleaned = "��������";
        }
        else {
            cout << "������������ ���. ������ ���������� ������ �� ��������." << endl;
            return;
        }

        // ��������� ������� ���������� � ��� �����
        PreparedStatement* pstmt = con->prepareStatement("UPDATE RoomCleaningStatus SET Cleaned=? WHERE RoomNumber=?");
        pstmt->setString(1, cleaned);
        pstmt->setInt(2, roomNumber);
        pstmt->executeUpdate();
        delete pstmt;
        cout << "������ ���������� ������ " << roomNumber << " ��������." << endl;

        // ��������, ���� ���������� �� ������� ����-��� ������, ����� ����������� ��������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore(); // �������� ��������� ���
        cin.get();    // ������� �� ����� ��� �� �����������
    }
    // ������� �������
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        cerr << "SQL State: " << e.getSQLState() << endl;
    }
}



void Room::viewCleaningStatus() {
    try {
        // ��������� ���� � �������� ���������� �����
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery("SELECT * FROM RoomCleaningStatus");

        // ��������� ��������� ����
        cout << "=============================" << endl;
        cout << " ������ ���������� �����:" << endl;
        cout << "=============================" << endl;
        cout << "ʳ�����\t\t������" << endl;
        cout << "-----------------------------" << endl;

        // ��������� ����� ��� ������ ���������� �����
        while (res->next()) {
            int roomNum = res->getInt("RoomNumber");
            string cleaned = res->getString("Cleaned");
            cout << roomNum << "\t\t" << cleaned << endl;
        }

        delete res;
        delete stmt;

        // ��������, ���� ���������� �� ������� ����-��� ������, ����� ����������� ��������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore(); // �������� ��������� ���
        cin.get();    // ������� �� ����� ��� �� �����������
    }
    // ������� �������
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
        cerr << "SQL State: " << e.getSQLState() << endl;
    }
}



