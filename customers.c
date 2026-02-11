#include "customers.h"
#include <time.h>
#include "items.h"

// --- פונקציות עזר ---

long totalDays(Date d){
    return (d.year*365) + (d.month*30) + d.day;
}

Date getCurrentDate(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    Date today;
    today.year = tm.tm_year + 1900;
    today.month = tm.tm_mon + 1;
    today.day = tm.tm_mday;
    return today;
}

int compareDates(Date d1, Date d2){
    if(d1.year != d2.year) return (d1.year - d2.year);
    if(d1.month != d2.month) return (d1.month - d2.month);
    return d1.day - d2.day;
}

// --- ניהול רשימות ---

Purchase* createPurchaseNode(int serial, float price, Date date){
    Purchase* newP = (Purchase*)malloc(sizeof(Purchase));
    if(newP == NULL) {
        printf("Allocation failed\n");
        return NULL;
    }
    newP->itemSerial = serial;
    newP->priceAtPurchase = price;
    newP->purchaseDate = date;
    newP->next = NULL;
    return newP;
}

Customer* createCustomer(char* name, char* id, Date joinDate){
    Customer* newCust = (Customer*)malloc(sizeof(Customer));
    if(newCust == NULL) {
        printf("Allocation failed\n");
        return NULL;
    }
    strcpy(newCust->fullName, name);
    strcpy(newCust->id, id);
    newCust->joinDate = joinDate;
    newCust->totalSpent = 0.0;
    newCust->purchaseHistory = NULL;
    newCust->next = NULL;
    return newCust;
}

Customer* addCustomer(Customer* head, Customer* newCust){
    if(head == NULL || compareDates(head->joinDate, newCust->joinDate) > 0){
        newCust->next = head;
        return newCust;
    }
    Customer* current = head;
    while(current->next != NULL && compareDates(current->next->joinDate, newCust->joinDate) <= 0){
        current = current->next;
    }
    newCust->next = current->next;
    current->next = newCust; 
    return head;
}

Customer* findCustomer(Customer* head, char* search_id){
    while(head != NULL){
        if(strcmp(head->id, search_id) == 0) return head;
        head = head->next;
    }
    printf("Customer not found\n");
    return NULL;
}

// --- מכירות והחזרות ---

int buyItem(Customer* cust, ItemNode* itemRoot, int itemSerial)
{
    if (cust == NULL) {
        printf("Error: Customer is NULL.\n");
        return 0;
    }

    /* חיפוש הפריט בעץ */
    ItemNode* foundNode = searchItem(itemRoot, (int)itemSerial);

    if (foundNode == NULL) {
        printf("Error: Item %ld not found.\n", itemSerial);
        return 0;
    }

    Item* item = &foundNode->data;

    /* בדיקת מלאי */
    if (item->stock <= 0) {
        printf("Error: Item %ld is out of stock.\n", itemSerial);
        return 0;
    }

    /* בדיקת מגבלת 3 פריטים ביום */
    Date today = getCurrentDate();
    int itemsBoughtToday = 0;

    Purchase* p = cust->purchaseHistory;
    while (p != NULL) {
        if (compareDates(p->purchaseDate, today) == 0)
            itemsBoughtToday++;
        p = p->next;
    }

    if (itemsBoughtToday >= 3) {
        printf("Denied: Daily limit (3 items) reached.\n");
        return 0;
    }

    /* יצירת רכישה חדשה */
    Purchase* newPurchase = createPurchaseNode(itemSerial, item->price, today);
    if (newPurchase == NULL)
        return 0;

    /* הכנסת הרכישה לראש הרשימה */
    newPurchase->next = cust->purchaseHistory;
    cust->purchaseHistory = newPurchase;

    /* עדכון נתונים */
    item->stock--;
    cust->totalSpent += item->price;

    printf("Success! Customer %s bought item %ld (%.2f)\n",
           cust->fullName,
           itemSerial,
           item->price);

    return 1;
}

int returnItem(Customer* cust, ItemNode* itemRoot, int itemSerial) {
    Purchase *prev = NULL, *curr = cust->purchaseHistory;
    Date today = getCurrentDate(); 
    while (curr != NULL) {
        if (curr->itemSerial == itemSerial) {   
            if (totalDays(today) - totalDays(curr->purchaseDate) > 14) {
                printf("Return denied: >14 days.\n");
                return 0;
            }
            ItemNode* item = searchItem(itemRoot, itemSerial);
            if (item != NULL) item->data.stock++;
            
            cust->totalSpent -= curr->priceAtPurchase;
            if (prev == NULL) cust->purchaseHistory = curr->next; 
            else prev->next = curr->next; 
            
            free(curr); 
            printf("Item %ld returned.\n", itemSerial);
            return 1; 
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Purchase not found in history.\n");
    return 0;
}

// --- שמירה וטעינה ---

void saveCustomersToTextFile(Customer* head, const char* filename){
    FILE* fp = fopen(filename, "w");
    if(!fp) {
        printf("Error opening file for writing\n");
        return;
    }
    Customer* curr = head;
    while(curr){
        int count = 0;
        Purchase* p = curr->purchaseHistory;
        while(p) { count++; p = p->next; }
        
        // שמירת פרטי הלקוח ומספר הקניות
        fprintf(fp, "%s\n%s\n%.2f\n%d %d %d\n%d\n", 
                curr->fullName, curr->id, curr->totalSpent, 
                curr->joinDate.day, curr->joinDate.month, curr->joinDate.year, count);
        
        // שמירת הקניות
        p = curr->purchaseHistory;
        while(p){
            fprintf(fp, "%ld %.2f %d %d %d\n", 
                    p->itemSerial, p->priceAtPurchase, 
                    p->purchaseDate.day, p->purchaseDate.month, p->purchaseDate.year);
            p = p->next;
        }
        curr = curr->next;
    }
    fclose(fp);
    printf("Saved to %s\n", filename);
}

Customer* loadCustomersFromTextFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;
    
    Customer *head = NULL, *tail = NULL;
    char tempName[MAX_STR];

    while (fscanf(fp, "%s", tempName) != EOF) {
        Customer* n = (Customer*)malloc(sizeof(Customer));
        strcpy(n->fullName, tempName);
        
        int pCount = 0;
        // קריאה מתוקנת: קוראים את ה-count למשתנה מקומי ולא ל-&n
        fscanf(fp, "%s %f %d %d %d %d", 
               n->id, &n->totalSpent, 
               &n->joinDate.day, &n->joinDate.month, &n->joinDate.year, 
               &pCount);
        
        n->purchaseHistory = NULL;
        Purchase* lastP = NULL;
        
        for (int i = 0; i < pCount; i++) {
            Purchase* p = (Purchase*)malloc(sizeof(Purchase));
            fscanf(fp, "%ld %f %d %d %d", 
                   &p->itemSerial, &p->priceAtPurchase, 
                   &p->purchaseDate.day, &p->purchaseDate.month, &p->purchaseDate.year);
            p->next = NULL;
            
            if (!n->purchaseHistory) n->purchaseHistory = p;
            else lastP->next = p;
            lastP = p;
        }
        
        n->next = NULL;
        if (!head) head = n; 
        else tail->next = n;
        tail = n;
    }
    fclose(fp);
    printf("Loaded from %s\n", filename);
    return head;
}
