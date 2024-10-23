/* This code is for the keypad firmware for the Hampod Program
* Written by Brendan Perez
* Last modified on 10/23/2023
*/
#ifndef KEYPAD_CODE
#define KEYPAD_CODE

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "hampod_queue.h"
#include "hampod_firm_packet.h"

#define KEYPAD_O "Keypad_o"
#define KEYPAD_I "Keypad_i"
#define R1 0 //Pin GPIOA0
#define R2 10 //Pin GPIOC3
#define R3 6 //Pin GPIOA1
#define R4 1 //Pin GPIOA6
#define C1 14 //Pin GPIOC2
#define C2 13 //Pin GPIOC1
#define C3 12 //Pin GPIOC0
#define C4 3 //Pin GPIOA3

#define KEYPAD_THREAD_COLOR "\033[0;97mKeypad - Main: "
#define KEYPAD_IO_THREAD_COLOR "\033[0;96mKeypad - IO: "

#ifdef DEBUG
#define KEYPAD_PRINTF(...) \
    do { \
        if(DEBUG) { \
            printf(KEYPAD_THREAD_COLOR); \
            printf(__VA_ARGS__); \
        } \
    } while(0)

#define KEYPAD_IO_PRINTF(...) \
    do { \
        if(DEBUG) { \
            printf(KEYPAD_IO_THREAD_COLOR); \
            printf(__VA_ARGS__); \
        } \
    } while(0)
#else

#define KEYPAD_PRINTF(...) \
    while(0)

#define KEYPAD_IO_PRINTF(...) \
    while(0)

#endif

typedef struct keypad_io_packet {
    int pipe_fd;
    Packet_queue* queue;
} keypad_io_packet;

void keypadTurnon();
void *keypad_io_thread(void* arg);
int readNumPad();
void keypad_process();



#ifndef SHAREDLIB
#include "keypad_firmware.c"
#endif

#endif
