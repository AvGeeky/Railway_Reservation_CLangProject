cmake_minimum_required(VERSION 3.27)
project(CFinal C)

set(CMAKE_C_STANDARD 11)

add_executable(CFinal new_main.c
        ticketcancel.h
        ticketcancel.c
        include.h
        include.c
        pnrenq.h
        pnrenq.c
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,54777216")