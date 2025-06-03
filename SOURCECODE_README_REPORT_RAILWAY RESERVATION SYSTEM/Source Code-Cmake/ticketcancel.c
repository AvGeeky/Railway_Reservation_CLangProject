#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

#define SERVICE_CHARGE 200
#define HOUR_TO_SECONDS 3600
#define DAY_TO_SECONDS (24 * HOUR_TO_SECONDS)

#define MAX_PNRS 20
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

void trimNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}


void removePNR(char *pnrList, const char *pnr) {
    char temp[MAX_STRING_LENGTH] = "";
    char *token = strtok(pnrList, ",");
    int first = 1,flag=0;

    while (token != NULL) {


        if (strncmp(token, pnr, 9) != 0) {
            if (!first) {
                strcat(temp, ",");
            }
            strcat(temp, token);
            first = 0;
            token = strtok(NULL, ",");
        }
        else {
            token = strtok(NULL, ",");
            if(token == NULL) flag=1;
        }
    }
    if(flag) {
        temp[strlen(temp)]='"';
        temp[strlen(temp)]='\0';
    }
    strcpy(pnrList, temp);
}

void updateWalletAmountAndRemovePNR(const char *username, int newWalletAmount, const char *pnrToRemove) {
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
            fileWallet += newWalletAmount;
            removePNR(filePNRs, pnrToRemove);
            printf("%s\n",filePNRs);
        }
        fprintf(tempFile, "%s,%s,%s,%d,%s\n", fileUsername, filePassword, fileEmail, fileWallet, filePNRs);
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the updated file
    remove("login.csv");
    rename("temp.csv", "login.csv");
}

int calculateRefund(struct tm journey_date, int ticket_cost) {
    time_t now;
    time(&now);

    time_t journey_time = mktime(&journey_date);
    double seconds_to_journey = difftime(journey_time, now);

    if (seconds_to_journey <= DAY_TO_SECONDS) {
        // Less than 24 hours to journey
        return 300;
    } else if (seconds_to_journey <= 2 * DAY_TO_SECONDS) {
        // Between 24 to 48 hours to journey
        return (ticket_cost * 300 / 100) - SERVICE_CHARGE;
    } else {
        // More than 48 hours to journey
        return (ticket_cost * 75 / 100) - SERVICE_CHARGE;
    }
}

void copy_2d_array(int src[TOTAL_COACHES][SEATS_S], int dest[TOTAL_COACHES][SEATS_S], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

void update_train_details(int trainno, int seats[TOTAL_COACHES][SEATS_S], int genderSeats[TOTAL_COACHES][SEATS_S]) {
    FILE *file = fopen("seatsf.bin", "rb+");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    struct seats s;
    int found = 0;

    // Find the train in the file
    while (fread(&s, sizeof(struct seats), 1, file)) {
        if (s.trainno == trainno) {
            // Copy updated details into the structure
            copy_2d_array(seats, s.s1, TOTAL_COACHES, SEATS_S);
            copy_2d_array(genderSeats, s.gS, TOTAL_COACHES, SEATS_S);

            // Move file pointer back to the start of the current structure
            fseek(file, -(long)sizeof(struct seats), SEEK_CUR);
            // Write the updated structure back to the file
            fwrite(&s, sizeof(struct seats), 1, file);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Train number %d not found.\n", trainno);
    }

    fclose(file);
}

void displayPlanningData(struct Planning *bookings, int num_Confirm, int num_RAC) {
    int num_bookings = num_Confirm + num_RAC;
    for (int i = 0; i < 6; i++) {
        struct Planning *booking = &bookings[i];
        printf("Booking %d:\n", i + 1);
        printf("PNR: %s\n", booking->pnr);
        printf("From: %s\n", booking->from);
        printf("To: %s\n", booking->to);
        printf("Date of Journey: %s\n", booking->doj);
        printf("Train Number: %d\n", booking->train_number);
        printf("Coach: %d\n", booking->coach);
        printf("Cost: %d\n", booking->cost);
        for (int j = 0; j < 10 && booking->name[j][0] != '\0'; j++) {
            printf("Passenger %d Name: %s\n", j + 1, booking->name[j]);
            printf("Passenger %d Age: %d\n", j + 1, booking->age[j]);
            printf("Passenger %d Gender: %s\n", j + 1, booking->gender[j]);
            printf("Passenger %d Seat: %d\n", j + 1, booking->seat[j]);
            if (i >= num_Confirm) {
                printf("Passenger %d RAC No: %d\n", j + 1, booking->rac_no[j]);
            }
        }
        printf("\n");
    }
}

int readBookingDataConfirm(const char* filename, struct Planning* planningArray) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return 0;
    }

    int count = 0;
    while (count < MAX_RECORDS && !feof(file)) {

            // Reading details from the file
            fscanf(file, "From: %s\n", planningArray[count].from);
            fscanf(file, "To: %s\n", planningArray[count].to);
            fscanf(file, "Date of Journey: %s\n", planningArray[count].doj);
            fscanf(file, "Train Number: %d\n", &planningArray[count].train_number);
            fscanf(file, "Cost: %d\n", &planningArray[count].cost);
            fscanf(file, "PNR: %s\n", planningArray[count].pnr);
            fscanf(file, "Coach No: %d\n", &planningArray[count].coach);

            for (int i = 0; i < 10; i++) {
                if (fscanf(file, "Passenger %*d Name: %s\n", planningArray[count].name[i]) == EOF) break;
                fscanf(file, "Passenger %*d Age: %d\n", &planningArray[count].age[i]);
                fscanf(file, "Passenger %*d Gender: %s\n", planningArray[count].gender[i]);
                fscanf(file, "Passenger %*d Seat: %d\n", &planningArray[count].seat[i]);
                // Assuming RAC number is also provided in the same pattern if applicable
                // fscanf(file, "Passenger %*d RAC: %d\n", &planningArray[count].rac_no[i]);
            }

        // Skip blank line between records
        fscanf(file, "\n");
        count++;
    }

    fclose(file);
    return count;
}


