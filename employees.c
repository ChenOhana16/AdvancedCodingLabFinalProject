#define _CRT_SECURE_NO_WARNINGS
#include "employees.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EMP_FILE "employees.txt"
#define LOG_FILE "log.txt"

static void safeCopy(char* dst, size_t dstSize, const char* src) {
    if (!dst || dstSize == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    strncpy(dst, src, dstSize - 1);
    dst[dstSize - 1] = '\0';
}

void writeToLog(const char* msg) {
    FILE* f = fopen(LOG_FILE, "a");
    if (!f) return;

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char ts[32];
    if (t) strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", t);
    else safeCopy(ts, sizeof(ts), "unknown-time");

    fprintf(f, "[%s] %s\n", ts, msg ? msg : "");
    fclose(f);
}

void saveEmployees(EmployeeSystem* sys) {
    if (!sys) return;

    FILE* f = fopen(EMP_FILE, "w");
    if (!f) return;

    for (int i = 0; i < sys->count; i++) {
        fprintf(f, "%s %s %s %d\n",
            sys->arr[i].username,
            sys->arr[i].firstname,
            sys->arr[i].password,
            (int)sys->arr[i].role);
    }

    fclose(f);
}

void initSystem(EmployeeSystem* sys) {
    if (!sys) return;

    sys->arr = NULL;
    sys->count = 0;

    FILE* f = fopen(EMP_FILE, "r");
    if (!f) {
        sys->arr = (Employee*)malloc(sizeof(Employee));
        if (!sys->arr) return;

        sys->count = 1;
        safeCopy(sys->arr[0].username, USERNAME_LEN, "admin");
        safeCopy(sys->arr[0].firstname, NAME_LEN, "manager");
        safeCopy(sys->arr[0].password, PASSWORD_LEN, "12345678");
        sys->arr[0].role = ROLE_ADMIN;

        saveEmployees(sys);
        writeToLog("System initialized with default admin");
        return;
    }

    Employee temp;
    int roleInt = 0;
    while (fscanf(f, "%29s %29s %29s %d",
        temp.username,
        temp.firstname,
        temp.password,
        &roleInt) == 4) {
        temp.role = (Role)roleInt;
        Employee* newArr = (Employee*)realloc(sys->arr, sizeof(Employee) * (sys->count + 1));
        if (!newArr) break;
        sys->arr = newArr;
        sys->arr[sys->count] = temp;
        sys->count++;
    }

    fclose(f);
    writeToLog("System initialized from file");
}

static int findUserIndex(EmployeeSystem* sys, const char* username) {
    if (!sys || !username) return -1;
    for (int i = 0; i < sys->count; i++) {
        if (strcmp(sys->arr[i].username, username) == 0) return i;
    }
    return -1;
}

int login(EmployeeSystem* sys, int* loggedIndex) {
    if (!sys || !loggedIndex) return 0;

    char u[USERNAME_LEN];
    char p[PASSWORD_LEN];

    for (int attempt = 1; attempt <= 3; attempt++) {
        printf("Username: ");
        if (scanf("%29s", u) != 1) return 0;

        printf("Password: ");
        if (scanf("%29s", p) != 1) return 0;

        int idx = findUserIndex(sys, u);
        if (idx >= 0 && strcmp(sys->arr[idx].password, p) == 0) {
            *loggedIndex = idx;
            writeToLog("Login success");
            return 1;
        }

        printf("Invalid credentials. Attempts left: %d\n", 3 - attempt);
        writeToLog("Login failed");
    }

    writeToLog("System locked after 3 failed attempts");
    return 0;
}

int hasPermission(Role r, int action) {

    switch (action) {
    case 1: // add employee
        return r == ROLE_ADMIN;

    case 2: // inventory
        return r == ROLE_ADMIN || r == ROLE_WORKER;

    case 3: // delete employee
        return r == ROLE_ADMIN;

    default:
        return 0;
    }
}


void showMenu(Role r) {
    printf("\n--- Menu ---\n");

    if (r == ROLE_ADMIN) {
        printf("1. Add employee\n");
        printf("2. Inventory (other module)\n");
        printf("3. Delete employee\n");
        printf("4. Show employees\n");
        printf("0. Exit\n");
    }
    else if (r == ROLE_WORKER) {
        printf("2. Inventory (other module)\n");
        printf("4. Show employees\n");
        printf("0. Exit\n");
    }
    else {
        printf("0. Exit\n");
    }
}


void addEmployee(EmployeeSystem* sys) {
    if (!sys) return;

    Employee e;
    int roleInt = 0;

    printf("New username: ");
    if (scanf("%29s", e.username) != 1) return;

    if (findUserIndex(sys, e.username) >= 0) {
        printf("Username already exists.\n");
        writeToLog("Add employee failed: username exists");
        return;
    }

    printf("Full name: ");
    if (scanf("%29s", e.firstname) != 1) return;

    printf("Password: ");
    if (scanf("%29s", e.password) != 1) return;

    printf("Role (1=Maneger, 2=Worker, 3=Trainee): ");
    if (scanf("%d", &roleInt) != 1) return;

    if (roleInt < 1 || roleInt > 3) {
        printf("Invalid role.\n");
        writeToLog("Add employee failed: invalid role");
        return;
    }

    e.role = (Role)roleInt;

    Employee* newArr = (Employee*)realloc(sys->arr, sizeof(Employee) * (sys->count + 1));
    if (!newArr) {
        printf("Memory allocation failed.\n");
        writeToLog("Add employee failed: realloc");
        return;
    }

    sys->arr = newArr;
    sys->arr[sys->count] = e;
    sys->count++;

    saveEmployees(sys);
    writeToLog("Employee added");
    printf("Employee added successfully.\n");
}

void printEmployees(const EmployeeSystem* sys) {
    if (!sys || sys->count == 0) {
        printf("No employees found.\n");
        return;
    }

    printf("\n--- Employees List ---\n");
    for (int i = 0; i < sys->count; i++) {
        printf("%d) Username: %s | Name: %s | Role: %d\n",
            i + 1,
            sys->arr[i].username,
            sys->arr[i].firstname,
            sys->arr[i].role);
    }
}

void deleteEmployee(EmployeeSystem* sys) {
    if (!sys || sys->count == 0) {
        printf("No employees to delete.\n");
        return;
    }

    char username[USERNAME_LEN];
    printf("Enter username to delete: ");
    if (scanf("%29s", username) != 1) return;

    int idx = findUserIndex(sys, username);
    if (idx < 0) {
        printf("Employee not found.\n");
        writeToLog("Delete employee failed: not found");
        return;
    }

    for (int i = idx; i < sys->count - 1; i++) {
        sys->arr[i] = sys->arr[i + 1];
    }

    sys->count--;

    if (sys->count == 0) {
        free(sys->arr);
        sys->arr = NULL;
    }
    else {
        Employee* newArr = realloc(sys->arr, sizeof(Employee) * sys->count);
        if (newArr)
            sys->arr = newArr;
    }

    saveEmployees(sys);
    writeToLog("Employee deleted");

    printf("Employee deleted successfully.\n");
}
