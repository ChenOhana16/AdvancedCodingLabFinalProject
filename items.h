#ifndef ITEMS_H
#define ITEMS_H

#define NAME_LEN 50
#define BRAND_LEN 30
#define DATE_LEN 11   // DD-MM-YYYY

typedef struct {
    int day;
    int month;
    int year;
} date;


typedef struct {
    int serialNumber;
    char name[NAME_LEN];
    char brand[BRAND_LEN];
    float price;
    int stock;
    int onSale;            // 0 / 1
    date entryDate;
    int isDeleted;         // 0 = קיים = 1 ,נמחק (soft delete)
} Item;

/* --------- צומת BST --------- */
typedef struct ItemNode {
    Item data;
    struct ItemNode* left;
    struct ItemNode* right;
} ItemNode;

/* --------- פונקציות עץ --------- */
ItemNode* insertItem(ItemNode* root, Item item);
ItemNode* searchItem(ItemNode* root, int serialNumber);
ItemNode* deleteItem(ItemNode* root, int serialNumber);

void inorderSave(ItemNode* root, FILE* fp);
void freeTree(ItemNode* root);

/* --------- טעינה ושמירה --------- */
ItemNode* loadItemsFromFile(const char* filename);
void saveItemsToFile(ItemNode* root, const char* filename);

/* --------- עזר --------- */
Item createItemFromUser();
ItemNode* deleteItemFromUser(ItemNode* root);

void printInorder(ItemNode* root);

void checkOutOfStock(ItemNode* root);

#endif
