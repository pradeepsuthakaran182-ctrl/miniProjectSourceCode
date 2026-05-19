// Professional Bank Management System in one C file.
// This program implements account creation, login, deposit, withdrawal,
// transfer, search, update, delete, loan and interest features.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define BANK_FILE "bank.dat"
#define TRANS_FILE "transactions.txt"
#define MAX_NAME 50
#define MAX_GENDER 10
#define MAX_PHONE 15
#define MAX_ADDRESS 100
#define MAX_AADHAAR 20
#define MAX_PAN 20
#define MAX_ACCOUNT_TYPE 20
#define MAX_USERNAME 30
#define MAX_PASSWORD 30

// Structure for bank customer records
struct clientData
{
    int acctNum;
    char fullName[MAX_NAME];
    int age;
    char gender[MAX_GENDER];
    char phone[MAX_PHONE];
    char address[MAX_ADDRESS];
    char aadhaar[MAX_AADHAAR];
    char pan[MAX_PAN];
    char accountType[MAX_ACCOUNT_TYPE];
    double balance;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int isActive;
};

// Helper functions for input and formatting
void trimNewline(char *text)
{
    size_t len = strcspn(text, "\r\n");
    text[len] = '\0';
}

void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

void pause(void)
{
    printf("\nPress Enter to continue...");
    getchar();
}

int caseInsensitiveCompare(const char *a, const char *b)
{
    while (*a && *b)
    {
        char ca = tolower((unsigned char)*a);
        char cb = tolower((unsigned char)*b);
        if (ca != cb)
            return (ca > cb) - (ca < cb);
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int caseInsensitiveContains(const char *text, const char *pattern)
{
    size_t lenText = strlen(text);
    size_t lenPattern = strlen(pattern);
    if (lenPattern == 0)
        return 1;
    for (size_t i = 0; i + lenPattern <= lenText; i++)
    {
        size_t j;
        for (j = 0; j < lenPattern; j++)
        {
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)pattern[j]))
                break;
        }
        if (j == lenPattern)
            return 1;
    }
    return 0;
}

int readInt(const char *prompt)
{
    char buffer[128];
    int value;

    while (1)
    {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin))
            exit(0);
        if (sscanf(buffer, "%d", &value) == 1)
            return value;
        printf("Invalid input. Please enter a number.\n");
    }
}

double readDouble(const char *prompt)
{
    char buffer[128];
    double value;

    while (1)
    {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin))
            exit(0);
        if (sscanf(buffer, "%lf", &value) == 1)
            return value;
        printf("Invalid input. Please enter a valid amount.\n");
    }
}

void readString(const char *prompt, char *output, int size)
{
    char buffer[256];

    while (1)
    {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin))
            exit(0);
        trimNewline(buffer);
        if (strlen(buffer) == 0)
        {
            printf("Input cannot be empty.\n");
            continue;
        }
        if ((int)strlen(buffer) >= size)
        {
            printf("Maximum %d characters allowed.\n", size - 1);
            continue;
        }
        strcpy(output, buffer);
        return;
    }
}

int yesNoPrompt(const char *prompt)
{
    char buffer[16];
    while (1)
    {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin))
            exit(0);
        if (buffer[0] == '\n')
            continue;
        char answer = toupper((unsigned char)buffer[0]);
        if (answer == 'Y')
            return 1;
        if (answer == 'N')
            return 0;
        printf("Please enter Y or N.\n");
    }
}

int validateDigits(const char *text, int required)
{
    if ((int)strlen(text) != required)
        return 0;
    for (int i = 0; i < required; i++)
        if (!isdigit((unsigned char)text[i]))
            return 0;
    return 1;
}

int validatePan(const char *pan)
{
    if ((int)strlen(pan) != 10)
        return 0;
    for (int i = 0; i < 10; i++)
    {
        if (i < 5)
        {
            if (!isalpha((unsigned char)pan[i]))
                return 0;
        }
        else if (i < 9)
        {
            if (!isdigit((unsigned char)pan[i]))
                return 0;
        }
        else
        {
            if (!isalpha((unsigned char)pan[i]))
                return 0;
        }
    }
    return 1;
}

