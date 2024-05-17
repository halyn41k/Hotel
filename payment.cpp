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
            // ����� ����������� �� �������� ID ������� ��� ��������� ��� �������
            cout << "~~ ������ ID ������� ��� ������ 0, ��� ������� �� ~~" << endl;
            string paymentIdStr;
            cout << "ID �������: ";
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, paymentIdStr);


            // �������� �� ������� ���
            if (isInputEmpty(paymentIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� ������� ��� 0." << endl;
                continue; // �������� ���� �����
            }

            if (paymentIdStr == "0") {
                // ��������� ��� �������
                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments");
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "���� ������ ��� �����������." << endl;
                }
                else {
                    // ��������� ��������� ��������
                    cout << setw(10) << "ID �������" << setw(10) << "ID �����" << setw(20) << "����� ������" << setw(10) << "����" << setw(20) << "���� �������" << endl;

                    // ��������� ����� �������
                    do {
                        cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;
                    } while (res->next());
                }

                delete pstmt;
                delete res;
                // ��������� ������ ����������� �� �����������
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // ���������� �� ������������ ����
                }
            }
            else {
                // ����� ������� �� �������� ID
                int paymentId;
                try {
                    paymentId = stoi(paymentIdStr);
                }
                catch (const std::invalid_argument& ia) {
                    cout << "�������: �������� ID ������� �� ���� ������ ��� 0." << endl;
                    continue; // �������� ���� �����
                }

                PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments WHERE PaymentId = ?");
                pstmt->setInt(1, paymentId);
                ResultSet* res = pstmt->executeQuery();

                if (!res->next()) {
                    cout << "�������: ����� � �������� ID �� ��������." << endl;
                }
                else {
                    // ��������� ���������� �������
                    cout << setw(10) << "ID �������" << setw(10) << "ID �����" << setw(20) << "����� ������" << setw(10) << "����" << setw(20) << "���� �������" << endl;
                    cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;
                }

                delete pstmt;
                delete res;
                // ��������� ������ ����������� �� �����������
                char choice = getValidChoice();

                if (choice == 'y' || choice == 'Y') {
                    return; // ���������� �� ������������ ����
                }
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

// �������� ��������� ����� �� ���� ID
bool Payment::isGuestExist(int guestId) {
    try {
        // ϳ�������� �� ��������� SQL-������
        PreparedStatement* pstmt = con->prepareStatement("SELECT COUNT(*) AS count FROM guests WHERE GuestId = ?");
        pstmt->setInt(1, guestId);
        ResultSet* res = pstmt->executeQuery();

        res->next();
        int guestCount = res->getInt("count");

        delete pstmt;
        delete res;

        return guestCount > 0; // ���������� ���������� ��������
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
        return false;
    }
}

// �������� ����������� ������� ������
bool Payment::isValidPaymentMethod(string method) {
    return method == "������" || method == "�������� �����" || method == "�������� �����";
}

// ��������� ������ �������
void Payment::addPayment() {
    try {
        cout << "~~ ������ ����� ����� ~~" << endl;

        bool validGuestId = false;
        do {
            cout << "ID �����: ";
            string guestIdInput;
            cin.clear(); // �������� ����� ������� ��������
            cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
            getline(cin, guestIdInput);

            stringstream guestIdStream(guestIdInput);
            guestIdStream >> guestId;

            if (!cin || !isGuestExist(guestId)) {
                cout << "�������: ����� � ����� ID �� ����. ���� �����, ������ �������� ID �����." << endl;
                cin.clear(); // ������� ����� ������� �����
                cin.ignore(INT_MAX, '\n'); // ������� ������ ��������
            }
            else {
                validGuestId = true;
            }
        } while (isInputEmpty(to_string(guestId)) || !validGuestId);

        // �������� ������� ������
        do {
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            cout << "����� ������ (������/�������� �����/�������� �����): ";
            getline(cin, paymentMethod);

            if (!isValidPaymentMethod(paymentMethod)) {
                cout << "�������: ������������ ����� ������. ���� �����, ������ '������', '�������� �����' ��� '�������� �����'." << endl;
            }
        } while (isInputEmpty(paymentMethod) || !isValidPaymentMethod(paymentMethod));

        // �������� ���� �������
        bool validAmount = false;
        do {
            cout << "����: ";
            string amountInput;
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            getline(cin, amountInput);
            stringstream amountStream(amountInput);
            amountStream >> amount;


            if (cin.fail() || amount < 0 || amount > 100000) {
                cin.clear(); // ������� ����� ������� �����
                cin.ignore(INT_MAX, '\n'); // ��������� ������������� ����� � ������
                cout << "�������: ���� ������� ������� ���� ������ �� 0 �� 100000." << endl;
            }
            else {
                validAmount = true; // ���� ������� ��������
            }
        } while (isInputEmpty(to_string(amount)) || !validAmount);

        // �������� ���� �������
        bool validDate = false;
        do {
            cin.clear(); // ������� ���� �������
            cin.ignore(INT_MAX, '\n'); // ������� ������� �����
            cout << "���� ������� (��-�����-����): ";
            getline(cin, paymentDate);

            // �������� ����������� ������� ����
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
                // ��������� ����� ���� �� ����� ������� (��, �����, ����)
                int year = stoi(paymentDate.substr(0, 4));
                int month = stoi(paymentDate.substr(5, 2));
                int day = stoi(paymentDate.substr(8, 2));

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
        } while (isInputEmpty(paymentDate) || !validDate);

        // ��������� SQL-������ ��� ��������� ������� �� ���� �����
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO payments (GuestId, PaymentMethod, Amount, PaymentDate) VALUES (?, ?, ?, ?)");
        pstmt->setInt(1, guestId);
        pstmt->setString(2, paymentMethod);
        pstmt->setDouble(3, amount);
        pstmt->setString(4, paymentDate);

        pstmt->executeUpdate();

        cout << "����� ����� ������ ������!" << endl;

        delete pstmt;
        // ��������� ������ ����������� �� �����������
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // ���������� �� ������������ ����
        }
    }
    catch (SQLException& e) {
        cout << "������� SQL: " << e.what() << endl;
    }
}

