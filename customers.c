#include "project.h"
#include <time.h>
long totalDays(Date d){
    return (d.year*365)+(d.month*30)+d.day;
}
Date getCurrentDate(){
    time_t t=time(NULL);
    struct tm tm=*localtime(&t);
    Date today;
    today.year=tm.tm_year+1900;
    today.month=tm.tm_mon+1;
    today.day=tm.tm_mday;
    return today;
}
Purchase* createPurchaseNode(long serial,float price,Date date){
    Purchase* newP=(Purchase*)malloc(sizeof(Purchase));
    if(newP==NULL){
        printf("The allocation is failed \n");
        return NULL;
    }
        newP->itemSerial=serial;
        newP->priceAtPurchase=price;
        newP->purchaseDate=date;
        newP->next=NULL;
        return newP;
    
}
int compareDates(Date d1,Date d2){
    if(d1.year!=d2.year){
        return (d1.year-d2.year);
    }
    if(d1.month!=d2.month){
        return(d1.month-d2.month);
    }
    return d1.day-d2.day;
}
Customer* createCustomer(char* name,char* id,Date joinDate){
    Customer* newCust=(Customer*)malloc(sizeof(Customer));
    if(newCust==NULL){
        printf("The allocation is failed \n");
        return NULL;
    }
    strcpy(newCust->fullName,name);
    strcpy(newCust->id,id);
    newCust->joinDate=joinDate;
    newCust->totalSpent=0.0;
    newCust->purchaseHistory=NULL;
    newCust->next=NULL;
    return newCust;
}
Customer* addCustomer(Customer* head,Customer* newCust){
    if(head==NULL || compareDates(head->joinDate,newCust->joinDate)>0){
        newCust->next=head;
        return newCust;
    }
    Customer* current=head;
    while(current->next!=NULL && compareDates(current->next->joinDate,newCust->joinDate)<=0){
        current=current->next;
    }
    newCust->next=current->next;
    current->next=newCust; 
    return head;
}
Customer* findCustomer(Customer* head,char * search_id){
    while(head!=NULL){
        if(strcmp(head->id,search_id)==0){
            return head;
        }
        else head=head->next;
    }
    printf("The customer doesnt exist\n");
    return NULL;
}
int buyItem(Customer* cust,Item * itemRoot,long itemSerial){
    Item* item=findItem(itemRoot,itemSerial);
    if(item==NULL){
        printf("Error:Item %ld not found in store.\n",itemSerial);
        return 0;
    }
    if(item->stock<=0){
        printf("Error:Item %ld is out of stock.\n",itemSerial);
        return 0;
    }
    Date today = getCurrentDate();

    int itemsBoughtToday=0;
    Purchase* p=cust->purchaseHistory;
    while(p!=NULL){
        if(compareDates(p->purchaseDate,today)==0){
            itemsBoughtToday++;
        }
        p=p->next;
    }

    if(itemsBoughtToday>=3){
        printf("Purchase denied: Customer reached daily limit of 3 items.\n");
        return 0;
    }
    Purchase* newPurch=createPurchaseNode(itemSerial,item->price,today);
    if(newPurch==NULL) return 0;
    newPurch->next=cust->purchaseHistory;
    cust->purchaseHistory=newPurch;
    item->stock--;
    cust->totalSpent+=item->price;
    printf("Success! Bought %s for %.2f on %d/%d/%d\n", 
           item->brand, item->price, today.day, today.month, today.year);
    return 1;
}
int returnItem(Customer* cust, Item* itemRoot, long itemSerial) {
    Purchase* prev = NULL;
    Purchase* curr = cust->purchaseHistory;
    Date today = getCurrentDate(); 
    while (curr != NULL) {
        if (curr->itemSerial == itemSerial) {   
            long dayPassed = totalDays(today) - totalDays(curr->purchaseDate);
            if (dayPassed > 14) {
                printf("Return denied: Purchase was %ld days ago (Limit is 14)\n", dayPassed);
                return 0;
            }
            Item* item = findItem(itemRoot, itemSerial);
            if (item != NULL) {
                item->stock++;
            }
            cust->totalSpent -= curr->priceAtPurchase;

            if (prev == NULL) {
                cust->purchaseHistory = curr->next; 
            } else {
                prev->next = curr->next; 
            }
            free(curr); 
            printf("Item %ld returned successfully.\n", itemSerial);
            return 1; 
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Error: Customer did not buy item %ld (or already returned it).\n", itemSerial);
    return 0;
}
void saveCustomers(Customer* head,const char* filename){
    FILE* fp=fopen(filename,"w");
    if(fp==NULL){
        printf("Error:Could not open file %s for writing\n",filename);
        return;
    }
    Customer* current=head;
    while(current!=NULL){
        int purchaseCount=0;
        Purchase* p=current->purchaseHistory;
        while(p!=NULL){
            purchaseCount++;
            p=p->next;
        }
        fprintf(fp, "%s\n", current->fullName);
        fprintf(fp, "%s\n", current->id);
        fprintf(fp, "%.2f\n", current->totalSpent);
        fprintf(fp, "%d %d %d\n", current->joinDate.day, current->joinDate.month, current->joinDate.year);
        fprintf(fp,"%d\n",purchaseCount);
        p=current->purchaseHistory;
        while(p!=NULL){
            fprintf(fp,"%ld %.2f %d %d %d\n",p->itemSerial,p->priceAtPurchase,p->purchaseDate.day,p->purchaseDate.month,p->purchaseDate.year);
            p=p->next;
        }
        current=current->next;
    }
    fclose(fp);
    printf("Data saved successfully to %s\n", filename);
}
Customer* loadCustomers(const char* filename) {
    int i;
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    Customer* head = NULL;
    Customer* tail = NULL;
    char tempName[100];

    while (fscanf(fp, "%s", tempName) != EOF) {
        Customer* newCust = (Customer*)malloc(sizeof(Customer));
        if (newCust == NULL) {
            printf("The allocation failed\n");
            return NULL;
        }

        strcpy(newCust->fullName, tempName);
        fscanf(fp, "%s", newCust->id);
        fscanf(fp, "%f", &newCust->totalSpent);
        fscanf(fp, "%d %d %d", &newCust->joinDate.day, &newCust->joinDate.month, &newCust->joinDate.year);
        
        newCust->purchaseHistory = NULL;
        newCust->next = NULL;

        int purchaseCount;
        fscanf(fp, "%d", &purchaseCount);

        Purchase* lastPurch = NULL;
        for (i = 0; i < purchaseCount; i++) {
            Purchase* newP = (Purchase*)malloc(sizeof(Purchase));
            if (newP == NULL) break;

            fscanf(fp, "%ld %f %d %d %d", &newP->itemSerial, &newP->priceAtPurchase, &newP->purchaseDate.day, &newP->purchaseDate.month, &newP->purchaseDate.year);
            newP->next = NULL;

            if (newCust->purchaseHistory == NULL) {
                newCust->purchaseHistory = newP;
            } else {
                lastPurch->next = newP;
            }
            lastPurch = newP;
        }

        if (head == NULL) {
            head = newCust;
            tail = newCust;
        } else {
            tail->next = newCust;
            tail = newCust;
        }
    }
    fclose(fp);
    printf("Data loaded successfully from %s\n", filename);
    return head;
}