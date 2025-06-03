#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_LENGTH 100
#define PNR_LENGTH 10
#define MAX_PASSENGERS 10
#define MAX_BOOKINGS 50

struct Planning {
    char from[MAX_STRING_LENGTH];
    char to[MAX_STRING_LENGTH];
    char doj[MAX_STRING_LENGTH];
    int train_number;
    int cost;
    char name[MAX_PASSENGERS][MAX_STRING_LENGTH];
    int age[MAX_PASSENGERS];
    char gender[MAX_PASSENGERS][10];
    char pnr[PNR_LENGTH];
    int seat[MAX_PASSENGERS];
    int coach;
    int rac_no[MAX_PASSENGERS];
    int passenger_count;
};

// Function to trim newline characters
void trimNewline(char *str) ;

// Function to read bookings from file
int readBookings(const char* filename, struct Planning* bookings) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return -1;
    }

    char line[256];
    int booking_count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        trimNewline(line);
        if (strncmp(line, "From:", 5) == 0) {
            sscanf(line, "From: %s", bookings[booking_count].from);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "To: %s", bookings[booking_count].to);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Date of Journey: %s", bookings[booking_count].doj);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Train Number: %d", &bookings[booking_count].train_number);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Cost: %d", &bookings[booking_count].cost);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "PNR: %s", bookings[booking_count].pnr);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Coach No: %d", &bookings[booking_count].coach);

            int passenger_count = 0;
            while (fgets(line, sizeof(line), file) != NULL && strncmp(line, "From:", 5) != 0) {
                trimNewline(line);
                if (strncmp(line, "Passenger", 9) == 0) {
                    sscanf(line, "Passenger %*d Name: %s", bookings[booking_count].name[passenger_count]);
                    fgets(line, sizeof(line), file); trimNewline(line);
                    sscanf(line, "Passenger %*d Age: %d", &bookings[booking_count].age[passenger_count]);
                    fgets(line, sizeof(line), file); trimNewline(line);
                    sscanf(line, "Passenger %*d Gender: %s", bookings[booking_count].gender[passenger_count]);
                    fgets(line, sizeof(line), file); trimNewline(line);
                    sscanf(line, "Passenger %*d Seat: %d", &bookings[booking_count].seat[passenger_count]);
                    bookings[booking_count].rac_no[passenger_count] = -1; // Not RAC
                    passenger_count++;
                }
            }
            bookings[booking_count].passenger_count = passenger_count;
            booking_count++;
            if (booking_count >= MAX_BOOKINGS) {
                break;
            }
            if (feof(file)) {
                break;
            }
            fseek(file, -strlen(line), SEEK_CUR);
        }
    }
    fclose(file);
    return booking_count;
}
// Function to read RAC bookings from file
int readRACBookings(const char* filename, struct Planning* bookings) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return -1;
    }

    char line[256];
    int booking_count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        trimNewline(line);
        if (strncmp(line, "PNR:", 4) == 0) {
            sscanf(line, "PNR: %s", bookings[booking_count].pnr);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Name: %s", bookings[booking_count].name[0]);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Age: %d", &bookings[booking_count].age[0]);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Gender: %s", bookings[booking_count].gender[0]);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Train: %d", &bookings[booking_count].train_number);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "RAC No: %d", &bookings[booking_count].rac_no[0]);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "From: %s", bookings[booking_count].from);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "To: %s", bookings[booking_count].to);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "DOJ: %s", bookings[booking_count].doj);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Coach: %d", &bookings[booking_count].coach);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Seat: %d", &bookings[booking_count].seat[0]);
            fgets(line, sizeof(line), file); trimNewline(line);
            sscanf(line, "Cost: %d", &bookings[booking_count].cost);

            bookings[booking_count].passenger_count = 1;
            booking_count++;
            if (booking_count >= MAX_BOOKINGS) {
                break;
            }
        }
    }
    fclose(file);
    return booking_count;
}

// Function to find booking by PNR
int findBookingByPNR(struct Planning* bookings, int booking_count, const char* pnr, struct Planning* result) {
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].pnr, pnr) == 0) {
            *result = bookings[i];
            return 1;
        }
    }
    return 0;
}


int pnr_enquiry() {
    struct Planning bookings[MAX_BOOKINGS];
    int booking_count = readBookings("book.txt", bookings);
    if (booking_count == -1) {
        return 1;
    }

    char pnr[PNR_LENGTH];
    printf("Enter PNR: ");
    fgets(pnr, sizeof(pnr), stdin);
    trimNewline(pnr);

    struct Planning result;
    if (findBookingByPNR(bookings, booking_count, pnr, &result)) {
        printf("Booking found in book.txt:\n");
    } else {
        char train_no_str[3];
        strncpy(train_no_str, pnr, 2);
        train_no_str[2] = '\0'; // Null terminate the string
        int train_no;
        train_no = atoi(train_no_str);
        char filename[50];
        sprintf(filename, "rac_%d.txt",  train_no);
        booking_count = readRACBookings(filename, bookings);
        if (booking_count == -1) {
            return 1;
        }

        if (findBookingByPNR(bookings, booking_count, pnr, &result)) {
            printf("Booking found in rac.txt:\n");
        } else {
            printf("Booking not found for PNR: %s\n", pnr);
            return 0;
        }
    }

    printf("From: %s\n", result.from);
    printf("To: %s\n", result.to);
    printf("Date of Journey: %s\n", result.doj);
    printf("Train Number: %d\n", result.train_number);
    printf("Cost: %d\n", result.cost);
    printf("PNR: %s\n", result.pnr);
    printf("Coach No: %d\n", result.coach);
    for (int i = 0; i < result.passenger_count; i++) {
        printf("Passenger %d Name: %s\n", i + 1, result.name[i]);
        printf("Passenger %d Age: %d\n", i + 1, result.age[i]);
        printf("Passenger %d Gender: %s\n", i + 1, result.gender[i]);
        printf("Passenger %d Seat: %d\n", i + 1, result.seat[i]);
        printf("Passenger %d RAC No: %d\n", i + 1, result.rac_no[i]);
    }

    return 0;
}