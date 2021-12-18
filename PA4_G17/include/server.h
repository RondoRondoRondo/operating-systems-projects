#ifndef SERVER_H
#define SERVER_H

#include "utils.h"

void printSyntax();

struct account
{
    char name[MAX_STR];
    char username[MAX_STR];
    time_t birthday;
    int acc_num;
    float balance;
};

struct account accounts[MAX_ACC];
sem_t semaphores[MAX_ACC];
sem_t sem_num;

#endif

