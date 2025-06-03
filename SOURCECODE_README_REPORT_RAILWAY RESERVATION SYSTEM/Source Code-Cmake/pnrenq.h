#ifndef TICKETCANCEL_H
#define TICKETCANCEL_H

#define MAX_STRING_LENGTH 100
#define PNR_LENGTH 10
#define MAX_PASSENGERS 10
#define MAX_BOOKINGS 50

struct Passenger {
    char name[MAX_STRING_LENGTH];
    int age;
    char gender[MAX_STRING_LENGTH];
    int seat;
};

struct Booking {
    char from[MAX_STRING_LENGTH];
    char to[MAX_STRING_LENGTH];
    char doj[MAX_STRING_LENGTH];
    int train_number;
    int cost;
    char pnr[PNR_LENGTH];
    int coach;
    struct Passenger passengers[MAX_PASSENGERS];
    int passenger_count;
    int rac_no; // Added RAC number field
};

int readBookings(const char* filename, struct Booking* bookings);
int readRACBookings(const char* filename, struct Booking* bookings);
int findBookingByPNR(struct Booking* bookings, int booking_count, const char* pnr, struct Booking* result);
int pnr_enquiry();
#endif // TICKETCANCEL_H
