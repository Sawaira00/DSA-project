#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <ctime>

using namespace std;

// Simple hash function for passwords
string hashPassword(const string& password) {
    return to_string(std::hash<string>()(password)); // Convert the hash value to a string
}

// Transaction class to store transaction details
class Transaction {
public:
    string type; // "add", "send", "receive", "pay"
    double amount;
    string date; // Store date of transaction

    Transaction(string t, double amt, string d) : type(t), amount(amt), date(d) {}
};

// Node for the account tree
class AccountNode {
public:
    string username;
    string email;
    string hashedPassword; // Store password hash
    double balance; // Cash balance
    vector<Transaction> transactionHistory; // Store transaction history
    int failedLoginAttempts; // Count of failed login attempts
    bool isLocked; // Account lock status
    AccountNode* left;
    AccountNode* right;

    AccountNode(string user, string mail) : username(user), email(mail), balance(0.0),
                                            failedLoginAttempts(0), isLocked(false),
                                            left(nullptr), right(nullptr) {}

    void recordTransaction(string type, double amount, string date) {
        transactionHistory.push_back(Transaction(type, amount, date));
    }
};

// Tree structure for storing accounts
class AccountTree {
private:
    AccountNode* root;

public:
    AccountTree() : root(nullptr) {}

    void insert(string username, string email, string password) {
        AccountNode* newNode = new AccountNode(username, email);
        newNode->hashedPassword = hashPassword(password);
        root = insertRec(root, newNode);
    }

    AccountNode* insertRec(AccountNode* root, AccountNode* newNode) {
        if (!root) return newNode;
        if (newNode->username < root->username) {
            root->left = insertRec(root->left, newNode);
        } else {
            root->right = insertRec(root->right, newNode);
        }
        return root;
    }

    AccountNode* find(string username) {
        return findRec(root, username);
    }

    AccountNode* findRec(AccountNode* root, string username) {
        if (!root || root->username == username) return root;
        return username < root->username ? findRec(root->left, username) : findRec(root->right, username);
    }

    void deleteAccount(string username) {
        root = deleteRec(root, username);
    }

    AccountNode* deleteRec(AccountNode* root, string username) {
        if (!root) return root;
        if (username < root->username) {
            root->left = deleteRec(root->left, username);
        } else if (username > root->username) {
            root->right = deleteRec(root->right, username);
        } else {
            if (!root->left) {
                AccountNode* temp = root->right;
                delete root;
                return temp;
            } else if (!root->right) {
                AccountNode* temp = root->left;
                delete root;
                return temp;
            }
            AccountNode* temp = minValueNode(root->right);
            root->username = temp->username;
            root->email = temp->email;
            root->hashedPassword = temp->hashedPassword;
            root->balance = temp->balance;
            root->transactionHistory = temp->transactionHistory;
            root->right = deleteRec(root->right, temp->username);
        }
        return root;
    }

    AccountNode* minValueNode(AccountNode* node) {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }
};

// Class for user interactions
class UserSystem {
private:
    AccountTree accountsTree;
    vector<pair<string, double>> bills; // List of bills

    string getCurrentDate() {
        time_t now = time(0);
        char* dt = ctime(&now);
        return string(dt);
    }

public:
    void registerAccount(string username, string email, string password) {
        accountsTree.insert(username, email, password);
        cout << "Account created successfully! Please verify your email: " << email << endl;
    }

    bool login(string username, string password) {
        AccountNode* account = accountsTree.find(username);
        if (account) {
            if (account->isLocked) {
 cout << "Account is locked due to multiple failed login attempts. Please contact support." << endl;
                return false;
            }
            if (account->hashedPassword == hashPassword(password)) {
                account->failedLoginAttempts = 0; // Reset on successful login
                cout << "Login successful! Welcome, " << username << "!" << endl;
                return true;
            } else {
                account->failedLoginAttempts++;
                if (account->failedLoginAttempts >= 3) {
                    account->isLocked = true;
                    cout << "Account locked due to too many failed attempts." << endl;
                } else {
                    cout << "Incorrect password. Attempts left: " << (3 - account->failedLoginAttempts) << endl;
                }
            }
        } else {
            cout << "Account not found!" << endl;
        }
        return false;
    }

