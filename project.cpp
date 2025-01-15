#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>
using namespace std;


// AVL Tree for storing transaction history
struct TransactionNode {
    string type;
    double amount;
    string recipient;
    string timestamp;
    int height;
    TransactionNode* left;
    TransactionNode* right;

    TransactionNode(string t, double a, string r) :
        type(t), amount(a), recipient(r), height(1), left(nullptr), right(nullptr) {}
};

// Hash table for user accounts
struct UserAccount {
    string name;
    string fatherName;
    int age;
    unsigned long long cnic;
    string email;
    string gender;
    string location;
    unsigned long long phoneNum;
    string password;
    double balance;
    string currency;
    vector<TransactionNode*> transactions;
};

// Global hash tables
unordered_map<string, UserAccount> userAccounts;
unordered_map<unsigned long long, string> phoneToUser;
unordered_map<unsigned long long, string> cnicToUser;

// Function declarations
bool inputCheck(string &input);
bool inputCheck(int &input);
bool inputCheck(unsigned long long &input);
bool inputCheck(double &input);
int getHeight(TransactionNode* node);
int getBalance(TransactionNode* node);
TransactionNode* rightRotate(TransactionNode* y);
TransactionNode* leftRotate(TransactionNode* x);
TransactionNode* insertTransaction(TransactionNode* node, string type, double amount, string recipient);
void saveTransactionToFile(const string& username, const string& type, double amount, const string& recipient);

