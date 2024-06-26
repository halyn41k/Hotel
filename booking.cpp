efg3eg
            cin.ignore(INT_MAX, '\n'); // Î÷èùàºìî âõ³äíèé áóôåð
            getline(cin, bookingIdStr);


            // Ïåðåâ³ðêà íà ïîðîæí³ñòü òà âèêëèê ôóíêö³¿ isInputEmpty
            if (isInputEmpty(bookingIdStr)) {
                cout << "Ïîìèëêà: ID áðîíþâàííÿ íå ìîæå áóòè ïîðîæí³ì." << endl;
                continue;
            }

            // Ïåðåâ³ðêà, ÷è ââåäåíèé ðÿäîê ñêëàäàºòüñÿ ëèøå ç öèôð
            if (bookingIdStr.find_first_not_of("0123456789") != string::npos) {
                cout << "Ïîìèëêà: Ââåäåíèé ID áðîíþâàííÿ ìàº áóòè ÷èñëîì àáî 0." << endl;
                continue;
            }

            int bookingId = stoi(bookingIdStr); // Ïåðåòâîðåííÿ ðÿäêà ó ö³ëå ÷èñëî

            // Çàïèò äî áàçè äàíèõ òà â³äîáðàæåííÿ ðåçóëüòàò³â
            PreparedStatement* pstmt;
            ResultSet* res;
            if (bookingId == 0) {
                // Âèâåäåííÿ âñ³õ áðîíþâàíü
                pstmt = con->prepareStatement("SELECT * FROM bookings");
                res = pstmt->executeQuery();
            }
            else {
                // Âèâåäåííÿ áðîíþâàííÿ çà çàäàíèì ID
                pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
                pstmt->setInt(1, bookingId);
                res = pstmt->executeQuery();
            }

            if (!res->next()) {
                cout << "Íåìàº çàïèñ³â äëÿ â³äîáðàæåííÿ." << endl;
            }
            else {
                // Âèâåäåííÿ çàãîëîâêà òàáëèö³
                cout << setw(15) << "ID áðîíþâàííÿ" << setw(15) << "ID ãîñòÿ" << setw(15) << "Íîìåð ê³ìíàòè" << setw(20) << "Äàòà áðîíþâàííÿ" << setw(15) << "Òåðì³í" << endl;

                // Âèâåäåííÿ äàíèõ ïðî áðîíþâàííÿ
                do {
                    cout << setw(15) << res->getInt("BookingId") << setw(15) << res->getInt("GuestId") << setw(15) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;
                } while (res->next());
            }

            delete pstmt;
            delete res;

            // Î÷³êóâàííÿ âèáîðó êîðèñòóâà÷à äëÿ ïðîäîâæåííÿ àáî çàâåðøåííÿ
            char choice = getValidChoice();

            if (choice == 'y' || choice == 'Y') {
                return;
            }
        }
    }
    // Îáðîáêà âèíÿòê³â
    catch (const std::invalid_argument& ia) {
        cout << "Ïîìèëêà: Ââåäåíèé ID áðîíþâàííÿ ìàº áóòè ÷èñëîì àáî 0." << endl;
    }
    catch (SQLException& e) {
        cout << "Ïîìèëêà SQL: " << e.what() << endl;
    }
}

