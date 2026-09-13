#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

// ==================== Account Class ====================
class Account {
private:
    int accountNumber;
    string accountHolder;
    double balance;
    string accountType;  // Savings, Checking, etc.
    time_t creationDate;

public:
    // Constructor
    Account() : accountNumber(0), accountHolder(""), balance(0), accountType(""), creationDate(0) {}
    
    Account(int accNum, string name, double initialBalance, string type)
        : accountNumber(accNum), accountHolder(name), balance(initialBalance), 
          accountType(type), creationDate(time(0)) {}

    // Getters
    int getAccountNumber() const { return accountNumber; }
    string getAccountHolder() const { return accountHolder; }
    double getBalance() const { return balance; }
    string getAccountType() const { return accountType; }

    // Banking Operations
    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "\nERROR: Deposit amount must be positive!" << endl;
            return false;
        }
        balance += amount;
        cout << "\nSUCCESS: Deposit successful!" << endl;
        cout << "New Balance: Rs. " << fixed << setprecision(2) << balance << endl;
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "\nERROR: Withdrawal amount must be positive!" << endl;
            return false;
        }
        if (amount > balance) {
            cout << "\nERROR: Insufficient funds! Available balance: Rs. " << fixed << setprecision(2) << balance << endl;
            return false;
        }
        balance -= amount;
        cout << "\nSUCCESS: Withdrawal successful!" << endl;
        cout << "Remaining Balance: Rs. " << fixed << setprecision(2) << balance << endl;
        return true;
    }

    void displayAccountInfo() const {
        cout << "\n" << string(50, '=') << endl;
        cout << "ACCOUNT INFORMATION" << endl;
        cout << string(50, '=') << endl;
        cout << "Account Number    : " << accountNumber << endl;
        cout << "Account Holder    : " << accountHolder << endl;
        cout << "Account Type      : " << accountType << endl;
        cout << "Current Balance   : Rs. " << fixed << setprecision(2) << balance << endl;
        cout << string(50, '=') << endl;
    }

    // File I/O - Write account to file
    void saveToFile(ofstream& outFile) const {
        outFile << accountNumber << "|" << accountHolder << "|" 
                << fixed << setprecision(2) << balance << "|" 
                << accountType << "|" << creationDate << "\n";
    }

    // File I/O - Read account from file
    void loadFromFile(ifstream& inFile) {
        string line;
        if (getline(inFile, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            size_t pos3 = line.find('|', pos2 + 1);
            size_t pos4 = line.find('|', pos3 + 1);

            accountNumber = stoi(line.substr(0, pos1));
            accountHolder = line.substr(pos1 + 1, pos2 - pos1 - 1);
            balance = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
            accountType = line.substr(pos3 + 1, pos4 - pos3 - 1);
            creationDate = stol(line.substr(pos4 + 1));
        }
    }

    bool operator==(int accNum) const {
        return accountNumber == accNum;
    }
};

// ==================== Bank Class ====================
class Bank {
private:
    vector<Account> accounts;
    const string FILENAME = "bank_data.txt";
    int nextAccountNumber;

public:
    Bank() : nextAccountNumber(1001) {
        loadAllAccounts();
        if (!accounts.empty()) {
            nextAccountNumber = accounts.back().getAccountNumber() + 1;
        }
    }

    ~Bank() {
        saveAllAccounts();
    }

    // Create new account
    bool createAccount(string name, double initialBalance, string accountType) {
        if (name.empty() || initialBalance < 0) {
            cout << "\nERROR: Invalid input! Name cannot be empty and balance must be non-negative." << endl;
            return false;
        }

        // Check if account holder already exists
        for (const auto& acc : accounts) {
            if (acc.getAccountHolder() == name) {
                cout << "\nERROR: Account holder already exists!" << endl;
                return false;
            }
        }

        accounts.push_back(Account(nextAccountNumber, name, initialBalance, accountType));
        cout << "\nSUCCESS: Account created successfully!" << endl;
        cout << "Account Number: " << nextAccountNumber << endl;
        nextAccountNumber++;
        saveAllAccounts();
        return true;
    }

    // Find account by account number
    Account* findAccount(int accNum) {
        auto it = find(accounts.begin(), accounts.end(), accNum);
        if (it != accounts.end()) {
            return &(*it);
        }
        return nullptr;
    }

