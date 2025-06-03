# __Railway Reservation System__
***
## __Introduction__

The Railway Reservation System project in C is designed to manage the various aspects of railway ticket booking, cancellation, and inquiry. This system automates the entire ticketing process, providing users with a seamless and efficient way to book tickets, check PNR status, and cancel bookings. It is implemented in the C programming language, leveraging its efficiency and control over system resources.
This software has been developed by
[Saipranav](mailto:saipranav2310324@ssn.edu.in),
[Shailesh](mailto:shailesh2310592@ssn.edu.in) & 
[Rahul](mailto:rahul2310329@ssn.edu.in).

## __Objectives__

The primary objectives of the Railway Reservation System are to:

1. Simplify the process of booking railway tickets.
2. Provide a user-friendly interface for ticket cancellations and refunds.
3. Enable passengers to check the status of their booked tickets using their PNR number.
4. Ensure data integrity and security in the management of ticket information.
5. Offer a robust backend system to handle various railway reservation operations efficiently.
6. Enables the passengers to easily cancel tickets and get refund in their respective wallets

## __Features__

The Railway Reservation System includes the following key features:

1. **Ticket Booking**: Allows users to search for trains, check availability, and book tickets.

2. **Ticket Cancellation**: Provides a straightforward process for canceling tickets and obtaining refunds.

3. **PNR Status Inquiry**: Enables users to check the status of their booked tickets using their PNR number.

4. **User Authentication**: Ensures that only authorized users can access and modify ticket information.

5. **Database Management**: Maintains an admin database of train schedules, bookings, cancellations, and refunds.

6. **Women Safety**: Ensures safety for women passengers by allocating seat next to a women only unless they choose family option.

## __Libraries and header files__
> __stdio.h__
+ The stdio.h header file allows us to perform input and output operations in C. The functions like printf() and scanf() are used to display output and take input from a user, respectively. This library allows us to communicate with users easily.
- The stdio.h library uses streams to communicate with physical devices. A stream is a communication channel between a program and the physical input and output devices. For example, a standard input (stdin) is a stream used to read input data from a keyboard.

> __stdlib.h__
+ One of the essential header files in C is stdlib.h. It provides several functions for performing various tasks, including memory allocation, string manipulation, mathematical operations, and more. In this article, we will explore the stdlib.h library in C for advanced programming, specifically focusing on its functionalities beyond the basics and how it can enhance your coding skills.
- The stdlib.h library in C contains several functions that offer advanced capabilities for handling memory, strings, numbers, and file I/O operations

> __time.h__ 
+  The time.h header file in C contains functions for obtaining and manipulating date and time information. It introduces three time-related data types:

+ __1.clock_t:__
Represents the processor time used by the program as an integer.
It is a part of the calendar time, measuring the CPU time taken for the execution of a program.
+ __2.time_t:__
Represents the calendar time as an integer.
It is a part of the calendar time and is commonly used to store time values, often representing the number of seconds elapsed since a specific epoch.
+ __3.struct tm:__
Holds the date and time information in a structured format.
Contains members such as seconds, minutes, hours, day of the month, month, year, and more.
Provides a convenient way to access and manipulate individual components of a date and time.

> __string.h__
+ string.h is the header in the C standard library for the C programming language which contains macro definitions, constants and declarations of functions and types used not only for string handling but also various memory handling functions; the name is thus something of a misnomer.
- Functions declared in string.h are extremely popular, since as a part of the C standard library, they are guaranteed to work on any platform which supports C. However, some security issues exist with these functions, such as buffer overflows, leading programmers to prefer safer, possibly less portable variants. Also, the string functions only work with characters encodings made of bytes, such as ASCII and UTF-8. In historical documentation the term "character" was often used instead of "byte", which if followed literally would mean that multi-byte encodings such as UTF-8 were not supported. The BSD documentation has been fixed to make this clear, but POSIX, Linux, and Windows documentation still uses "character" in many places. Functions to handle character encodings made up of larger code units than bytes, such as UTF-16 is generally achieved through wchar.h.

> __ctype.h__
+ As string.h header file contains inbuilt functions to handle Strings in C/C++, the ctype.h/<cctype> contains inbuilt functions to handle characters in C/C++ respectively.
Characters are of two types:
+ Printable Characters: The characters that are displayed on the terminal.

+ Control Characters: The characters that are initiated to perform a specific operation.

+ The arguments passed to character functions should be of integer type. If we pass characters instead of an integer, the characters are typecasted into integers(corresponding ASCII values) and those integers are passed as arguments.
The below functions under ctype.h/<cctype> header file are applied on normal characters. Wide character functions are used for the characters of type wchar_t.

> __include.h__
+  This header file, named include.h, declares various functions and a structure relevant to the management of train seats and word occurrence counting. Here's a brief description of each function and the context in which they might be used:

>> __String and List Operations:__

