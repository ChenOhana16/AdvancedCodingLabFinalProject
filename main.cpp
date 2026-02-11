#include "employees.h"
#include <stdio.h>

int main() {
    EmployeeSystem sys;
    int loggedIndex;

    initSystem(&sys);

    if (!login(&sys, &loggedIndex)) {
        printf("System locked.\n");
        return 0;
    }

    Role r = sys.arr[loggedIndex].role;
    int choice;

    while (1) {
        showMenu(r);
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1 && r == ROLE_ADMIN) {
            addEmployee(&sys);
        }
        else {
            printf("Inventory handled by other module\n");
        }
    }

    saveEmployees(&sys);
    return 0;
}
