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

static void clearInputBufferMain(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int readPositiveIntMain(const char* prompt) {
    int value;
    int readCount;

    while (1) {
        printf("%s", prompt);
        readCount = scanf("%d", &value);

        if (readCount == 1 && value > 0) {
            return value;
        }

        printf("Invalid input. Please enter a positive number greater than 0.\n");
        clearInputBufferMain();
    }
}

static void logAction(const char* action) {
    writeToLog(action);
}

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
        printf("8. Save All\n");
        printf("11. Search Items\n");
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
    setActiveLogUser(empSys.arr[loggedIndex].username, userRole);
    logAction("Session started");

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
                char logMsg[160];
                Item newItem = createItemFromUser();

                while (serialExistsInTree(itemRoot, newItem.serialNumber)) {
                    printf("Serial number %d already exists.\n", newItem.serialNumber);
                    newItem.serialNumber = readPositiveIntMain("Enter a different serial number: ");
                }

                itemRoot = insertItem(itemRoot, newItem);
                snprintf(logMsg, sizeof(logMsg), "Add item success | serial=%d name=%s", newItem.serialNumber, newItem.name);
                logAction(logMsg);
            }
            else {
                printf("No permission.\n");
                logAction("Add item denied: no permission");
            }
            break;

        case 2:
            if (userRole == ROLE_ADMIN) {
                itemRoot = deleteItemFromUser(itemRoot);
                logAction("Delete item requested");
            }
            else {
                printInorder(itemRoot);
                logAction("Show items");
            }
            break;

        case 3:
            if (userRole != ROLE_TRAINEE) {
                char logMsg[160];
                char name[50], id[20];
                date today = getCurrentDate();

                printf("Full Name: ");
                scanf("%s", name);

                printf("ID: ");
                scanf("%s", id);

                Customer* newCust = createCustomer(name, id, today);
                if (newCust) {
                    customerList = addCustomer(customerList, newCust);
                    snprintf(logMsg, sizeof(logMsg), "Add customer success | id=%s name=%s", id, name);
                    logAction(logMsg);
                }
                else {
                    logAction("Add customer failed: allocation");
                }
            }
            else {
                printf("No permission.\n");
                logAction("Add customer denied: no permission");
            }
            break;

        case 4:
            printCustomers(customerList);
            logAction("Show customers");
            break;

        case 5:
            if (userRole != ROLE_TRAINEE) {
                char logMsg[160];
                char id[20];
                long serial;

                printf("Customer ID: ");
                scanf("%s", id);

                Customer* cust = findCustomer(customerList, id);
                if (!cust) {
                    snprintf(logMsg, sizeof(logMsg), "Buy item failed: customer not found | id=%s", id);
                    logAction(logMsg);
                    break;
                }

                printf("Item Serial: ");
                scanf("%ld", &serial);

                if (buyItem(cust, itemRoot, (int)serial)) {
                    snprintf(logMsg, sizeof(logMsg), "Buy item success | customer=%s serial=%ld", id, serial);
                }
                else {
                    snprintf(logMsg, sizeof(logMsg), "Buy item failed | customer=%s serial=%ld", id, serial);
                }
                logAction(logMsg);
            }
            else {
                printf("No permission.\n");
                logAction("Buy item denied: no permission");
            }
            break;

        case 6:
            if (userRole != ROLE_TRAINEE) {
                char logMsg[160];
                char id[20];
                long serial;

                printf("Customer ID: ");
                scanf("%s", id);

                Customer* cust = findCustomer(customerList, id);
                if (!cust) {
                    snprintf(logMsg, sizeof(logMsg), "Return item failed: customer not found | id=%s", id);
                    logAction(logMsg);
                    break;
                }

                printf("Item Serial to return: ");
                scanf("%ld", &serial);

                if (returnItem(cust, itemRoot, (int)serial)) {
                    snprintf(logMsg, sizeof(logMsg), "Return item success | customer=%s serial=%ld", id, serial);
                }
                else {
                    snprintf(logMsg, sizeof(logMsg), "Return item failed | customer=%s serial=%ld", id, serial);
                }
                logAction(logMsg);
            }
            else {
                printf("No permission.\n");
                logAction("Return item denied: no permission");
            }
            break;

        case 7:
            if (userRole == ROLE_ADMIN) {
                addEmployee(&empSys);
                logAction("Add employee requested");
            }
            else {
                printf("No permission.\n");
                logAction("Add employee denied: no permission");
            }
            break;

        case 8:
            saveItemsToFile(itemRoot, "items.dat");
            saveCustomersToTextFile(customerList, "customers.txt");
            saveEmployees(&empSys);
            printf("All data saved.\n");
            logAction("Save all data");
            break;

        case 9:
            if (userRole == ROLE_ADMIN) {
                checkOutOfStock(itemRoot);
                logAction("Check out-of-stock items");
            }
            else {
                printf("No permission.\n");
                logAction("Check out-of-stock denied: no permission");
            }
            break;

        case 10:
            if (userRole == ROLE_ADMIN) {
                printVIPCustomers(customerList);
                logAction("Show VIP customers");
            }
            else {
                printf("No permission.\n");
                logAction("Show VIP customers denied: no permission");
            }

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
            {
                char logMsg[120];
                snprintf(logMsg, sizeof(logMsg), "Search items | option=%d", searchOption);
                logAction(logMsg);
            }

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
                    char logMsg[120];
                    ItemNode* found = searchItem(itemRoot, selected);

                    if (found)
                        printf("Selected: %s | %.2f\n",
                            found->data.brand,
                            found->data.price);
                    else
                        printf("Not found\n");

                    snprintf(logMsg, sizeof(logMsg), "Search select item | serial=%ld found=%s",
                        selected, found ? "yes" : "no");
                    logAction(logMsg);
                }
                else {
                    logAction("Search select item canceled");
                }
            break;

        case 0:
            printf("Saving and exiting...\n");
            saveItemsToFile(itemRoot, "items.dat");
            saveCustomersToTextFile(customerList, "customers.txt");
            saveEmployees(&empSys);
            logAction("Exit system and auto-save");
            break;

        default:
            printf("Invalid choice\n");
            {
                char logMsg[120];
                snprintf(logMsg, sizeof(logMsg), "Invalid menu choice | choice=%d", choice);
                logAction(logMsg);
            }
        }

    } while (choice != 0);

    /* ===== Free Memory ===== */

    freeTree(itemRoot);
    free(empSys.arr);

    printf("System closed successfully.\n");
    return 0;
}

