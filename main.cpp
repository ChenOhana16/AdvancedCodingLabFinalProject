#define _CRT_SECURE_NO_WARNINGS
#include "employees.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    EmployeeSystem sys;
    int loggedIndex = -1;

    initSystem(&sys);

    if (!login(&sys, &loggedIndex)) {
        printf("System locked.\n");
        free(sys.arr);
        return 0;
    }

    Role r = sys.arr[loggedIndex].role;
    int choice = 0;

    while (1) {
        showMenu(r);
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) break;

        if (choice == 0) {
            saveEmployees(&sys);
            writeToLog("Exit");
            break;
        }

        if (choice == 1) {
            if (hasPermission(r, 1)) addEmployee(&sys);
            else printf("No permission.\n");
            continue;
        }

        if (choice == 2) {
            printf("Inventory handled by other module\n");
            continue;
        }

        printf("Unknown choice.\n");
    }

    free(sys.arr);
    return 0;
}