void displayHeader(const char *title)
{
    printf("\n=====================================\n");
    printf("      %s\n", title);
    printf("=====================================\n");
}

long createTransactionRecord(const char *type, int fromAcct, int toAcct, double amount, double updatedBalance)
{
    FILE *fp = fopen(TRANS_FILE, "a");
    if (!fp)
    {
        printf("Unable to update transaction history.\n");
        return 0;
    }
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char date[16];
    char timeStr[16];
    strftime(date, sizeof(date), "%Y-%m-%d", local);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", local);
    long txnID = (long)now;
    fprintf(fp, "%ld,%s,%s,%s,%d,%d,%.2f,%.2f\n", txnID, date, timeStr, type, fromAcct, toAcct, amount, updatedBalance);
    fclose(fp);
    return txnID;
}

void printReceipt(long txnID, const char *txnType, int acctNum, int destAcct, double amount, double balance, const char *note)
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char dateTime[32];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", local);
    displayHeader("TRANSACTION RECEIPT");
    printf("Transaction ID : %ld\n", txnID);
    printf("Date & Time    : %s\n", dateTime);
    printf("Account Number : %d\n", acctNum);
    if (destAcct > 0)
        printf("To Account     : %d\n", destAcct);
    printf("Transaction Type: %s\n", txnType);
    printf("Amount         : %.2f\n", amount);
    printf("Updated Balance: %.2f\n", balance);
    if (note && note[0] != '\0')
        printf("Note           : %s\n", note);
    displayHeader("");
}

int ensureFilesExist(void)
{
    FILE *fp = fopen(BANK_FILE, "ab+");
    if (!fp)
    {
        printf("Cannot create or open %s.\n", BANK_FILE);
        return 0;
    }
    fclose(fp);
    fp = fopen(TRANS_FILE, "a");
    if (!fp)
    {
        printf("Cannot create or open %s.\n", TRANS_FILE);
        return 0;
    }
    fclose(fp);
    return 1;
}