int readBookingDataRAC(const char* filename,struct Planning* bookings, int start_index) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    char line[1024];
    int index = start_index;

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        // Parse the line
        char *token = strtok(line, ",");
        if (token) sscanf(token, "PNR: %s", bookings[index].pnr);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Name: %s", bookings[index].name[0]);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Age: %d", &bookings[index].age[0]);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Gender: %s", bookings[index].gender[0]);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Train: %d", &bookings[index].train_number);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " RAC No: %d", &bookings[index].rac_no[0]);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " From: %s", bookings[index].from);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " To: %s", bookings[index].to);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " DOJ: %s", bookings[index].doj);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Coach: %d", &bookings[index].coach);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Seat: %d", &bookings[index].seat[0]);

        token = strtok(NULL, ",");
        if (token) sscanf(token, " Cost: %d", &bookings[index].cost);

        index++;
    }

    fclose(file);
    return index-start_index;
}


void writePlanningDataConfirm(const char *filename, struct Planning *plan, int con, int a) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

   for(int i=0;i<con;i++) {
       if(i==a) continue;
       int k=1;
        while(strcmp(plan[i].pnr,plan[i-1].pnr)==0 && i<con) {
            for (int j = 0; j < 10; j++) {
                if (plan[i].name[j][0] != '\0') {
                    fprintf(file, "Passenger %d Name: %s\n", k + 1,  plan[i].name[j]);
                    fprintf(file, "Passenger %d Age: %d\n", k + 1,  plan[i].age[j]);
                    fprintf(file, "Passenger %d Gender: %s\n", k + 1,  plan[i].gender[j]);
                    fprintf(file, "Passenger %d Seat: %d\n", k + 1,  plan[i].seat[j]);
                    k++;
                }
            }
            i++;
        }
       fprintf(file, "\n");
       if(i>=con) continue;

       fprintf(file, "From: %s\n", plan[i].from);
       fprintf(file, "To: %s\n",  plan[i].to);
       fprintf(file, "Date of Journey: %s\n",  plan[i].doj);
       fprintf(file, "Train Number: %d\n",  plan[i].train_number);
       fprintf(file, "Cost: %d\n",  plan[i].cost);
       fprintf(file, "PNR: %s\n",  plan[i].pnr);
       fprintf(file, "Coach No: %d\n",  plan[i].coach);
       for (int j = 0; j < 10; j++) {
           if (plan[i].name[j][0] != '\0') { // Check if the name is not empty
               fprintf(file, "Passenger %d Name: %s\n", j + 1,  plan[i].name[j]);
               fprintf(file, "Passenger %d Age: %d\n", j + 1,  plan[i].age[j]);
               fprintf(file, "Passenger %d Gender: %s\n", j + 1,  plan[i].gender[j]);
               fprintf(file, "Passenger %d Seat: %d\n", j + 1,  plan[i].seat[j]);
           }
       }
   }
    if (fclose(file) == 0) {
        printf("Program successfully quit.\n");
    } else {
        perror("Failed to close file");
    }
}

