#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <ctype.h>
#include <time.h>
#define FILE_PATH "manage_system_code/abnormal_item.txt" // ???¡¤??????????§Ò??
#define COMPENSATION_LOG_PATH "manage_system_code/compensation_log.txt" // ???¡¤??????????§Ò??
// ????? strptime ??????????????????????
char *strptime(const char *s, const char *format, struct tm *tm)
{
    (void)format; // Mark 'format' as unused to suppress the warning
    sscanf(s, "%d-%d-%d", &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
    tm->tm_year -= 1900; // ???? 1900 ???
    tm->tm_mon -= 1;     // ?¡¤?? 0 ???
    return (char *)s + strlen(s);
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
// ????????????
typedef struct userdata
{
    char phonenumber[12];  // ??????11¦Ë??
    char password[9];      // ???????8¦Ë??
    char name[20];         // ????
    int isvip;            // ???VIP
    char join_date[11];    // ???????
    char expire_date[11];  // ????????
    struct userdata *next; // ????????????????
} user;
struct user_node
{
    struct userdata data;
    struct user_node *next;
};
struct manager
{
    char number[20];
    char password[20];
};
struct manager_node
{
    struct manager data;
    struct manager_node *next;
};
// ???????????
struct package
{
    char type[20];
    char recipient[20];
    char account[20];//????????
    char pickupCode[10];
    char parcelin_time[20];
    char parcelout_time[20];
    char to_building; // 'y' ????????????'n' ??????????
    char date[20];       // ??????????
    float fee;           // ????
    char oper[20];   // ?????
};
struct package_node
{
    struct package data;
    struct package_node *next;
};
struct abnormal_item
{
    char type[20];       // ????
    double costs;        // ??????
    char pickupcode[10]; // ?????
    char name[20];       // ????
    char phone[15];      // ?´Â
    double Compensation; // ?????
};
struct abnormal_item_node
{
    struct abnormal_item data;
    struct abnormal_item_node *next;
};
typedef struct shipment
{
    char sender[100];
    char sender_phone[20];
    char receiver[100];
    char receiver_phone[20];
    char address[100];
    char type_shipment[20];
    char is_to_building;
    float weight;
    char payment[30];
    float payment_amount;
    char shipment_code[10];
    char status[20];
} shipment;
typedef struct shipment_node
{
    shipment data;
    struct shipment_node *next;
} shipment_node;
// ??????????????
typedef struct
{
    char province[20]; // ???????
    int first_weight;  // ???????¦Ë?????
    int add_weight;    // ???????¦Ë???/????
} Province;
//???????
int count_shipments(shipment_node *head)
{
    int count = 0;
    while (head)
    {
        count++;
        head = head->next;
    }
    return count;
}
//??????????
bool validate_phonenumber(const char *phone) {
    if (phone == NULL) {
        return false;
    }
    // ??øA??????11
    if (strlen(phone) != 11) {
        return false;
    }
    // ????????????????§¹???§Û??????????????1?????
    if (phone[0] != '1') {
        return false;
    }
    // ????????????????§¹??????3,4,5,6,7,8,9??
    char second = phone[1];
    if (second != '3' && second != '4' && second != '5' && 
        second != '6' && second != '7' && second != '8' && second != '9') {
        return false;
    }
    // ??????????????????
    for (size_t i = 2; i < 11; i++) {
        if (!isdigit((unsigned char)phone[i])) {
            return false;
        }
    }
    return true;
}
// ????????????
struct abnormal_item_node *Create_abnormal_item_node(struct abnormal_item *data)
{
    struct abnormal_item_node *pnew = (struct abnormal_item_node *)malloc(sizeof(struct abnormal_item_node));
    if (pnew == NULL)
    {
        printf("?????????!\n");
        return NULL;
    }
    pnew->data = *data; // ?????????
    pnew->next = NULL;
    return pnew;
}
// ???????????¦Â
void append_abnormal_item(struct abnormal_item_node **root, struct abnormal_item *data)
{
    if (root == NULL)
        return;
    if (*root == NULL)
    {
        *root = Create_abnormal_item_node(data);
        return;
    }
    struct abnormal_item_node *ptemp = *root;
    while (ptemp->next)
    {
        ptemp = ptemp->next;
    }
    ptemp->next = Create_abnormal_item_node(data);
}
// ?????????
void generate_compensation(double *compensation, double *costs)
{
    *compensation = (*costs) * 1.5;
}
// ??????????
void free_abnormal_item_list(struct abnormal_item_node *root)
{
    struct abnormal_item_node *temp;
    while (root)
    {
        temp = root;
        root = root->next;
        free(temp);
    }
}
// ?????????????????
void load_abnormal_items(struct abnormal_item_node **root)
{
    FILE *fp = fopen(FILE_PATH, "r");
    if (fp == NULL)
    {
        printf("???????????????????\n");
        return;
    }
    struct abnormal_item temp;
    while (fscanf(fp, "%19s %lf %9s %19s %14s %lf", temp.type, &temp.costs, temp.pickupcode, temp.name, temp.phone, &temp.Compensation) != EOF)
    {
        append_abnormal_item(root, &temp);
    }
    fclose(fp);
}
// ?????????????????
void save_abnormal_items(struct abnormal_item_node *root)
{
    FILE *fp = fopen(FILE_PATH, "w");
    if (fp == NULL)
    {
        printf("????????????§Õ??\n");
        return;
    }
    struct abnormal_item_node *ptemp = root;
    while (ptemp)
    {
        fprintf(fp, "%s %.2lf %s %s %s %.2lf\n", ptemp->data.type, ptemp->data.costs, ptemp->data.pickupcode, ptemp->data.name, ptemp->data.phone, ptemp->data.Compensation);
        ptemp = ptemp->next;
    }
    fclose(fp);
}
void log_compensation_to_file(struct abnormal_item *item)
{
    FILE *fp = fopen(COMPENSATION_LOG_PATH, "a");
    if (fp == NULL)
    {
        printf("???????????????\n");
        return;
    }
    fprintf(fp, "%.2lf\n", item->Compensation);
    fflush(fp); // ?????????????§Õ?????
    fclose(fp);
    printf("??????????????????§³?\n");
}
// ????????????
void add_abnormal_item(struct abnormal_item_node **root) {
    char selectflag = 'y';
    char input_buffer[100];   
    while (selectflag == 'y' || selectflag == 'Y') {
        struct abnormal_item new_abnormal_item;       
        system("cls");
        printf("-----------------???????????------------------\n");       
        // 1. ???????????
        printf("????????????(??????/??????/????????/????????/?????): ");
        while (1) {
            fgets(input_buffer, sizeof(input_buffer), stdin);
            input_buffer[strcspn(input_buffer, "\n")] = '\0';
            if (strlen(input_buffer) > 0 &&
                (strcmp(input_buffer, "??????") == 0 ||
                 strcmp(input_buffer, "??????") == 0 ||
                 strcmp(input_buffer, "????????") == 0 ||
                 strcmp(input_buffer, "????????") == 0 ||
                 strcmp(input_buffer, "?????") == 0)) {
                strncpy(new_abnormal_item.type, input_buffer, sizeof(new_abnormal_item.type) - 1);
                break;
            }
            printf("?????????????????(??????/??????/????????/????????/?????): ");
        }
        // 2. ?????????
        printf("??????????: ");
        while (fgets(input_buffer, sizeof(input_buffer), stdin)) {
            if (sscanf(input_buffer, "%lf", &new_abnormal_item.costs) == 1) {
                break;
            }
            printf("??????§¹????????????????: ");
        }
        // 3. ?????????????????A-0001??
        printf("?????????????(?????A-0001): ");
        while (fgets(input_buffer, sizeof(input_buffer),stdin), stdin) {
            input_buffer[strcspn(input_buffer, "\n")] = '\0';  
            // ??????????
            if (strlen(input_buffer) == 6 && 
                isalpha(input_buffer[0]) && 
                input_buffer[1] == '-' && 
                isdigit(input_buffer[2]) && 
                isdigit(input_buffer[3]) && 
                isdigit(input_buffer[4]) && 
                isdigit(input_buffer[5])) {
                strncpy(new_abnormal_item.pickupcode, input_buffer, sizeof(new_abnormal_item.pickupcode) - 1);
                break;
            }
            printf("?????????????????????(?????A-0001): ");
        }
        // 4. ???????????????????10???????
        printf("???????????????(??????10?????): ");
        while (fgets(input_buffer, sizeof(input_buffer), stdin)) {
            input_buffer[strcspn(input_buffer, "\n")] = '\0';
            
            if (strlen(input_buffer) > 0 && strlen(input_buffer) <= 10) {
                strncpy(new_abnormal_item.name, input_buffer, sizeof(new_abnormal_item.name) - 1);
                break;
            }
            printf("??????????§¹????????????(1-10?????): ");
        }
        // 5. ?????´Â?????????????
        printf("????????????????: ");
        while (fgets(input_buffer, sizeof(input_buffer), stdin)) {
            input_buffer[strcspn(input_buffer, "\n")] = '\0';
            
            if (validate_phonenumber(input_buffer)) {
                strncpy(new_abnormal_item.phone, input_buffer, sizeof(new_abnormal_item.phone) - 1);
                break;
            }
            printf("?????????????????????11¦Ë??§¹?????: ");
        }
        // ?????????
        generate_compensation(&new_abnormal_item.Compensation, &new_abnormal_item.costs);   
        // ?????????
        append_abnormal_item(root, &new_abnormal_item); 
        // ?????????
        printf("\n?????????(y?????, ?????????): ");
        fgets(input_buffer, sizeof(input_buffer), stdin);
        selectflag = input_buffer[0];
    }
    // ???????????
    save_abnormal_items(*root);
}
// ???????
void delete_abnormal_item(struct abnormal_item_node **root)
{
    char selectflag = 'y';
    while (selectflag == 'y' || selectflag == 'Y')
    {
        char pickupcode[10];
        system("cls");
        printf("???????????????????: ");
        scanf("%9s", pickupcode);
        struct abnormal_item_node *ptemp = *root;
        struct abnormal_item_node *prev = NULL;
        int found = 0; // ?????????????????
        while (ptemp)
        {
            if (strcmp(ptemp->data.pickupcode, pickupcode) == 0)
            {
                found = 1;
                log_compensation_to_file(&ptemp->data);
                if (prev == NULL)
                {
                    *root = ptemp->next; // ???????
                }
                else
                {
                    prev->next = ptemp->next; // ????§Þ??¦Â???
                }
                free(ptemp);
                printf("???????????!\n");
                save_abnormal_items(*root); // ?????????
                break;
            }
            prev = ptemp;
            ptemp = ptemp->next;
        }
        if (!found)
        {
            printf("¦Ä????????? %s ???????!\n", pickupcode);
        }
        printf("?????????(y????????): ");
        scanf(" %c", &selectflag); // ?????????????§Ù?
    }
}
//????????
void check_abnormal_items(struct abnormal_item_node *root) {
    system("cls");
    printf("----------??????????----------\n");
    printf("%-16s %-10s %-10s %-10s %-12s %-10s\n", 
           "????", "???", "?????", "?????", "?????", "?????");
    struct abnormal_item_node *current = root;
    while (current != NULL) {
        printf("%-16s %-10.2f %-10s %-10s %-12s %-10.2f\n",
               current->data.type,
               current->data.costs,
               current->data.pickupcode,
               current->data.name,
               current->data.phone,
               current->data.Compensation);
        current = current->next;
    }
    printf("\n");
    system("pause");
}
struct package_node *Create_package_node(struct package *data)
{
    struct package_node *pnew = (struct package_node *)malloc(sizeof(struct package_node));
    if (pnew == NULL)
    {
        printf("?????????!\n");
        return NULL;
    }
    memcpy(&pnew->data, data, sizeof(struct package));
    pnew->next = NULL;
    return pnew;
}

void append_package(struct package_node **root, struct package *data)
{
    if (root == NULL)
    {
        printf("??????????????????????\n");
        return;
    }

    struct package_node *pnew = Create_package_node(data);
    if (pnew == NULL)
    {
        printf("????????????\n");
        return;
    }

    if (*root == NULL)
    {
        *root = pnew;
    }
    else
    {
        struct package_node *ptemp = *root;
        while (ptemp->next)
        {
            ptemp = ptemp->next; // ?????????????????????? ptemp ?????????????????????????
        }
        ptemp->next = pnew;
    }
}
static void save_sequence(int sequence[7])
{
    FILE *file = fopen("manage_system_code/sequence.txt", "w");
    if (file)
    {
        for (int i = 0; i < 7; i++)
        {
            fprintf(file, "%d %d\n", i, sequence[i]); // ???????????
        }
        fclose(file);
    }
}
void generate_pickupcode(char* code, char* type)
{
    static int sequence[7] = {0}; // ???????????????????????
    static int is_loaded = 0;
    if (!is_loaded)
    {
        FILE *file = fopen("manage_system_code/sequence.txt", "r");
        if (file) {
            int idx, value;
            while (fscanf(file, "%d %d", &idx, &value) == 2) {
                if (idx >= 0 && idx < 7) {
                    sequence[idx] = value;
                }
            }
            fclose(file);
        }
        is_loaded = 1;
    }
    char pre = 'A';
    if (strcmp(type, "??????") == 0 || strcmp(type, "??????")==0)
    {
        pre = 'A' + rand() % 3; // ?????? A/B/C
    }
    else if (strcmp(type, "????????") == 0 || strcmp(type, "????????")==0)
    {
        pre = 'D';
    }
    else if (strcmp(type, "?????") == 0)
    {
        pre = 'E';
    }
    int index = pre - 'A'; // ????????????????
    if (index < 0 || index >= 7)
    {
        printf("??????????????¦¶??\n");
        return;
    }
    int number = (sequence[index] % 10000) + 1;
    sprintf(code, "%c-%04d", pre, number); // ??????????
    sequence[index]++;
    save_sequence(sequence);
}
int is_valid_work_time(const char *datetime_str)
{
    // ????????YYYY-MM-DD HH:MM
    if (strlen(datetime_str) != 16 ||
        datetime_str[4] != '-' ||
        datetime_str[7] != '-' ||
        datetime_str[10] != ' ' ||
        datetime_str[13] != ':')
    {
        return 0;
    }

    // ????????
    int hour, minute;
    if (sscanf(datetime_str + 11, "%d:%d", &hour, &minute) != 2)
    {
        return 0;
    }

    // ????????÷Ï8:00-18:00??
    return (hour >= 8 && hour < 18) ||
           (hour == 18 && minute == 0);
}
// ???›Ô???
void save_package_list(struct package_node *root, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("???????? %s ???§Ò??—ž\n", filename);
        return;
    }
    struct package_node *current = root;
    while (current != NULL)
    {
        fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%c\t%s\t%.2f\t%s\n",
                current->data.type,
                current->data.recipient,
                current->data.account,
                current->data.pickupCode,
                current->data.parcelin_time,
                current->data.parcelout_time,
                current->data.to_building,
                current->data.date,
                current->data.fee,
                current->data.oper);
        current = current->next;
    }
    fclose(fp);
}
// ???????????????????
void add_package(struct package_node** root)
{
    char input_buffer[100];
    char selectflag = 'y';
    while (selectflag == 'y' || selectflag == 'Y')
    {
        struct package new_package;
        strcpy(new_package.date, "??");
        strcpy(new_package.oper, "??");
        strcpy(new_package.parcelout_time, "??");
        new_package.to_building = 'n';
        new_package.fee = 0.00;
        system("cls");
        printf("-----------------??????------------------\n");
        printf("????????????:(??????????/??????/????????/????????/?????) ");
        // ???????????
        while (1)
        {
            fgets(input_buffer, sizeof(input_buffer), stdin);
            input_buffer[strcspn(input_buffer, "\n")] = '\0';

            if (strlen(input_buffer) > 0 &&
                (strcmp(input_buffer, "??????") == 0 ||
                 strcmp(input_buffer, "??????") == 0 ||
                 strcmp(input_buffer, "????????") == 0 ||
                 strcmp(input_buffer, "????????") == 0 ||
                 strcmp(input_buffer, "?????") == 0))
            {
                strncpy(new_package.type, input_buffer, sizeof(new_package.type) - 1);
                break;
            }
            printf("???????????????????/??????/????????/????????/?????: ");
        }

        // ?????????
        printf("?????????????: ");
        fgets(input_buffer, sizeof(input_buffer), stdin);
        input_buffer[strcspn(input_buffer, "\n")] = '\0';
        strncpy(new_package.recipient, input_buffer, sizeof(new_package.recipient) - 1);

        // ???????
        printf("??????????????: ");
        fgets(input_buffer, sizeof(input_buffer), stdin);
        input_buffer[strcspn(input_buffer, "\n")] = '\0';
        strncpy(new_package.account, input_buffer, sizeof(new_package.account) - 1);

        // ?????????????
        generate_pickupcode(new_package.pickupCode, new_package.type);

        time_t now = time(NULL);
        struct tm *local = localtime(&now);
        strftime(new_package.parcelin_time, sizeof(new_package.parcelin_time), "%Y-%m-%d %H:%M:%S", local);

        // ?????????
        append_package(root, &new_package);
        printf("\n????????????: %s\n", new_package.pickupCode);
        printf("??????%s\n", new_package.parcelin_time);

        // ?????????
        printf("\n?????????(y?????, ?????????): ");
        fgets(input_buffer, sizeof(input_buffer), stdin);
        selectflag = input_buffer[0];
    }

    // ????????

    save_package_list(*root, "manage_system_code/package.txt");
    system("pause");
}
void load_package_list(struct package_node **root, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("??? %s ??????????????????????????\n", filename);
        return;
    }

    char line[512];
    int line_num = 0;
    while (fgets(line, sizeof(line), fp))
    {
        line_num++;
        // ?????¦Â???§Ù?
        line[strcspn(line, "\n")] = '\0';

        // ????????
        if (strlen(line) == 0)
            continue;

        struct package temp;

        // ???sscanf??????????
        sscanf(line, "%19[^\t]\t%19[^\t]\t%19[^\t]\t%9[^\t]\t%19[^\t]\t%19[^\t]\t%c\t%19[^\t]\t%f\t%19[^\t\n]",
               temp.type,
               temp.recipient,
               temp.account,
               temp.pickupCode,
               temp.parcelin_time,
               temp.parcelout_time,
               &temp.to_building,
               temp.date,
               &temp.fee,
               temp.oper);

        append_package(root, &temp);
    }
    fclose(fp);
}
void show_to_building_packageList(struct package_node **root)
{
    printf("----------?????????????????----------\n");
    printf("%-20s %-20s %-20s %-10s %-20s\n",
           "????", "?????", "???", "?????", "?????");

    struct package_node *current = *root;
    int count = 0;
    while (current != NULL)
    {
        if (current->data.to_building== 'y')
        {
            printf("%-20s %-20s %-20s %-10s %-20s\n",
                   current->data.type,
                   current->data.recipient,
                   current->data.account,
                   current->data.pickupCode,
                   current->data.date);
            count++;
        }
        current = current->next;
    }

    if (count == 0)
        printf("\n??§Õ??????????????????\n");

    printf("\n");
    system("pause");
}

