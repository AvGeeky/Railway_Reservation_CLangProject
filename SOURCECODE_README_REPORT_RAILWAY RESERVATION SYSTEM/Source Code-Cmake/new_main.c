#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ticketcancel.h"
#include "include.h"
#include "pnrenq.h"

#define MAX_USERS 50
/*
#define MAX_PNRS 10
#define MRAC 500
#define FRAC 600
#define MM 700
#define MF 800
#define FM 900
#define FF 10000
#define MAX_LINE_LENGTH 150
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define PNR_LENGTH 11
#define MAX_TRAINS 100
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
#define MALE 100  // Greater than 72
#define FEMALE 200  // Greater than 72
#define MAX_RECORDS 200
*/

int walletAmount = 0;
int sears[TOTAL_COACHES];

typedef struct {
    char username[MAX_STRING_LENGTH];
    char password[MAX_STRING_LENGTH];
    char email[MAX_STRING_LENGTH];
    int balance;
    char pnrs[MAX_PNRS][MAX_STRING_LENGTH];
    int pnr_count;
} User;

struct Booking {
    char from[MAX_STRING_LENGTH];
    char to[MAX_STRING_LENGTH];
    char doj[MAX_STRING_LENGTH];
    int train_number;
    char coach_type[MAX_STRING_LENGTH];
    char seat_number[15];
    int cost;
};


struct Availability {
    char route[MAX_STRING_LENGTH];
    char doj[MAX_STRING_LENGTH];
    char destination[MAX_STRING_LENGTH];
    char train_name[100];
    int train_number;
    int ac_1;
    int sleeper_seats;
    int ac_2;
    int ac_3;
    int gen;
};



void readUserData(const char *filename, User *users, int *user_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    *user_count = 0;

    // Skip the header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        char *token;
        token = strtok(line, ",");
        strcpy(users[*user_count].username, token);

        token = strtok(NULL, ",");
        strcpy(users[*user_count].password, token);

        token = strtok(NULL, ",");
        strcpy(users[*user_count].email, token);

        token = strtok(NULL, ",");
        users[*user_count].balance = atoi(token);

        token = strtok(NULL, "\"");
        if (token) {
            char *pnr_token = strtok(token, ",");
            users[*user_count].pnr_count = 0;
            while (pnr_token) {
                strcpy(users[*user_count].pnrs[users[*user_count].pnr_count], pnr_token);
                users[*user_count].pnr_count++;
                pnr_token = strtok(NULL, ",");
            }
        } else {
            users[*user_count].pnr_count = 0;
        }

        (*user_count)++;
    }

    fclose(file);
}

void writeUserData(const char *filename, User *users, int user_count, const char *username) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Username,Password,Email,Balance,PNRs\n");

    for (int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username,username)==0) users[i].balance=walletAmount;
        fprintf(file, "%s,%s,%s,%d,\"", users[i].username, users[i].password, users[i].email, users[i].balance);
        for (int j = 0; j < users[i].pnr_count; j++) {
            fprintf(file, "%s", users[i].pnrs[j]);
            if (j < users[i].pnr_count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\"\n");
    }

    fclose(file);
}

void addPNR(User *users, int user_count, const char *username, const char *new_pnr) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            if (users[i].pnr_count < MAX_PNRS) {
                strcpy(users[i].pnrs[users[i].pnr_count], new_pnr);
                users[i].pnr_count++;
            } else {
                printf("User %s has reached the maximum number of PNRs.\n", username);
            }
            return;
        }
    }
    printf("User %s not found.\n", username);
}






void writeStructToFileIfRacPopulated(struct Planning *plan) {
    if (plan->rac_no > 0) {
        FILE *file;
        char filename[MAX_STRING_LENGTH];
        sprintf(filename, "rac_%d.txt", plan->train_number);  // File name like "rac_<train_number>.txt"

        file = fopen(filename, "a+");  // Append mode with create if not exist
        if (file == NULL) {
            printf("Error opening file %s.\n", filename);
            return;
        }

        // Write each passenger's details
        for (int i = 0; i < 10; ++i) {
            if (strlen(plan->name[i]) == 0) {
                continue;  // Skip empty passenger details
            }

            fprintf(file, "PNR: %s, Name: %s, Age: %d, Gender: %s, Train: %d, RAC No: %d, From: %s, To: %s, DOJ: %s, Coach: %d, Seat: %d, Cost: %d\n",
                    plan->pnr, plan->name[i], plan->age[i], plan->gender[i],
                    plan->train_number, plan->rac_no[i], plan->from,
                    plan->to, plan->doj, plan->coach, plan->seat[i],
                    plan->cost);
        }

        fclose(file);
    }
}
int findLatestRacNumber(int train_number, const char *date) {
    char filename[50]; // Assuming the file name to read RAC numbers from
    sprintf(filename, "rac_%d.txt", train_number); // Constructing filename based on train number

    FILE *file = fopen(filename, "r");
    if (file == NULL) {

        return 0; // Return 0 if file opening fails
    }

    int latest_rac_number = 0;
    char pnr[PNR_LENGTH];
    char name[MAX_STRING_LENGTH];
    int age;
    char gender[MAX_STRING_LENGTH];
    int train;
    int rac_no;
    char from[MAX_STRING_LENGTH];
    char to[MAX_STRING_LENGTH];
    char doj[MAX_STRING_LENGTH];
    int coach;
    int seat;
    int cost;
    char line[1024]; // Increased buffer size to accommodate the full line


    // Read each line until the end of file
    while (fgets(line, sizeof(line), file)) {

        line[strcspn(line, "\n")] = 0;


        if (sscanf(line, "PNR: %19[^,], Name: %99[^,], Age: %d, Gender: %99[^,], Train: %d, RAC No: %d, From: %99[^,], To: %99[^,], DOJ: %10[^,], Coach: %d, Seat: %d, Cost: %d",
                   pnr, name, &age, gender, &train, &rac_no, from, to, doj, &coach, &seat, &cost) == 12) {

            //printf("Parsed entry: PNR: %s, Name: %s, Age: %d, Gender: %s, Train: %d, RAC No: %d, From: %s, To: %s, DOJ: %s, Coach: %d, Seat: %d, Cost: %d\n",
              //     pnr, name, age, gender, train, rac_no, from, to, doj, coach, seat, cost);


            if (train == train_number && strcmp(doj, date) == 0) {
                //printf("Matching entry found: RAC No: %d\n", rac_no);
                if (rac_no > latest_rac_number) {
                    latest_rac_number = rac_no;
                }
            }
        } else {
            //printf("Line parsing failed: %s\n", line);
        }
    }

    fclose(file);
    //printf("Latest RAC Number found: %d\n", latest_rac_number);
    return latest_rac_number;
}

