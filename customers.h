#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 50

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct Purchase {
    int itemSerial;
    float priceAtPurchase;
    Date purchaseDate;
    struct Purchase* next;
} Purchase;

typedef struct Customer {
    char fullName[MAX_STR];
    char id[20];
    Date joinDate;
    float totalSpent;
    Purchase* purchaseHistory; 
    struct Customer* next;     
} Customer;

Customer* createCustomer(char* name, char* id, Date joinDate);
Customer* addCustomer(Customer* head, Customer* newCust);
Customer* findCustomer(Customer* head, char* id);
int buyItem(Customer* cust, ItemNode* itemRoot, int itemSerial);
int returnItem(Customer* cust, ItemNode* itemRoot, int itemSerial);
void saveCustomersToTextFile(Customer* head, const char* filename);
Customer* loadCustomersFromTextFile(const char* filename);
ItemNode* searchItem(ItemNode* root, int serialNumber);

#endif