void manager_parcelout(struct package_node **root)
{
    char pickupCode[10];
    char input_buffer[100];
    system("cls");
    printf("----------??????----------\n");
    printf("???????????: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    input_buffer[strcspn(input_buffer, "\n")] = '\0';
    strncpy(pickupCode, input_buffer, sizeof(pickupCode) - 1);

    struct package_node **pptr = root;
    int found = 0;

    while (*pptr != NULL)
    {
        if (strcmp((*pptr)->data.pickupCode, pickupCode) == 0)
        {
            found = 1;
            // ??????????
            time_t now = time(NULL);
            struct tm *local = localtime(&now);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);
            strcpy((*pptr)->data.parcelout_time, time_str);

            // ???¨°?????????
            strcpy((*pptr)->data.oper, "manager");
            // ???›Ô??????
            FILE *out = fopen("manage_system_code/parcelout.txt", "w");
            if (out)
            {
                fprintf(out, "%s\t%s\t%s\t%s\t%s\t%s\t%c\t%s\t%.2f\t%s\n",
                    (*pptr)->data.type,
                    (*pptr)->data.recipient,
                    (*pptr)->data.account,
                    (*pptr)->data.pickupCode,
                    (*pptr)->data.parcelin_time,
                    (*pptr)->data.parcelout_time,
                    (*pptr)->data.to_building,
                    (*pptr)->data.date,
                    (*pptr)->data.fee,
                    (*pptr)->data.oper);
            fclose(out);
            }
            // ???????????
            struct package_node *temp = *pptr;
            *pptr = temp->next;
            free(temp);
            printf("\n????????????\n");
            break;
        }
        pptr = &(*pptr)->next;
    }
    if (!found)
    {
        printf("\n¦Ä??????????\n");
    }
        save_package_list(*root, "manage_system_code/package.txt");
    system("pause");
}
void manager_pickup_package(struct package_node **root)
{
    int choose;
    do
    {
        system("cls");
        printf("-----------------????????????------------------\n");
        printf("1. ????????????????\n");
        printf("2. ????\n");
        printf("0. ????\n");
        printf("???????(0-2): ");

        scanf("%d", &choose);
        clear_input_buffer();
        switch (choose)
        {
        case 1:
        {
            system("cls");
            show_to_building_packageList(root);
            break;
        }
        case 2:
        {
            system("cls");
            manager_parcelout(root);
            break;
        }
        case 0:
            return;
        default:
            printf("??§¹?????????????\n");
            system("pause");
        }
    } while (1);
}