int authenticateUser(const char *username, const char *password, User *users, int user_count) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            walletAmount = users[i].balance;
            return 1;
        }
    }
    return 0;
}

void reverseString(char *str) {
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    char temp;

    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int stationsCrossed(char *route, char *from, char *to) {
    char *posFrom = strstr(route, from);
    char *posTo = strstr(route, to);

    int fromNum = posFrom-route;
    int toNum=posTo-route;
    char s[10];
    char t[10];
    int i=0;
    int n1=fromNum-1;
    while((route[n1]-'0')%10==(route[n1]-'0') && route[n1]!='-') {
        s[i++]=route[n1--];
    }
    s[i]='\0';
    reverseString(s);
    i=0;
    n1=toNum-1;
    while((route[n1]-'0')%10==(route[n1]-'0') && route[n1]!='-') {
        t[i++]=route[n1--];
    }
    t[i]='\0';
    reverseString(t);
    int ans=atoi(t)-atoi(s);
    return ans;
}
void updateWalletAmount(const char *username, int newWalletAmount) {
    FILE *file = fopen("login.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w");
    if (file == NULL || tempFile == NULL) {
        perror("Unable to open file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char fileUsername[MAX_USERNAME_LENGTH];
        char filePassword[MAX_PASSWORD_LENGTH];
        char fileEmail[MAX_STRING_LENGTH];
        int fileWallet;
        char filePNRs[MAX_STRING_LENGTH];

        trimNewline(line);
        sscanf(line, "%49[^,],%49[^,],%99[^,],%d,%[^\n]", fileUsername, filePassword, fileEmail, &fileWallet, filePNRs);
        if (strcmp(username, fileUsername) == 0) {
            walletAmount=newWalletAmount;
            fileWallet = newWalletAmount;
        }
        fprintf(tempFile, "%s,%s,%s,%d,%s\n", fileUsername, filePassword, fileEmail, fileWallet, filePNRs);
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the updated file
    remove("login.csv");
    rename("temp.csv", "login.csv");
}

int calculateTotalCost(int stationsCrossed, int coachType) {
    int baseCost=0;
    int i=coachType;
    if (i<=12){
        baseCost = SLEEPER_BASE_COST;

    }
    if (i>12 && i<17){
        baseCost = AC3_BASE_COST;
    }
    if (i>=17 && i+1<=19){
        baseCost = AC2_BASE_COST;
    }
    if (i+1>19){
        baseCost = AC1_BASE_COST;
    }
    int totalCost = baseCost * stationsCrossed;
    return totalCost;
}
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int readTrainData(struct Availability *trains, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return 0;
    }

    char line[1000];
    int count = 0;

    // Skip the header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) != NULL) {
        // Ignore empty lines
        if (strcmp(line, "\n") == 0) {
            continue;
        }

        char *token = strtok(line, ",");
        strcpy(trains[count].route, token);

        token = strtok(NULL, ",");
        strcpy(trains[count].destination, token);

        token = strtok(NULL, ",");
        strcpy(trains[count].doj, token);

        token = strtok(NULL, ",");
        strcpy(trains[count].train_name, token);

        token = strtok(NULL, ",");
        trains[count].train_number = atoi(token);

        token = strtok(NULL, ",");
        trains[count].ac_1 = atoi(token);

        token = strtok(NULL, ",");
        trains[count].ac_2 = atoi(token);

        token = strtok(NULL, ",");
        trains[count].ac_3 = atoi(token);

        token = strtok(NULL, ",");
        trains[count].sleeper_seats = atoi(token);
        char a12[10];
        token = strtok(NULL, ",");
        strcpy(a12, token);

        count++;
    }

    fclose(file);
    return count;
}

char route101[500];

int showAvailableTrains(struct Availability *trains, int num_trains, char *from, char *to, char *date) {
    printf("Available trains between %s and %s on %s:\n", from, to, date);
    printf("-----------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < num_trains; i++) {
        if (strcmp(trains[i].doj, date) == 0 && strstr(trains[i].route, from) != NULL && strstr(trains[i].route, to) != NULL && (strstr(trains[i].route, from)-trains[i].route < strstr(trains[i].route, to)-trains[i].route))
        {
            printf("Train Name and Departure Time: %s\n", trains[i].train_name);
            printf("Train Number: %d\n", trains[i].train_number);
            printf("Date of Journey: %s\n", trains[i].doj);
            printf("Route: %s\n", trains[i].route);
            strcpy(route101,trains[i].route);
            printf("Available Coaches and Seats:\n");
            printf("AC1: %d\n", trains[i].ac_1);
            printf("AC2: %d\n", trains[i].ac_2);
            printf("AC3: %d\n", trains[i].ac_3);
            printf("Sleeper: %d\n", trains[i].sleeper_seats);
            printf("\n");
            found = 1;
            return 1;
        }
    }
    if (!found) {
        printf("No trains found between %s and %s on %s.\n", from, to, date);
        return 0;
    }
}


int calculateDaysDifference(struct tm currentDate, struct tm journeyDate) {
    // Normalize times to midnight to avoid time differences
    currentDate.tm_hour = 0;
    currentDate.tm_min = 0;
    currentDate.tm_sec = 0;
    journeyDate.tm_hour = 0;
    journeyDate.tm_min = 0;
    journeyDate.tm_sec = 0;

    // Convert both times to time_t for comparison
    time_t current_time = mktime(&currentDate);
    time_t journey_time = mktime(&journeyDate);

    // Calculate difference in seconds and convert to days
    double seconds_difference = difftime(journey_time, current_time);
    int days_difference = seconds_difference / (60 * 60 * 24);

    return days_difference;
}


void displayChart(int perms,int coach, int seats[TOTAL_COACHES][SEATS_S], int genderSeats[TOTAL_COACHES][SEATS_S], int seatCount, int rows, int columnsLeft, int columnsRight, struct tm journeyDate) {
    printf("Train Coach Seat Selection Chart for Coach %d:\n", coach + 1);
    printf("\nDisclaimer: Last 5 seats are tatkal and open only one day before departure.\nThey have Rs.300 Addon Fare.\n");
    if (perms==1) {
        printf("\nThe next 5 seats before tatkal open only when any coach has been fully booked.\n2 people travel in one seat.\nThe legend for those seats are:\n MR:One Male has booked the seat. It is open for further booking.\n FR:One Female has booked the seat. It is open for further booking.\n MM/MF/FM/FF: 2 males/1 male 1 female/1 female one male/2 females have booked the seat. It cant be booked further.\n");
    }
    else printf("\nThe next 5 seats before tatkal open only when any coach has been fully booked.\n");
    if (coach >= S_COACHES + AC3_COACHES && coach < S_COACHES + AC3_COACHES + AC2_COACHES) {
        printf("  SL  SU       LL  LU  RL  RU\n  31  32\n");
    } else if (coach == TOTAL_COACHES - 1) {
        printf("  LL  LU  RL  RU\n");
    } else {
        printf("  SU  SL       LL  LM  LU  RL  RM  RU\n");
    }
    int temp[20];
    int k=0;
    for (int j=0;j<TOTAL_COACHES;j++){
        if (sears[j]==0){
            temp[k]=j+1;
        }
    }
    time_t t = time(NULL);
    struct tm *currentDate = localtime(&t);
    int daysBeforeJourney = calculateDaysDifference(*currentDate, journeyDate);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columnsLeft; j++) {
            int seatIndex = i * (columnsLeft + columnsRight) + j;
            if (seatIndex >= seatCount - 5 && daysBeforeJourney > 1) {
                printf("  --"); // Do not display last 5 seats
            } else {
                int seatCode = seats[coach][seatIndex];
                if (seatCode == BOOKED) {
                    if (genderSeats[coach][seatIndex] == MALE) {
                        printf("  M ");
                    } else if (genderSeats[coach][seatIndex] == FEMALE) {
                        printf("  F ");
                    } else {
                        printf("  X "); // Unknown gender
                    }
                } else if (seatCode == MRAC) {
                    printf(" MR "); // Male RAC
                } else if (seatCode == FRAC) {
                    printf(" FR "); // Female RAC
                } else if (seatCode == MM) {
                    printf(" MM "); // Male confirmed
                } else if (seatCode == MF) {
                    printf(" MF "); // Male confirmed
                } else if (seatCode == FM) {
                    printf(" FM "); // Female confirmed
                } else if (seatCode == FF) {
                    printf(" FF "); // Female confirmed
                } else {
                    printf("  %2d", seatCode); // Normal seat number
                }
            }
        }
        if (columnsRight > 0) {
            printf("    ");
            for (int j = 0; j < columnsRight; j++) {
                int seatIndex = i * (columnsLeft + columnsRight) + columnsLeft + j;
                if (seatIndex >= seatCount - 5 && daysBeforeJourney > 1) {
                    printf("  --"); // Do not display last 5 seats
                } else {
                    int seatCode = seats[coach][seatIndex];
                    if (seatCode == BOOKED) {
                        if (genderSeats[coach][seatIndex] == MALE) {
                            printf("  M ");
                        } else if (genderSeats[coach][seatIndex] == FEMALE) {
                            printf("  F ");
                        } else {
                            printf("  X "); // Unknown gender
                        }
                    } else if (seatCode == MRAC) {
                        printf(" MR "); // Male RAC
                    } else if (seatCode == FRAC) {
                        printf(" FR "); // Female RAC
                    } else if (seatCode == MM) {
                        printf(" MM "); // Male confirmed
                    } else if (seatCode == MF) {
                        printf(" MF "); // Male confirmed
                    } else if (seatCode == FM) {
                        printf(" FM "); // Female confirmed
                    } else if (seatCode == FF) {
                        printf(" FF "); // Female confirmed
                    } else {
                        printf("  %2d", seatCode); // Normal seat number
                    }
                }
            }
        }
        printf("\n");
    }
}