int usernameExists(const char *username)
{
    FILE *fp = fopen(BANK_FILE, "rb");
    if (!fp)
        return 0;
    struct clientData client;
    while (fread(&client, sizeof(client), 1, fp) == 1)
    {
        if (client.isActive && caseInsensitiveCompare(client.username, username) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int getAccountOffset(int acctNum, struct clientData *client, long *offset, int activeOnly)
{
    FILE *fp = fopen(BANK_FILE, "rb");
    if (!fp)
        return 0;
    struct clientData temp;
    long pos = 0;
    while (fread(&temp, sizeof(temp), 1, fp) == 1)
    {
        if (temp.acctNum == acctNum && (!activeOnly || temp.isActive))
        {
            if (client)
                *client = temp;
            if (offset)
                *offset = pos;
            fclose(fp);
            return 1;
        }
        pos += sizeof(temp);
    }
    fclose(fp);
    return 0;
}

int findAccountByCredentials(const char *username, const char *password, struct clientData *client, long *offset)
{
    FILE *fp = fopen(BANK_FILE, "rb");
    if (!fp)
        return 0;
    struct clientData temp;
    long pos = 0;
    while (fread(&temp, sizeof(temp), 1, fp) == 1)
    {
        if (temp.isActive && caseInsensitiveCompare(temp.username, username) == 0 && strcmp(temp.password, password) == 0)
        {
            if (client)
                *client = temp;
            if (offset)
                *offset = pos;
            fclose(fp);
            return 1;
        }
        pos += sizeof(temp);
    }
    fclose(fp);
    return 0;
}

int generateAccountNumber(void)
{
    FILE *fp = fopen(BANK_FILE, "rb");
    if (!fp)
        return 1001;
    struct clientData temp;
    int maxID = 1000;
    while (fread(&temp, sizeof(temp), 1, fp) == 1)
    {
        if (temp.acctNum > maxID)
            maxID = temp.acctNum;
    }
    fclose(fp);
    return maxID + 1;
}

void writeAccountAtOffset(const struct clientData *client, long offset)
{
    FILE *fp = fopen(BANK_FILE, "rb+");
    if (!fp)
    {
        printf("Unable to update bank record.\n");
        return;
    }
    fseek(fp, offset, SEEK_SET);
    fwrite(client, sizeof(*client), 1, fp);
    fclose(fp);
}

void appendAccount(const struct clientData *client)
{
    FILE *fp = fopen(BANK_FILE, "ab");
    if (!fp)
    {
        printf("Unable to save bank record.\n");
        return;
    }
    fwrite(client, sizeof(*client), 1, fp);
    fclose(fp);
}

void adminDashboard(void);
void generateReport(void);
void transactionHistory(void);

void adminLogin(void)
{
    displayHeader("ADMIN LOGIN");
    const char adminUser[] = "admin";
    const char adminPass[] = "admin123";
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 0;
    while (attempts < 3)
    {
        readString("Username: ", username, MAX_USERNAME);
        readString("Password: ", password, MAX_PASSWORD);
        if (strcmp(username, adminUser) == 0 && strcmp(password, adminPass) == 0)
        {
            printf("\nLogin successful. Welcome, Admin.\n");
            adminDashboard();
            return;
        }
        attempts++;
        printf("Wrong credentials. Attempts left: %d\n", 3 - attempts);
    }
    printf("Maximum login attempts reached. Returning to main menu.\n");
}

void customerMenu(struct clientData customer, long offset);

void customerLogin(void)
{
    displayHeader("CUSTOMER LOGIN");
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 0;
    struct clientData customer;
    long offset;
    while (attempts < 3)
    {
        readString("Username: ", username, MAX_USERNAME);
        readString("Password: ", password, MAX_PASSWORD);
        if (findAccountByCredentials(username, password, &customer, &offset))
        {
            printf("\nLogin successful. Welcome, %s.\n", customer.fullName);
            customerMenu(customer, offset);
            return;
        }
        attempts++;
        printf("Login failed. Attempts left: %d\n", 3 - attempts);
    }
    printf("Maximum login attempts reached. Returning to main menu.\n");
}

void createAccount(void)
{
    displayHeader("CREATE ACCOUNT");
    struct clientData client = {0};
    client.isActive = 1;
    client.acctNum = generateAccountNumber();
    printf("Generated Account Number: %d\n", client.acctNum);
    readString("Full Name: ", client.fullName, MAX_NAME);
    client.age = readInt("Age: ");
    while (client.age <= 0 || client.age > 120)
    {
        printf("Enter a valid age.\n");
        client.age = readInt("Age: ");
    }
    readString("Gender: ", client.gender, MAX_GENDER);
    while (1)
    {
        readString("Phone Number (10 digits): ", client.phone, MAX_PHONE);
        if (validateDigits(client.phone, 10))
            break;
        printf("Invalid phone number.\n");
    }
    readString("Address: ", client.address, MAX_ADDRESS);
    while (1)
    {
        readString("Aadhaar Number (12 digits): ", client.aadhaar, MAX_AADHAAR);
        if (validateDigits(client.aadhaar, 12))
            break;
        printf("Invalid Aadhaar number.\n");
    }
    while (1)
    {
        readString("PAN Number (10 chars): ", client.pan, MAX_PAN);
        if (validatePan(client.pan))
            break;
        printf("Invalid PAN number.\n");
    }
    while (1)
    {
        readString("Account Type [Savings/Current/Fixed Deposit]: ", client.accountType, MAX_ACCOUNT_TYPE);
        if (caseInsensitiveCompare(client.accountType, "Savings") == 0 || caseInsensitiveCompare(client.accountType, "Current") == 0 || caseInsensitiveCompare(client.accountType, "Fixed Deposit") == 0)
            break;
        printf("Please choose Savings, Current, or Fixed Deposit.\n");
    }
    while (1)
    {
        readString("Username: ", client.username, MAX_USERNAME);
        if (usernameExists(client.username))
            printf("Username already exists. Choose another.\n");
        else
            break;
    }
    readString("Password: ", client.password, MAX_PASSWORD);
    client.balance = readDouble("Initial Deposit: ");
    while (client.balance <= 0)
    {
        printf("Deposit must be greater than 0.\n");
        client.balance = readDouble("Initial Deposit: ");
    }
    appendAccount(&client);
    printf("\nAccount created successfully!\n");
    printf("Account Number: %d\nCustomer Name: %s\nBalance: %.2f\n", client.acctNum, client.fullName, client.balance);
    createTransactionRecord("Account Opening", client.acctNum, 0, client.balance, client.balance);
    pause();
}

void depositMoney(void)
{
    displayHeader("DEPOSIT MONEY");
    int acctNum = readInt("Enter account number: ");
    struct clientData client;
    long offset;
    if (!getAccountOffset(acctNum, &client, &offset, 1))
    {
        printf("Account %d not found.\n", acctNum);
        pause();
        return;
    }
    double amount = readDouble("Enter deposit amount: ");
    if (amount <= 0)
    {
        printf("Amount must be positive.\n");
        pause();
        return;
    }
    client.balance += amount;
    writeAccountAtOffset(&client, offset);
    long txnID = createTransactionRecord("Deposit", acctNum, 0, amount, client.balance);
    printReceipt(txnID, "Deposit", acctNum, 0, amount, client.balance, "");
    pause();
}

void withdrawMoney(void)
{
    displayHeader("WITHDRAW MONEY");
    int acctNum = readInt("Enter account number: ");
    struct clientData client;
    long offset;
    if (!getAccountOffset(acctNum, &client, &offset, 1))
    {
        printf("Account %d not found.\n", acctNum);
        pause();
        return;
    }
    double amount = readDouble("Enter withdrawal amount: ");
    if (amount <= 0)
    {
        printf("Amount must be positive.\n");
        pause();
        return;
    }
    if (amount > client.balance)
    {
        printf("Insufficient balance. Current balance: %.2f\n", client.balance);
        pause();
        return;
    }
    client.balance -= amount;
    writeAccountAtOffset(&client, offset);
    long txnID = createTransactionRecord("Withdrawal", acctNum, 0, amount, client.balance);
    printReceipt(txnID, "Withdrawal", acctNum, 0, amount, client.balance, "");
    pause();
}

void transferMoney(void)
{
    displayHeader("TRANSFER MONEY");
    int fromAcct = readInt("Sender account number: ");
    struct clientData sender, receiver;
    long fromOffset, toOffset;
    if (!getAccountOffset(fromAcct, &sender, &fromOffset, 1))
    {
        printf("Sender account not found.\n");
        pause();
        return;
    }
    int toAcct = readInt("Receiver account number: ");
    if (toAcct == fromAcct)
    {
        printf("Sender and receiver cannot be the same.\n");
        pause();
        return;
    }
    if (!getAccountOffset(toAcct, &receiver, &toOffset, 1))
    {
        printf("Receiver account not found.\n");
        pause();
        return;
    }
    double amount = readDouble("Enter transfer amount: ");
    if (amount <= 0)
    {
        printf("Amount must be positive.\n");
        pause();
        return;
    }
    if (amount > sender.balance)
    {
        printf("Insufficient balance in sender account.\n");
        pause();
        return;
    }
    sender.balance -= amount;
    receiver.balance += amount;
    writeAccountAtOffset(&sender, fromOffset);
    writeAccountAtOffset(&receiver, toOffset);
    long txnID = createTransactionRecord("Transfer", fromAcct, toAcct, amount, sender.balance);
    printReceipt(txnID, "Transfer", fromAcct, toAcct, amount, sender.balance, "Amount transferred successfully");
    pause();
}

void balanceInquiry(void)
{
    displayHeader("BALANCE INQUIRY");
    int acctNum = readInt("Enter account number: ");
    struct clientData client;
    if (!getAccountOffset(acctNum, &client, NULL, 1))
    {
        printf("Account %d not found.\n", acctNum);
        pause();
        return;
    }
    printf("\nAccount Number : %d\n", client.acctNum);
    printf("Customer Name  : %s\n", client.fullName);
    printf("Account Type   : %s\n", client.accountType);
    printf("Current Balance: %.2f\n", client.balance);
    pause();
}

void searchAccount(void)
{
    displayHeader("SEARCH ACCOUNT");
    printf("1. By Account Number\n");
    printf("2. By Customer Name\n");
    printf("3. By Phone Number\n");
    int choice = readInt("Enter choice: ");
    FILE *fp = fopen(BANK_FILE, "rb");
    if (!fp)
    {
        printf("Unable to read bank records.\n");
        pause();
        return;
    }
    struct clientData client;
    int found = 0;
    char query[128];
    if (choice == 1)
    {
        int acctNum = readInt("Enter account number: ");
        if (getAccountOffset(acctNum, &client, NULL, 1))
        {
            found = 1;
            printf("\nAccount Number : %d\nName: %s\nAge: %d\nGender: %s\nPhone: %s\nAddress: %s\nAadhaar: %s\nPAN: %s\nAccount Type: %s\nBalance: %.2f\nUsername: %s\n", client.acctNum, client.fullName, client.age, client.gender, client.phone, client.address, client.aadhaar, client.pan, client.accountType, client.balance, client.username);
        }
    }
    else if (choice == 2)
    {
        readString("Enter customer name or partial name: ", query, sizeof(query));
        while (fread(&client, sizeof(client), 1, fp) == 1)
        {
            if (client.isActive && caseInsensitiveContains(client.fullName, query))
            {
                found = 1;
                printf("\n---\nAccount Number: %d\nName: %s\nPhone: %s\nAccount Type: %s\nBalance: %.2f\n", client.acctNum, client.fullName, client.phone, client.accountType, client.balance);
            }
        }
    }
    else if (choice == 3)
    {
        readString("Enter phone number: ", query, sizeof(query));
        while (fread(&client, sizeof(client), 1, fp) == 1)
        {
            if (client.isActive && strcmp(client.phone, query) == 0)
            {
                found = 1;
                printf("\n---\nAccount Number: %d\nName: %s\nPhone: %s\nAccount Type: %s\nBalance: %.2f\n", client.acctNum, client.fullName, client.phone, client.accountType, client.balance);
            }
        }
    }
    else
    {
        printf("Invalid menu choice.\n");
    }
    fclose(fp);
    if (!found)
        printf("No matching account found.\n");
    pause();
}

void updateAccount(void)
{
    displayHeader("UPDATE ACCOUNT");
    int acctNum = readInt("Enter account number: ");
    struct clientData client;
    long offset;
    if (!getAccountOffset(acctNum, &client, &offset, 1))
    {
        printf("Account not found.\n");
        pause();
        return;
    }
    printf("\nCurrent address: %s\n", client.address);
    printf("Current phone  : %s\n", client.phone);
    if (yesNoPrompt("Update address? (Y/N): "))
        readString("New address: ", client.address, MAX_ADDRESS);
    if (yesNoPrompt("Update phone number? (Y/N): "))
    {
        while (1)
        {
            readString("New phone number (10 digits): ", client.phone, MAX_PHONE);
            if (validateDigits(client.phone, 10))
                break;
            printf("Invalid phone number.\n");
        }
    }
    if (yesNoPrompt("Update password? (Y/N): "))
        readString("New password: ", client.password, MAX_PASSWORD);
    writeAccountAtOffset(&client, offset);
    printf("Account updated successfully.\n");
    pause();
}

void deleteAccount(void)
{
    displayHeader("DELETE ACCOUNT");
    int acctNum = readInt("Enter account number to delete: ");
    struct clientData client;
    long offset;
    if (!getAccountOffset(acctNum, &client, &offset, 1))
    {
        printf("Account not found or already deleted.\n");
        pause();
        return;
    }
    if (!yesNoPrompt("Are you sure you want to delete this account? (Y/N): "))
    {
        printf("Deletion cancelled.\n");
        pause();
        return;
    }
    client.isActive = 0;
    writeAccountAtOffset(&client, offset);
    printf("Account %d deleted successfully.\n", acctNum);
    pause();
}

void viewTransactionHistory(void)
{
    displayHeader("TRANSACTION HISTORY");
    char record[256];
    FILE *fp = fopen(TRANS_FILE, "r");
    if (!fp)
    {
        printf("Unable to read transaction history.\n");
        pause();
        return;
    }
    printf("%-12s %-10s %-8s %-10s %-10s %-10s %-10s\n", "Txn ID", "Date", "Time", "Type", "From", "To", "Amount");
    while (fgets(record, sizeof(record), fp))
    {
        long id;
        char date[16], timeStr[16], type[32];
        int from, to;
        double amount, balance;
        if (sscanf(record, "%ld,%15[^,],%15[^,],%31[^,],%d,%d,%lf,%lf", &id, date, timeStr, type, &from, &to, &amount, &balance) == 8)
        {
            printf("%-12ld %-10s %-8s %-10s %-10d %-10d %-10.2f\n", id, date, timeStr, type, from, to, amount);
        }
    }
    fclose(fp);
    pause();
}

void transactionHistory(void)
{
    displayHeader("TRANSACTION HISTORY");
    if (yesNoPrompt("View all transactions? (Y/N): "))
    {
        viewTransactionHistory();
        return;
    }
    int acctNum = readInt("Enter account number to search transactions: ");
    char record[256];
    FILE *fp = fopen(TRANS_FILE, "r");
    if (!fp)
    {
        printf("Unable to read transaction history.\n");
        pause();
        return;
    }
    int found = 0;
    printf("%-12s %-10s %-8s %-10s %-10s %-10s\n", "Txn ID", "Date", "Time", "Type", "From", "To");
    while (fgets(record, sizeof(record), fp))
    {
        long id;
        char date[16], timeStr[16], type[32];
        int from, to;
        double amount, balance;
        if (sscanf(record, "%ld,%15[^,],%15[^,],%31[^,],%d,%d,%lf,%lf", &id, date, timeStr, type, &from, &to, &amount, &balance) == 8)
        {
            if (from == acctNum || to == acctNum)
            {
                found = 1;
                printf("%-12ld %-10s %-8s %-10s %-10d %-10d\n", id, date, timeStr, type, from, to);
            }
        }
    }
    fclose(fp);
    if (!found)
        printf("No transactions found for account %d.\n", acctNum);
    pause();
}

void interestCalculator(void)
{
    displayHeader("INTEREST CALCULATOR");
    printf("1. Savings Interest\n");
    printf("2. Fixed Deposit Interest\n");
    printf("3. Recurring Deposit\n");
    int choice = readInt("Enter option: ");
    double principal, rate, years;
    double maturity;
    switch (choice)
    {
    case 1:
        principal = readDouble("Principal amount: ");
        rate = readDouble("Annual interest rate (percent): ");
        years = readDouble("Time in years: ");
        maturity = principal * rate * years / 100.0;
        printf("Savings interest earned: %.2f\nTotal amount after %.2f years: %.2f\n", maturity, years, principal + maturity);
        break;
    case 2:
        principal = readDouble("Principal amount: ");
        rate = readDouble("Annual interest rate (percent): ");
        years = readDouble("Time in years: ");
        maturity = principal * pow(1 + rate / 100.0, years);
        printf("Maturity amount for Fixed Deposit: %.2f\nInterest earned: %.2f\n", maturity, maturity - principal);
        break;
    case 3:
        principal = readDouble("Monthly deposit amount: ");
        rate = readDouble("Annual interest rate (percent): ");
        years = readDouble("Total duration in years: ");
        {
            int months = (int)(years * 12);
            double monthlyRate = rate / 100.0 / 12.0;
            maturity = principal * months + principal * monthlyRate * months * (months + 1) / 2.0;
            printf("Recurring deposit maturity amount: %.2f\n", maturity);
        }
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }
    pause();
}

void loanManagement(void)
{
    displayHeader("LOAN MANAGEMENT");
    int acctNum = readInt("Enter account number: ");
    struct clientData client;
    long offset;
    if (!getAccountOffset(acctNum, &client, &offset, 1))
    {
        printf("Account not found.\n");
        pause();
        return;
    }
    printf("Customer Name: %s\nCurrent Balance: %.2f\n", client.fullName, client.balance);
    double requested = readDouble("Loan amount requested: ");
    int tenureYears = readInt("Loan tenure in years: ");
    double salary = readDouble("Monthly salary: ");
    double eligibleAmount = salary * 20.0;
    if (requested <= 0 || tenureYears <= 0 || salary <= 0)
    {
        printf("Invalid loan parameters.\n");
        pause();
        return;
    }
    if (requested > eligibleAmount)
    {
        printf("Loan rejected. Requested amount exceeds eligibility of %.2f.\n", eligibleAmount);
        pause();
        return;
    }
    double monthlyRate = 10.0 / 100.0 / 12.0;
    int months = tenureYears * 12;
    double emi = (requested * monthlyRate * pow(1 + monthlyRate, months)) / (pow(1 + monthlyRate, months) - 1);
    printf("\nLoan Approved.\nRequested Amount: %.2f\nTenure: %d years\nMonthly EMI: %.2f\nTotal Payment: %.2f\n", requested, tenureYears, emi, emi * months);
    pause();
}

void generateReport(void)
{
    displayHeader("BANK REPORT");
    FILE *fp = fopen(BANK_FILE, "rb");
    if (!fp)
    {
        printf("Unable to read bank records.\n");
        pause();
        return;
    }
    struct clientData client;
    int activeCount = 0;
    int deletedCount = 0;
    int savingCount = 0;
    int currentCount = 0;
    int fixedCount = 0;
    double totalBalance = 0.0;
    while (fread(&client, sizeof(client), 1, fp) == 1)
    {
        if (client.acctNum == 0)
            continue;
        if (client.isActive)
        {
            activeCount++;
            totalBalance += client.balance;
            if (caseInsensitiveCompare(client.accountType, "Savings") == 0)
                savingCount++;
            else if (caseInsensitiveCompare(client.accountType, "Current") == 0)
                currentCount++;
            else if (caseInsensitiveCompare(client.accountType, "Fixed Deposit") == 0)
                fixedCount++;
        }
        else
            deletedCount++;
    }
    fclose(fp);
    printf("Total active accounts   : %d\n", activeCount);
    printf("Total deleted accounts  : %d\n", deletedCount);
    printf("Savings accounts        : %d\n", savingCount);
    printf("Current accounts        : %d\n", currentCount);
    printf("Fixed Deposit accounts  : %d\n", fixedCount);
    printf("Total bank balance      : %.2f\n", totalBalance);
    pause();
}

void adminDashboard(void)
{
    while (1)
    {
        displayHeader("ADMIN DASHBOARD");
        printf("1. View all customers\n");
        printf("2. Total accounts\n");
        printf("3. Total bank balance\n");
        printf("4. Search accounts\n");
        printf("5. View deleted accounts\n");
        printf("6. Generate report\n");
        printf("7. Logout\n");
        int choice = readInt("Enter choice: ");
        FILE *fp;
        struct clientData client;
        int count = 0;
        double totalBalance = 0.0;
        if (choice == 1)
        {
            fp = fopen(BANK_FILE, "rb");
            if (!fp)
            {
                printf("Unable to read bank records.\n");
                pause();
                continue;
            }
            printf("\n%-6s %-20s %-15s %-12s\n", "Acct", "Name", "Type", "Balance");
            while (fread(&client, sizeof(client), 1, fp) == 1)
            {
                if (client.isActive)
                    printf("%-6d %-20s %-15s %.2f\n", client.acctNum, client.fullName, client.accountType, client.balance);
            }
            fclose(fp);
            pause();
        }
        else if (choice == 2)
        {
            fp = fopen(BANK_FILE, "rb");
            if (!fp)
            {
                printf("Unable to read bank records.\n");
                pause();
                continue;
            }
            while (fread(&client, sizeof(client), 1, fp) == 1)
                if (client.isActive)
                    count++;
            fclose(fp);
            printf("Total active accounts: %d\n", count);
            pause();
        }
        else if (choice == 3)
        {
            fp = fopen(BANK_FILE, "rb");
            if (!fp)
            {
                printf("Unable to read bank records.\n");
                pause();
                continue;
            }
            while (fread(&client, sizeof(client), 1, fp) == 1)
                if (client.isActive)
                    totalBalance += client.balance;
            fclose(fp);
            printf("Total bank balance across active accounts: %.2f\n", totalBalance);
            pause();
        }
        else if (choice == 4)
        {
            searchAccount();
        }
        else if (choice == 5)
        {
            fp = fopen(BANK_FILE, "rb");
            if (!fp)
            {
                printf("Unable to read bank records.\n");
                pause();
                continue;
            }
            printf("\nDeleted Accounts:\n%-6s %-20s %-15s\n", "Acct", "Name", "Type");
            while (fread(&client, sizeof(client), 1, fp) == 1)
            {
                if (!client.isActive && client.acctNum != 0)
                    printf("%-6d %-20s %-15s\n", client.acctNum, client.fullName, client.accountType);
            }
            fclose(fp);
            pause();
        }
        else if (choice == 6)
        {
            generateReport();
        }
        else if (choice == 7)
        {
            printf("Logging out of admin dashboard.\n");
            break;
        }
        else
        {
            printf("Invalid menu choice.\n");
        }
    }
}

void customerMenu(struct clientData customer, long offset)
{
    while (1)
    {
        displayHeader("CUSTOMER MENU");
        printf("1. Deposit Money\n");
        printf("2. Withdraw Money\n");
        printf("3. Transfer Money\n");
        printf("4. Balance Inquiry\n");
        printf("5. Transaction History\n");
        printf("6. Update Account\n");
        printf("7. Logout\n");
        int choice = readInt("Enter option: ");
        if (choice == 1)
            depositMoney();
        else if (choice == 2)
            withdrawMoney();
        else if (choice == 3)
            transferMoney();
        else if (choice == 4)
            balanceInquiry();
        else if (choice == 5)
            transactionHistory();
        else if (choice == 6)
            updateAccount();
        else if (choice == 7)
        {
            printf("Logging out.\n");
            break;
        }
        else
            printf("Invalid choice. Please try again.\n");
    }
}

void printMainMenu(void)
{
    printf("\n===================================\n");
    printf("       BANK MANAGEMENT SYSTEM\n");
    printf("===================================\n");
    printf("1. Admin Login\n");
    printf("2. Customer Login\n");
    printf("3. Create Account\n");
    printf("4. Deposit Money\n");
    printf("5. Withdraw Money\n");
    printf("6. Transfer Money\n");
    printf("7. Balance Inquiry\n");
    printf("8. Search Account\n");
    printf("9. Update Account\n");
    printf("10. Delete Account\n");
    printf("11. Loan Management\n");
    printf("12. Interest Calculator\n");
    printf("13. Transaction History\n");
    printf("14. Admin Dashboard\n");
    printf("15. Generate Report\n");
    printf("16. Exit\n");
}

int main(void)
{
    if (!ensureFilesExist())
        return 1;
    while (1)
    {
        printMainMenu();
        int choice = readInt("Select an option: ");
        switch (choice)
        {
        case 1:
            adminLogin();
            break;
        case 2:
            customerLogin();
            break;
        case 3:
            createAccount();
            break;
        case 4:
            depositMoney();
            break;
        case 5:
            withdrawMoney();
            break;
        case 6:
            transferMoney();
            break;
        case 7:
            balanceInquiry();
            break;
        case 8:
            searchAccount();
            break;
        case 9:
            updateAccount();
            break;
        case 10:
            deleteAccount();
            break;
        case 11:
            loanManagement();
            break;
        case 12:
            interestCalculator();
            break;
        case 13:
            transactionHistory();
            break;
        case 14:
            adminDashboard();
            break;
        case 15:
            generateReport();
            break;
        case 16:
            printf("Exiting application. Goodbye.\n");
            return 0;
        default:
            printf("Invalid menu input. Enter value 1-16.\n");
            break;
        }
    }
    return 0;
}