// ??? strptime ?????
int parse_time(const char *time_str, struct tm *tm_ptr, const char *format)
{
    if (sscanf(time_str, format, &tm_ptr->tm_year, &tm_ptr->tm_mon, &tm_ptr->tm_mday,
               &tm_ptr->tm_hour, &tm_ptr->tm_min) != 5)
    {
        return 0; // ???????
    }
    tm_ptr->tm_year -= 1900; // tm_year ??? 1900 ?????
    tm_ptr->tm_mon -= 1;     // tm_mon ??? 0 ?????
    return 1;                // ???????
}
void user_pickup_package(struct package_node **root, user *u)
{
    char input_buffer[100];
    system("cls");
    printf("-----------------??????????------------------\n");
    printf("???????????: ");

    fgets(input_buffer, sizeof(input_buffer), stdin);
    input_buffer[strcspn(input_buffer, "\n")] = '\0';

    struct package_node **pptr = root;
    while (*pptr != NULL)
    {
        if (strcmp((*pptr)->data.pickupCode, input_buffer) == 0)
        {
            printf("\n???????????\n");
            printf("1. ???????\n");
            printf("2. ???????\n");
            printf("?????(1-2): ");

            int choice = -1;

            fgets(input_buffer, sizeof(input_buffer), stdin);
            if (sscanf(input_buffer, "%d", &choice) != 1 || (choice != 1 && choice != 2))
            {
                printf("??????§¹??\n");
                system("pause");
                return;
            }

            if (choice == 2 && strcmp(u->phonenumber, (*pptr)->data.account) == 0)
            {
                // ????????????????
                int valid = 0;
                do
                {
                    printf("\n???????????(YYYY-MM-DD HH:MM ???????8:00-18:00): ");
                    fgets(input_buffer, sizeof(input_buffer), stdin);
                    input_buffer[strcspn(input_buffer, "\n")] = '\0';
                    strncpy((*pptr)->data.date, input_buffer, sizeof((*pptr)->data.date) - 1);

                    // ???????????????????
                    if (!is_valid_work_time((*pptr)->data.date))
                    {
                        printf("\n???????????????????????????\n");
                        printf("?????§¹???: 2023-10-25 09:30\n");
                        printf("??????????(y/n): ");
                        fgets(input_buffer, sizeof(input_buffer), stdin);
                        if (input_buffer[0] != 'y' && input_buffer[0] != 'Y')
                        {
                            return;
                        }
                        continue;
                    }

                    // ?????????????????????????????
                    struct tm parcelin_tm;
                    if (!parse_time((*pptr)->data.parcelin_time, &parcelin_tm, "%d-%d-%d %d:%d"))
                    {
                        printf("????????????\n");
                        return;
                    }
                    time_t parcelin_time = mktime(&parcelin_tm);

                    struct tm input_tm;
                    if (!parse_time((*pptr)->data.date, &input_tm, "%d-%d-%d %d:%d"))
                    {
                        printf("???????????\n");
                        return;
                    }
                    input_tm.tm_sec = 0;
                    time_t input_time = mktime(&input_tm);

                    if (input_time <= parcelin_time)
                    {
                        printf("\n?????????????????????\n");
                        printf("??????: %s\n", (*pptr)->data.parcelin_time);
                        printf("??????????(y/n): ");
                        fgets(input_buffer, sizeof(input_buffer), stdin);
                        if (input_buffer[0] != 'y' && input_buffer[0] != 'Y')
                        {
                            return;
                        }
                        continue;
                    }

                    time_t max_time = parcelin_time + 3 * 24 * 60 * 60;
                    if (input_time > max_time)
                    {
                        printf("\n????????????????????????\n");
                        printf("??????: %s\n", (*pptr)->data.parcelin_time);
                        printf("??????????(y/n): ");
                        fgets(input_buffer, sizeof(input_buffer), stdin);
                        if (input_buffer[0] != 'y' && input_buffer[0] != 'Y')
                        {
                            return;
                        }
                        continue;
                    }

                    valid = 1;
                } while (valid!=1);

                (*pptr)->data.to_building = 'y';
                (*pptr)->data.fee = 3.00;
                printf("\n???????????????\n");
                printf("??????3.00\n");
                save_package_list(*root, "package.txt");
                system("pause");
                return;
            }
            else if (choice == 2 && strcmp(u->phonenumber, (*pptr)->data.account) != 0)
            {
                printf("???????????\n");
                system("pause");
                return;
            }

            else if (choice == 1)
            {
                // ??????????
                time_t now = time(NULL);
                struct tm *local = localtime(&now);
                char time_str[20];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);
                strcpy((*pptr)->data.parcelout_time, time_str);
                strcpy((*pptr)->data.oper, u->name);
                (*pptr)->data.to_building = 'n';

                // ???›Ô??????
                FILE *out = fopen("parcelout.txt", "a"); // ????????
                if (out)
                {
                    fprintf(out, "%s\t%s\t%s\t%s\t%s\t%s\t%c\t%s\t%.2f\t%s\n",
                            (*pptr)->data.type,
                            (*pptr)->data.recipient,
                            (*pptr)->data.account,
                            (*pptr)->data.pickupCode,
                            (*pptr)->data.parcelin_time,
                            (*pptr)->data.parcelout_time,
                            (*pptr)->data.to_building,
                            (*pptr)->data.date,
                            (*pptr)->data.fee,
                            (*pptr)->data.oper);
                    fclose(out);
                }
                if (strcmp(u->phonenumber, (*pptr)->data.account) != 0)
                {
                    printf("\n????????\n");
                }
                else
                {
                    printf("\n????????????\n");
                }
                // ???????????
                struct package_node *temp = *pptr;
                *pptr = temp->next;
                free(temp);
                save_package_list(*root, "package.txt");
                system("pause");
                return;
            }
        }
        pptr = &(*pptr)->next;
    }
    printf("\n??§¹???????\n");
    system("pause");
}
void free_package_list(struct package_node *root)
{
    while (root != NULL)
    {
        struct package_node *temp = root;
        root = root->next;
        free(temp);
    }
}

void check_package(struct package_node *root)
{
    system("cls");
    printf("----------????????----------\n");
    printf("%-20s %-20s %-20s %-10s\n", "????", "?????", "???", "?????");

    struct package_node *current = root;
    while (current != NULL)
    {
        printf("%-20s %-20s %-20s %-10s\n",
               current->data.type,
               current->data.recipient,
               current->data.account,
               current->data.pickupCode);
        current = current->next;
    }

    printf("\n");
    system("pause");
}