int perms;
int countAvailableSeats(int *perms_rac, int coach, int seats[TOTAL_COACHES][SEATS_S], int seatCount, struct tm journeyDate) {
    int count = 0;

    time_t t = time(NULL);
    struct tm *currentDate = localtime(&t);
    int daysBeforeJourney = calculateDaysDifference(*currentDate, journeyDate);

    int normalSeatsAvailable = 0;
    for (int i = 0; i < seatCount - 10; i++) {
        if (seats[coach][i] != BOOKED) {
            normalSeatsAvailable++;
        }
    }

    if (daysBeforeJourney <= 1) {
        for (int i = seatCount - 5; i < seatCount; i++) {
            if (seats[coach][i] != BOOKED) {
                count++;
            }
        }
    }

        if (normalSeatsAvailable+count == 0) {
            *perms_rac=1;
            for (int i = seatCount - 10; i < seatCount - 5; i++) {
                if (seats[coach][i] == MRAC || seats[coach][i] == FRAC) {
                    count++;
                } else if (seats[coach][i] == MM || seats[coach][i] == FF || seats[coach][i] == MF || seats[coach][i] == FM) {
                    continue;
                } else {
                    count += 2;
                }
            }
        }

        for (int i = seatCount - 5; i < seatCount; i++) {
            if (seats[coach][i] != BOOKED) {
                count++;
            }
        }
    return normalSeatsAvailable+count;
}