void writePlanningDataRAC(const char *filename, struct Planning *plannings, int startIndex, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    for (int i = startIndex; i < startIndex + count; i++) {
        fprintf(file, "PNR: %s, Name: %s, Age: %d, Gender: %s, Train: %d, RAC No: %d, From: %s, To: %s, DOJ: %s, Coach: %d, Seat: %d, Cost: %d\n",
                plannings[i].pnr,
                plannings[i].name[0],
                plannings[i].age[0],
                plannings[i].gender[0],
                plannings[i].train_number,
                plannings[i].rac_no[0],
                plannings[i].from,
                plannings[i].to,
                plannings[i].doj,
                plannings[i].coach,
                plannings[i].seat[0],
                plannings[i].cost);
    }

    fclose(file);
    printf("Data written to %s successfully.\n", filename);
}


int cancelBookingByPNR(struct Planning *bookings, int *num_Confirm, int *num_RAC, char *pnr, const char* filename, const char *username) {
    int num_bookings = *num_Confirm + *num_RAC;

    for (int i = 0; i < num_bookings; i++) {
        if (strcmp(bookings[i].pnr, pnr) == 0) {
            struct Planning cancelled_booking = bookings[i];
            int a=*num_Confirm-1;
            if(i<*num_Confirm) {
                for (int j = i + 1; j < *num_Confirm; j++) {
                    bookings[j - 1] = bookings[j];
                }
            }
            else {
                for (int j = i + 1; j < num_bookings; j++) {
                    bookings[j - 1] = bookings[j];
                }
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


            struct seats *s = NULL;
            for (int j = 0; j < train_count; j++) {
                if (all_trains[j].trainno == cancelled_booking.train_number) {
                    s = &all_trains[j];
                    break;
                }
            }

            int year = s->year;
            int month = s->month;
            int day = s->day;
            struct tm journeyDate = {0};
            journeyDate.tm_year = year - 1900;
            journeyDate.tm_mon = month - 1;
            journeyDate.tm_mday = day;
            mktime(&journeyDate);

            int reFund=calculateRefund(journeyDate , cancelled_booking.cost);

            int cou=0;
            if(i<*num_Confirm) {
                for(int j=0;j<10;j++) {
                    if(cancelled_booking.name[j][0]!='\0') {
                        s->s1[cancelled_booking.coach-1][cancelled_booking.seat[j]-1]=cancelled_booking.seat[j];
                        s->gS[cancelled_booking.coach-1][cancelled_booking.seat[j]-1]=0;
                        cou++;
                    }
                }
            }
            else {
                if(s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]==MM) {
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=MRAC;
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=MALE;
                }
                else if(s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]==FF) {
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=FRAC;
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=FEMALE;
                }
                else if(s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]==MF) {
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=FRAC;
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=FEMALE;
                }
                else if(s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]==FM) {
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=MRAC;
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=MALE;
                }
                else if(s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]==MRAC) {
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=cancelled_booking.seat[0];
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=0;
                }
                else if(s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]==FRAC) {
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=cancelled_booking.seat[0];
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[0]-1]=0;
                }
                cou++;
            }
            if (i < *num_Confirm && (*num_RAC)==0) {
                update_train_details(cancelled_booking.train_number, s->s1, s->gS);
                (*num_Confirm)--;
            }
            else if(i < *num_Confirm && (*num_RAC)!=0){
                int cos=cancelled_booking.cost/cou;
                int hash[*num_RAC];
                memset(hash, 0, sizeof(int) * (*num_RAC));

                int rac_con=0;
                for(int j=0;j<*num_RAC && rac_con<cou;j++) {
                    if((bookings[(*num_Confirm)+j].coach<12 && cancelled_booking.coach<12) ||
                        (bookings[(*num_Confirm)+j].coach>=12 && bookings[(*num_Confirm)+j].coach<16 && cancelled_booking.coach>=12 && cancelled_booking.coach<16) ||
                        (bookings[(*num_Confirm)+j].coach>=16 && bookings[(*num_Confirm)+j].coach<19 && cancelled_booking.coach>=16 && cancelled_booking.coach<19) ||
                        (bookings[(*num_Confirm)+j].coach==19 && cancelled_booking.coach==19)) {
                        if(s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]==MM) {
                            s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=MRAC;
                            s->gS[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=MALE;
                        }
                        else if(s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]==FF) {
                            s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=FRAC;
                            s->gS[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=FEMALE;
                        }
                        else if(s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]==MF) {
                            s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=FRAC;
                            s->gS[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=FEMALE;
                        }
                        else if(s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]==FM) {
                            s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=MRAC;
                            s->gS[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=MALE;
                        }
                        else if(s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]==MRAC) {
                            s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=bookings[(*num_Confirm)+j].seat[0];
                            s->gS[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=0;
                        }
                        else if(s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]==FRAC) {
                            s->s1[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=bookings[(*num_Confirm)+j].seat[0];
                            s->gS[bookings[(*num_Confirm)+j].coach-1][bookings[(*num_Confirm)+j].seat[0]-1]=0;
                        }
                        hash[j]=1;
                        rac_con++;
                    }
                }

                int end=*num_Confirm;
                int start=*num_Confirm;
                while(end<*num_RAC+*num_Confirm) {
                    if(hash[end-*num_Confirm]==0) {
                        end++;
                    }else {
                        struct Planning temp=bookings[end];
                        for(int k=end;k>start;k--) {
                            bookings[k]=bookings[k-1];
                        }
                        bookings[start]=temp;
                        start++;
                        end++;
                    }
                }

                for(int j=0;j<rac_con;j++) {
                    printf("hi");
                    s->s1[cancelled_booking.coach-1][cancelled_booking.seat[j]-1]=BOOKED;
                    s->gS[cancelled_booking.coach-1][cancelled_booking.seat[j]-1]=(strcmp(bookings[*num_Confirm+j].gender[0],"Male")==0) ? MALE : FEMALE;
                }

                (*num_Confirm)+=rac_con;
                (*num_RAC)-=rac_con;

                for(int j=*num_Confirm-rac_con;j<*num_Confirm;j++) {
                    bookings[j].seat[0]=cancelled_booking.seat[j-(*num_Confirm-rac_con)];
                    bookings[j].coach=cancelled_booking.coach;
                    bookings[j].cost=cos;
                    bookings[j].rac_no[0]=0;
                }
                num_bookings=*num_Confirm + *num_RAC;
                for(int j=(*num_Confirm);j<num_bookings;j++) {
                    bookings[j].rac_no[0]=j-(*num_Confirm)+1;
                }
                update_train_details(cancelled_booking.train_number, s->s1, s->gS);
                writePlanningDataRAC(filename,bookings,*num_Confirm,*num_RAC);
            }
            else {
                (*num_RAC)--;
                update_train_details(cancelled_booking.train_number, s->s1, s->gS);
                writePlanningDataRAC(filename,bookings,*num_Confirm,*num_RAC);
            }

            printf("Refund amnt %d", reFund);
            updateWalletAmountAndRemovePNR(username, reFund, cancelled_booking.pnr);
            writePlanningDataConfirm("book.txt", bookings, *num_Confirm, a);
            return 1;
        }
    }
    return 0;
}

void ticketcancel() {
    struct Planning bookings[MAX_RECORDS];

    int num_Confirm = readBookingDataConfirm("book.txt", bookings);
    char username[MAX_STRING_LENGTH];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    trimNewline(username);
    char pnr[20];
    printf("Enter the PNR number to cancel the booking: ");
    scanf("%s", pnr);

    char train_no_str[3];
    strncpy(train_no_str, pnr, 2);
    train_no_str[2] = '\0'; // Null terminate the string
    int train_no;
    train_no = atoi(train_no_str);
    char filename[50];
    sprintf(filename, "rac_%d.txt",  train_no);
    int num_RAC = readBookingDataRAC(filename, bookings, num_Confirm);

    if (cancelBookingByPNR(bookings, &num_Confirm, &num_RAC, pnr, filename, username)) {
        printf("Booking cancelled successfully!");
    }
    else {
        printf("No booking found with the given PNR number.");
    }
}

