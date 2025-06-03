//
// Created by ASUS on 21-06-2024.
//

#ifndef TICKETCANCEL_H
#define TICKETCANCEL_H

#define SERVICE_CHARGE 200
#define HOUR_TO_SECONDS 3600
#define DAY_TO_SECONDS (24 * HOUR_TO_SECONDS)

#define MAX_PNRS 10
#define MRAC 500
#define FRAC 600
#define MM 700
#define MF 800
#define FM 900
#define FF 10000
#define MAX_LINE_LENGTH 200
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define PNR_LENGTH 10
#define MAX_TRAINS 50
#define SLEEPER_BASE_COST 50
#define AC1_BASE_COST 100
#define AC2_BASE_COST 80
#define AC3_BASE_COST 60
#define GENERAL_BASE_COST 40
#define MAX_STRING_LENGTH 100
#define TOTAL_COACHES 20
#define S_COACHES 12
#define AC3_COACHES 4
#define AC2_COACHES 3
#define AC1_COACHES 1
#define SEATS_S 72
#define SEATS_AC3 64
#define SEATS_AC2 32
#define SEATS_AC1 16
#define SEATS_MIXED 32
#define ROWS_S 9
#define ROWS_AC3 8
#define ROWS_AC2 5
#define ROWS_AC1 4
#define ROWS_MIXED 4
#define COLUMNS_LEFT_S 2
#define COLUMNS_RIGHT_S 6
#define COLUMNS_LEFT_AC3 2
#define COLUMNS_RIGHT_AC3 6
#define COLUMNS_AC2 2
#define COLUMNS_AC1 4
#define COLUMNS_LEFT_MIXED 2
#define COLUMNS_RIGHT_MIXED 4
#define BOOKED -1
#define MALE 100
#define FEMALE 200
#define MAX_RECORDS 100

struct Planning {
    char from[MAX_STRING_LENGTH];
    char to[MAX_STRING_LENGTH];
    char doj[MAX_STRING_LENGTH];
    int train_number;
    int cost;
    char name[10][MAX_STRING_LENGTH];
    int age[10];
    char gender[10][10];
    char pnr[PNR_LENGTH];
    int seat[10];
    int coach;
    int rac_no[10];
};

struct seats {
    int s1[TOTAL_COACHES][SEATS_S];
    int gS[TOTAL_COACHES][SEATS_S];
    int year, month, day, trainno;
};

void trimNewline(char *str);
void removePNR(char *pnrList, const char *pnr);
void updateWalletAmountAndRemovePNR(const char *username, int newWalletAmount, const char *pnrToRemove);
int calculateRefund(struct tm journey_date, int ticket_cost);
void copy_2d_array(int src[TOTAL_COACHES][SEATS_S], int dest[TOTAL_COACHES][SEATS_S], int rows, int cols);
void update_train_details(int trainno, int seats[TOTAL_COACHES][SEATS_S], int genderSeats[TOTAL_COACHES][SEATS_S]);
void displayPlanningData(struct Planning *bookings, int num_Confirm, int num_RAC);
int readBookingDataConfirm(const char* filename, struct Planning* planningArray);
int readBookingDataRAC(const char* filename,struct Planning* bookings, int start_index);
void writePlanningDataConfirm(const char *filename, struct Planning *plan, int con, int a);
void writePlanningDataRAC(const char *filename, struct Planning *plannings, int startIndex, int count);
int cancelBookingByPNR(struct Planning *bookings, int *num_Confirm, int *num_RAC, char *pnr, const char* filename, const char *username);
void ticketcancel() ;



#endif //TICKETCANCEL_H
