#ifndef PAYMENT_H
#define PAYMENT_H

#include <iostream>
#include "hotelconnecting.h"

class Payment : public HotelConnecting {
private:
    int guestId;
    std::string paymentMethod, paymentDate;
    double amount;
    HotelConnecting& hotel;
    bool isGuestExist(int guestId);
    bool isValidPaymentMethod(std::string method);

public:
    Payment();
    Payment(HotelConnecting& h);
    void displayPayments();
    void addPayment();
    void deletePayment();
    void updatePayment();
    void sortPayments();
};

#endif // PAYMENT_H