    // Deposit money
    bool deposit(int accNum, double amount) {
        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "\nERROR: Account not found!" << endl;
            return false;
        }
        bool result = acc->deposit(amount);
        if (result) {
            saveAllAccounts();
        }
        return result;
    }

    // Withdraw money
    bool withdraw(int accNum, double amount) {
        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "\nERROR: Account not found!" << endl;
            return false;
        }
        bool result = acc->withdraw(amount);
        if (result) {
            saveAllAccounts();
        }
        return result;
    }

    // Check balance
    bool checkBalance(int accNum) {
        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "\nERROR: Account not found!" << endl;
            return false;
        }
        acc->displayAccountInfo();
        return true;
    }

    // Display all accounts
    void displayAllAccounts() const {
        if (accounts.empty()) {
            cout << "\nNo accounts in the system!" << endl;
            return;
        }

        cout << "\n" << string(70, '=') << endl;
        cout << "ALL ACCOUNTS" << endl;
        cout << string(70, '=') << endl;
        cout << left << setw(12) << "Acc No." << setw(25) << "Account Holder" 
             << setw(15) << "Type" << setw(18) << "Balance (Rs.)" << endl;
        cout << string(70, '-') << endl;

        for (const auto& acc : accounts) {
            cout << left << setw(12) << acc.getAccountNumber() 
                 << setw(25) << acc.getAccountHolder() 
                 << setw(15) << acc.getAccountType() 
                 << "Rs. " << fixed << setprecision(2) << setw(13) << acc.getBalance() << endl;
        }
        cout << string(70, '=') << endl;
    }

    // Delete account
    bool deleteAccount(int accNum) {
        auto it = find(accounts.begin(), accounts.end(), accNum);
        if (it != accounts.end()) {
            cout << "\nSUCCESS: Account for " << it->getAccountHolder() << " deleted successfully!" << endl;
            accounts.erase(it);
            saveAllAccounts();
            return true;
        }
        cout << "\nERROR: Account not found!" << endl;
        return false;
    }

    // Save all accounts to file
    void saveAllAccounts() const {
        ofstream outFile(FILENAME);
        if (!outFile) {
            cerr << "ERROR: Error opening file for writing!" << endl;
            return;
        }
        for (const auto& acc : accounts) {
            acc.saveToFile(outFile);
        }
        outFile.close();
    }

    // Load all accounts from file
    void loadAllAccounts() {
        ifstream inFile(FILENAME);
        if (!inFile) {
            cout << "INFO: No existing data file found. Starting with empty system." << endl;
            return;
        }

        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                size_t pos1 = line.find('|');
                size_t pos2 = line.find('|', pos1 + 1);
                size_t pos3 = line.find('|', pos2 + 1);
                size_t pos4 = line.find('|', pos3 + 1);

                int accNum = stoi(line.substr(0, pos1));
                string holder = line.substr(pos1 + 1, pos2 - pos1 - 1);
                double balance = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
                string type = line.substr(pos3 + 1, pos4 - pos3 - 1);
                time_t creationDate = stol(line.substr(pos4 + 1));

                accounts.push_back(Account(accNum, holder, balance, type));
            }
        }
        inFile.close();
    }
};

// ==================== User Interface ====================
class BankUI {
private:
    Bank bank;

public:
    void displayMainMenu() {
        cout << "\n" << string(50, '=') << endl;
        cout << "BANK MANAGEMENT SYSTEM" << endl;
        cout << string(50, '=') << endl;
        cout << "1. Create New Account" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. Check Balance" << endl;
        cout << "5. View All Accounts" << endl;
        cout << "6. Delete Account" << endl;
        cout << "7. Exit" << endl;
        cout << string(50, '=') << endl;
        cout << "Enter your choice (1-7): ";
    }

    void createNewAccount() {
        string name, type;
        double initialBalance;

        cout << "\n--- Create New Account ---" << endl;
        cout << "Enter account holder name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter account type (Savings/Checking/Business): ";
        getline(cin, type);

        cout << "Enter initial balance (Rs.): ";
        cin >> initialBalance;

        bank.createAccount(name, initialBalance, type);
    }

    void depositMoney() {
        int accNum;
        double amount;

        cout << "\n--- Deposit Money ---" << endl;
        cout << "Enter account number: ";
        cin >> accNum;

        cout << "Enter deposit amount (Rs.): ";
        cin >> amount;

        bank.deposit(accNum, amount);
    }

    void withdrawMoney() {
        int accNum;
        double amount;

        cout << "\n--- Withdraw Money ---" << endl;
        cout << "Enter account number: ";
        cin >> accNum;

        cout << "Enter withdrawal amount (Rs.): ";
        cin >> amount;

        bank.withdraw(accNum, amount);
    }

    void checkBalance() {
        int accNum;

        cout << "\n--- Check Balance ---" << endl;
        cout << "Enter account number: ";
        cin >> accNum;

        bank.checkBalance(accNum);
    }

    void viewAllAccounts() {
        bank.displayAllAccounts();
    }

    void deleteAccount() {
        int accNum;

        cout << "\n--- Delete Account ---" << endl;
        cout << "Enter account number to delete: ";
        cin >> accNum;

        cout << "Are you sure? (y/n): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            bank.deleteAccount(accNum);
        } else {
            cout << "\nERROR: Deletion cancelled!" << endl;
        }
    }

    void run() {
        int choice;
        bool running = true;

        cout << "\nWelcome to Bank Management System!" << endl;

        while (running) {
            displayMainMenu();
            cin >> choice;

            switch (choice) {
                case 1:
                    createNewAccount();
                    break;
                case 2:
                    depositMoney();
                    break;
                case 3:
                    withdrawMoney();
                    break;
                case 4:
                    checkBalance();
                    break;
                case 5:
                    viewAllAccounts();
                    break;
                case 6:
                    deleteAccount();
                    break;
                case 7:
                    cout << "\nThank you for using Bank Management System!" << endl;
                    cout << "Goodbye!" << endl;
                    running = false;
                    break;
                default:
                    cout << "\nERROR: Invalid choice! Please try again." << endl;
            }
        }
    }
};

// ==================== Main Function ====================
int main() {
    BankUI ui;
    ui.run();
    return 0;
}