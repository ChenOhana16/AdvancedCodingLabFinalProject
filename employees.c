#include "employees.h"
#include <stdlib.h>
#include <string.h>

#define EMP_FILE "employees.txt"

void initSystem(EmployeeSystem* sys) {
    FILE* f = fopen(EMP_FILE, "r");
    sys->arr = NULL;
    sys->count = 0;

    if (!f) {
        // יצירת מנהל דיפולטי
        sys->arr = malloc(sizeof(Employee));
        sys->count = 1;

        strcpy(sys->arr[0].username, "admin");
        strcpy(sys->arr[0].firstname, "manager");
        strcpy(sys->arr[0].password, "12345678");
        sys->arr[0].role = ROLE_ADMIN;

        saveEmployees(sys);
        writeToLog("System initialized with default admin");
        return;
    }

    Employee temp;
    while (fscanf(f, "%s %s %s %d",
        temp.username,
        temp.firstname,
        temp.password,
        (int*)&temp.role) == 4) {

        sys->arr = realloc(sys->arr, sizeof(Employee) * (sys->count + 1));
        sys->arr[sys->count++] = temp;
    }

    fclose(f);
}