void Booking::addBooking() {
    try {
        // Âèâåäåííÿ ïîâ³äîìëåííÿ ïðî äîäàâàííÿ íîâîãî áðîíþâàííÿ
        cout << "~~ Äîäàòè íîâå áðîíþâàííÿ ~~" << endl;

        int guestId, roomNumber;
        string bookingDate, term;

        // Ïåðåâ³ðêà ID ãîñòÿ
        bool validGuestId = false;
        while (!validGuestId) {
            cout << "ID ãîñòÿ: ";
            cin.clear(); // Î÷èùåííÿ ôëàãà ïîìèëêè ââåäåííÿ
            cin.ignore(INT_MAX, '\n'); // Âèäàëåííÿ íåïðàâèëüíîãî ââîäó ç áóôåðà
            string guestIdInput;
            getline(cin, guestIdInput);

            stringstream guestIDStream(guestIdInput);
            guestIDStream >> guestId;

            if (cin.fail()) {
                cout << "Ïîìèëêà: Ââåä³òü ÷èñëî äëÿ ID ãîñòÿ." << endl;
                cin.clear(); // Î÷èùåííÿ ôëàãà ïîìèëêè ââåäåííÿ
                cin.ignore(INT_MAX, '\n'); // ²ãíîðóâàííÿ íåïðàâèëüíîãî ââîäó
            }
            else if (guestId == 0) {
                cout << "Ïîìèëêà: ID ãîñòÿ íå ìîæå áóòè 0. Áóäü ëàñêà, ââåä³òü êîðåêòíèé ID ãîñòÿ." << endl;
            }
            else {
                if (!checkGuestId(guestId)) {
                    cout << "Ïîìèëêà: Ãîñòÿ ç òàêèì ID íå ³ñíóº." << endl;
                }
                else {
                    validGuestId = true;
                }
            }
        }

        // Ïåðåâ³ðêà íàÿâíîñò³ íîìåðà ê³ìíàòè
        bool validRoomNumber = false;
        while (!validRoomNumber) {
            cout << "Íîìåð ê³ìíàòè: ";
            string roomNumberInput;
            cin.clear(); // Î÷èùåííÿ ôëàãà ïîìèëêè ââåäåííÿ
            cin.ignore(INT_MAX, '\n'); // Âèäàëåííÿ íåïðàâèëüíîãî ââîäó ç áóôåðà
            getline(cin, roomNumberInput);
            stringstream roomNumberStream(roomNumberInput);
            roomNumberStream >> roomNumber;
            if (!checkRoomNumber(roomNumber)) {
                cout << "Ïîìèëêà: Ê³ìíàòà ç òàêèì íîìåðîì íå ³ñíóº." << endl;
                cin.clear(); // Î÷èùåííÿ ôëàãà ïîìèëêè ââåäåííÿ
                cin.ignore(INT_MAX, '\n'); // Âèäàëåííÿ íåïðàâèëüíîãî ââîäó ç áóôåðà
            }
            else {
                validRoomNumber = true;
            }
        }

        // Ïåðåâ³ðêà äàòè áðîíþâàííÿ
        bool validDate = false;
        do {
            cout << "Äàòà áðîíþâàííÿ (ð³ê-ì³ñÿöü-äåíü): ";
            cin.clear(); // Î÷èùåííÿ ôëàãà ïîìèëêè ââåäåííÿ
            cin.ignore(INT_MAX, '\n'); // Âèäàëåííÿ íåïðàâèëüíîãî ââîäó ç áóôåðà
            getline(cin,bookingDate);

            if (isInputEmpty(bookingDate)) {
                cout << "Ïîìèëêà: Äàòà áðîíþâàííÿ íå ìîæå áóòè ïîðîæíüîþ." << endl;
            }
            else {
                if (!isValidDateFormat(bookingDate)) {
                    cout << "Ïîìèëêà: Íåïðàâèëüíèé ôîðìàò äàòè. Áóäü ëàñêà, ââåä³òü äàòó ó ôîðìàò³ 'ð³ê-ì³ñÿöü-äåíü'." << endl;
                }
                else {
                    validDate = true;
                }
            }
        } while (!validDate);

        // Ïåðåâ³ðêà òåðì³íó
        bool validTerm = false;
        do {
            cout << "Òåðì³í: ";
            cin.clear(); // Î÷èùåííÿ ôëàãà ïîìèëêè ââåäåííÿ
            cin.ignore(INT_MAX, '\n'); // Âèäàëåííÿ íåïðàâèëüíîãî ââîäó ç áóôåðà
            getline(cin, term);

            if (isInputEmpty(term)) {
                cout << "Ïîìèëêà: Òåðì³í íå ìîæå áóòè ïîðîæí³ì." << endl;
            }
            else {
                validTerm = true;
            }
        } while (!validTerm);

        // Äîäàâàííÿ íîâîãî áðîíþâàííÿ äî áàçè äàíèõ
        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO bookings (GuestId, RoomNumber, BookingDate, Term) VALUES (?, ?, ?, ?)");
        pstmt->setInt(1, guestId);
        pstmt->setInt(2, roomNumber);
        pstmt->setString(3, bookingDate);
        pstmt->setString(4, term);

        pstmt->executeUpdate();

        cout << "Íîâå áðîíþâàííÿ óñï³øíî äîäàíî!" << endl;

        delete pstmt;

        // Î÷³êóâàííÿ âèáîðó êîðèñòóâà÷à äëÿ ïðîäîâæåííÿ àáî çàâåðøåííÿ
        char choice = getChoice();

        if (choice == 'n' || choice == 'N') {
            return; // Ïîâåðíåííÿ áåç âèâåäåííÿ ãîñòåé, ÿêùî êîðèñòóâà÷ âèáðàâ 'n' àáî 'N'
        }
    }
    catch (SQLException& e) {
        cout << "Ïîìèëêà SQL: " << e.what() << endl;
    }
}