void displayAvailableSeats(int seats[TOTAL_COACHES][SEATS_S], struct tm journeyDate) {
    printf("Available seats in each coach:\n");
    for (int i = 0; i < TOTAL_COACHES; i++) {
        int seatCount, rows, columnsLeft, columnsRight;

        if (i < S_COACHES) {
            seatCount = SEATS_S;
            rows = ROWS_S;
            columnsLeft = COLUMNS_LEFT_S;
            columnsRight = COLUMNS_RIGHT_S;
        } else if (i < S_COACHES + AC3_COACHES) {
            seatCount = SEATS_AC3;
            rows = ROWS_AC3;
            columnsLeft = COLUMNS_LEFT_AC3;
            columnsRight = COLUMNS_RIGHT_AC3;
        } else if (i < S_COACHES + AC3_COACHES + AC2_COACHES) {
            seatCount = SEATS_AC2;
            rows = ROWS_AC2;
            columnsLeft = COLUMNS_AC2;
            columnsRight = 4;
        }
        else {
            seatCount = SEATS_AC1;
            rows = ROWS_AC1;
            columnsLeft = COLUMNS_AC1;
            columnsRight = 0;
        }

        int availableSeats = countAvailableSeats(&perms,i, seats, seatCount, journeyDate);
        if (availableSeats==-5){
            printf("All seats in coach %2d has been booked. Only RAC seats are left. 2 people can book in one seat");
            sears[i]=10;
            continue;
        }
        if (i+1<=12){
            printf("Coach S-%2d: %2d available seats\n", i + 1, availableSeats);
        }
        if (i+1>12 && i+1<17){

            printf("Coach AC3-%2d: %2d available seats\n", i + 1, availableSeats);
        }
        if (i+1>=17 && i+1<=19){

            printf("Coach AC2-%2d: %2d available seats\n", i + 1, availableSeats);
        }
        if (i+1>19){

            printf("Coach AC1-%2d: %2d available seats\n", i+1, availableSeats);
        }


    }
    printf("\n");
}


