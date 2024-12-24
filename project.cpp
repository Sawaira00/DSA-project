#include <iostream>
#include <string>
#include <unordered_map>
#include <functional> // For std::hash
#include <cstdlib>    // For system("clear") or system("cls")

using namespace std;

// Hash function using std::hash (not cryptographically secure)
string hashPassword(const string& password) {
    size_t hashValue = hash<string>()(password);
    return to_string(hashValue); // Convert the hash value to a string
}

// Node for the tree
class AccountNode {
public:
    string username;
    string email;
    unordered_map<string, string> hashedCredentials; // Hash table for storing password hashes
    AccountNode* left;
    AccountNode* right;

    AccountNode(string user, string mail) {
        username = user;
        email = mail;
        left = right = nullptr;
    }
};

// Tree structure for storing accounts
class AccountTree {
private:
    AccountNode* root;

public:
    AccountTree() : root(nullptr) {}

    // Insert a new account into the tree
    void insert(string username, string email, string password) {
        AccountNode* newNode = new AccountNode(username, email);
        newNode->hashedCredentials["password"] = hashPassword(password);

        if (!root) {
            root = newNode;
        } else {
            insertRec(root, newNode);
        }
    }

    // Recursive insert function
    void insertRec(AccountNode* root, AccountNode* newNode) {
        if (newNode->username < root->username) {
            if (root->left) {
                insertRec(root->left, newNode);
            } else {
                root->left = newNode;
            }
        } else {
            if (root->right) {
                insertRec(root->right, newNode);
            } else {
                root->right = newNode;
            }
        }
    }

    // Find an account by username
    AccountNode* find(string username) {
        return findRec(root, username);
    }

    AccountNode* findRec(AccountNode* root, string username) {
        if (!root || root->username == username) {
            return root;
        }
        if (username < root->username) {
            return findRec(root->left, username);
        } else {
            return findRec(root->right, username);
        }
    }

    // Delete an account by username
    void deleteAccount(string username) {
        root = deleteRec(root, username);
    }

    // Recursive delete function
    AccountNode* deleteRec(AccountNode* root, string username) {
        if (!root) return root;

        if (username < root->username) {
            root->left = deleteRec(root->left, username);
        } else if (username > root->username) {
            root->right = deleteRec(root->right, username);
        } else {
            // Node with only one child or no child
            if (!root->left) {
                AccountNode* temp = root->right;
                delete root;
                return temp;
            } else if (!root->right) {
                AccountNode* temp = root->left;
                delete root;
                return temp;
            }

            // Node with two children
            AccountNode* temp = minValueNode(root->right);
            root->username = temp->username;
            root->email = temp->email;
            root->hashedCredentials = temp->hashedCredentials;
            root->right = deleteRec(root->right, temp->username);
        }
        return root;
    }

    // Helper function to find the node with the minimum value
    AccountNode* minValueNode(AccountNode* node) {
        AccountNode* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }
};

// Class for user interactions (Registration, Login, etc.)
class UserSystem {
private:
    AccountTree accountsTree;

public:
    void registerAccount(string username, string email, string password) {
        accountsTree.insert(username, email, password);
        cout << "Account created successfully!" << endl;
    }

    bool login(string username, string password) {
        AccountNode* account = accountsTree.find(username);
        if (account) {
            // Compare the entered password hash with the stored hash
            if (account->hashedCredentials["password"] == hashPassword(password)) {
                cout << "Login successful!" << endl;
                return true;
            } else {
                cout << "Invalid password!" << endl;
                return false;
            }
        } else {
            cout << "Account not found!" << endl;
            return false;
        }
    }

    void changePassword(string username, string newPassword) {
        AccountNode* account = accountsTree.find(username);
        if (account) {
            account->hashedCredentials["password"] = hashPassword(newPassword);
            cout << "Password updated successfully!" << endl;
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void deleteAccount(string username) {
        AccountNode* account = accountsTree.find(username);
        if (account) {
            accountsTree.deleteAccount(username);
            cout << "Account deleted successfully!" << endl;
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void clearScreen() {
        // Clear the console screen (cross-platform)
        system("cls || clear");
    }

    void menu() {
        int choice;
        string username, email, password;
        do {
            clearScreen();
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "3. Change Password" << endl;
            cout << "4. Delete Account" << endl;
            cout << "5. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) {
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter email: ";
                cin >> email;
                cout << "Enter password: ";
                cin >> password;
                registerAccount(username, email, password);
            } else if (choice == 2) {
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;
                login(username, password);
            } else if (choice == 3) {
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter new password: ";
                cin >> password;
                changePassword(username, password);
            } else if (choice == 4) {
                cout << "Enter username: ";
                cin >> username;
                deleteAccount(username);
            }

            cout << "Press any key to continue...";
            cin.ignore();
            cin.get();
        } while (choice != 5);
    }
};

int main() {
    UserSystem userSystem;
    userSystem.menu();
    return 0;
}
