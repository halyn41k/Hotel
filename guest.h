#ifndef GUEST_H
#define GUEST_H

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

class Guest : public HotelConnecting {
private:
    string firstName;
    string lastName;
    string gender;
    int age;
    string phoneNumber;
    int guestId;
    int roomId;
    string checkInDate;
    int roomNumber;
    HotelConnecting& hotel;
public:
    Guest();
    Guest(HotelConnecting& h);
    bool isRoomAvailable(int roomNumber);
    void displayGuests();
    void addGuest();
    void deleteGuest();
    void updateGuest();
    void sortGuests();
};

#endif // GUEST_H