int canBookSeat(int coach, int seatIndex, int gender, int genderSeats[TOTAL_COACHES][SEATS_S], int seatCount, struct tm journeyDate, char fam[]) {
    time_t t = time(NULL);
    struct tm *currentDate = localtime(&t);

    int daysBeforeJourney = calculateDaysDifference(*currentDate, journeyDate);

    if (seatIndex >= seatCount - 5 && daysBeforeJourney > 1) {
        return 0;
    }

    if (strcmp(fam, "Yes") != 0) {
        int adjacentSeats[] = {seatIndex - 1, seatIndex + 1};
        for (int i = 0; i < 2; i++) {
            int adjIndex = adjacentSeats[i];
            if (adjIndex >= 0 && adjIndex < seatCount) {
                if (genderSeats[coach][adjIndex] != 0 && genderSeats[coach][adjIndex] != gender) {
                    int ro_seats=0;
                    if(coach+1<17) ro_seats=8;
                    else if(coach+1>=17 && coach+1<20) ro_seats=6;
                    else ro_seats=4;
                    if(i==0 && ((adjIndex+1)%(ro_seats)==0 || ((adjIndex+1)%(ro_seats)==2 && coach!=19))) {
                    }
                    else if(i==1 && ((adjIndex+1)%(ro_seats)==1 || ((adjIndex+1)%(ro_seats)==3 && coach!=19))) {
                    }
                    else {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}
void add_review(const char *filename) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Could not open file for appending");
        exit(EXIT_FAILURE);
    }

    int stars;
    char review[MAX_LINE_LENGTH];

    printf("Enter the star rating (1-5): ");
    scanf("%d", &stars);
    getchar(); // consume the newline character after the star input

    printf("Enter the review: ");
    fgets(review, sizeof(review), stdin);

    // Write the review followed by a newline for separation
    fprintf(file, "%d %s\n\n", stars, review);
    fclose(file);

    printf("Review added successfully.\n");
}

void generatePNR(struct Planning *ticket) {
    static int counter = 0;
    char train_number_str[3];
    char coach_str[3];
    char random_str[7];

    snprintf(train_number_str, sizeof(train_number_str), "%02d", ticket->train_number % 100); // Ensuring 2 digits
    snprintf(coach_str, sizeof(coach_str), "%02d", ticket->coach % 100); // Ensuring 2 digits

    // Seed the random number generator with the current time and counter
    srand((unsigned int) time(NULL) + counter);
    counter++;

    // Generate 6 random alphanumeric characters
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 6; i++) {
        random_str[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    random_str[6] = '\0';

    // Combine parts to form the PNR
    snprintf(ticket->pnr, PNR_LENGTH, "%s%s%s", train_number_str, coach_str, random_str);
}



void writePlanningDataToFile(const char *filename, struct Planning *plan) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "From: %s\n", plan->from);
    fprintf(file, "To: %s\n", plan->to);
    fprintf(file, "Date of Journey: %s\n", plan->doj);
    fprintf(file, "Train Number: %d\n", plan->train_number);
    fprintf(file, "Cost: %d\n", plan->cost);
    fprintf(file, "PNR: %s\n", plan->pnr);
    fprintf(file, "Coach No: %d\n", plan->coach);
    for (int i = 0; i < 10; i++) {
        if (plan->name[i][0] != '\0') { // Check if the name is not empty
            fprintf(file, "Passenger %d Name: %s\n", i + 1, plan->name[i]);
            fprintf(file, "Passenger %d Age: %d\n", i + 1, plan->age[i]);
            fprintf(file, "Passenger %d Gender: %s\n", i + 1, plan->gender[i]);
            fprintf(file, "Passenger %d Seat: %d\n", i + 1, plan->seat[i]);
        }
    }
    fprintf(file, "\n");

    if (fclose(file) == 0) {
        printf("Program successfully quit.\n");
    } else {
        perror("Failed to close file");
    }
}
void send_email(struct Planning *plan, const char *to_email) {
    FILE *file;
    file = fopen("email_content.txt", "w"); // Open a file to write the email content

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Write the formatted travel plan into the file
    fprintf(file, "From: %s\n", plan->from);
    fprintf(file, "To: %s\n", plan->to);
    fprintf(file, "Date of Journey: %s\n", plan->doj);
    fprintf(file, "Train Number: %d\n", plan->train_number);
    fprintf(file, "Cost: %d\n", plan->cost);
    fprintf(file, "PNR: %s\n", plan->pnr);
    fprintf(file, "Coach No: %d\n", plan->coach);
    for (int i = 0; i < 10; i++) {
        if (plan->name[i][0] != '\0') { // Check if the name is not empty
            fprintf(file, "Passenger %d Name: %s\n", i + 1, plan->name[i]);
            fprintf(file, "Passenger %d Age: %d\n", i + 1, plan->age[i]);
            fprintf(file, "Passenger %d Gender: %s\n", i + 1, plan->gender[i]);
            fprintf(file, "Passenger %d Seat: %d\n", i + 1, plan->seat[i]);
        }
    }

    fclose(file);

    // Construct the command to call the Python script with the email content file
    char command[1024];
    snprintf(command, sizeof(command), "python emaily.py \"%s\" email_content.txt", to_email);

    // Print the command for debugging
    //printf("Command: %s\n", command);

    // Execute the command
    int result = system(command);

    // Check the result
    if (result == -1) {
        perror("system");
        return;
    }

    remove("email_content.txt"); // Remove the temporary file after sending
}
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len-1] == '\n') {
        str[len-1] = '\0';
    }
}

