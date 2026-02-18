#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "items.h"

static void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int readIntInRange(const char* prompt, int min, int max) {
    int value;
    int readCount;

    while (1) {
        printf("%s", prompt);
        readCount = scanf("%d", &value);

        if (readCount == 1 && value >= min && value <= max) {
            return value;
        }

        printf("Invalid input. Please enter a number between %d and %d.\n", min, max);
        clearInputBuffer();
    }
}

static float readFloatValue(const char* prompt) {
    float value;
    int readCount;

    while (1) {
        printf("%s", prompt);
        readCount = scanf("%f", &value);

        if (readCount == 1 && value > 0.0f) {
            return value;
        }

        printf("Invalid input. Please enter a positive number greater than 0.\n");
        clearInputBuffer();
    }
}
/* ---------- יצירת צומת חדש ---------- */
static ItemNode* createNode(Item item) {
    ItemNode* node = (ItemNode*)malloc(sizeof(ItemNode));
    if (!node) {
        printf("Error: memory allocation failed\n");
        exit(1);
    }

    node->data = item;
    node->left = NULL;
    node->right = NULL;

    return node;
}

/* ---------- הוספת פריט ל-BST ---------- */
ItemNode* insertItem(ItemNode* root, Item item) {
    if (root == NULL)
        return createNode(item);

    if (item.serialNumber < root->data.serialNumber)
        root->left = insertItem(root->left, item);
    else if (item.serialNumber > root->data.serialNumber)
        root->right = insertItem(root->right, item);
    else
        printf("Warning: item with serial %d already exists\n", item.serialNumber);

    return root;
}

/* ---------- חיפוש פריט לפי מספר סידורי ---------- */
ItemNode* searchItem(ItemNode* root, int serialNumber) {
    if (root == NULL)
        return NULL;

    if (serialNumber == root->data.serialNumber) {
        if (root->data.isDeleted)
            return NULL;   // פריט מחוק לוגית
        return root;
    }

    if (serialNumber < root->data.serialNumber)
        return searchItem(root->left, serialNumber);
    else
        return searchItem(root->right, serialNumber);
}

/* ---------- מחיקה רכה (Soft Delete) ---------- */
ItemNode* deleteItem(ItemNode* root, int serialNumber) {
    ItemNode* node = searchItem(root, serialNumber);

    if (!node) {
        printf("Item not found\n");
        return root;
    }

    node->data.isDeleted = 1;
    printf("Item %d marked as deleted\n", serialNumber);

    return root;
}

/* ---------- שמירת הפריטים inorder ---------- */
void inorderSave(ItemNode* root, FILE* fp) {
    if (root == NULL)
        return;

    inorderSave(root->left, fp);

    if (!root->data.isDeleted)
        fwrite(&root->data, sizeof(Item), 1, fp);

    inorderSave(root->right, fp);
}

/* ---------- שמירה לקובץ בינארי ---------- */
void saveItemsToFile(ItemNode* root, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error: could not open items file for writing\n");
        return;
    }

    inorderSave(root, fp);
    fclose(fp);
}

/* ---------- טעינה מקובץ בינארי ---------- */
ItemNode* loadItemsFromFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    ItemNode* root = NULL;
    Item temp;

    if (!fp)
        return NULL;   // קובץ לא קיים – תקין לחלוטין

    while (fread(&temp, sizeof(Item), 1, fp) == 1) {
        temp.isDeleted = 0;  // ביטחון
        root = insertItem(root, temp);
    }

    fclose(fp);
    return root;
}

/* ---------- שחרור זיכרון ---------- */
void freeTree(ItemNode* root) {
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* ---------- קליטת פריט מהמשתמש ---------- */
Item createItemFromUser() {
    Item item;

    item.serialNumber = readIntInRange("Serial number: ", 1, INT_MAX);

    printf("Name: ");
    scanf(" %49[^\n]", item.name);

    printf("Brand: ");
    scanf(" %29[^\n]", item.brand);

    item.price = readFloatValue("Price: ");

    item.stock = readIntInRange("Stock: ", 1, INT_MAX);

    item.onSale = readIntInRange("On sale (1=yes, 0=no): ", 0, 1);

    printf("Entry date:\n");
    item.entryDate.day = readIntInRange("Day (1-31): ", 1, 31);
    item.entryDate.month = readIntInRange("Month (1-12): ", 1, 12);
    item.entryDate.year = readIntInRange("Year (4 digits): ", 1000, 9999);

    item.isDeleted = 0;

    return item;

}

ItemNode* deleteItemFromUser(ItemNode* root) {
    int serialNumber;

    if (root == NULL) {
        printf("No items available.\n");
        return root;
    }

    printf("Enter serial number to delete: ");
    if (scanf("%d", &serialNumber) != 1) {
        printf("Invalid serial number input.\n");
        clearInputBuffer();
        return root;
    }

    return deleteItem(root, serialNumber);
}

void printInorder(ItemNode* root) {
    if (!root)
        return;

    printInorder(root->left);

    if (!root->data.isDeleted) {
        printf("Serial: %d | Name: %s | Brand: %s | Price: %.2f | Stock: %d | OnSale: %d | Date: %02d-%02d-%04d\n",
               root->data.serialNumber,
               root->data.name,
               root->data.brand,
               root->data.price,
               root->data.stock,
               root->data.onSale,
               root->data.entryDate.day,
               root->data.entryDate.month,
               root->data.entryDate.year);
    }

    printInorder(root->right);
}


void checkOutOfStock(ItemNode* root) {
    if (!root) return;

    checkOutOfStock(root->left);

    if (root->data.stock == 0) {
        printf("OUT OF STOCK: %s | Serial: %d\n",
               root->data.name,
               root->data.serialNumber);
    }

    checkOutOfStock(root->right);
}