+ void to_lowercase(char *str);: Converts the given string to lowercase.
int is_in_list(const char *word, const char *list[], int list_size);: Checks if a given word is present in a list of words.
Word Occurrence Counting:

+ void count_word_occurrences(const char *word, const char *positive_words[], int positive_size, const char *negative_words[], int negative_size, int *positive_counts, int *negative_counts);: Counts the occurrences of a word in two lists (positive and negative words) and updates the respective counts.

>> __Train Seat Management:__

+ void initializeSeats(struct seats *s);: Initializes the seat structure.
int getSeatLimit(int coach);: Returns the seat limit for a given coach.
int unbookSeat(int trainno, int coach, int seat);: Unbooks a seat given the train number, coach, and seat number.
+ void writeTrainDetailsToCSV(struct seats *s, const char *route, const char *destination, const char *trainName, char quota[]);: Writes train details to a CSV file.

>> __Program Control:__

+ void start();: Likely the main entry point to start the application or a specific process.

> __ticketcancel.h__
+ This header file, ticketcancel.h, is designed for a train reservation and cancellation system. It defines various constants, structures, and function prototypes used in the system. Here's a brief description of each part:

>> __Constants__
+ These constants are used throughout the system for various purposes, including cost calculations, array sizes, and seat configurations:

>> __Service Charges and Time Conversions:__

SERVICE_CHARGE: Fixed charge for cancellation.
HOUR_TO_SECONDS, DAY_TO_SECONDS: Time conversion constants.

>> __PNR and Pricing Constants:__

MAX_PNRS: Maximum number of PNRs.
MRAC, FRAC, MM, MF, FM, FF: Codes for RAC and gender combinations.
MAX_LINE_LENGTH: Maximum length of a line in input files.
MAX_USERNAME_LENGTH, MAX_PASSWORD_LENGTH: Maximum lengths for username and password.
PNR_LENGTH: Length of the PNR number.
MAX_TRAINS: Maximum number of trains.
SLEEPER_BASE_COST, AC1_BASE_COST, AC2_BASE_COST, AC3_BASE_COST, GENERAL_BASE_COST: Base costs for different types of seats.
MAX_STRING_LENGTH: Maximum length for various strings.
TOTAL_COACHES: Total number of coaches.

>> __Coach and Seat Configuration:__

S_COACHES, AC3_COACHES, AC2_COACHES, AC1_COACHES: Number of coaches for each type.
SEATS_S, SEATS_AC3, SEATS_AC2, SEATS_AC1, SEATS_MIXED: Number of seats in each coach type.
ROWS_S, ROWS_AC3, ROWS_AC2, ROWS_AC1, ROWS_MIXED: Number of rows in each coach type.
COLUMNS_LEFT_S, COLUMNS_RIGHT_S, COLUMNS_LEFT_AC3, COLUMNS_RIGHT_AC3, COLUMNS_AC2, COLUMNS_AC1, COLUMNS_LEFT_MIXED, COLUMNS_RIGHT_MIXED: Number of columns in each coach type.
BOOKED: Constant representing a booked seat.
MALE, FEMALE: Gender constants.
MAX_RECORDS: Maximum number of records.

>> __Structures__

+ struct Planning: Contains details of a booking:

Fields for source and destination (from, to), date of journey (doj), train number (train_number), cost (cost), passenger details (name, age, gender), PNR number (pnr), seat numbers (seat), coach number (coach), and RAC numbers (rac_no).

+ struct seats: Manages seat allocation:

Arrays for seats in different coaches (s1, gS), and fields for date and train number (year, month, day, trainno).
Function Prototypes
These functions provide the core functionality for the train reservation and cancellation system:

>> __String and PNR Handling:__

+ void trimNewline(char *str);: Trims newline characters from strings.

+ void removePNR(char *pnrList, const char *pnr);: Removes a PNR from the list.
+ void updateWalletAmountAndRemovePNR(const char *username, int newWalletAmount, const char *pnrToRemove);: Updates wallet amount and removes a PNR.

>> __Refund Calculation and Seat Management:__

+ int calculateRefund(struct tm journey_date, int ticket_cost);: Calculates the refund based on the journey date and ticket cost.
+ void copy_2d_array(int src[TOTAL_COACHES][SEATS_S], int dest[TOTAL_COACHES][SEATS_S], int rows, int cols);: Copies a 2D array.
+ void update_train_details(int trainno, int seats[TOTAL_COACHES][SEATS_S], int genderSeats[TOTAL_COACHES][SEATS_S]);: Updates train seat details.

>> __Booking Data Handling:__

+ void displayPlanningData(struct Planning *bookings, int num_Confirm, int num_RAC);: Displays planning data.

+ int readBookingDataConfirm(const char* filename, struct Planning* planningArray);: Reads confirmed booking data from a file.

+ int readBookingDataRAC(const char* filename, struct Planning* bookings, int start_index);: Reads RAC booking data from a file.

