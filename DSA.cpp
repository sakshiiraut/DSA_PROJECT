#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include <unordered_map>
using namespace std;

class Transaction {
public:
    string type;
    string description;
    double amount;

    Transaction(const string& t, const string& d, double a) : type(t), description(d), amount(a) {}
    bool operator<(const Transaction& other) const {
        return amount < other.amount;
    }
};

class BudgetTracker {
private:
    vector<Transaction> transactions;
    stack<Transaction> undoStack;
    stack<Transaction> redoStack;
    queue<Transaction> transactionQueue;
    priority_queue<Transaction> amountHeap; 
    unordered_map<string, double> typeMap; 

    void saveToFile() {
        ofstream file("transactions.txt");
        if (file.is_open()) {
            for (const auto& transaction : transactions) {
                file << transaction.type << "," << transaction.description << "," << transaction.amount << "\n";
            }
            file.close();
        } else {
            cerr << "Unable to open file for writing." << endl;
        }
    }

    void loadFromFile() {
        ifstream file("transactions.txt");
        if (file.is_open()) {
            string type, description;
            double amount;
            string line;
            while (getline(file, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);

                type = line.substr(0, pos1);
                description = line.substr(pos1 + 1, pos2 - pos1 - 1);
                amount = stod(line.substr(pos2 + 1));

                transactions.emplace_back(type, description, amount);
                typeMap[type] += amount; 
            }
            file.close();
        } else {
            cerr << "Unable to open file for reading." << endl;
        }
    }

public:
    BudgetTracker() {
        loadFromFile();
        for (const auto& transaction : transactions) {
            amountHeap.push(transaction);
        }
    }

    ~BudgetTracker() {
        saveToFile();
    }

    void addTransaction(const string& type, const string& description, double amount) {
        Transaction newTransaction(type, description, amount);
        transactions.push_back(newTransaction);
        transactionQueue.push(newTransaction);
        undoStack.push(newTransaction);
        typeMap[type] += amount; 
        amountHeap.push(newTransaction); 
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

    void viewTransactions() {
        for (const auto& transaction : transactions) {
            cout << transaction.type << ": " << transaction.description << " - $" << transaction.amount << endl;
        }
    }

    void generateReport() {
        double totalIncome = 0, totalExpenses = 0;
        for (auto it = typeMap.begin(); it != typeMap.end(); ++it) {
            string type = it->first;
            double amount = it->second;
            if (type == "Income") {
                totalIncome += amount;
            } else if (type == "Expense") {
                totalExpenses += amount;
            }
        }

        cout << "Total Income: $" << totalIncome << endl;
        cout << "Total Expenses: $" << totalExpenses << endl;
        cout << "Remaining Balance: $" << (totalIncome - totalExpenses) << endl;
    }

    void undo() {
        if (!undoStack.empty()) {
            Transaction lastTransaction = undoStack.top();
            undoStack.pop();
            transactions.pop_back(); 
            typeMap[lastTransaction.type] -= lastTransaction.amount; 
            redoStack.push(lastTransaction);
        } else {
            cout << "Nothing to undo." << endl;
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            Transaction lastUndoneTransaction = redoStack.top();
            redoStack.pop();
            transactions.push_back(lastUndoneTransaction);
            typeMap[lastUndoneTransaction.type] += lastUndoneTransaction.amount; 
            undoStack.push(lastUndoneTransaction); 
        } else {
            cout << "Nothing to redo." << endl;
        }
    }

    void processQueue() {
        while (!transactionQueue.empty()) {
            Transaction t = transactionQueue.front();
            transactionQueue.pop();
            cout << "Processing transaction: " << t.type << ": " << t.description << " - $" << t.amount << endl;
        }
    }

    void printHighestTransaction() {
        if (!amountHeap.empty()) {
            Transaction highest = amountHeap.top();
            cout << "Highest transaction: " << highest.type << ": " << highest.description << " - $" << highest.amount << endl;
        } else {
            cout << "No transactions to display." << endl;
        }
    }

    void printLowestTransaction() {
        if (transactions.empty()) {
            cout << "No transactions to display." << endl;
            return;
        }
        auto minIt = min_element(transactions.begin(), transactions.end(), [](const Transaction& a, const Transaction& b) {
            return a.amount < b.amount;
        });

        cout << "Lowest transaction: " << minIt->type << ": " << minIt->description << " - $" << minIt->amount << endl;
    }

    void sortByAmount() {
        sort(transactions.begin(), transactions.end(), [](const Transaction& a, const Transaction& b) {
            return a.amount < b.amount;
        });
    }

    void sortByDescription() {
        sort(transactions.begin(), transactions.end(), [](const Transaction& a, const Transaction& b) {
            return a.description < b.description;
        });
    }

    void searchByDescription(const string& desc) {
        bool found = false;
        for (const auto& transaction : transactions) {
            if (transaction.description.find(desc) != string::npos) {
                cout << transaction.type << ": " << transaction.description << " - $" << transaction.amount << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No transactions found with description containing: " << desc << endl;
        }
    }

    void searchByAmount(double amount) {
        bool found = false;
        for (const auto& transaction : transactions) {
            if (transaction.amount == amount) {
                cout << transaction.type << ": " << transaction.description << " - $" << transaction.amount << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No transactions found with amount: $" << amount << endl;
        }
    }
};

int main() {
    BudgetTracker tracker;
    int choice;
    do {
        cout << "\nPersonal Budget Tracker" << endl;
        cout << "1. Add Income" << endl;
        cout << "2. Add Expense" << endl;
        cout << "3. View Transactions" << endl;
        cout << "4. Generate Report" << endl;
        cout << "5. Undo Last Transaction" << endl;
        cout << "6. Redo Last Transaction" << endl;
        cout << "7. Process Transaction Queue" << endl;
        cout << "8. Print Highest Transaction" << endl;
        cout << "9. Print Lowest Transaction" << endl;
        cout << "10. Sort Transactions by Amount" << endl;
        cout << "11. Sort Transactions by Description" << endl;
        cout << "12. Search Transactions by Description" << endl;
        cout << "13. Search Transactions by Amount" << endl;
        cout << "14. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Ignore the newline character left by cin

        if (choice == 1 || choice == 2) {
            string type = (choice == 1) ? "Income" : "Expense";
            string description;
            double amount;
            cout << "Enter description: ";
            getline(cin, description);
            cout << "Enter amount: ";
            cin >> amount;
            tracker.addTransaction(type, description, amount);
        } else if (choice == 3) {
            tracker.viewTransactions();
        } else if (choice == 4) {
            tracker.generateReport();
        } else if (choice == 5) {
            tracker.undo();
        } else if (choice == 6) {
            tracker.redo();
        } else if (choice == 7) {
            tracker.processQueue();
        } else if (choice == 8) {
            tracker.printHighestTransaction();
        } else if (choice == 9) {
            tracker.printLowestTransaction();
        } else if (choice == 10) {
            tracker.sortByAmount();
            cout << "Transactions sorted by amount." << endl;
        } else if (choice == 11) {
            tracker.sortByDescription();
            cout << "Transactions sorted by description." << endl;
        } else if (choice == 12){
            string description;
            cout << "Enter description to search for: ";
            getline(cin, description);
            tracker.searchByDescription(description);
        } else if (choice == 13) {
            double amount;
            cout << "Enter amount to search for: ";
            cin >> amount;
            tracker.searchByAmount(amount);
        }
    } while (choice != 14);

    return 0;
}
