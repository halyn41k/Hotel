#ifndef BOOKING_H
#define BOOKING_H

#include <iostream>
#include <iomanip>
#include <string>
#include "hotelconnecting.h"

class Booking : public HotelConnecting {
private:
    int bookingId;
    int roomNumber;
    int guestId;
    std::string bookingDate;
    std::string term;

public:
    Booking();

    void displayBookings();
    void addBooking();
    bool checkGuestId(int guestId);
    bool checkRoomNumber(int roomNumber);
    void deleteBooking();
    void updateBooking();
    void sortBookings();
};

#endif // BOOKING_H
