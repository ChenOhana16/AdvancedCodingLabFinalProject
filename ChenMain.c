#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "items.c"
#include "items.h"
#include "customers.c"
#include "customers.h"

void showMainMenu() {
    printf("\n=========== STORE SYSTEM ===========\n");
    printf("1. Add Item\n");
    printf("2. Show Items\n");
    printf("3. Add Customer\n");
    printf("4. Show Customers\n");
    printf("5. Buy Item\n");
    printf("6. Return Item\n");
    printf("7. Save All\n");
    printf("8. Exit\n");
    printf("Choose: ");
}

void printCustomers(Customer* head) {
    while (head) {
        printf("Name: %s | ID: %s | Total Spent: %.2f\n",
               head->fullName, head->id, head->totalSpent);
        head = head->next;
    }
}

int main() {

    ItemNode* itemRoot = loadItemsFromFile("items.dat");
    Customer* customerList = loadCustomersFromTextFile("customers.txt");

    int choice;

    do {
        showMainMenu();
        scanf("%d", &choice);

        switch (choice) {

        case 1: {
            Item newItem = createItemFromUser();
            itemRoot = insertItem(itemRoot, newItem);
            break;
        }

        case 2:
            printInorder(itemRoot);
            break;

        case 3: {
            char name[50], id[20];
            Date today = getCurrentDate();

            printf("Full Name: ");
            scanf("%s", name);

            printf("ID: ");
            scanf("%s", id);

            Customer* newCust = createCustomer(name, id, today);
            customerList = addCustomer(customerList, newCust);
            break;
        }

        case 4:
            printCustomers(customerList);
            break;

        case 5: {
            char id[20];
            long serial;

            printf("Customer ID: ");
            scanf("%s", id);

            Customer* cust = findCustomer(customerList, id);
            if (!cust) break;

            printf("Item Serial: ");
            scanf("%ld", &serial);

            buyItem(cust, itemRoot, serial);
            break;
        }

        case 6: {
            char id[20];
            long serial;

            printf("Customer ID: ");
            scanf("%s", id);

            Customer* cust = findCustomer(customerList, id);
            if (!cust) break;

            printf("Item Serial to return: ");
            scanf("%ld", &serial);

            returnItem(cust, itemRoot, serial);
            break;
        }

        case 7:
            saveItemsToFile(itemRoot, "items.dat");
            saveCustomersToTextFile(customerList, "customers.txt");
            printf("All data saved.\n");
            break;

        case 8:
            printf("Saving and exiting...\n");
            saveItemsToFile(itemRoot, "items.dat");
            saveCustomersToTextFile(customerList, "customers.txt");
            break;

        default:
            printf("Invalid choice\n");
        }

    } while (choice != 8);

    freeTree(itemRoot);

    printf("System closed successfully.\n");
    return 0;
}
