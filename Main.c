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

void searchItemsRecursive(ItemNode* root);
int serialExistsInTree(ItemNode* root, int serialNumber);

int serialExistsInTree(ItemNode* root, int serialNumber) {
    if (root == NULL)
        return 0;

    if (root->data.serialNumber == serialNumber)
        return 1;

    if (serialNumber < root->data.serialNumber)
        return serialExistsInTree(root->left, serialNumber);

    return serialExistsInTree(root->right, serialNumber);
}

void showMainMenu(Role r) {

    printf("\n=========== STORE SYSTEM ===========\n");

    if (r == ROLE_ADMIN) {
        printf("1. Add Item\n");
        printf("2. Delete Item\n");
        printf("3. Add Customer\n");
        printf("4. Show Customers\n");
        printf("5. Buy Item\n");
        printf("6. Return Item\n");
        printf("7. Add Employee\n");
        printf("8. Save All\n");
        printf("9. Check Out Of Stock\n");
        printf("10. Show VIP Customers\n");
        printf("11. Search Items\n");
        printf("0. Exit\n");
    }
    else if (r == ROLE_WORKER) {
        printf("2. Show Items\n");
        printf("3. Add Customer\n");
        printf("4. Show Customers\n");
        printf("5. Buy Item\n");
        printf("6. Return Item\n");
        printf("8. Save All\n");
        printf("11. Search Items\n");
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
        printf("Name: %s | ID: %s | Total Spent: %.2f",
            head->fullName,
            head->id,
            head->totalSpent);

        if (isVIP(head))
            printf(" | VIP ");

        printf("\n");

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

                while (serialExistsInTree(itemRoot, newItem.serialNumber)) {
                    printf("Serial number %d already exists. Enter a different serial number: ", newItem.serialNumber);
                    scanf("%d", &newItem.serialNumber);
                }

                itemRoot = insertItem(itemRoot, newItem);
            }
            else printf("No permission.\n");
            break;

        case 2:
            if (userRole == ROLE_ADMIN)
                itemRoot = deleteItemFromUser(itemRoot);
            else
                printInorder(itemRoot);
            break;

        case 3:
            if (userRole != ROLE_TRAINEE) {
                char name[50], id[20];
                date today = getCurrentDate();

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
            if (userRole != ROLE_TRAINEE)
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
            if (userRole == ROLE_ADMIN)
                checkOutOfStock(itemRoot);
                
            else
                printf("No permission.\n");
            break;

        case 10:
            if (userRole == ROLE_ADMIN)
                printVIPCustomers(customerList);
            else
                printf("No permission.\n");

            break;

        case 11:

            printf("\n=== SEARCH MENU ===\n");
            printf("1. Brand + Name\n");
            printf("2. Price comparison\n");
            printf("3. On Sale\n");
            printf("4. Date comparison\n");
            printf("5. Show all items\n");

            int searchOption;

            scanf("%d", &searchOption);

            ItemNode* stack[100];
            int top = -1;
            ItemNode* current = itemRoot;

            char brand[50], name[50];
            float price;
            int boolValue;
            date d;
            int mode;

            if (searchOption == 1) {
                printf("Enter brand: ");
                scanf("%s", brand);

                printf("Enter name: ");
                scanf("%s", name);
            }

            if (searchOption == 2) {
                printf("Enter price: ");
                scanf("%f", &price);

                printf("1.Greater 2.Less 3.Equal\n");
                scanf("%d", &mode);
            }

            if (searchOption == 3) {
                printf("Enter OnSale (0/1): ");
                scanf("%d", &boolValue);
            }

            if (searchOption == 4) {
                printf("Enter date:\n");
                d.day = readIntInRange("Day (1-31): ", 1, 31);
                d.month = readIntInRange("Month (1-12): ", 1, 12);
                d.year = readIntInRange("Year (4 digits): ", 1000, 9999);

                printf("1.Before 2.After 3.Equal\n");
                mode = readIntInRange("Choose mode (1-3): ", 1, 3);
            }

            printf("\n--- RESULTS ---\n");

            while (current || top >= 0) {
                while (current) {
                    stack[++top] = current;
                    current = current->left;
                }

                current = stack[top--];

                int match = 0;

                if (searchOption == 1) {
                    if (strstr(current->data.brand, brand))
                        match = 1;
                }
                
                if (searchOption == 2) {
                    if (mode == 1 && current->data.price > price)
                        match = 1;
                    else if (mode == 2 && current->data.price < price)
                        match = 1;
                    else if (mode == 3 && current->data.price == price)
                        match = 1;
                }
                
                if (searchOption == 3) {
                    if (current->data.onSale == boolValue)
                        match = 1;
                }
                
             
                if (searchOption == 4) {
                    int cmp = compareDates(current->data.entryDate, d);
                    if (mode == 1 && cmp < 0)
                        match = 1;
                    else if (mode == 2 && cmp > 0)
                        match = 1;
                    else if (mode == 3 && cmp == 0)
                        match = 1;
                }
              

                if (searchOption == 5) {
                    if (!current->data.isDeleted)
                        match = 1;
                }

                if (match) {
                    printf("Serial: %d | Name: %s | Brand: %s | Price: %.2f | Stock: %d | OnSale: %d | Date: %02d-%02d-%04d\n",
                        current->data.serialNumber,
                        current->data.name,
                        current->data.brand,
                        current->data.price,
                        current->data.stock,
                        current->data.onSale,
                        current->data.entryDate.day,
                        current->data.entryDate.month,
                        current->data.entryDate.year);
                }

                current = current->right;
            }

            long selected;

            printf("\nEnter serial number to select item (0 cancel): ");
            scanf("%ld", &selected);

                if (selected != 0) {
                    ItemNode* found = searchItem(itemRoot, selected);

                    if (found)
                        printf("Selected: %s | %.2f\n",
                            found->data.brand,
                            found->data.price);
                    else
                        printf("Not found\n");
                }
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