void notify_overdue_parcels(struct package_node *head)
{
    time_t now = time(NULL);
    int count = 0;
    struct package_node *current = head; // ???????????????
    printf("????????????????????\n\n");
    while (current != NULL)
    {
        struct tm in_time_tm;
        memset(&in_time_tm, 0, sizeof(in_time_tm));
        sscanf(current->data.parcelin_time, "%4d-%2d-%2d %2d:%2d:%2d",
               &in_time_tm.tm_year, &in_time_tm.tm_mon, &in_time_tm.tm_mday,
               &in_time_tm.tm_hour, &in_time_tm.tm_min, &in_time_tm.tm_sec);
        in_time_tm.tm_year -= 1900; // tm_year ??? 1900 ?????
        in_time_tm.tm_mon -= 1;     // tm_mon ??? 0 ?????

        time_t in_time = mktime(&in_time_tm);

        double seconds = difftime(now, in_time);
        int days = (int)(seconds / (60 * 60 * 24));
        if (days >= 3)
        {
            printf("???ID: %s, ?????: %s, ??????: %s",
                   current->data.account, current->data.recipient, current->data.parcelin_time);
            printf("????%s?????????????? %d ?????????\n\n",
                   current->data.recipient, days);
            count++;
        }
        current = current->next;
    }
    if (count == 0)
    {
        printf("????????\n");
    }
    system("pause");
}

