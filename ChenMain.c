#include <stdio.h>
#include "items.h"
#include "items.c"

int main() {

    ItemNode* root = NULL;

    printf("=== Loading items from file ===\n");
    root = loadItemsFromFile("items.dat");

    printf("\n=== Inserting test items ===\n");

    Item item1 = {100, "T-Shirt", "Nike", 99.9, 10, 1, "01-01-2026", 0};
    Item item2 = {50, "Jeans", "Levis", 199.9, 5, 0, "02-01-2026", 0};
    Item item3 = {150, "Jacket", "Adidas", 299.9, 3, 1, "03-01-2026", 0};

    root = insertItem(root, item1);
    root = insertItem(root, item2);
    root = insertItem(root, item3);

    printf("\n=== Current Items (Inorder) ===\n");
    printInorder(root);

    printf("\n=== Testing Search ===\n");
    ItemNode* found = searchItem(root, 100);
    if (found)
        printf("Found item: %s\n", found->data.name);
    else
        printf("Item not found\n");

    printf("\n=== Testing Delete (Soft Delete) ===\n");
    root = deleteItem(root, 100);

    printf("\n=== Items After Delete ===\n");
    printInorder(root);

    printf("\n=== Saving to File ===\n");
    saveItemsToFile(root, "items.dat");

    printf("\n=== Freeing Tree ===\n");
    freeTree(root);

    printf("\n=== Reloading From File ===\n");
    root = loadItemsFromFile("items.dat");

    printf("\n=== Items After Reload ===\n");
    printInorder(root);

    freeTree(root);

    printf("\n=== TEST COMPLETE ===\n");

    return 0;
}
