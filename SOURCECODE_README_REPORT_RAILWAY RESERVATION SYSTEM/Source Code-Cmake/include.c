#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TOTAL_COACHES 20
#define S_COACHES 12
#define AC3_COACHES 4
#define AC2_COACHES 3
#define AC1_COACHES 1
#define MAX_LINE_LENGTH 1024
#define SEATS_S 72
#define SEATS_AC3 64
#define SEATS_AC2 32
#define SEATS_AC1 16

#define BOOKED -1
#define MALE 100  // Greater than 72
#define FEMALE 200  // Greater than 72

struct seats {
    int s1[TOTAL_COACHES][SEATS_S];  // Keeping max possible seats for any coach type
    int gS[TOTAL_COACHES][SEATS_S];
    int year, month, day, trainno;
};

void to_lowercase(char *str) {
    for (; *str; ++str) *str = tolower(*str);
}

// Check if a word is in a given list
int is_in_list(const char *word, const char *list[], int list_size) {
    for (int i = 0; i < list_size; ++i) {
        if (strcmp(word, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Count the occurrences of words in the positive and negative lists
void count_word_occurrences(const char *word, const char *positive_words[], int positive_size,
                            const char *negative_words[], int negative_size,
                            int *positive_counts, int *negative_counts) {
    for (int i = 0; i < positive_size; ++i) {
        if (strcmp(word, positive_words[i]) == 0) {
            positive_counts[i]++;
        }
    }
    for (int i = 0; i < negative_size; ++i) {
        if (strcmp(word, negative_words[i]) == 0) {
            negative_counts[i]++;
        }
    }
}

// Function to add a new review to the file


void initializeSeats(struct seats *s) {
    // Initialize S coaches
    for (int i = 0; i < S_COACHES; i++) {
        for (int j = 0; j < SEATS_S; j++) {
            s->s1[i][j] = j + 1;
            s->gS[i][j] = 0;
        }
    }

    // Initialize AC3 coaches
    for (int i = S_COACHES; i < S_COACHES + AC3_COACHES; i++) {
        for (int j = 0; j < SEATS_AC3; j++) {
            s->s1[i][j] = j + 1;
            s->gS[i][j] = 0;
        }
    }

    // Initialize AC2 coaches
    for (int i = S_COACHES + AC3_COACHES; i < S_COACHES + AC3_COACHES + AC2_COACHES; i++) {
        for (int j = 0; j < SEATS_AC2; j++) {
            s->s1[i][j] = j + 1;
            s->gS[i][j] = 0;
        }
    }

    // Initialize AC1 coach
    for (int i = TOTAL_COACHES - 1; i < TOTAL_COACHES; i++) {
        for (int j = 0; j < SEATS_AC1; j++) {
            s->s1[i][j] = j + 1;
            s->gS[i][j] = 0;
        }
    }
}

// Function to get seat limit based on coach type
int getSeatLimit(int coach) {
    if (coach < S_COACHES) {
        return SEATS_S;
    } else if (coach < S_COACHES + AC3_COACHES) {
        return SEATS_AC3;
    } else if (coach < S_COACHES + AC3_COACHES + AC2_COACHES) {
        return SEATS_AC2;
    } else {
        return SEATS_AC1;
    }
}

// Function to unbook a seat
int unbookSeat(int trainno, int coach, int seat) {
    FILE *file = fopen("seatsf.bin", "rb+");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    struct seats s;
    while (fread(&s, sizeof(struct seats), 1, file) == 1) {
        if (s.trainno == trainno) {
            if (coach < 0 || coach >= TOTAL_COACHES) {
                printf("Invalid coach number.\n");
                fclose(file);
                return 1;
            }

            int seat_limit = getSeatLimit(coach);

            if (seat < 1 || seat > seat_limit) {
                printf("Invalid seat number.\n");
                fclose(file);
                return 1;
            }

            // Adjust seat index
            int seat_index = seat - 1;

            if (s.s1[coach][seat_index] == BOOKED || s.gS[coach][seat_index] == MALE || s.gS[coach][seat_index] == FEMALE) {
                s.s1[coach][seat_index] = seat;
                s.gS[coach][seat_index] = 0;

                // Move the file pointer to the correct position to update the record
                fseek(file, -sizeof(struct seats), SEEK_CUR);
                fwrite(&s, sizeof(struct seats), 1, file);
                fclose(file);
                printf("Seat unbooked successfully.\n");
                return 0;
            } else {
                printf("Seat is not booked.\n");
                fclose(file);
                return 1;
            }
        }
    }

    printf("Train not found.\n");
    fclose(file);
    return 1;
}

// Function to write train details to CSV
void writeTrainDetailsToCSV(struct seats *s, const char *route, const char *destination, const char *trainName,char quota[]) {
    FILE *csvFile = fopen("trains.csv", "a");
    if (!csvFile) {
        perror("Failed to open CSV file");
        return;
    }

    //fprintf(csvFile, "Route,Destination,Date of Journey,Train Name,TrainNumber,AC1,AC2,AC3,Sleeper,Quota\n");
    fprintf(csvFile, "%s,%s,%d-%02d-%02d,%s,%d,%d,%d,%d,%d,%s\n",
            route,
            destination,
            s->year, s->month, s->day,
            trainName,
            s->trainno,
            AC1_COACHES * SEATS_AC1,
            AC2_COACHES * SEATS_AC2,
            AC3_COACHES * SEATS_AC3,
            S_COACHES * SEATS_S,quota);

    fclose(csvFile);
}

int start() {
    int ch1;
    printf("Enter (1) to add a train\nEnter (2) to read the sentiment analysis of feedback.\n Enter (3) for Clearing Seats\n");
    scanf("%d", &ch1);
    if (ch1 == 1) {
        int n;
        printf("Enter the number of trains: ");
        scanf("%d", &n);

        FILE *file = fopen("seatsf.bin", "ab");
        if (!file) {
            perror("Failed to open file");
            return 1;
        }

        for (int i = 0; i < n; i++) {
            struct seats s;
            printf("Enter the journey date (YYYY MM DD): ");
            scanf("%d %d %d", &s.year, &s.month, &s.day);
            printf("Enter [Train Number]: ");
            scanf("%d", &s.trainno);

            initializeSeats(&s);
            int ch=0;
            printf("Do you want to populate a coach for demonstration?(YES=1/NO=0) ");
            scanf("%d",&ch);
            if (ch==1){
                for (int j = 0; j < SEATS_AC3-10; j++) {
                    s.s1[12][j] = BOOKED;
                    s.gS[12][j] = FEMALE;
                }
                for (int j = 0; j < SEATS_AC3-10; j++) {
                    s.s1[13][j] = BOOKED;
                    s.gS[13][j] = MALE;
                }
                for (int j = 0; j < SEATS_AC3-10; j++) {
                    s.s1[14][j] = BOOKED;
                    s.gS[14][j] = FEMALE;
                }
                for (int j = 0; j < SEATS_AC3-12; j++) {
                    s.s1[15][j] = BOOKED;
                    s.gS[15][j] = MALE;
                }
            }

            fwrite(&s, sizeof(struct seats), 1, file);

            // Adding details to CSV file
            char route[50], destination[50], trainName[50],quota[10];
            printf("Enter the route (e.g., 1CityA-2CityB): ");
            scanf("%s", route);
            printf("Enter the destination: ");
            scanf("%s", destination);
            printf("Enter the [Train Name-Time of departure](No Space) ");
            scanf("%s", trainName);
            strcpy(quota,"night");
            writeTrainDetailsToCSV(&s, route, destination, trainName,quota);
        }

        fclose(file);
    }
    if (ch1==2){
        const char *positive_words[] = {
                "easy", "convenient", "fast", "quick", "smooth", "efficient", "reliable", "user-friendly",
                "intuitive", "helpful", "accurate", "secure", "clear", "responsive", "excellent", "great",
                "amazing", "fantastic", "perfect", "satisfied", "happy", "impressed", "love", "best", "awesome",
                "brilliant", "outstanding", "superb", "flawless", "seamless", "accessible"
        };

        const char *negative_words[] = {
                "difficult", "slow", "complicated", "unreliable", "confusing", "frustrating", "inaccurate",
                "unsecure", "buggy", "crash", "error", "poor", "terrible", "bad", "worst", "horrible", "disappointed",
                "annoying", "useless", "awful", "glitchy", "laggy", "problematic", "inefficient", "unfriendly",
                "unintuitive", "irresponsive"
        };

        int positive_size = sizeof(positive_words) / sizeof(positive_words[0]);
        int negative_size = sizeof(negative_words) / sizeof(negative_words[0]);
        int positive_counts[positive_size];
        int negative_counts[negative_size];

        // Initialize word occurrence counters
        memset(positive_counts, 0, sizeof(positive_counts));
        memset(negative_counts, 0, sizeof(negative_counts));

        int positive_count, negative_count;
        int total_positive_reviews = 0, total_negative_reviews = 0, total_reviews = 0;
        int total_stars = 0;

        // Open the reviews file
        FILE *file = fopen("reviews.txt", "r");
        if (!file) {
            perror("Could not open file");
            return EXIT_FAILURE;
        }

        char line[MAX_LINE_LENGTH];
        char review[MAX_LINE_LENGTH * 10] = "";  // Assuming a review can span multiple lines
        int is_new_review = 1;

        // Read the file line by line
        while (fgets(line, sizeof(line), file)) {
            // Check if the line is empty (indicating the end of a review)
            if (strcmp(line, "\n") == 0) {
                if (is_new_review == 0) {
                    // Process the review
                    positive_count = 0;
                    negative_count = 0;
                    total_reviews++;

                    // Extract the star rating
                    char *review_text = strchr(review, ' ') + 1;
                    int stars = atoi(strtok(review, " "));
                    total_stars += stars;

                    // Tokenize the review text
                    char *token = strtok(review_text, " .,!?;\n");
                    while (token) {
                        // Convert the token to lowercase
                        to_lowercase(token);

                        // Check if the token is in the positive or negative words list
                        if (is_in_list(token, positive_words, positive_size)) {
                            ++positive_count;
                        } else if (is_in_list(token, negative_words, negative_size)) {
                            ++negative_count;
                        }

                        // Count word occurrences for highlighting
                        count_word_occurrences(token, positive_words, positive_size, negative_words, negative_size, positive_counts, negative_counts);

                        token = strtok(NULL, " .,!?;\n");
                    }


                    if (positive_count > negative_count) {
                        //  printf("Positive: %s\n", review_text);
                        total_positive_reviews++;
                    } else if (negative_count > positive_count) {
                        //printf("Negative: %s\n", review_text);
                        total_negative_reviews++;
                    } else {
                        //printf("Neutral: %s\n", review_text);
                    }

                    // Reset the review buffer
                    review[0] = '\0';
                }
                is_new_review = 1;
            } else {
                // Append line to the current review
                strcat(review, line);
                is_new_review = 0;
            }
        }

        // Process the last review if the file doesn't end with an empty line
        if (is_new_review == 0) {
            positive_count = 0;
            negative_count = 0;
            total_reviews++;

            char *review_text = strchr(review, ' ') + 1;
            int stars = atoi(strtok(review, " "));
            total_stars += stars;

            char *token = strtok(review_text, " .,!?;\n");
            while (token) {
                to_lowercase(token);

                if (is_in_list(token, positive_words, positive_size)) {
                    ++positive_count;
                } else if (is_in_list(token, negative_words, negative_size)) {
                    ++negative_count;
                }

                count_word_occurrences(token, positive_words, positive_size, negative_words, negative_size, positive_counts, negative_counts);

                token = strtok(NULL, " .,!?;\n");
            }

            if (positive_count > negative_count) {
                printf("Positive: %s\n", review_text);
                total_positive_reviews++;
            } else if (negative_count > positive_count) {
                printf("Negative: %s\n", review_text);
                total_negative_reviews++;
            } else {
                printf("Neutral: %s\n", review_text);
            }
        }

        fclose(file);

        // Calculate and print the total positive and negative reviews as a percentage
        float positive_percentage = (float)total_positive_reviews / total_reviews * 100;
        float negative_percentage = (float)total_negative_reviews / total_reviews * 100;
        float average_stars = (float)total_stars / total_reviews;

        printf("\nTotal positive reviews: %.2f%%\n", positive_percentage);
        printf("Total negative reviews: %.2f%%\n", negative_percentage);
        float neut=100-(positive_percentage+negative_percentage);
        printf("Total neutral reviews: %.2f%%\n", neut);
        printf("Average star rating: %.2f\n", average_stars);

        // Highlight the most used positive and negative words
        printf("\nMost used positive words:\n");
        for (int i = 0; i < positive_size; ++i) {
            if (positive_counts[i] > 0) {
                printf("%s: %d\n", positive_words[i], positive_counts[i]);
            }
        }

        printf("\nMost used negative words:\n");
        for (int i = 0; i < negative_size; ++i) {
            if (negative_counts[i] > 0) {
                printf("%s: %d\n", negative_words[i], negative_counts[i]);
            }
        }
    }


    if (ch1 == 3) {
        int trainno, coach, seat;
        printf("Enter the train number to unbook seat: ");
        scanf("%d", &trainno);
        printf("Enter the coach number to unbook seat: ");
        scanf("%d", &coach);
        printf("Enter the seat number to unbook: ");
        scanf("%d", &seat);

        unbookSeat(trainno, coach - 1, seat);  // Adjusting user input to zero-based index for coach
    }

    return 0;
}

