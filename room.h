#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <iomanip>
#include <string>
#include "hotelconnecting.h"

class Room : public HotelConnecting {
private:
    int roomNumber;
    std::string roomType;
    std::string status;
    int capacity;
    double price;
    HotelConnecting& hotel;
public:
    Room();
    Room(HotelConnecting& h);

    bool isNumber(std::string str);
    void displayRooms();
    void addRoom();
    void deleteRoom();
    void updateRoom();
    void sortRooms();
    void updateCleaningStatus();
    void viewCleaningStatus();
};

#endif // ROOM_H