void Payment::deletePayment() {
    try {
        while (true) {
            string paymentIdStr;
            cout << "������ ID �������, ���� �� ������ ��������: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, paymentIdStr);

            if (paymentIdStr == "0") {
                break; // �������� � �����, ���� ���������� ��� 0
            }

            if (isInputEmpty(paymentIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                continue; // �������� ���� �����
            }

            int paymentId;
            try {
                paymentId = stoi(paymentIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ID ������� �� ���� ������ ��� 0." << endl;
                continue; // �������� ���� �����
            }

            // ϳ�������� �� ��������� SQL-������ ��� ��������� ������� �� �������� ID
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments WHERE PaymentId = ?");
            pstmt->setInt(1, paymentId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ����� � �������� ID �� ��������." << endl;
            }
            else {
                // ��������� ���������� ��� ��������� �����
                cout << "�������� ����� ��� ���������:" << endl;
                cout << setw(10) << "ID �������" << setw(10) << "ID �����" << setw(20) << "����� ������" << setw(10) << "����" << setw(20) << "���� �������" << endl;
                cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;

                // ϳ����������� ��������� ������� �� �����������
                char confirmDelete;
                cout << "�� �������, �� ������ �������� ��� �����? (y/n): ";
                cin >> confirmDelete;

                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    // ��������� SQL-������ ��� ��������� ������� � ���� �����
                    pstmt = con->prepareStatement("DELETE FROM payments WHERE PaymentId = ?");
                    pstmt->setInt(1, paymentId);
                    int rowsAffected = pstmt->executeUpdate();

                    if (rowsAffected > 0) {
                        cout << "����� ������ ��������." << endl;
                    }
                    else {
                        cout << "�������: ����� � �������� ID �� ��������." << endl;
                    }
                }
                else {
                    cout << "��������� ������� ���������." << endl;
                }
            }

            delete pstmt;
            delete res;

            // ������� ����������� ��� ������� ���������� ���������
            char choice;
            cout << "������ �������� �� ���� �����? (y/n): ";
            cin >> choice;

            if (choice == 'n' || choice == 'N') {
                break; // �������� � �����, ���� ���������� �� ���� �������� �� �����
            }
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� �������: " << e.what() << endl;
    }
}
void Payment::updatePayment() {
    try {
        while (true) {
            string paymentIdStr;
            cout << "������ ID �������, ���� �� ������ ������� (��� 0, ��� ����������� �� ��������� ����): ";
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            getline(cin, paymentIdStr);

            if (paymentIdStr == "0") {
                break;
            }

            if (isInputEmpty(paymentIdStr)) {
                cout << "�������: ���� �� ���� ���� �������. ���� �����, ������ ������������� �������." << endl;
                continue;
            }

            int paymentId;
            try {
                paymentId = stoi(paymentIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "�������: �������� ID ������� �� ���� ������ ��� 0." << endl;
                continue;
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments WHERE PaymentId = ?");
            pstmt->setInt(1, paymentId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "�������: ����� � �������� ID �� ��������." << endl;
            }
            else {
                cout << "�������� ����� ��� ���������:" << endl;
                cout << setw(10) << "ID �������" << setw(20) << "����� ������" << setw(10) << "����" << setw(20) << "���� �������" << endl;
                cout << setw(10) << res->getInt("PaymentId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;

                cout << "������ �������� ��� ���������:" << endl;
                cout << "1. ����� ������" << endl;
                cout << "2. ����" << endl;
                cout << "3. ���� �������" << endl;
                cout << "0. ����������� �� ��������� ����" << endl;

                int updateChoice;
                cout << "��� ����: ";
                cin >> updateChoice;

                if (cin.fail() || updateChoice < 0 || updateChoice > 3) {
                    cout << "������������ ���. ���� �����, ������ ����� �� 0 �� 3." << endl;
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
                        cout << "����� ����� ������ (������/�������� �����/�������� �����): ";
                        getline(cin, paymentMethod);

                        if (paymentMethod != "������" && paymentMethod != "�������� �����" && paymentMethod != "�������� �����") {
                            cout << "�������: ����������� ����� ������." << endl;
                        }
                    } while (paymentMethod != "������" && paymentMethod != "�������� �����" && paymentMethod != "�������� �����");

                    pstmt = con->prepareStatement("UPDATE payments SET PaymentMethod = ? WHERE PaymentId = ?");
                    pstmt->setString(1, paymentMethod);
                    pstmt->setInt(2, paymentId);
                    pstmt->executeUpdate();
                    cout << "����� ������ ��������." << endl;
                    break;
                }
                case 2: {
                    double amount;
                    string amountInput;
                    bool validAmount = false; // ���� ��� �������� ����������� �������� ����
                    do {
                        cin.ignore(INT_MAX, '\n');
                        cout << "���� ����: ";
                        getline(cin, amountInput);

                        // ��������, �� ������� ���� ����� ��� ����
                        bool isValidAmount = true;
                        for (char c : amountInput) {
                            if (!isdigit(c)) {
                                isValidAmount = false;
                                break;
                            }
                        }

                        if (!isValidAmount || amountInput.empty()) {
                            cout << "�������: ���� ������� ���� ������� ����� ������� � �� ���� ���� ���������." << endl;
                        }
                        else {
                            validAmount = true; // ���� ������� ���������
                            stringstream amountStream(amountInput);
                            amountStream >> amount;

                            if (amount < 0 || amount > 1000000) {
                                cout << "�������: ���� ������� ���� � ����� �� 0 �� 1000000." << endl;
                                validAmount = false; // ��������� ���� �� �����������
                            }
                        }
                    } while (!validAmount);

                    pstmt = con->prepareStatement("UPDATE payments SET Amount = ? WHERE PaymentId = ?");
                    pstmt->setDouble(1, amount);
                    pstmt->setInt(2, paymentId);
                    pstmt->executeUpdate();
                    cout << "����� ������ ��������." << endl;
                    break;
                }

                case 3: {
                    string paymentDate;
                    do {
                        cin.clear();
                        cin.ignore(INT_MAX, '\n');
                        cout << "���� ���� ������� (��-�����-����): ";
                        getline(cin, paymentDate);

                        if (!isValidDateFormat(paymentDate)) {
                            cout << "�������: ����������� ������ ����. �������������� ������ ��-�����-����." << endl;
                        }
                    } while (!isValidDateFormat(paymentDate));

                    pstmt = con->prepareStatement("UPDATE payments SET PaymentDate = ? WHERE PaymentId = ?");
                    pstmt->setString(1, paymentDate);
                    pstmt->setInt(2, paymentId);
                    pstmt->executeUpdate();
                    cout << "����� ������ ��������." << endl;
                    break;
                }
                default: {
                    cout << "���� �����, ������ ��������� ������ (0-3)." << endl;
                    continue;
                }
                }
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cout << "������� ��������� �������: " << e.what() << endl;
    }
}


void Payment::sortPayments() {
    try {
        int sortingCriterion;
        // ���� ��������� ������� ����������
        cout << "������ ������� ���������� �������:" << endl;
        cout << "1. �� ID �������." << endl;
        cout << "2. �� ID �����." << endl;
        cout << "3. �� �����." << endl;
        cout << "4. �� ����� �������." << endl;
        cout << "0. ����������� �� ������������ ����." << endl;
        cout << "��� ����: ";

        // ��������, �� ���������� ��� �������� ���� �����
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("������� �����. ���� �����, ������ ���� �����.");
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
            return; // ����������� �� ������������ ����
        default:
            cout << "������� ����. ���� �����, ������ ����� �� 0 �� 4." << endl;
            return;
        }

        // ϳ�������� �� ��������� SQL-������ ��� ������������ �������
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM payments " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // ���� ������������� ������ �������
        cout << setw(10) << "ID �������" << setw(10) << "ID �����" << setw(20) << "����� ������" << setw(10) << "����" << setw(20) << "���� �������" << endl;
        while (res->next()) {
            cout << setw(10) << res->getInt("PaymentId") << setw(10) << res->getInt("GuestId") << setw(20) << res->getString("PaymentMethod") << setw(10) << res->getDouble("Amount") << setw(20) << res->getString("PaymentDate") << endl;
        }

        delete pstmt;
        delete res;

        // ���������� ���������� ����-��� ������ ����� ����������� ��������
        cout << "�������� ����-��� ������, ��� ����������...";
        cin.ignore(); // �������� ��������� ���
        cin.get();    // ������� �� ����� ��� �� �����������
    }
    catch (const SQLException& e) {
        // ������� ������� SQL
        cout << "������� �� ��� ��������� ������ �� ���� �����: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        // ������� ����� ���������
        cout << "������� �������: " << ex.what() << endl;
    }
}
