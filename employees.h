#ifndef EMPLOYEES_H
#define EMPLOYEES_H

#include <stdio.h>

typedef enum {
    ROLE_ADMIN = 1,
    ROLE_WORKER = 2,
    ROLE_TRAINEE = 3
} Role;

#define USERNAME_LEN 30
#define PASSWORD_LEN 30
#define NAME_LEN 30

typedef struct {
    char username[USERNAME_LEN];
    char firstname[NAME_LEN];
    char password[PASSWORD_LEN];
    Role role;
} Employee;

typedef struct {
    Employee* arr;
    int count;
} EmployeeSystem;

void initSystem(EmployeeSystem* sys);
void saveEmployees(EmployeeSystem* sys);

int login(EmployeeSystem* sys, int* loggedIndex);
void addEmployee(EmployeeSystem* sys);

void showMenu(Role r);
int hasPermission(Role r, int action);

void writeToLog(const char* msg);

void printEmployees(const EmployeeSystem* sys);
void deleteEmployee(EmployeeSystem* sys);


#endif