+ void writePlanningDataConfirm(const char *filename, struct Planning *plan, int con, int a);: Writes confirmed booking data to a file.

+ void writePlanningDataRAC(const char *filename, struct Planning *plannings, int startIndex, int count);: Writes RAC booking data to a file.

>> __Booking Cancellation:__

+ int cancelBookingByPNR(struct Planning *bookings, int *num_Confirm, int *num_RAC, char *pnr, const char* filename, const char *username);: Cancels a booking by PNR.

+ void ticketcancel();: Main function to handle ticket cancellation.


## __Modules__

The system is divided into several modules, each responsible for specific functionalities:

1. **Login module**: Authenticates the user to login using his/her username and password.
2. **Booking Module**: Handles the booking of tickets, including seat selection and fare calculation.
3. **Cancellation Module**: Manages the cancellation of tickets, processing refunds, and updating the database.
4. **PNR Status Module**: Provides real-time status of booked tickets using the PNR number.
5. **User Interface Module**: Facilitates user interactions with the system through a command-line interface.

## __Technologies Used__

- **Programming Language**: C
- **Data Structures**: Arrays, Structures
- **File Handling**: Used for storing and retrieving booking and cancellation data 
- **File Type**: csv and bin files
- **Standard Libraries**: `stdio.h`, `stdlib.h`, `string.h`, `time.h`

## __Pre Requisites__

To compile and run the Railway Reservation System project, ensure you have the following prerequisites:

### __Software Requirements__
1. **C Compiler**: A C compiler such as GCC (GNU Compiler Collection).
   - Installation on Linux:
     ```sh
     sudo apt-get install gcc
     ```
   - Installation on macOS:
     ```sh
     brew install gcc
     ```
   - Installation on Windows:
     - Download and install [MinGW](http://www.mingw.org/) or use an IDE like Code::Blocks which comes with GCC.

2. **Make Utility**: For building the project.
   - Installation on Linux:
     ```sh
     sudo apt-get install make
     ```
   - Installation on macOS:
     ```sh
     brew install make
     ```
   - Installation on Windows:
     - Included with MinGW or can be downloaded from [GnuWin](http://gnuwin32.sourceforge.net/packages/make.htm).

### __Development Environment__
- **Text Editor/IDE**: Any text editor or IDE for writing and editing C code, such as:
  - Visual Studio Code
  - Code::Blocks
  - Eclipse
  - Vim/Emacs

## __Additional Tools (Optional)__

- **Git**: For version control.
  - Installation on Linux:
    ```sh
    sudo apt-get install git
    ```
  - Installation on macOS:
    ```sh
    brew install git
    ```
  - Installation on Windows:
    - Download and install from [Git for Windows](https://git-scm.com/).


## __Compilation and Execution__


To set up the Railway Reservation System on your local machine:

1. **Download the Source Code**: Clone the repository (https://github.com/AvGeeky/railway_reservation_sys)
2. **Compile the Code**: Use a C compiler (such as GCC) to compile the source code.
   ```sh
   gcc -o railway_reservation_system main.c cancellationtkts.c pnr.c
   ```
3. **Run the Executable**: Execute the compiled program from the command line.
   ```sh
   ./railway_reservation_system
   ```

## __Usage__

1. **Launch the Program**: Run the executable to start the system.
2. **Navigate the Menu**: Use the menu options to book tickets, cancel tickets, or check PNR status.
3. **Follow Prompts**: Enter the required information as prompted by the system (e.g., PNR number, train details).

## __Implementation__

> ### __Data Organization and Rationale__

>> #### __Arrays__
- Arrays are used for storing multiple items of the same type in a single data structure. In this project, arrays can be used for managing lists of train schedules, available seats, and booked tickets.
  ```c
  int available_seats[100];

>> #### __Structures__
- Structures (struct) are used to group different types of data together. In this project, structures are used to represent a ticket with fields such as PNR number, passenger name, train number, and seat number.

```c
struct Ticket {
    int pnr;
    char passenger_name[100];
    int train_no;
    int seat_no;
};
```
>> #### __Array of Structures__
- An array of structures is used to manage a collection of related data items. For example, an array of Ticket structures is used to store all the tickets booked by passengers.

```c
struct Ticket tickets[100];
```
>> __Files__
- Files are used for persistent storage of data, such as storing the list of tickets, trains, and passenger details. This allows the data to be saved and retrieved even after the program is closed.

```c
FILE *file = fopen("tickets.dat", "rb+"); 
```

## __Conclusion__

The Railway Reservation System in C is a comprehensive project designed to automate the ticketing process for railway journeys. It simplifies booking, cancellation, and status inquiry operations, providing a user-friendly interface and ensuring efficient management of ticket information. This system serves as a valuable tool for both passengers and railway administration, streamlining operations and enhancing the travel experience.

## __License__ 
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (C) 2007 Free Software Foundation, Inc. https://fsf.org/

Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.