// ??????????????
void find_package(struct package_node *package_list)
{
    char pickupCode[12];
    printf("???????????????????: ");
    scanf("%s", pickupCode);
    clear_input_buffer();
    int found = 0;
    struct package_node *current = package_list;
    while (current != NULL)
    {
        if (strcmp(pickupCode, current->data.pickupCode) == 0)
        {
            found = 1;
            printf("%-20s %-20s %-20s %-10s\n", "????", "?????", "???", "?????");
            printf("%-20s %-20s %-20s %-10s\n",
                   current->data.type,
                   current->data.recipient,
                   current->data.account,
                   current->data.pickupCode);
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printf("????????\n");
    }
    system("pause");
}
void find_user_package(struct package_node *package_list,user *u)
{
    struct package_node *current = package_list;
    int found=0;
    while (current != NULL)
    {
        if (strcmp(u->phonenumber, current->data.account) == 0)
        {
            found=1;
            printf("%-20s %-20s %-20s %-10s\n", "????", "?????", "???", "?????");
            printf("%-20s %-20s %-20s %-10s\n",
                   current->data.type,
                   current->data.recipient,
                   current->data.account,
                   current->data.pickupCode);
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printf("???????????\n");
    }
    system("pause");
}
// ????????
shipment_node* create_shipment_node(shipment* data)
{
    shipment_node* pnew = (shipment_node*)malloc(sizeof(shipment_node));
    if (pnew == NULL)
    {
        printf("?????????");
        return NULL;
    }
    strcpy(pnew->data.sender, data->sender);
    strcpy(pnew->data.sender_phone, data->sender_phone);
    strcpy(pnew->data.receiver, data->receiver);
    strcpy(pnew->data.receiver_phone, data->receiver_phone);
    strcpy(pnew->data.address, data->address);
    strcpy(pnew->data.type_shipment, data->type_shipment);
    pnew->data.is_to_building = data->is_to_building;
    pnew->data.weight = data->weight;
    strcpy(pnew->data.payment, data->payment);
    pnew->data.payment_amount = data->payment_amount;
    strcpy(pnew->data.shipment_code, data->shipment_code);
    strcpy(pnew->data.status, data->status);
    pnew->next = NULL;
    return pnew;
}

// ???????????¦Â
void append_shipment(shipment_node **head, shipment *data)
{
    if (head == NULL)
    {
        return;
    }
    if (*head == NULL)
    {
        *head = create_shipment_node(data);
        return;
    }
    shipment_node *current = *head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = create_shipment_node(data);
}
bool is_code_exists(shipment_node *head, const char *code)
{
    shipment_node *current = head;
    while (current != NULL)
    {
        if (strcmp(current->data.shipment_code, code) == 0)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

// ????¦·??????
void generate_shipment_code(shipment_node *head, char *code)
{
    do
    {
        int num = rand() % 10000;
        sprintf(code, "%04d", num);
    } while (is_code_exists(head, code));
}

// ???????????
void load_from_shipment(shipment_node** head, const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("????????");
        return;
    }
    char line[512];
    while (fgets(line, sizeof(line), file))
    {
        shipment s = { 0 };
        int field_count = 0;

        char* token = strtok(line, "|"); // ????????????
        while (token && field_count < 12)
        {
            switch (field_count)
            {
            case 0:
                strncpy(s.sender, token, sizeof(s.sender) - 1);
                s.sender[sizeof(s.sender) - 1] = '\0';
                break;
            case 1:
                strncpy(s.sender_phone, token, sizeof(s.sender_phone) - 1);
                s.sender_phone[sizeof(s.sender_phone) - 1] = '\0';
                break;
            case 2:
                strncpy(s.receiver, token, sizeof(s.receiver) - 1);
                s.receiver[sizeof(s.receiver) - 1] = '\0';
                break;
            case 3:
                strncpy(s.receiver_phone, token, sizeof(s.receiver_phone) - 1);
                s.receiver_phone[sizeof(s.receiver_phone) - 1] = '\0';
                break;
            case 4:
                strncpy(s.address, token, sizeof(s.address) - 1);
                s.address[sizeof(s.address) - 1] = '\0';
                break;
            case 5:
                strncpy(s.type_shipment, token, sizeof(s.type_shipment) - 1);
                s.type_shipment[sizeof(s.type_shipment) - 1] = '\0';
                break;
            case 6:
                s.is_to_building = token[0];
                break;
            case 7:
                s.weight = atof(token);
                break;
            case 8:
                strncpy(s.payment, token, sizeof(s.payment) - 1);
                s.payment[sizeof(s.payment) - 1] = '\0';
                break;
            case 9:
                s.payment_amount = atof(token);
                break;
            case 10:
                strncpy(s.shipment_code, token, sizeof(s.shipment_code) - 1);
                s.shipment_code[sizeof(s.shipment_code) - 1] = '\0';
                break;
            case 11:
                strncpy(s.status, token, sizeof(s.status) - 1);
                s.status[sizeof(s.status) - 1] = '\0';
                break;
            }
            token = strtok(NULL, "|");
            field_count++;
        }
        if (field_count == 12)
        {
            append_shipment(head, &s);
        }
    }
    fclose(file);
}
// ????????????
void save_to_shipmentfile(shipment_node* head, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("????????");
        return;
    }

    shipment_node* current = head;
    while (current != NULL)
    {
        fprintf(file, "%s|%s|%s|%s|%s|%s|%c|%.2f|%s|%.2f|%s|%s|\n",
            current->data.sender,
            current->data.sender_phone,
            current->data.receiver,
            current->data.receiver_phone,
            current->data.address,
            current->data.type_shipment,
            current->data.is_to_building,
            current->data.weight,
            current->data.payment,
            current->data.payment_amount,
            current->data.shipment_code,
            current->data.status);
        current = current->next;
    }
    fclose(file);
}
// ??????????
void free_shipment_list(shipment_node *head)
{
    shipment_node *current = head;
    while (current != NULL)
    {
        shipment_node *temp = current;
        current = current->next;
        free(temp);
    }
}
// ??????????

// ??????????
bool validate_payment(const char *payment)
{
    if (strcmp(payment, "???") == 0 || strcmp(payment, "????") == 0 || strcmp(payment, "??") == 0)
    {
        return true;
    }
    return false;
}

// ?????§Û????????????
const char *provinces[] = {
    "????", "???", "???", "????",
    "???", "???", "????", "????", "??????",
    "????", "??", "????", "????", "????", "???",
    "????", "????", "????", "??", "????",
    "???", "????", "????", "????", "????",
    "??", "???",
    "?????", "????", "????", "????", "???",
    "???", "????"};

bool validate_province(const char *province)
{
    if (province == NULL || strlen(province) == 0)
    {
        return false;
    }

    for (size_t i = 0; i < sizeof(provinces) / sizeof(provinces[0]); i++)
    {
        if (strcmp(province, provinces[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// ????????¨²???
float calculate_formula(int flag, int weight, int first_weight, int add_weight,char type_shipment[], char to_building)
{
    float total_cost = 0.0;

    // ???????¨¹???
    if (weight <= 1)
    {
        total_cost = first_weight; // ????1???????????
    }
    else
    {
        total_cost = first_weight + (weight - 1) * add_weight; // ????1????????????????
    }

    // ??????
    if (flag == 1)
    {
        total_cost *= 0.8; // ???????8??
    }
    else if(flag==2)
    {
        total_cost *= 0.7;
    }
    else if(flag==3)
    {
        total_cost *= 0.6;
    }
    else if(flag==4)
    {
        total_cost *= 0.5;
    }
    // ??9???1????
    if (total_cost >= 9)
    {
        total_cost -= 1;
    }

    if (to_building == 'y')
    {
        total_cost += 3;
    }
    if (strcmp(type_shipment, "??????") == 0) {
        total_cost *= 0.8;
    }
    if (strcmp(type_shipment, "????????") == 0) {
        total_cost *= 1.2;
    }
    if (strcmp(type_shipment, "????????") == 0) {
        total_cost *= 1.5;
    }
    if (strcmp(type_shipment, "???????") == 0) {
        total_cost *= 2.0;
    }
    return total_cost;
}
float calculateCost(shipment root,user *u)
{
    // ???????????????
    Province provinces[] = {
        {"????", 8, 2},  // ???????
        {"????", 10, 3}, // ?????
        {"??????", 10, 3},
        {"?????", 10, 3},
        {"???", 12, 5}, // ?????
        {"????", 12, 5},
        {"???", 12, 5},
        {"???", 12, 5},
        {"???", 12, 5},
        {"????", 12, 5},
        {"????", 12, 5},
        {"????", 12, 5},
        {"????", 12, 5},
        {"???", 15, 8}, // ??????
        {"??", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"???", 15, 8},
        {"????", 15, 8},
        {"??", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"??", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"????", 15, 8},
        {"???", 15, 8},
        {"???", 40, 15}, // ?????
        {"????", 40, 15},
        {"???", 50, 20}};

    int num_provinces = sizeof(provinces) / sizeof(Province); // ???????

    // ???????????????????
    char input_province[20];
    int weight, flag;
    char to_building; // ??????????
    char type_shipment[20];
    // ???
    sscanf(root.address, "%[^,]", input_province); // ?????????????
    // ????????
    weight = root.weight;
    // ???
    flag =u->isvip;
    strcpy(type_shipment, root.type_shipment);//???????
    // ???????
    to_building = root.is_to_building;

    // ??????????????????
    for (int i = 0; i < num_provinces; i++)
    {
        if (strcmp(input_province, provinces[i].province) == 0)
        {
            // ??????????
            float cost = calculate_formula(flag, weight, provinces[i].first_weight, provinces[i].add_weight,type_shipment, to_building);
            return cost;
        }
    }
    return -1;
}
// ??§Õ???????????
void write_shipment(shipment_node** root, user* u)
{
    srand(time(NULL));
    char flag = 'y';
    shipment_node* pList = NULL;
    load_from_shipment(&pList, "manage_system_code/shipment.txt");
    while (flag == 'y' || flag == 'Y')
    {
        shipment new_shipment = { 0 };
        system("cls");
        printf("\n???????????:\n");

        // ??????
        printf("??????????????:");
        fgets(new_shipment.sender, sizeof(new_shipment.sender), stdin);
        new_shipment.sender[strcspn(new_shipment.sender, "\n")] = '\0';

        // ????????´Â?????
        do
        {
            printf("??????????´Â(11¦Ë????):");
            fgets(new_shipment.sender_phone, sizeof(new_shipment.sender_phone), stdin);
            new_shipment.sender_phone[strcspn(new_shipment.sender_phone, "\n")] = '\0';
            if (!validate_phonenumber(new_shipment.sender_phone))
            {
                printf("?´Â????????????????????\n");
            }
        } while (!validate_phonenumber(new_shipment.sender_phone));

        printf("???????????????:");
        fgets(new_shipment.receiver, sizeof(new_shipment.receiver), stdin);
        new_shipment.receiver[strcspn(new_shipment.receiver, "\n")] = '\0';

        // ?????????´Â?????
        do
        {
            printf("???????????´Â(11¦Ë????):");
            fgets(new_shipment.receiver_phone, sizeof(new_shipment.receiver_phone), stdin);
            new_shipment.receiver_phone[strcspn(new_shipment.receiver_phone, "\n")] = '\0';
            if (!validate_phonenumber(new_shipment.receiver_phone))
            {
                printf("?´Â????????????????????\n");
            }
        } while (!validate_phonenumber(new_shipment.receiver_phone));


        //??????????
        while (1) {
            printf("????????????/??????/????????/????????/???????: ");
            fgets(new_shipment.type_shipment, sizeof(new_shipment.type_shipment), stdin);
            new_shipment.type_shipment[strcspn(new_shipment.type_shipment, "\n")] = '\0';

            if ((strcmp(new_shipment.type_shipment, "??????") == 0 ||
                strcmp(new_shipment.type_shipment, "??????") == 0 ||
                strcmp(new_shipment.type_shipment, "????????") == 0 ||
                strcmp(new_shipment.type_shipment, "????????") == 0 ||
                strcmp(new_shipment.type_shipment, "???????") == 0)) {
                break;
            }
            else {
                printf("\n???????\n");
            }
        }



        // ??????????????????
        do
        {
            printf("????????????(\"????\", \"???\", \"???\", \"????\", \"???\", \"???\", \"????\", \"????\", \"??????\", \"????\", \"??\", \"????\", \"????\", \"????\", \"???\");\n");
            printf("\"????\", \"????\", \"????\", \"??\", \"????\", \"???\", \"????\", \"????\", \"????\", \"????\", \"??\", \"???\", \"?????\", \"????\", \"????\", \"????\", \"???\", \"???\", \"????\")");
            fgets(new_shipment.address, sizeof(new_shipment.address), stdin);
            new_shipment.address[strcspn(new_shipment.address, "\n")] = '\0';

            // ??????????
            char province[50];
            sscanf(new_shipment.address, "%[^,]", province); // ?????????????

            if (!validate_province(province))
            {
                printf("?????????§¹????????????\n");
            }
        } while (!validate_province(new_shipment.address));

        printf("????????????????('y'/'n'):");
        scanf("%c", &new_shipment.is_to_building);
        clear_input_buffer();

        // ??????????????áÃ??
        printf("????????(kg):");
        scanf("%f", &new_shipment.weight);
        clear_input_buffer(); // ??????1?????????????????§Ù?

        // ????????????
        do
        {
            printf("??????????/????/???:");
            fgets(new_shipment.payment, sizeof(new_shipment.payment), stdin);
            new_shipment.payment[strcspn(new_shipment.payment, "\n")] = '\0';
            if (!validate_payment(new_shipment.payment))
            {
                printf("???????§¹?????????????????????????????\n");
            }
        } while (!validate_payment(new_shipment.payment));

        calculateCost(new_shipment, u);
        // ????????????????
        generate_shipment_code(pList, new_shipment.shipment_code);
        new_shipment.payment_amount = calculateCost(new_shipment, u);

        strcpy(new_shipment.status, "?????");

        // ?????????
        append_shipment(&pList, &new_shipment);
        save_to_shipmentfile(pList, "manage_system_code/shipment.txt");
        
        // ??????
        printf("?????%s\n", new_shipment.shipment_code);
        printf("??????%.2f?\n", new_shipment.payment_amount);

        // ???????????
        printf("?????????(????'y'??'Y'???????????????): ");
        scanf(" %c", &flag);
        clear_input_buffer();
    }
    *root = pList;
}
// ???????1

void display_shipment(shipment* s)
{
    printf("\n=== ?????? ===\n");
    printf("?????: %-20s ?´Â: %-15s\n", s->sender, s->sender_phone);
    printf("?????: %-20s ?´Â: %-15s\n", s->receiver, s->receiver_phone);
    printf("???: %s\n", s->address);
    printf("????????%s\n", s->type_shipment);
    printf("??????????: %c\n", s->is_to_building);
    printf("????: %.2fkg  ????: ??%.2f\n", s->weight, s->payment_amount);
    printf("?????: %-8s ??: %-10s\n", s->payment, s->status);
    printf("?????: %s\n", s->shipment_code);
}

void check_shipment(shipment_node *pending, shipment_node *completed)
{
    printf("\n=== ?????????? ===\n");
    while (pending)
    {
        display_shipment(&pending->data);
        pending = pending->next;
    }

    printf("\n=== ???????? ===\n");
    while (completed)
    {
        display_shipment(&completed->data);
        completed = completed->next;
    }
    system("pause");
}

void process_shipment(shipment_node **pending, shipment_node **completed)
{
    if (!*pending)
    {
        printf("??§Õ???????????\n");
        return;
    }

    char code[10];
    printf("??????????: ");
    scanf("%4s", code);
    clear_input_buffer();

    shipment_node **pp = pending;
    while (*pp)
    {
        if (strcmp((*pp)->data.shipment_code, code) == 0)
        {
            shipment_node *processed = *pp;
            *pp = processed->next; // ?? pending ???
            // ????? completed ???????
            processed->next = *completed;
            *completed = processed;
            strcpy(processed->data.status, "????");
            save_to_shipmentfile(*pending, "manage_system_code/shipment.txt");
            save_to_shipmentfile(*completed, "manage_system_code/completed.txt");
            printf("???? %s ?????????\n", code);
            system("pause");
            return;
        }
        pp = &(*pp)->next;
    }
    printf("¦Ä???????? %s!\n", code);
    system("pause");
}
// ??????????§Ñ???????
int count_package_from_list(struct package_node *root) {
    int count = 0;
    while (root != NULL) {
        count++;
        root = root->next;
    }
    return count;
}
//??????
void show_stats(shipment_node* pending_shipment, struct package_node *package_list) {
    while(1) {
        system("cls");
        printf("====== ?????? ======\n");
        printf("1. ???????????\n");
        printf("2. ?????????????\n");
        printf("3. ????????????\n");
        printf("0. ??????????\n");
        printf("????????: ");
        
        int choice=-1;
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("??????§¹!\n");
            Sleep(1000);
            continue;
        }
        clear_input_buffer();

        // ?????????????????
        int package_count = count_package_from_list(package_list);
        int shipment_count = count_shipments(pending_shipment);
        
        switch(choice) {
            case 1:
                printf("\n?????????: %d\n", package_count);
                break;
            case 2:
                printf("\n??????????????: %d\n", shipment_count);
                break;
            case 3: {
                int remaining = 500 - package_count - shipment_count;
                printf("\n?????????????: %d\n", remaining);
                if (remaining < 0) {
                    printf("??????????????????\n");
                } else if (remaining < 50) {
                    printf("???????????????\n");
                } else {
                    printf("??????\n");
                }
                break;
            }
            case 0:
                return;
            default:
                printf("\n??§¹???!\n");
        }
        
        printf("\n");
        system("pause");
    }
}
struct user_node *Create_user_node(user *data)
{
    struct user_node *pnew = (struct user_node *)malloc(sizeof(struct user_node));
    if (pnew == NULL)
    {
        printf("??????????\n");
        return NULL;
    }
    strcpy(pnew->data.phonenumber, data->phonenumber);
    strcpy(pnew->data.password, data->password);
    strcpy(pnew->data.name, data->name);
    pnew->data.isvip = data->isvip;
    strcpy(pnew->data.join_date, data->join_date);
    strcpy(pnew->data.expire_date, data->expire_date);
    pnew->next = NULL;
    return pnew;
}
void append_user(struct user_node **root, user *data)
{
    if (root == NULL)
        return;
    if (*root == NULL)
    {
        *root = Create_user_node(data);
        return;
    }
    struct user_node *ptemp = *root;
    while (ptemp->next)
    { // ????????????
        ptemp = ptemp->next;
    }
    ptemp->next = Create_user_node(data);
}
void free_user_list(struct user_node **root) // ??????????
{
    if (root == NULL)
        return;
    struct user_node *current = *root;
    while (current != NULL)
    {
        struct user_node *next = current->next;
        free(current);
        current = next;
    }
    *root = NULL;
}
void travel_user() // ????
{
    FILE *fp = fopen("manage_system_code/user.txt", "r");
    if (fp == NULL)
    {
        printf("?????????\n");
        Sleep(1000);
        return;
    }
    struct user_node *pList = NULL;
    user temp; // ????›¥????????????
    while (fscanf(fp, "%s %s %s %d %s %s", temp.phonenumber, temp.password, temp.name, &temp.isvip, temp.join_date, temp.expire_date) != EOF)
    {
        append_user(&pList, &temp);
    }
    fclose(fp);
    if (pList == NULL)
    {
        printf("????????\n");
        return;
    }
    printf("%-12s %-20s %-18s %-4s %-12s %-12s\n", "?????", "????", "????", "VIP", "???????", "????????");
    struct user_node *ptemp = pList;
    while (ptemp)
    {
        printf("%-12s %-20s %-18s %-4d %-12s %-12s\n",
               ptemp->data.phonenumber,
               ptemp->data.password,
               ptemp->data.name,
               ptemp->data.isvip,
               ptemp->data.join_date,
               ptemp->data.expire_date);
        ptemp = ptemp->next;
    }
    system("pause");
}
void find_user(struct user_node *root, char phonenumber[])
{
    if (root == NULL)
    {
        printf("????????\n");
        return;
    }
    struct user_node *ptemp = root;
    while (ptemp)
    {
        if (strcmp(ptemp->data.phonenumber, phonenumber) == 0)
        {
            printf("??????????????\n");
            printf("%-12s %-20s %-18s %-4s %-12s %-12s\n",
                   "?????", "????", "????", "VIP", "???????", "????????");
            printf("%-12s %-20s %-18s %-4d %-12s %-12s\n",
                   ptemp->data.phonenumber,
                   ptemp->data.password,
                   ptemp->data.name,
                   ptemp->data.isvip,
                   ptemp->data.join_date,
                   ptemp->data.expire_date);
            system("pause");
            return;
        }
        ptemp = ptemp->next;
    }
    printf("¦Ä??????? %s ???????\n", phonenumber);
}
void show_user()
{
    FILE *fp = fopen("manage_system_code/user.txt", "r");
    if (fp == NULL)
    {
        printf("?????????\n");
        Sleep(1000);
        return;
    }
    struct user_node *pList = NULL;
    user temp; // ????›¥????????????
    while (fscanf(fp, "%s %s %s %d %s %s", temp.phonenumber, temp.password,temp.name,&temp.isvip,temp.join_date,temp.expire_date) != EOF)
    {
        append_user(&pList, &temp);
    }
    fclose(fp);
    char phonenumber[12] = {0};
    int attempts = 3;
    while (attempts > 0)
    {
        printf("?????????????????(11¦Ë????): ");
        if (scanf("%11s", phonenumber) != 1)
        {
            printf("?????????????????\n");
            // ???????????
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            attempts--;
            continue;
        }
        // ????????????§Ø???????
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        if (!validate_phonenumber(phonenumber))
        {
            printf("??§¹?????????!?????11¦Ë??????\n");
            attempts--;
            if (attempts > 0)
            {
                printf("?????? %d ?¦Ã??????\n", attempts);
            }
            continue;
        }
        break;
    }
    if (attempts == 0)
    {
        printf("\n????:?????????????,?????????\n");
        free_user_list(&pList);
        return;
    }
    find_user(pList, phonenumber);
    free_user_list(&pList);
}
// ??????§Ø???????????????????????
int UserdataFile_read(user *u)
{
    FILE *fp;
    char line[256];
    int check = 0;

    if ((fp = fopen("manage_system_code/user.txt", "r")) == NULL)
    {
        perror("???????????");
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        line[strcspn(line, "\n")] = '\0'; // ??????§Ù?

        // ????????›¥??????§Ø????????
        user temp;

        // ???????????????????
        if (sscanf(line, "%11s %8s %19s %d %10s %10s",
                   temp.phonenumber,
                   temp.password,
                   temp.name,
                   &temp.isvip,
                   temp.join_date,
                   temp.expire_date) == 6)
        {
            // ???????????????????
            if (strcmp(temp.phonenumber, u->phonenumber) == 0 &&
                strcmp(temp.password, u->password) == 0)
            {
                // ??????????????????? u ??
                *u = temp;
                check = 1;
                break;
            }
        }
    }

    fclose(fp);
    return check;
}
// §Õ?????????????
void UserdataFile_write(user *u)
{
    FILE *fp;
    if ((fp = fopen("manage_system_code/user.txt", "a")) == NULL)
    { // ????????????
        perror("???????????");
        return;
    }

    // §Õ?????????????????????????
    fprintf(fp, "%s %s %s %d %s %s\n",
            u->phonenumber,                                 // ?????
            u->password,                                    // ????
            u->name,                                        // ????                                  // ???
            u->isvip,                                       
            u->join_date[0] ? u->join_date : "¦Ä??§Õ",      // ???????
            u->expire_date[0] ? u->expire_date : "¦Ä??§Õ"); // ????????

    fclose(fp);
}
// ???????????????
void UpdateUserInFile(user *u) {
    FILE *fp = fopen("manage_system_code/user.txt", "r");
    FILE *temp_fp = fopen("manage_system_code/temp.txt", "w");
    
    if (fp == NULL || temp_fp == NULL) {
        perror("????????");
        return;
    }

    char line[256];
    user temp;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%s %s %s %d %s %s",  // ??????recharge()??????????
               temp.phonenumber,
               temp.password,
               temp.name,
               &temp.isvip,
               temp.join_date,
               temp.expire_date);

        if (strcmp(temp.phonenumber, u->phonenumber) == 0) {
            // ?????fscanf??????§Õ????
            fprintf(temp_fp, "%s %s %s %d %s %s\n",  // ??????§Ú????????
                    u->phonenumber,
                    u->password,
                    u->name,
                    u->isvip,
                    u->join_date,
                    u->expire_date);
        } else {
            fputs(line, temp_fp); // ???§Õ????????
        }
    }

    fclose(fp);
    fclose(temp_fp);
    remove("manage_system_code/user.txt");
    rename("manage_system_code/temp.txt", "manage_system_code/user.txt");
}
int IsPhoneNumberRegistered(const char *phonenumber) {
    FILE *fp = fopen("manage_system_code/user.txt", "r");
    if (fp == NULL) return 0; // ????????????¦Ä???

    char line[256];
    int registered = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        char stored_phone[12];
        if (sscanf(line, "%11s", stored_phone) == 1) { // ?????????
            if (strcmp(stored_phone, phonenumber) == 0) {
                registered = 1;
                break;
            }
        }
    }
    
    fclose(fp);
    return registered;
}
void delete_user_from_file(const char *phonenumber) 
{
    FILE *fp = fopen("manage_system_code/user.txt", "r");
    if (fp == NULL) 
    {
        perror("???????????????");
        return;
    }
    FILE *temp_fp = fopen("manage_system_code/temp.txt", "w");
    if (temp_fp == NULL) 
    {
        perror("??????????????");
        fclose(fp);
        return;
    }
    char line[256];
    user temp;
    int found = 0;
    while (fgets(line, sizeof(line), fp)) 
    {
        sscanf(line, "%11s %8s %19s %d %10s %10s",
               temp.phonenumber,
               temp.password,
               temp.name,
               &temp.isvip,
               temp.join_date,
               temp.expire_date);

        // ??????????????????§Õ????????
        if (strcmp(temp.phonenumber, phonenumber) != 0) 
        {
            fputs(line, temp_fp);
        } 
        else 
        {
            found = 1; // ?????????????
        }
    }
    fclose(fp);
    fclose(temp_fp);
    if (found) 
    {
        // ????????????????????I
        remove("manage_system_code/user.txt");
        rename("manage_system_code/temp.txt", "manage_system_code/user.txt");
    } 
    else 
    {
        remove("manage_system_code/temp.txt"); // ??????????????????
    }
}
// ??????????§Õ?????????
void Fillout(user *u) {
    // ???????????????
    while (1) {
        printf("?????????????(11¦Ë): ");
        fgets(u->phonenumber, sizeof(u->phonenumber), stdin);
        // ????????????????
        if (strchr(u->phonenumber, '\n') == NULL) {
            // ????§Ó??????????????????
            printf("???????????11¦Ë??\n");
            // ????????
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        
        u->phonenumber[strcspn(u->phonenumber, "\n")] = '\0';
        
        if (!validate_phonenumber(u->phonenumber)) {
            printf("????????????????????????\n");
        } else {
            break;
        }
    }
    // ????????????????
    if (IsPhoneNumberRegistered(u->phonenumber)) {
        printf("?????????????????????\n");
        Sleep(2000);
        return;
    }
    // ??????????????
    char confirm_password[20]; // ???????????
    while (1) {
        // ????????
        while (1) {
            printf("??????????(6-7¦Ë): ");
            fgets(u->password, sizeof(u->password), stdin);
            if (strchr(u->password, '\n') == NULL) {
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
            }
            u->password[strcspn(u->password, "\n")] = '\0';
            int len = strlen(u->password);
            if (len >= 6 && len < 8) {
                break;
            }
            printf("????????????????????????\n");
        }
        // ???????
        printf("????????????????: ");
        fgets(confirm_password, sizeof(confirm_password), stdin);
        if (strchr(confirm_password, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        confirm_password[strcspn(confirm_password, "\n")] = '\0';

        // ????????????????
        if (strcmp(u->password, confirm_password) == 0) {
            break;
        } else {
            printf("???????????????????????????????\n");
        }
    }
    // ???????????????
    while (1) {
        printf("??????????????(1-10?????): ");
        fgets(u->name, sizeof(u->name), stdin);
        if (strchr(u->name, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        } 
        u->name[strcspn(u->name, "\n")] = '\0';
        int len = strlen(u->name);
        if (len > 0 && len <= 10) {
            break;
        }
        printf("??????????????????????????\n");
    }
    // ????????????
    u->isvip = 0;
    strcpy(u->join_date, "¦Ä??§Õ");
    strcpy(u->expire_date, "¦Ä??§Õ");
    UserdataFile_write(u);
    printf("\n??????????%s\n", u->name);
    Sleep(2000);
}
// ?????
struct manager_node *Create_manager_node(struct manager *data)
{
    struct manager_node *pnew = (struct manager_node *)malloc(sizeof(struct manager_node));
    if (pnew == NULL)
    {
        printf("?????????!\n");
        return NULL;
    }
    strcpy(pnew->data.number, data->number);
    strcpy(pnew->data.password, data->password);
    pnew->next = NULL;
    return pnew;
}
void append_manager(struct manager_node **root, struct manager *data)
{
    if (root == NULL)
        return;
    if (*root == NULL)
    {
        *root = Create_manager_node(data);
        return;
    }
    struct manager_node *ptemp = *root;
    while (ptemp->next)
    {
        ptemp = ptemp->next;
    }
    ptemp->next = Create_manager_node(data);
}
void add_manager(struct manager_node **root)
{
    char selectflag = 'y';
    FILE *fp = fopen("manage_system_code/manager.txt", "r");
    if (fp == NULL)
    {
        printf("????????!\n");
        Sleep(1000);
        return;
    }
    struct manager_node *pList = NULL;
    struct manager temp;
    while (fscanf(fp, "%s %s", temp.number, temp.password) != EOF)
    {
        append_manager(&pList, &temp);
    }
    fclose(fp);
    while (selectflag == 'y' || selectflag == 'Y')
    {
        struct manager new_manager;
        system("cls");
        printf("?????????????: ");
        scanf("%19s", new_manager.number); // ??????????????????
        printf("??????????????: ");
        scanf("%19s", new_manager.password);
        // ????????????????????
        append_manager(&pList, &new_manager);
        printf("??????????????!\n");
        printf("?????????(y????????): ");
        scanf(" %c", &selectflag); // ?????????????§Ù?
    }
    fp = fopen("manage_system_code/manager.txt", "w");
    if (fp == NULL)
    {
        printf("??????????\n");
        return;
    }
    struct manager_node *ptemp = pList;
    while (ptemp)
    {
        fprintf(fp, "%s %s\n", ptemp->data.number, ptemp->data.password);
        ptemp = ptemp->next;
    }
    fclose(fp);
    *root = pList;
}
void delete_manager(struct manager_node **root)
{
    char selectflag = 'y';
    FILE *fp = fopen("manage_system_code/manager.txt", "r");
    if (fp == NULL)
    {
        printf("????????!\n");
        Sleep(1000);
        return;
    }
    struct manager_node *pList = NULL;
    struct manager temp;
    while (fscanf(fp, "%s %s", temp.number, temp.password) != EOF)
    {
        append_manager(&pList, &temp);
    }
    fclose(fp);
    char number[20];
    while (selectflag == 'y' || selectflag == 'Y')
    {
        system("cls");
        printf("?????????????: ");
        scanf("%19s", number); // ??????????????????
        struct manager_node *ptemp = pList;
        struct manager_node *prev = NULL;
        int found = 0; // ?????????????????
        while (ptemp)
        {
            if (strcmp(ptemp->data.number, number) == 0)
            {
                found = 1;
                if (prev == NULL)
                {
                    pList = ptemp->next; // ???????
                }
                else
                {
                    prev->next = ptemp->next; // ????§Þ??¦Â???
                }
                free(ptemp);
                printf("?????????????!\n");
                break;
            }
            prev = ptemp;
            ptemp = ptemp->next;
        }
        if (!found)
        {
            printf("¦Ä????¨´???????!\n");
        }
        printf("?????????(y????????): ");
        scanf(" %c", &selectflag); // ?????????????§Ù?
    }
    fp = fopen("manage_system_code/manager.txt", "w");
    if (fp == NULL)
    {
        printf("??????????\n");
        return;
    }
    struct manager_node *ptemp = pList;
    while (ptemp)
    {
        fprintf(fp, "%s %s\n", ptemp->data.number, ptemp->data.password);
        ptemp = ptemp->next;
    }
    fclose(fp);
    *root = pList;
}
// ???????????
void checkExpiredMembers(user *head)
{
    time_t t = time(NULL); // ?????????
    struct tm *current_time = localtime(&t);
    char current_date[11];
    strftime(current_date, sizeof(current_date), "%Y-%m-%d", current_time); // ??????????????? YYYY-MM-DD
    user *temp = head;
    while (temp != NULL)
    {
        if (temp->isvip)
        { // ??????????
            struct tm expire_tm = {0};
            if (strptime(temp->expire_date, "%Y-%m-%d", &expire_tm) != NULL)
            { // ?? expire_date ???? struct tm
                if (difftime(mktime(current_time), mktime(&expire_tm)) > 0)
                { // ?§Ø?????????
                    printf("?????????%s ??????????????????????????\n", temp->phonenumber);
                    temp->isvip = 0; // ????????????????
                    strcpy(temp->join_date, "¦Ä??§Õ");
                    strcpy(temp->expire_date, "¦Ä??§Õ");

                    // ???????????????
                    UpdateUserInFile(temp);
                }
            }
        }
        temp = temp->next; // ???????????????????
    }
}
void recharge(user *u) {
    printf("\n????????");
    printf("\n%-12s %-8s", "?????", "??????");
    printf("\n%-12s", u->phonenumber);   
    // ?????????????
    switch(u->isvip) {
        case 0: printf("??????"); break;
        case 1: printf("???????"); break;
        case 2: printf("?????"); break;
        case 3: printf("??????"); break;
        case 4: printf("SVIP???"); break;
        default: printf("¦Ä?"); break;
    }
    if (u->isvip > 0) {
        printf("\n\n???????????");
        printf("\n????????");
        switch(u->isvip) {
            case 1: printf("???????(8?????)"); break;
            case 2: printf("?????(7?????)"); break;
            case 3: printf("??????(6?????)"); break;
            case 4: printf("SVIP???(5?????)"); break;
        }
        printf("\n????????%s", u->join_date);
        printf("\n?????????%s", u->expire_date);
        printf("\n\n[1]????1????(+5?)");
        printf("\n[2]??????????");
        printf("\n[0]???????");
        printf("\n??????????");        
        int choice=-1;
        scanf("%d", &choice);
        while (getchar() != '\n');
        if (choice == 1) {
            // ???????
            printf("\n???5????????");           
            // ???????????
            int year, month, day;
            sscanf(u->expire_date, "%d-%d-%d", &year, &month, &day);

            if (month == 12) {
                month = 1;
                year++;
            } else {
                month++;
            }
            sprintf(u->expire_date, "%04d-%02d-%02d", year, month, day);
            UpdateUserInFile(u);
            printf("\n???????????????????%s", u->expire_date);
        } 
        else if (choice == 2) {
            // ???????
            printf("\n???????????");
            switch(u->isvip) {
                case 1: printf("??????????ÙK10?"); break;
                case 2: printf("????????ÙK15?"); break;
                case 3: printf("?????SVIP?ÙK15?"); break;
                default: printf("?????????"); break;
            }          
            if (u->isvip <= 3) {
                printf("\n?????????(1=??? 0=???)??");
                int confirm;
                scanf("%d", &confirm);
                while (getchar() != '\n');
                if (confirm == 1) {
                    int upgrade_cost[] = {0, 10, 15, 15};
                    printf("\n???%d??????????", upgrade_cost[u->isvip]);
                    u->isvip++;
                    UpdateUserInFile(u);
                    printf("\n???????????");
                    switch(u->isvip) {
                        case 1: printf("???????"); break;
                        case 2: printf("?????"); break;
                        case 3: printf("??????"); break;
                        case 4: printf("SVIP???"); break;
                    }
                }
            }
        }
    } 
    else {
        // ????????
        printf("\n\n??????????????");
        printf("\n[1]??????????(5?/??)");
        printf("\n[2]????????(15?/??)");
        printf("\n[3]?????????(30?/??)");
        printf("\n[4]???SVIP???(45?/??)");
        printf("\n[0]???????");
        printf("\n?????");
        int choice=-1;
        scanf("%d", &choice);
        while (getchar() != '\n');
        if (choice >= 1 && choice <= 4) {
            int cost[] = {0, 5, 15, 30, 45};
            printf("\n???%d?????????", cost[choice]);         
            // ?????????
            time_t t = time(NULL);
            struct tm* current_time = localtime(&t);
            strftime(u->join_date, sizeof(u->join_date), "%Y-%m-%d", current_time);
            int year = current_time->tm_year + 1900;
            int month = current_time->tm_mon + 1;
            int day = current_time->tm_mday;
            if (month == 12) {
                month = 1;
                year++;
            } else {
                month++;
            }
            sprintf(u->expire_date, "%04d-%02d-%02d", year, month, day);
            u->isvip = choice;
            UpdateUserInFile(u);
            printf("\n??????");
            switch(u->isvip) {
                case 1: printf("???????"); break;
                case 2: printf("?????"); break;
                case 3: printf("??????"); break;
                case 4: printf("SVIP???"); break;
            }
            printf("\n?????????%s", u->expire_date);
        }
    }
    system("pause");
}
void SuperAdminSystem()
{
    int choice=-1;
    struct manager_node *root = NULL;

    while (1)
    {
        system("cls");
        printf("????????????:\n");
        printf("1. ????????\n");
        printf("2. ????????\n");
        printf("3. ?????????\n");
        printf("????????: ");
        scanf("%d", &choice);
        clear_input_buffer(); // ?????????

        switch (choice)
        {
        case 1:
            add_manager(&root); // ????????
            Sleep(1000);
            break;
        case 2:
            delete_manager(&root); // ????????
            Sleep(1000);
            break;
        case 3:
            return;
        default:
            printf("????????????????\n");
            Sleep(2000);
            break;
        }
    }
}
// ???????????
void AdminSystem(int is_super_admin, struct package_node *package_list, shipment_node *pending_shipment, shipment_node *completed_shipment, user *u)
{
    if (is_super_admin == 2)
    {
        while (1)
        {
            system("cls");
            printf("???????:\n");
            printf("1. ???\n");
            printf("2. ????\n");
            printf("3. ???\n");
            printf("4. ??????????\n");
            printf("5. ????????\n");
            printf("6. ??????????\n");
            printf("7. ???????????????????\n");
            printf("8. ??????????\n");
            printf("9. ??????????\n");
            printf("10. ??????\n");
            printf("11. ?????????\n");
            printf("????????: ");
            int admin_choice=-1;
            scanf("%d", &admin_choice);
            clear_input_buffer(); // ???????????
            switch (admin_choice)
            {
            case 1:
                add_package(&package_list); // ?????
                break;
            case 2:
                manager_pickup_package(&package_list); // ??????
                break;
            case 3:
            {
                int sub_choice=-1;
                while (1)
                {
                    system("cls");
                    printf("???????:\n");
                    printf("1. ??????\n");
                    printf("2. ????????\n");
                    printf("3. ???????\n");
                    printf("0. ????\n");
                    printf("????????: ");
                    scanf("%d", &sub_choice);
                    clear_input_buffer();

                    switch (sub_choice)
                    {
                    case 1:
                        write_shipment(&pending_shipment, u); // ??????
                        break;
                    case 2:
                        check_shipment(pending_shipment, completed_shipment); // ????????
                        break;
                    case 3:
                        process_shipment(&pending_shipment, &completed_shipment); // ???????
                        break;
                    case 0:
                        goto exit_submenu;
                    default:
                        printf("????????????????\n");
                        Sleep(2000);
                        break;
                    }
                }
            exit_submenu:
                break;
            }
            case 4:
                find_package(package_list); // ????????
                break;
                case 5: // ????????
                {
                    int abnormal_choice=-1;
                    struct abnormal_item_node *root = NULL;
                    load_abnormal_items(&root); // ?????????????
                    while (1)
                    {
                        system("cls");
                        printf("????????:\n");
                        printf("1. ????????\n");
                        printf("2. ?????????\n");
                        printf("3. ??????????\n");
                        printf("4. ???\n"); // ?????????
                        scanf("%d", &abnormal_choice);
                        clear_input_buffer();  // ??????????????????????????
                        switch (abnormal_choice)
                        {
                        case 1:
                            add_abnormal_item(&root);
                            break;
                        case 2:
                            check_abnormal_items(root); // ??????????
                            break;
                        case 3:
                            delete_abnormal_item(&root);
                            break;
                        case 4:
                            free_abnormal_item_list(root); // ??????????
                            goto exit_abnormal_menu;  // ?????????????????????
                        default:
                            printf("??§¹???????????????\n");
                            Sleep(1000);
                            break;
                        }
                    }
                exit_abnormal_menu:
                    break;  // ??? AdminSystem ?? switch-case
                }
            case 6:
                check_package(package_list);
                break;
            case 7:
                notify_overdue_parcels(package_list);
                break;
            case 8:
            {
                show_user(); // ??????????
                break;
            }
            case 9:
            {
                travel_user();//??????????
                break;
            }
            case 10:show_stats(pending_shipment, package_list);break;
            case 11:
                free_package_list(package_list); // ???????????
                return;                          // ?????????
            default:
                printf("????????????????\n");
                Sleep(2000);
                break;
            }
        }
    }
    else if (is_super_admin == 3)
    {
        SuperAdminSystem();
    }
}
// ?????
int LoginSystem(user *u)
{
    char username[20];
    char password[20];
    printf("?????????: ");
    scanf("%19s", username);
    clear_input_buffer(); // ?????????
    printf("??????????: ");
    scanf("%19s", password);
    clear_input_buffer(); // ?????????
    // ??????????
    strcpy(u->phonenumber, username);
    strcpy(u->password, password);
    if (UserdataFile_read(u))
    {
        printf("??????????????????%s!\n", u->name);
        Sleep(2000);
        return 1; // ???? 1 ??????
    }
    // ?????????????????
    if (strcmp(username, "12345678910") == 0 && strcmp(password, "1233211234567") == 0)
    {
        printf("??????????????????\n");
        Sleep(2000);
        return 3; // ???? 3 ?????????????
    }
    // ???????????????
    FILE *fp = fopen("manage_system_code/manager.txt", "r");
    if (fp == NULL)
    {
        perror("?????????????");
        return 0;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char file_username[20], file_password[20];
        if (sscanf(line, "%19s %19s", file_username, file_password) == 2)
        {
            if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0)
            {
                fclose(fp);
                printf("?????????????\n");
                Sleep(2000);
                return 2; // ???? 2 ???????????
            }
        }
    }
    fclose(fp);
    printf("?????????????????¦Ä???\n");
    Sleep(2000);
    return 0; // ???? 0 ?????????
}
// ?????
int UserSystem(struct package_node *package_list, struct shipment_node *pending_shipment, user *u) 
{
    while (1)
    {
        system("cls");
        printf("?????:\n");
        printf("1. ???\n");
        printf("2. ????????????\n");
        printf("3. ??§Õ??????\n");
        printf("4. ???\n");
        printf("5. ?????????\n");
        printf("6. ??????\n");  // ??????????
        printf("????????: ");
        
        int user_choice=-1;
        scanf("%d", &user_choice);
        clear_input_buffer(); // ???????????
        
        switch (user_choice)
        {
        case 1:
            user_pickup_package(&package_list, u);
            break;
        case 2:
            find_user_package(package_list, u);
            Sleep(2000);
            break;
        case 3:
            write_shipment(&pending_shipment, u);
            break;
        case 4:
            recharge(u);
            break;
        case 5:
            return 0; // ?????????????
        case 6:      // ??????
        {
            printf("????????????(Y/N): ");
            char confirm;
            scanf(" %c", &confirm);
            clear_input_buffer();
            
            if (confirm == 'Y' || confirm == 'y')
            {
                // ????1?????????????????????
                delete_user_from_file(u->phonenumber);
                printf("???????????????????????...\n");
                Sleep(2000);
                return 1; // ???? 1 ???????????
            }
            else
            {
                printf("????????????\n");
                Sleep(1000);
            }
            break;
        }
        default:
            printf("????????????????\n");
            Sleep(2000);
            break;
        }
    }
}
// ??????
int main()
{
    user *u = (user *)malloc(sizeof(user));
    if (u == NULL)
    {
        printf("?????????\n");
        return 1;
    }
    struct package_node *package_list = NULL;
    load_package_list(&package_list, "manage_system_code/package.txt");
    shipment_node *pending_shipment = NULL;
    shipment_node *completed_shipment = NULL;
    load_from_shipment(&pending_shipment, "manage_system_code/shipment.txt");
    load_from_shipment(&completed_shipment, "manage_system_code/completed.txt");
    int login_result;
    srand((unsigned)time(NULL));
    while (1)
    {
        system("cls");
        printf("????????:\n");
        printf("1. ???\n");
        printf("2. ???\n");
        printf("0. ???\n");
        printf("????????: ");
        int choice=-1;
        if (scanf("%d", &choice) != 1) {  // ???????????????
            clear_input_buffer();
            printf("?????????????\n");
            Sleep(2000);
            continue;
        }
        clear_input_buffer();
        switch (choice)
        {
        case 1:
        {
            login_result = LoginSystem(u);
            if (login_result == 1)
            {
                int logout_status = UserSystem(package_list, pending_shipment, u);
                if (logout_status == 1) // ????????
                {
                    free(u); // ???????????
                    u = (user *)malloc(sizeof(user)); // ???¡¤??????
                    if (u == NULL)
                    {
                        printf("?????????\n");
                        return 1;
                    }
                }
            }
            else
            {
                AdminSystem(login_result, package_list, pending_shipment, completed_shipment, u);
            }
            break;
        }
        case 2:
            Fillout(u);
            break;
        case 0:
            free(u);
            free_package_list(package_list);
            free_shipment_list(pending_shipment);
            free_shipment_list(completed_shipment);
            printf("???????????\n");
            return 0;
        default:
            printf("????????????????\n");
            Sleep(2000);
            break;
        }
    }
    return 0;
}