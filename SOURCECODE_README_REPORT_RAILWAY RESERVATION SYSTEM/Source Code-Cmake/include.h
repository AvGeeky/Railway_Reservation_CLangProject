//
// Created by ASUS on 21-06-2024.
//

#ifndef INCLUDE_H
#define INCLUDE_H

void to_lowercase(char *str);
int is_in_list(const char *word, const char *list[], int list_size);
void count_word_occurrences(const char *word, const char *positive_words[], int positive_size,
                            const char *negative_words[], int negative_size,
                            int *positive_counts, int *negative_counts);
void initializeSeats(struct seats *s);
int getSeatLimit(int coach);
int unbookSeat(int trainno, int coach, int seat);
void writeTrainDetailsToCSV(struct seats *s, const char *route, const char *destination, const char *trainName,char quota[]);
void initializeSeats(struct seats *s);
int getSeatLimit(int coach);
int unbookSeat(int trainno, int coach, int seat);
void writeTrainDetailsToCSV(struct seats *s, const char *route, const char *destination, const char *trainName,char quota[]);
void start();

#endif //INCLUDE_H
