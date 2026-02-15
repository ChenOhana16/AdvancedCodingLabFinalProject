#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "items.h"
#include "items.c"
#include "customers.h"
#include "customers.c"
#include "employees.h"
#include "employees.c"

void showMainMenu(Role r) {

    printf("\n=========== STORE SYSTEM ===========\n");

    if (r == ROLE_ADMIN) {
        printf("1. Add Item\n");
        printf("2. Show Items\n");
        printf("3. Add Customer\n");
        printf("4. Show Customers\n");
        printf("5. Buy Item\n");
        printf("6. Return Item\n");
        printf("7. Add Employee\n");
        printf("8. Save All\n");
        printf("9. Check Out Of Stock\n");
        printf("0. Exit\n");
    }
    else if (r == ROLE_WORKER) {
        printf("2. Show Items\n");
        printf("3. Add Customer\n");
        printf("4. Show Customers\n");
        printf("5. Buy Item\n");
        printf("6. Return Item\n");
        printf("8. Save All\n");
        printf("0. Exit\n");
    }
    else {
        printf("2. Show Items\n");
        printf("4. Show Customers\n");
        printf("0. Exit\n");
    }

    printf("Choose: ");
}

void printCustomers(Customer* head) {
    while (head) {
        printf("Name: %s | ID: %s | Total Spent: %.2f\n",
            head->fullName,
            head->id,
            head->totalSpent);
        head = head->next;
    }
}

int main() {

    /* ===== Employees Login ===== */

    EmployeeSystem empSys;
    int loggedIndex = -1;

    initSystem(&empSys);

    if (!login(&empSys, &loggedIndex)) {
        printf("System locked.\n");
        free(empSys.arr);
        return 0;
    }

    Role userRole = empSys.arr[loggedIndex].role;

    /* ===== Load Store Data ===== */

    ItemNode* itemRoot = loadItemsFromFile("items.dat");
    Customer* customerList = loadCustomersFromTextFile("customers.txt");

    int choice;

    do {

        showMainMenu(userRole);
        scanf("%d", &choice);

        switch (choice) {

        case 1:
            if (userRole == ROLE_ADMIN) {
                Item newItem = createItemFromUser();
                itemRoot = insertItem(itemRoot, newItem);
            }
            else printf("No permission.\n");
            break;

        case 2:
            printInorder(itemRoot);
            break;

        case 3:
            if (userRole != ROLE_TRAINEE) {
                char name[50], id[20];
                Date today = getCurrentDate();

                printf("Full Name: ");
                scanf("%s", name);

                printf("ID: ");
                scanf("%s", id);

                Customer* newCust = createCustomer(name, id, today);
                customerList = addCustomer(customerList, newCust);
            }
            else printf("No permission.\n");
            break;

        case 4:
            printCustomers(customerList);
            break;

        case 5:
            if (userRole != ROLE_TRAINEE) {
                char id[20];
                long serial;

                printf("Customer ID: ");
                scanf("%s", id);

                Customer* cust = findCustomer(customerList, id);
                if (!cust) break;

                printf("Item Serial: ");
                scanf("%ld", &serial);

                buyItem(cust, itemRoot, serial);
            }
            else printf("No permission.\n");
            break;

        case 6:
            if (userRole != ROLE_TRAINEE) {
                char id[20];
                long serial;

                printf("Customer ID: ");
                scanf("%s", id);

                Customer* cust = findCustomer(customerList, id);
                if (!cust) break;

                printf("Item Serial to return: ");
                scanf("%ld", &serial);

                returnItem(cust, itemRoot, serial);
            }
            else printf("No permission.\n");
            break;

        case 7:
            if (userRole == ROLE_ADMIN)
                addEmployee(&empSys);
            else
                printf("No permission.\n");
            break;

        case 8:
            saveItemsToFile(itemRoot, "items.dat");
            saveCustomersToTextFile(customerList, "customers.txt");
            saveEmployees(&empSys);
            printf("All data saved.\n");
            break;

        case 9:
            checkOutOfStock(itemRoot);
            break;

        case 0:
            printf("Saving and exiting...\n");
            saveItemsToFile(itemRoot, "items.dat");
            saveCustomersToTextFile(customerList, "customers.txt");
            saveEmployees(&empSys);
            break;

        default:
            printf("Invalid choice\n");
        }

    } while (choice != 0);

    /* ===== Free Memory ===== */

    freeTree(itemRoot);
    free(empSys.arr);

    printf("System closed successfully.\n");
    return 0;
}