// Ïåðåâ³ðêà ³ñíóâàííÿ ãîñòÿ ç ââåäåíèì ID
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
        cout << "Ïîìèëêà SQL: " << e.what() << endl;
        return false;
    }
}


// Ïåðåâ³ðêà ³ñíóâàííÿ ê³ìíàòè ç ââåäåíèì íîìåðîì
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
        cout << "Ïîìèëêà SQL: " << e.what() << endl;
        return false;
    }
}
void Booking::deleteBooking() {
    try {
        while (true) {
            string bookingIdStr;
            cout << "Ââåä³òü ID áðîíþâàííÿ, ÿêå âè õî÷åòå âèäàëèòè: ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, bookingIdStr);

            if (isInputEmpty(bookingIdStr)) {
                cout << "Ïîìèëêà: Ïîëå íå ìîæå áóòè ïîðîæí³ì. Áóäü ëàñêà, ââåä³òü êîðåêòíå çíà÷åííÿ." << endl;
                continue; // Ïî÷èíàºìî öèêë çíîâó
            }

            if (bookingIdStr == "0") {
                break; // Âèõîäèìî ç öèêëó, ÿêùî êîðèñòóâà÷ ââ³â 0
            }

            int bookingId;
            try {
                bookingId = stoi(bookingIdStr);
            }
            catch (const std::invalid_argument& ia) {
                cout << "Ïîìèëêà: Ââåäåíèé ID áðîíþâàííÿ ìàº áóòè ÷èñëîì àáî 0." << endl;
                continue; // Ïî÷èíàºìî öèêë çíîâó
            }

            // Ï³äãîòîâêà òà âèêîíàííÿ çàïèòó äî áàçè äàíèõ äëÿ îòðèìàííÿ ³íôîðìàö³¿ ïðî áðîíþâàííÿ
            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
            pstmt->setInt(1, bookingId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Ïîìèëêà: Áðîíþâàííÿ ç ââåäåíèì ID íå çíàéäåíî." << endl;
            }
            else {
                // Âèâåäåííÿ ³íôîðìàö³¿ ïðî îáðàíå áðîíþâàííÿ äëÿ ï³äòâåðäæåííÿ âèäàëåííÿ
                cout << "Âèáðàíå áðîíþâàííÿ äëÿ âèäàëåííÿ:" << endl;
                cout << setw(10) << "ID áðîíþâàííÿ" << setw(10) << "ID ãîñòÿ" << setw(17) << "Íîìåð ê³ìíàòè" << setw(20) << "Äàòà áðîíþâàííÿ" << setw(15) << "Òåðì³í" << endl;
                cout << setw(10) << res->getInt("BookingId") << setw(10) << res->getInt("GuestId") << setw(17) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;

                char confirmDelete;
                cout << "Âè âïåâíåí³, ùî õî÷åòå âèäàëèòè öå áðîíþâàííÿ? (y/n): ";
                cin >> confirmDelete;

                if (isInputEmpty(to_string(confirmDelete))) {
                    cout << "Ïîìèëêà: Ïîëå íå ìîæå áóòè ïîðîæí³ì. Áóäü ëàñêà, ââåä³òü y àáî n." << endl;
                    continue; // Ïî÷èíàºìî öèêë çíîâó
                }

                // Ï³äòâåðäæåííÿ àáî ñêàñóâàííÿ âèäàëåííÿ áðîíþâàííÿ
                if (confirmDelete == 'y' || confirmDelete == 'Y') {
                    pstmt = con->prepareStatement("DELETE FROM bookings WHERE BookingId = ?");
                    pstmt->setInt(1, bookingId);
                    int rowsAffected = pstmt->executeUpdate();

                    if (rowsAffected > 0) {
                        cout << "Áðîíþâàííÿ óñï³øíî âèäàëåíî." << endl;
                    }
                    else {
                        cout << "Ïîìèëêà: Áðîíþâàííÿ ç âêàçàíèì ID íå çíàéäåíî." << endl;
                    }
                }
                else {
                    cout << "Âèäàëåííÿ áðîíþâàííÿ ñêàñîâàíî." << endl;
                }
            }

            delete pstmt;
            delete res;

            char choice;
            cout << "Õî÷åòå âèäàëèòè ùå îäíå áðîíþâàííÿ? (y/n): ";
            cin >> choice;

            if (isInputEmpty(to_string(choice))) {
                cout << "Ïîìèëêà: Ïîëå íå ìîæå áóòè ïîðîæí³ì. Áóäü ëàñêà, ââåä³òü y àáî n." << endl;
                continue; // Ïî÷èíàºìî öèêë çíîâó
            }

            if (choice == 'n' || choice == 'N') {
                break; // Âèõîäèìî ç öèêëó, ÿêùî êîðèñòóâà÷ íå õî÷å âèäàëÿòè ùå áðîíþâàííÿ
            }
        }
    }
    catch (SQLException& e) {
        cout << "Ïîìèëêà âèäàëåííÿ áðîíþâàííÿ: " << e.what() << endl;
    }
}
void Booking::updateBooking() {
    try {
        while (true) {
            string bookingIdStr;
            cout << "Ââåä³òü ID áðîíþâàííÿ, ÿêå âè õî÷åòå îíîâèòè (àáî 0, ùîá ïîâåðíóòèñÿ äî ãîëîâíîãî ìåíþ): ";
            cin.ignore(INT_MAX, '\n');
            getline(cin, bookingIdStr);

            if (isInputEmpty(bookingIdStr)) {
                cout << "Ïîìèëêà: Ïîëå íå ìîæå áóòè ïîðîæí³ì. Áóäü ëàñêà, ââåä³òü êîðåêòíå çíà÷åííÿ." << endl;
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
                cout << "Ïîìèëêà: Ââåäåíèé ID áðîíþâàííÿ ìàº áóòè ÷èñëîì àáî 0." << endl;
                continue;
            }

            PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings WHERE BookingId = ?");
            pstmt->setInt(1, bookingId);
            ResultSet* res = pstmt->executeQuery();

            if (!res->next()) {
                cout << "Ïîìèëêà: Áðîíþâàííÿ ç ââåäåíèì ID íå çíàéäåíî." << endl;
                delete pstmt;
                delete res;
                continue;
            }

            cout << "Îáåð³òü ïàðàìåòð äëÿ îíîâëåííÿ:" << endl;
            cout << "1. Íîìåð ê³ìíàòè" << endl;
            cout << "2. Äàòó áðîíþâàííÿ" << endl;
            cout << "3. Òåðì³í" << endl;
            cout << "0. Ïîâåðíóòèñÿ äî ãîëîâíîãî ìåíþ" << endl;
            int updateChoice;
            cout << "Âàø âèá³ð: ";
            cin >> updateChoice;

            switch (updateChoice) {
            case 0:
                delete pstmt;
                delete res;
                return;
            case 1: {
                // Îíîâëåííÿ íîìåðó ê³ìíàòè
                do {
                    string roomNumberStr;
                    cout << "Ââåä³òü íîâèé íîìåð ê³ìíàòè: ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, roomNumberStr);
                    if (isInputEmpty(roomNumberStr) || !isNumber(roomNumberStr)) {
                        cout << "Ïîìèëêà: Íîìåð ê³ìíàòè ìàº áóòè ââåäåíèé ò³ëüêè öèôðîþ." << endl;
                        continue;
                    }
                    int newRoomNumber = stoi(roomNumberStr);

                    PreparedStatement* checkRoomStmt = con->prepareStatement("SELECT * FROM rooms WHERE RoomNumber = ?");
                    checkRoomStmt->setInt(1, newRoomNumber);
                    ResultSet* roomRes = checkRoomStmt->executeQuery();
                    if (!roomRes->next()) {
                        cout << "Ïîìèëêà: Ê³ìíàòè ç ââåäåíèì íîìåðîì íå ³ñíóº." << endl;
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
                // Îíîâëåííÿ äàòè áðîíþâàííÿ
                do {
                    string newBookingDate;
                    cout << "Ââåä³òü íîâó äàòó áðîíþâàííÿ (ð³ê-ì³ñÿöü-äåíü): ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, newBookingDate);
                    if (isInputEmpty(newBookingDate) || !isValidDateFormat(newBookingDate)) {
                        cout << "Ïîìèëêà: Íåêîðåêòíèé ôîðìàò äàòè. Áóäü ëàñêà, âèêîðèñòîâóéòå ôîðìàò ð³ê-ì³ñÿöü-äåíü." << endl;
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
                // Îíîâëåííÿ òåðì³íó
                do {
                    string newTermStr;
                    cout << "Ââåä³òü íîâèé òåðì³í (â³ä 1 äî 30 äí³â): ";
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    getline(cin, newTermStr);
                    if (isInputEmpty(newTermStr) || !isNumber(newTermStr)) {
                        cout << "Ïîìèëêà: Òåðì³í ìàº áóòè ââåäåíèé ò³ëüêè öèôðîþ." << endl;
                        continue;
                    }
                    int newTerm = stoi(newTermStr);
                    if (newTerm < 1 || newTerm > 30) {
                        cout << "Ïîìèëêà: Òåðì³í ïîâèíåí áóòè â ìåæàõ â³ä 1 äî 30." << endl;
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
                cout << "Áóäü ëàñêà, ââåä³òü êîðåêòíèé âàð³àíò (0-3)." << endl;
                continue;
            }


            char confirmUpdate;
            cout << "Âè âïåâíåí³, ùî õî÷åòå îíîâèòè öå áðîíþâàííÿ? (y/n): ";
            cin >> confirmUpdate;

            if (confirmUpdate == 'y' || confirmUpdate == 'Y') {
                int rowsAffected = pstmt->executeUpdate();

                if (rowsAffected > 0) {
                    cout << "²íôîðìàö³þ ïðî áðîíþâàííÿ óñï³øíî îíîâëåíî." << endl;
                }
                else {
                    cout << "Ïîìèëêà: Íå âäàëîñÿ îíîâèòè ³íôîðìàö³þ ïðî áðîíþâàííÿ." << endl;
                }
            }
            else {
                cout << "Îíîâëåííÿ áðîíþâàííÿ ñêàñîâàíî." << endl;
            }

            delete pstmt;
            delete res;
        }
    }
    catch (SQLException& e) {
        cout << "Ïîìèëêà îíîâëåííÿ ³íôîðìàö³¿ ïðî áðîíþâàííÿ: " << e.what() << endl;
    }
}



void Booking::sortBookings() {
    try {
        // Çàïèò êîðèñòóâà÷à íà âèá³ð êðèòåð³þ ñîðòóâàííÿ
        int sortingCriterion;
        cout << "Îáåð³òü êðèòåð³é ñîðòóâàííÿ áðîíþâàíü:" << endl;
        cout << "1. Çà ID áðîíþâàííÿ." << endl;
        cout << "2. Çà ID ãîñòÿ." << endl;
        cout << "3. Çà íîìåðîì ê³ìíàòè." << endl;
        cout << "4. Çà äàòîþ áðîíþâàííÿ." << endl;
        cout << "5. Çà òåðì³íîì." << endl;
        cout << "0. Ïîâåðíóòèñÿ äî ïîïåðåäíüîãî ìåíþ." << endl;
        cout << "Âàø âèá³ð: ";

        // Ïåðåâ³ðêà, ÷è êîðèñòóâà÷ ââ³â êîðåêòíå ö³ëå ÷èñëî
        if (!(cin >> sortingCriterion)) {
            throw std::runtime_error("Ïîìèëêà ââîäó. Áóäü ëàñêà, ââåä³òü ö³ëå ÷èñëî.");
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
            return; // Ïîâåðíåííÿ äî ïîïåðåäíüîãî ìåíþ
        default:
            cout << "Íåâ³ðíèé âèá³ð. Áóäü ëàñêà, ââåä³òü ÷èñëî â³ä 0 äî 5." << endl;
            return; // Âèõ³ä ç ôóíêö³¿ ó ðàç³ íåâ³ðíîãî âèáîðó
        }

        // Âèêîíàííÿ çàïèòó äî áàçè äàíèõ ç âèêîðèñòàííÿì âèáðàíîãî êðèòåð³þ ñîðòóâàííÿ
        PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM bookings " + orderByClause);
        ResultSet* res = pstmt->executeQuery();

        // Âèâåäåííÿ â³äñîðòîâàíîãî ñïèñêó áðîíþâàíü
        cout << setw(15) << "ID áðîíþâàííÿ" << setw(10) << "ID ãîñòÿ" << setw(17) << "Íîìåð ê³ìíàòè" << setw(20) << "Äàòà áðîíþâàííÿ" << setw(15) << "Òåðì³í" << endl;
        while (res->next()) {
            cout << setw(15) << res->getInt("BookingId") << setw(10) << res->getInt("GuestId") << setw(17) << res->getInt("RoomNumber") << setw(20) << res->getString("BookingDate") << setw(15) << res->getString("Term") << endl;
        }

        delete pstmt;
        delete res;

        // Î÷³êóâàííÿ íàòèñêàííÿ êîðèñòóâà÷åì áóäü-ÿêî¿ êëàâ³ø³ ïåðåä ïðîäîâæåííÿì
        cout << "Íàòèñí³òü áóäü-ÿêó êëàâ³øó, ùîá ïðîäîâæèòè...";
        cin.ignore();
        cin.get();
    }
    catch (const SQLException& e) {
        cout << "Ïîìèëêà ï³ä ÷àñ âèêîíàííÿ çàïèòó äî áàçè äàíèõ: " << e.what() << endl;
    }
    catch (const std::exception& ex) {
        cout << "Ñòàëàñÿ ïîìèëêà: " << ex.what() << endl;
    }
}
