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

typedef struct Item {
    long serialNumber;
    char brand[MAX_STR];
    char type[MAX_STR]; 
    float price;
    int isOutlet;       
    int stock;
    Date arrivalDate;   
    struct Item* left;
    struct Item* right;
} Item;

typedef struct Purchase {
    long itemSerial;
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
int buyItem(Customer* cust, Item* itemRoot, long itemSerial);
int returnItem(Customer* cust, Item* itemRoot, long itemSerial);
void saveCustomersToTextFile(Customer* head, const char* filename);
Customer* loadCustomersFromTextFile(const char* filename);
Item* findItem(Item* root, long serialNumber);

#endif