// Input validation functions
bool inputCheck(string &input) {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool inputCheck(int &input) {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool inputCheck(unsigned long long &input) {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool inputCheck(double &input) {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

// AVL Tree functions
int getHeight(TransactionNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

int getBalance(TransactionNode* node) {
    if (node == nullptr) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

TransactionNode* rightRotate(TransactionNode* y) {
    TransactionNode* x = y->left;
    TransactionNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

TransactionNode* leftRotate(TransactionNode* x) {
    TransactionNode* y = x->right;
    TransactionNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

TransactionNode* insertTransaction(TransactionNode* node, string type, double amount, string recipient) {
    if (node == nullptr)
        return new TransactionNode(type, amount, recipient);

    if (amount < node->amount)
        node->left = insertTransaction(node->left, type, amount, recipient);
    else if (amount > node->amount)
        node->right = insertTransaction(node->right, type, amount, recipient);
    else
        return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && amount < node->left->amount)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && amount > node->right->amount)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && amount > node->left->amount) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && amount < node->right->amount) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}
void inorderTraversal(TransactionNode* root, vector<TransactionNode*>& transactions) {
    if (root == nullptr) return;

    inorderTraversal(root->left, transactions);
    transactions.push_back(root);
    inorderTraversal(root->right, transactions);
}

void printTransactionHistory(const string& username, const string& password) {
    // First verify if user exists with given username and password
    if (userAccounts.find(username) == userAccounts.end() ||
        userAccounts[username].password != password) {
        cout << "Invalid credentials.\n";
        return;
    }

    ifstream file("transactions.txt");
    string line;
    vector<TransactionNode*> userTransactions;

    // Create root node for AVL tree
    TransactionNode* root = nullptr;

    // Read transactions from file and build AVL tree
    while (getline(file, line)) {
        stringstream ss(line);
        string user, type, recipient;
        double amount;

        getline(ss, user, ',');
        getline(ss, type, ',');
        ss >> amount;
        ss.ignore(); // Skip comma
        getline(ss, recipient);

        // Only show transactions for user with matching username AND password
        if (user == username && userAccounts[user].password == password) {
            root = insertTransaction(root, type, amount, recipient);
        }
    }
    file.close();

    // Get sorted transactions using inorder traversal
    inorderTraversal(root, userTransactions);

    // Print transaction history
    cout << "\nTransaction History for " << username << ":\n";
    cout << "----------------------------------------\n";
    cout << "Type\t\tAmount\t\tRecipient\n";
    cout << "----------------------------------------\n";

    for (const auto& transaction : userTransactions) {
        cout << transaction->type << "\t\t"
             << transaction->amount << " PKR\t"
             << transaction->recipient << "\n";
    }
    cout << "----------------------------------------\n";

    // Cleanup AVL tree
    while (!userTransactions.empty()) {
        delete userTransactions.back();
        userTransactions.pop_back();
    }
}
// Account management functions

// New functions for file handling
void saveUserToFile(const UserAccount& user) {
    ofstream file("users.txt", ios::app);
    file << user.name << ","
         << user.fatherName << ","
         << user.age << ","
         << user.cnic << ","
         << user.email << ","
         << user.gender << ","
         << user.location << ","
         << user.phoneNum << ","
         << user.password << ","
         << user.balance << ","
         << user.currency << "\n";
    file.close();
}

void loadUsersFromFile() {
    ifstream file("users.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        UserAccount user;
        string balanceStr;

        getline(ss, user.name, ',');
        getline(ss, user.fatherName, ',');
        ss >> user.age;
        ss.ignore(); // Skip comma
        ss >> user.cnic;
        ss.ignore();
        getline(ss, user.email, ',');
        getline(ss, user.gender, ',');
        getline(ss, user.location, ',');
        ss >> user.phoneNum;
        ss.ignore();
        getline(ss, user.password, ',');
        ss >> user.balance;
        ss.ignore();
        getline(ss, user.currency);

        // Store in hash tables
        userAccounts[user.name] = user;
        if (user.phoneNum != 0) phoneToUser[user.phoneNum] = user.name;
        if (user.cnic != 0) cnicToUser[user.cnic] = user.name;
    }
    file.close();
}
bool registerAccount() {
    UserAccount newUser;
    string accountType;

    cout << "Enter Account Type (EasyPaisa/JazzCash/NayaPay/SadaPay/HBL/UBL): ";
    cin >> accountType;

    cout << "Enter Name: ";
    cin >> newUser.name;

    // Check if user already exists
    if (userAccounts.find(newUser.name) != userAccounts.end()) {
        cout << "Username already exists.\n";
        return false;
    }

    cout << "Enter Father's Name: ";
    cin >> newUser.fatherName;

    cout << "Enter Age: ";
    cin >> newUser.age;
    if (!inputCheck(newUser.age) || newUser.age < 18) {
        cout << "Invalid age. Must be 18 or older.\n";
        return false;
    }

    cout << "Enter CNIC (13 digits): ";
    cin >> newUser.cnic;
    if (!inputCheck(newUser.cnic) || to_string(newUser.cnic).length() != 13) {
        cout << "Invalid CNIC.\n";
        return false;
    }

    // Additional information
    cout << "Enter Email: ";
    cin >> newUser.email;

    cout << "Enter Gender (M/F): ";
    cin >> newUser.gender;

    cout << "Enter Location: ";
    cin >> newUser.location;

    cout << "Enter Phone Number: ";
    cin >> newUser.phoneNum;

    cout << "Enter Password: ";
    cin >> newUser.password;

    newUser.balance = 0;
    newUser.currency = "PKR";

    // Save to file first
    saveUserToFile(newUser);

    // Then update in-memory data structures
    userAccounts[newUser.name] = newUser;
    phoneToUser[newUser.phoneNum] = newUser.name;
    cnicToUser[newUser.cnic] = newUser.name;

    cout << "Account created successfully!\n";
    return true;
}

bool login(string &username) {
    string password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    if (userAccounts.find(username) != userAccounts.end() &&
        userAccounts[username].password == password) {
        cout << "Login successful!\n";
        return true;
    }

    cout << "Invalid credentials.\n";
    return false;
}

// Transaction functions
void addCash(string username) {
    double amount;
    string currency;

    cout << "Enter amount: ";
    cin >> amount;
    if (!inputCheck(amount) || amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    cout << "Enter currency (PKR/USD/EUR): ";
    cin >> currency;

    // Convert to PKR if necessary
    if (currency != "PKR") {
        // Add conversion rates here
        if (currency == "USD") amount *= 285.50;
        else if (currency == "EUR") amount *= 312.20;
    }

    userAccounts[username].balance += amount;
    saveTransactionToFile(username, "DEPOSIT", amount, "SELF");
    cout << "Amount added successfully. New balance: " << userAccounts[username].balance << " PKR\n";
}

void transferMoney(string username) {
    string recipientName;
    double amount;

    cout << "Enter recipient username: ";
    cin >> recipientName;

    if (userAccounts.find(recipientName) == userAccounts.end()) {
        cout << "Recipient not found.\n";
        return;
    }

    cout << "Enter amount: ";
    cin >> amount;
    if (!inputCheck(amount) || amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    if (userAccounts[username].balance < amount) {
        cout << "Insufficient balance.\n";
        return;
    }

    userAccounts[username].balance -= amount;
    userAccounts[recipientName].balance += amount;

    saveTransactionToFile(username, "TRANSFER_OUT", amount, recipientName);
    saveTransactionToFile(recipientName, "TRANSFER_IN", amount, username);

    cout << "Transfer successful. New balance: " << userAccounts[username].balance << " PKR\n";
}

void billPayment(string username) {
    string billType;
    double amount;

    cout << "Enter bill type (Electricity/Gas/Water): ";
    cin >> billType;

    cout << "Enter amount: ";
    cin >> amount;
    if (!inputCheck(amount) || amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    if (userAccounts[username].balance < amount) {
        cout << "Insufficient balance.\n";
        return;
    }

    userAccounts[username].balance -= amount;
    saveTransactionToFile(username, "BILL_" + billType, amount, "UTILITY");

    cout << "Bill payment successful. New balance: " << userAccounts[username].balance << " PKR\n";
}

// File operations
void saveTransactionToFile(const string& username, const string& type, double amount, const string& recipient) {
    ofstream file("transactions.txt", ios::app);
    file << username << "," << type << "," << amount << "," << recipient << "\n";
    file.close();
}

// Main menu function
void showMenu(string username) {
    int choice;
    do {
        system("cls");  // Clear at menu start
        cout << "\nMain Menu:\n";
        cout << "1. Add Cash\n";
        cout << "2. Transfer Money\n";
        cout << "3. Pay Bills\n";
        cout << "4. Check Balance\n";
        cout << "5. Transaction History\n";
        cout << "6. Logout\n";
        cout << "Enter choice: ";

        cin >> choice;
        if (!inputCheck(choice)) {
            cout << "Invalid input.\n";
            system("pause");
            continue;
        }

        switch(choice) {
            case 1:
                system("cls");
                addCash(username);
                system("pause");
                break;
            case 2:
                system("cls");
                transferMoney(username);
                system("pause");
                break;
            case 3:
                system("cls");
                billPayment(username);
                system("pause");
                break;
            case 4:
                system("cls");
                cout << "Current balance: " << userAccounts[username].balance << " PKR\n";
                system("pause");
                break;
            case 5:
                system("cls");
                printTransactionHistory(username, userAccounts[username].password); // Updated to pass password
                system("pause");
                break;
            case 6:
                system("cls");
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice.\n";
                system("pause");
        }
    } while (choice != 6);
}

int main() {
    loadUsersFromFile();

    int choice;
    string username;

    do {
        system("cls");  // Clear at start
        cout << "\nWelcome to Mobile Wallet\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";

        cin >> choice;
        if (!inputCheck(choice)) {
            cout << "Invalid input.\n";
            system("pause");
            continue;
        }

        switch(choice) {
            case 1:
                system("cls");
                registerAccount();
                system("pause");
                break;
            case 2:
                system("cls");
                if (login(username)) {
                    showMenu(username);
                }
                system("pause");
                break;
            case 3:
                system("cls");
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice.\n";
                system("pause");
        }
    } while (choice != 3);

    return 0;
}
