#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "items.h"

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

    printf("Serial number: ");
    scanf("%d", &item.serialNumber);

    printf("Name: ");
    scanf(" %49[^\n]", item.name);

    printf("Brand: ");
    scanf(" %29[^\n]", item.brand);

    printf("Price: ");
    scanf("%f", &item.price);

    printf("Stock: ");
    scanf("%d", &item.stock);

    printf("On sale (1=yes, 0=no): ");
    scanf("%d", &item.onSale);

    printf("Entry date (DD-MM-YYYY): ");
    scanf("%10s", item.entryDate);

    item.isDeleted = 0;

    return item;

}

void printInorder(ItemNode* root) {
    if (!root)
        return;

    printInorder(root->left);

    if (!root->data.isDeleted) {
        printf("Serial: %d | Name: %s | Brand: %s | Price: %.2f | Stock: %d | OnSale: %d | Date: %s\n",
               root->data.serialNumber,
               root->data.name,
               root->data.brand,
               root->data.price,
               root->data.stock,
               root->data.onSale,
               root->data.entryDate);
    }

    printInorder(root->right);
}
