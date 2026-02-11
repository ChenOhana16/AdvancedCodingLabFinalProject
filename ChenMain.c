#include <stdio.h>
#include "items.h"
#include "items.c"

void showMenu() {
    printf("\n========== ITEM MANAGEMENT ==========\n");
    printf("1. Insert new item\n");
    printf("2. Search item by serial number\n");
    printf("3. Delete item\n");
    printf("4. Show all items\n");
    printf("5. Save items to file\n");
    printf("6. Exit\n");
    printf("Choose option: ");
}

int main() {

    ItemNode* root = NULL;
    int choice;
    int serial;

    /* טעינה בתחילת התוכנית */
    root = loadItemsFromFile("items.dat");

    do {
        showMenu();
        scanf("%d", &choice);

        switch (choice) {

        case 1: {
            Item newItem = createItemFromUser();
            root = insertItem(root, newItem);
            break;
        }

        case 2: {
            printf("Enter serial number to search: ");
            scanf("%d", &serial);

            ItemNode* found = searchItem(root, serial);
            if (found) {
                printf("\nItem found:\n");
                printf("Serial: %d | Name: %s | Brand: %s | Price: %.2f | Stock: %d | OnSale: %d | Date: %s\n",
                       found->data.serialNumber,
                       found->data.name,
                       found->data.brand,
                       found->data.price,
                       found->data.stock,
                       found->data.onSale,
                       found->data.entryDate);
            } else {
                printf("Item not found.\n");
            }
            break;
        }

        case 3: {
            printf("Enter serial number to delete: ");
            scanf("%d", &serial);
            root = deleteItem(root, serial);
            break;
        }

        case 4:
            printf("\n=== All Items (Inorder Sorted) ===\n");
            printInorder(root);
            break;

        case 5:
            saveItemsToFile(root, "items.dat");
            printf("Items saved successfully.\n");
            break;

        case 6:
            printf("Saving and exiting...\n");
            saveItemsToFile(root, "items.dat");
            break;

        default:
            printf("Invalid option.\n");
        }

    } while (choice != 6);

    freeTree(root);
    return 1;
}