    void addCash(string username, double amount) {
        AccountNode* account = accountsTree.find(username);
        if (account && amount > 0) {
            account->balance += amount;
            account->recordTransaction("add", amount, getCurrentDate());
            cout << "Cash added successfully! Current balance: $" << account->balance << endl;
        } else {
            cout << "Invalid amount or account not found!" << endl;
        }
    }

    void sendCash(string senderUsername, string receiverUsername, double amount) {
        AccountNode* sender = accountsTree.find(senderUsername);
        AccountNode* receiver = accountsTree.find(receiverUsername);

        if (sender && receiver && sender->balance >= amount && amount > 0) {
            sender->balance -= amount;
            receiver->balance += amount;
            sender->recordTransaction("send", amount, getCurrentDate());
            receiver->recordTransaction("receive", amount, getCurrentDate());
            cout << "Cash sent successfully! " << senderUsername << "'s new balance: $"
                 << sender->balance << ", " << receiverUsername << "'s new balance: $"
                 << receiver->balance << endl;
        } else {
            cout << "Transaction failed! Check account details or balance." << endl;
        }
    }

    void viewTransactionHistory(string username) {
        AccountNode* account = accountsTree.find(username);
        if (account) {
            cout << "Transaction history for " << username << ":" << endl;
            for (const auto& transaction : account->transactionHistory) {
                cout << transaction.date << " - " << transaction.type << ": $" << transaction.amount << endl;
            }
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void updateProfile(string username, string newEmail) {
        AccountNode* account = accountsTree.find(username);
        if (account) {
            account->email = newEmail;
            cout << "Profile updated successfully! New email: " << newEmail << endl;
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void addBill(string billName, double amount) {
        bills.push_back(make_pair(billName, amount));
        cout << "Bill added: " << billName << " for $" << amount << endl;
    }

    void remindBills() {
        cout << "Upcoming bills:" << endl;
        for (const auto& bill : bills) {
            cout << "Bill: " << bill.first << " - Amount: $" << bill.second << endl;
        }
    }

    void userInput() {
        int choice;
        string username, email, password, receiverUsername, billName;
        double amount;

        while (true) {
            cout << "\n1. Register Account\n2. Login\n3. Add Cash\n4. Send Cash\n5. View Transaction History\n6. Update Profile\n7. Add Bill\n8. Remind Bills\n9. Exit\nChoose an option: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    cout << "Enter username: ";
                    cin >> username;
                    cout << "Enter email: ";
                    cin >> email;
                    cout << "Enter password: ";
                    cin >> password;
                    registerAccount(username, email, password);
                    break;
                case 2:
                    cout << "Enter username: ";
                    cin >> username;
                    cout << "Enter password: ";
                    cin >> password;
                    login(username, password);
                    break;
                case 3:
                    cout << "Enter username: ";
                    cin >> username;
                    cout << "Enter amount to add: ";
                    cin >> amount;
                    addCash(username, amount);
                    break;
                case 4:
                    cout << "Enter sender username: ";
                    cin >> username;
                    cout << "Enter receiver username: ";
                    cin >> receiverUsername;
                    cout << "Enter amount to send : ";
                    cin >> amount;
                    sendCash(username, receiverUsername, amount);
                    break;
                case 5:
                    cout << "Enter username: ";
                    cin >> username;
                    viewTransactionHistory(username);
                    break;
                case 6:
                    cout << "Enter username: ";
                    cin >> username;
                    cout << "Enter new email: ";
                    cin >> email;
                    updateProfile(username, email);
                    break;
                case 7:
                    cout << "Enter bill name: ";
                    cin >> billName;
                    cout << "Enter bill amount: ";
                    cin >> amount;
                    addBill(billName, amount);
                    break;
                case 8:
                    remindBills();
                    break;
                case 9:
                    cout << "Exiting the system." << endl;
                    return;
                default:
                    cout << "Invalid option. Please try again." << endl;
            }
        }
    }
};

int main() {
    UserSystem userSystem;
    userSystem.userInput();
    return 0;
}