// Function to verify the username and password
int verify_credentials(const char *username, const char *password) {
    FILE *fp = fopen("admin.csv", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    char line[256];
    char file_username[256], file_password[256];

    // Skip the header line
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);

        // Parse the line
        sscanf(line, "%[^,],%s", file_username, file_password);

        // Check if the credentials match
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            fclose(fp);
            return 1; // Credentials are correct
        }
    }

    fclose(fp);
    return 0; // Credentials are incorrect
}
int main() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    trimNewline(username);

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    trimNewline(password);

    User users[MAX_USERS];
    int user_count = 0;

    const char *filename = "login.csv";

    readUserData(filename, users, &user_count);

    if (authenticateUser(username, password, users, user_count)) {
        printf("Login successful! Wallet amount: %d\n", walletAmount);
        int ch=1;

        while(ch==1){
            label1:
            printf("Welcome to the Railway Reservation System\n");
            printf("Enter 2 to Access Admin\nEnter 3 to Cancel Ticket\nEnter 4 to Book Ticket\nEnter 5 Enquire PNR\nEnter 6 to Submit Feedback.\nEnter 7 to Quit\nEnter your choice::");
            scanf("%d", &ch);
            clearInputBuffer();
            if(ch==5) {
                int dh=pnr_enquiry();
                if(dh){
                    printf("Success");
                }
                ch=1;
            }
            else if(ch==2) {
                char username[256];
                char password[256];

                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                trim_newline(username);

                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                trim_newline(password);

                if (verify_credentials(username, password)) {
                    printf("Login successful!\n");
                    start();
                    clearInputBuffer();
                    ch=1;
                } else {
                    printf("Invalid username or password.\n");
                }
            }
            else if(ch==3) {
                ticketcancel();
                clearInputBuffer();
                ch=1;
            }
            else if(ch==4) {
                struct Availability trains[100];
                struct Planning plan;
                int num_trains = readTrainData(trains, "trains.csv");
                Repeat:
                printf("\n");
                char pnr[PNR_LENGTH];



                char from[MAX_STRING_LENGTH], to[MAX_STRING_LENGTH], date[MAX_STRING_LENGTH];
                printf("Enter from station: ");
                fgets(from, MAX_STRING_LENGTH, stdin);
                trimNewline(from);

                printf("Enter to station: ");
                fgets(to, MAX_STRING_LENGTH, stdin);
                trimNewline(to);

                printf("Enter date of journey (YYYY-MM-DD): ");
                fgets(date, MAX_STRING_LENGTH, stdin);
                trimNewline(date);
                strcpy(plan.doj, date);
                int a = showAvailableTrains(trains, num_trains, from, to, date);
                if (a == 0) {
                    goto Repeat;
                }

                FILE *file = fopen("seatsf.bin", "rb");
                if (!file) {
                    perror("Failed to open file");
                    return 1;
                }

                struct seats all_trains[100];
                int train_count = 0;

                while (fread(&all_trains[train_count], sizeof(struct seats), 1, file)) {
                    train_count++;
                }
                fclose(file);

                int search_trainno;
                printf("Enter the train number to search: ");
                scanf("%d", &search_trainno);
                clearInputBuffer();

                struct seats *s = NULL;
                for (int i = 0; i < train_count; i++) {
                    if (all_trains[i].trainno == search_trainno) {
                        s = &all_trains[i];
                        break;
                    }
                }

                if (s == NULL) {
                    printf("Train number %d not found.\n", search_trainno);
                    return 1;
                }

                int seats[TOTAL_COACHES][SEATS_S];
                int genderSeats[TOTAL_COACHES][SEATS_S];

                copy_2d_array(s->s1, seats, TOTAL_COACHES, SEATS_S);
                copy_2d_array(s->gS, genderSeats, TOTAL_COACHES, SEATS_S);

                int year = s->year;
                int month = s->month;
                int day = s->day;
                struct tm journeyDate = {0};
                journeyDate.tm_year = year - 1900;
                journeyDate.tm_mon = month - 1;
                journeyDate.tm_mday = day;
                mktime(&journeyDate);

                int num_passengers;
                printf("Enter number of passengers: ");
                scanf("%d", &num_passengers);
                clearInputBuffer();

                displayAvailableSeats(seats, journeyDate);

                int coachToDisplay;
                printf("Enter coach number (1-%d): ", TOTAL_COACHES);
                scanf("%d", &coachToDisplay);
                plan.coach = coachToDisplay;
                coachToDisplay--;

                if (coachToDisplay >= 0 && coachToDisplay < TOTAL_COACHES) {
                    int seatCount, rows, columnsLeft, columnsRight;

                    if (coachToDisplay < S_COACHES) {
                        seatCount = SEATS_S;
                        rows = ROWS_S;
                        columnsLeft = COLUMNS_LEFT_S;
                        columnsRight = COLUMNS_RIGHT_S;
                    } else if (coachToDisplay < S_COACHES + AC3_COACHES) {
                        seatCount = SEATS_AC3;
                        rows = ROWS_AC3;
                        columnsLeft = COLUMNS_LEFT_AC3;
                        columnsRight = COLUMNS_RIGHT_AC3;
                    } else if (coachToDisplay < S_COACHES + AC3_COACHES + AC2_COACHES) {
                        seatCount = SEATS_AC2;
                        rows = ROWS_AC2;
                        columnsLeft = COLUMNS_AC2;
                        columnsRight = 4;
                    } else if (coachToDisplay < TOTAL_COACHES - 1) {
                        seatCount = SEATS_MIXED;
                        rows = ROWS_MIXED;
                        columnsLeft = COLUMNS_LEFT_MIXED;
                        columnsRight = COLUMNS_RIGHT_MIXED;
                    } else {
                        seatCount = SEATS_AC1;
                        rows = ROWS_AC1;
                        columnsLeft = COLUMNS_AC1;
                        columnsRight = 0;
                    }

                    int availableSeats = countAvailableSeats(&perms,coachToDisplay, seats, seatCount, journeyDate);
                    printf("Number of available seats in Coach %d: %d\n\n", coachToDisplay + 1, availableSeats);
                    displayChart(perms,coachToDisplay, seats, genderSeats, seatCount, rows, columnsLeft, columnsRight, journeyDate);

                    int count = num_passengers;
                    char fam[5] = "No";
                    int plann = 0;
                    if (count >= 2) {
                        printf("Are you travelling as a family? (adjacent gender restriction doesn't apply [Yes/No]): ");
                        scanf("%s", fam);
                        clearInputBuffer();
                    }
                    int currency_of_gender=0;
                    int rac=findLatestRacNumber(search_trainno,date);
                    int rac_ch=0;
                    printf("The current rac no is %d\n",rac);

                    while (count > 0) {

                        int gender;
                        printf("Enter your gender (1 for Male, 2 for Female): ");
                        scanf("%d", &gender);
                        gender = (gender == 1) ? MALE : FEMALE;
                        clearInputBuffer();
                        int seatNumber;
                        printf("Enter seat number to book (1-%d):", seatCount);
                        scanf("%d", &seatNumber);
                        int cpy=seatNumber-1;
                        plan.seat[plann] = seatNumber;
                        seatNumber=seatNumber-1;

                        clearInputBuffer();

                        if (seats[coachToDisplay][seatNumber] == MM || seats[coachToDisplay][seatNumber] == FF || seats[coachToDisplay][seatNumber] == FM || seats[coachToDisplay][seatNumber] == MF || seats[coachToDisplay][seatNumber] == BOOKED) {
                            printf("Invalid seat number or seat already booked.\n");
                            continue;
                        }
                        if (perms==0 && seatNumber >= seatCount - 10 && seatNumber < seatCount - 5 ){
                            printf("RAC opens only if all seats are booked in any coach in the train.\n");
                            continue;
                        }
                        if (perms == 1 && seatNumber >= seatCount - 10 && seatNumber < seatCount - 5 &&
                            seats[coachToDisplay][seatNumber] != MM && seats[coachToDisplay][seatNumber] != FF) {

                            //printf("TEST\n%d\n",currency_of_gender);

                            if (strcmp("No", fam) == 0) {
                                if (seats[coachToDisplay][cpy] == MRAC && gender == FEMALE) {
                                    printf("Female can't sit with male unless traveling as family.\n");
                                    continue;
                                }

                                else if (seats[coachToDisplay][cpy] == FRAC && gender == MALE) {
                                    printf("Male can't sit with female unless traveling as family.\n");
                                    continue;
                                }
                            }
                            else{
                                if (currency_of_gender==0){
                                    if (seats[coachToDisplay][cpy] == MRAC && gender == FEMALE) {
                                        printf("Female can't sit with male unless that male is of same family.\n");
                                        continue;
                                    }

                                    else if (seats[coachToDisplay][cpy] == FRAC && gender == MALE) {
                                        printf("Male can't sit with female unless that female is of same family.\n");
                                        continue;
                                    }
                                }

                            }
                            printf("\n You are trying to book an rac seat.\n Book this only if all the other seats are full.\n 2 people travel in one seat.\n If any confirm seat gets cancelled, you will be upgraded free of cost.");
                            printf("\nDo you want to continue? (Yes / No): ");
                            char cho[4];
                            fgets(cho, MAX_STRING_LENGTH, stdin);
                            if (strcmp("Yes",cho)==0){
                                continue;
                            }
                            rac++;
                            printf("Enter details for passenger:\n Name:");
                            fgets(plan.name[plann], MAX_STRING_LENGTH, stdin);
                            trimNewline(plan.name[plann]);
                            printf("Age: ");
                            plan.rac_no[plann]=rac;
                            scanf("%d", &plan.age[plann]);
                            clearInputBuffer();
                            strcpy(plan.gender[plann], (gender == MALE) ? "Male" : "Female");


                            //plan.seat[plann] = (seatNumber + 1);
                            printf("%d IS YOUR SEAT NUMBER\n",plan.seat[plann]);
                            plann++;
                            int nost = stationsCrossed(route101, from, to);
                            int cost_temp = calculateTotalCost(nost, coachToDisplay);
                            if (cost_temp <= walletAmount) {
                                walletAmount -= cost_temp;
                                updateWalletAmount(username, walletAmount);
                            } else {
                                int choices;
                                printf("\nWallet amount not sufficient. Do you want to add money? (1-Yes/2-No): ");
                                scanf("%d", &choices);
                                clearInputBuffer();
                                if (choices == 1) {
                                    int amnt_added;
                                    printf("Enter amount to be added: ");
                                    scanf("%d", &amnt_added);
                                    clearInputBuffer();
                                    walletAmount += amnt_added;
                                    walletAmount -= cost_temp;
                                    updateWalletAmount(username, walletAmount);
                                    printf("Amount %d added to wallet, and then ticket was booked.\n", amnt_added);

                                } else {
                                    exit(1);
                                }
                            }

                            if (seats[coachToDisplay][cpy] == MRAC) {
                                seats[coachToDisplay][cpy] = (gender == MALE) ? MM : MF;
                            } else if (seats[coachToDisplay][cpy] == FRAC) {
                                seats[coachToDisplay][cpy] = (gender == MALE) ? FM : FF;
                            } else {
                                currency_of_gender=1;
                                seats[coachToDisplay][cpy] = (gender == MALE) ? MRAC : FRAC;
                            }

                            genderSeats[coachToDisplay][cpy] = gender;
                            plan.cost = cost_temp * num_passengers;


                            printf("Seat %d in Coach %d booked successfully.\n", plan.seat[plann-1], coachToDisplay + 1);
                            count--;
                            rac_ch=1;
                            }
                        else if (seatNumber >= 0 && seatNumber < seatCount && seats[coachToDisplay][seatNumber] != BOOKED) {
                            if (canBookSeat(coachToDisplay, seatNumber, gender, genderSeats, seatCount, journeyDate, fam)) {
                                printf("Enter details for passenger:\n");
                                printf("Name: ");
                                fgets(plan.name[plann], MAX_STRING_LENGTH, stdin);
                                trimNewline(plan.name[plann]);
                                printf("Age: ");
                                scanf("%d", &plan.age[plann]);
                                clearInputBuffer();
                                strcpy(plan.gender[plann], (gender == MALE) ? "Male" : "Female");
                                plann++;

                                int nost = stationsCrossed(route101, from, to);
                                int cost_temp = calculateTotalCost(nost, coachToDisplay);
                                if ((seatNumber >= 68 && coachToDisplay < 13) ||
                                    (seatNumber >= 60 && coachToDisplay >= 13 && coachToDisplay <= 16) ||
                                    (seatNumber >= 28 && coachToDisplay >= 17 && coachToDisplay <= 19) ||
                                    (seatNumber >= 12 && coachToDisplay == 20)) {
                                    cost_temp += 300;
                                    printf("Warning! Tatkal Quota, Cost + 300\n");
                                    }

                                if (cost_temp <= walletAmount) {
                                    walletAmount -= cost_temp;
                                    updateWalletAmount(username, walletAmount);
                                } else {
                                    int choices;
                                    printf("\nWallet amount not sufficient. Do you want to add money? (1-Yes/2-No): ");
                                    scanf("%d", &choices);
                                    clearInputBuffer();
                                    if (choices == 1) {
                                        int amnt_added;
                                        printf("Enter amount to be added: ");
                                        scanf("%d", &amnt_added);
                                        clearInputBuffer();
                                        walletAmount += amnt_added;
                                        walletAmount -= cost_temp;
                                        updateWalletAmount(username, walletAmount);
                                        printf("Amount %d added to wallet, and then ticket was booked.\n", amnt_added);
                                    } else {
                                        exit(1);
                                    }
                                }

                                seats[coachToDisplay][cpy] = BOOKED;
                                genderSeats[coachToDisplay][cpy] = gender;
                                plan.cost = cost_temp * num_passengers;
                                printf("Seat %d in Coach %d booked successfully.\n", cpy+1, coachToDisplay + 1);
                                count--;
                            } else {
                                printf("Cannot book seat %d due to adjacent gender restriction or last 5 seats restriction.\n", seatNumber + 1);
                            }
                        } else if (seatNumber < 0 || seatNumber >= seatCount) {
                            printf("Invalid seat number or seat already booked.\n");
                        }
                    }



                    update_train_details(search_trainno, seats, genderSeats);

                    strcpy(plan.from, from);
                    strcpy(plan.to, to);

                    plan.train_number = search_trainno;
                    char to_email[256];
                    generatePNR(&plan);
                    printf("Enter recipient email address for ticket: ");
                    scanf("%255s", to_email);

                    // Call function to send email with the travel plan
                    send_email(&plan, to_email);

                    printf("\nThe details of ticket booked are:\nFrom: %s\nTo: %s\nDOJ: %s\nTrainNo: %d\nCost: %d\nPNR: %s\n\n\n",
                           plan.from, plan.to, plan.doj, plan.train_number, plan.cost, plan.pnr,plan);

                    addPNR(users, user_count, username, plan.pnr);
                    writeUserData("login.csv", users, user_count, username);

                    if (rac_ch>0) writeStructToFileIfRacPopulated(&plan);
                    else writePlanningDataToFile("book.txt", &plan);


                } else {
                    printf("Invalid coach number.\n");
                }
                ch=1;
            }else if(ch==6) {
                    add_review("reviews.txt");
                    goto label1;
            }
            else if(ch==7) {
                break;
            }
        }
    }else {
       printf("Invalid username or password.\n");
   }
        return 0;
}


