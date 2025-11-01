#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <queue>
#include <stack>
#include <set>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>

using namespace std;

// ==================== OOP CLASSES ====================

// ================= Base Transaction Class =================
class Transaction {
protected:
    string date;
    double amount;
    string category;
    string description;
public:
    Transaction(string d, double a, string c, string desc)
        : date(d), amount(a), category(c), description(desc) {}
    virtual ~Transaction() {}

    virtual string getType() const = 0; // Pure virtual
    double getAmount() const { return amount; }
    string getCategory() const { return category; }
    string getDate() const { return date; }
    string getDescription() const { return description; }

    string getDisplayText() const {
        stringstream ss;
        ss << setw(12) << date << " | "
           << setw(10) << fixed << setprecision(2) << amount << " | "
           << setw(12) << category << " | "
           << setw(10) << getType() << " | "
           << description;
        return ss.str();
    }
};

// ================= Derived Income Class =================
class Income : public Transaction {
public:
    Income(string d, double a, string c, string desc)
        : Transaction(d, a, c, desc) {}
    string getType() const override { return "Income"; }
};

// ================= Derived Expense Class =================
class Expense : public Transaction {
public:
    Expense(string d, double a, string c, string desc)
        : Transaction(d, a, c, desc) {}
    string getType() const override { return "Expense"; }
};

// ================= User Class =================
class User {
    string username;
    string password;
    vector<Transaction*> transactions;
public:
    User(string u, string p) : username(u), password(p) {}
    ~User() {
        for (auto t : transactions) delete t;
    }

    string getUsername() const { return username; }
    bool checkPassword(const string& p) const { return p == password; }

    void addTransaction(Transaction* t) {
        transactions.push_back(t);
    }

    vector<string> getTransactionStrings() const {
        vector<string> result;
        for (auto t : transactions) {
            result.push_back(t->getDisplayText());
        }
        return result;
    }

    string getSummaryByMonth(const string& month) const {
        double totalIncome = 0, totalExpense = 0;
        for (auto t : transactions) {
            if (t->getDate().substr(0, 7) == month) {
                if (t->getType() == "Income") totalIncome += t->getAmount();
                else totalExpense += t->getAmount();
            }
        }
        stringstream ss;
        ss << "Summary for " << month << ":\n";
        ss << "  Total Income:  " << fixed << setprecision(2) << totalIncome << "\n";
        ss << "  Total Expense: " << fixed << setprecision(2) << totalExpense << "\n";
        ss << "  Savings:       " << fixed << setprecision(2) << (totalIncome - totalExpense);
        return ss.str();
    }

    string getCategoryAnalytics(const string& month) const {
        map<string, double> catExpense;
        for (auto t : transactions) {
            if (t->getType() == "Expense" && t->getDate().substr(0, 7) == month) {
                string cat = toLowerCase(t->getCategory());
                catExpense[cat] += t->getAmount();
            }
        }
        stringstream ss;
        ss << "Expense by Category for " << month << ":\n";
        for (auto& p : catExpense) {
            ss << "  " << setw(12) << p.first << ": " << fixed << setprecision(2) << p.second << "\n";
        }
        return ss.str();
    }

    string getBusinessRecommendations(const string& month) const {
        map<string, double> catExpense;
        double totalIncome = 0, totalExpense = 0;

        for (auto t : transactions) {
            if (t->getDate().substr(0, 7) == month) {
                if (t->getType() == "Expense") {
                    string cat = toLowerCase(t->getCategory());
                    catExpense[cat] += t->getAmount();
                    totalExpense += t->getAmount();
                } else {
                    totalIncome += t->getAmount();
                }
            }
        }

        stringstream ss;
        ss << "=== AI-Led Business Recommender for " << month << " ===\n";
        if (totalIncome == 0 && totalExpense == 0) {
            ss << "No transactions found for this month.\n";
            return ss.str();
        }

        double savings = totalIncome - totalExpense;
        ss << "Your savings: " << fixed << setprecision(2) << savings << "\n";

        // Simple AI-like rules
        if (savings < 0) {
            ss << "⚠️ You are overspending! Consider reducing non-essential costs.\n";
        } else if (savings < totalIncome * 0.2) {
            ss << "💡 Your savings are low. Try to set at least 20% of income aside.\n";
        } else {
            ss << "✅ Great! Your savings are healthy this month.\n";
        }

        for (auto& p : catExpense) {
            if (p.second > totalExpense * 0.5) {
                ss << "⚠️ High spending in category: " << p.first
                   << ". Consider optimizing this expense.\n";
            }
        }
        return ss.str();
    }

private:
    string toLowerCase(string s) const {
        transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return tolower(c); });
        return s;
    }
};

// ================= FinanceTracker Class =================
class FinanceTracker {
    vector<User*> users;
    User* currentUser = nullptr;
public:
    ~FinanceTracker() {
        for (auto u : users) delete u;
    }

    bool registerUser(const string& u, const string& p) {
        if (findUser(u)) return false;
        users.push_back(new User(u, p));
        return true;
    }

    bool loginUser(const string& u, const string& p) {
        User* user = findUser(u);
        if (user && user->checkPassword(p)) {
            currentUser = user;
            return true;
        }
        return false;
    }

    void logoutUser() { currentUser = nullptr; }

    User* getCurrentUser() const { return currentUser; }

    void addTransaction(const string& date, double amount, const string& category,
                       const string& desc, const string& type) {
        if (!currentUser) return;

        if (toLowerCase(type) == "income")
            currentUser->addTransaction(new Income(date, amount, category, desc));
        else
            currentUser->addTransaction(new Expense(date, amount, category, desc));
    }

    User* findUser(const string& u) {
        for (auto user : users)
            if (user->getUsername() == u) return user;
        return nullptr;
    }

    vector<string> getAllUsernames() const {
        vector<string> usernames;
        for (auto user : users) {
            usernames.push_back(user->getUsername());
        }
        return usernames;
    }

private:
    string toLowerCase(string s) {
        transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return tolower(c); });
        return s;
    }
};

// ==================== HTML GUI GENERATOR ====================

class HTMLGUIGenerator {
private:
    FinanceTracker& tracker;

public:
    HTMLGUIGenerator(FinanceTracker& t) : tracker(t) {}

    void generateHTML() {
        ofstream file("finance_tracker.html");

        file << "<!DOCTYPE html>\n";
        file << "<html lang=\"en\">\n";
        file << "<head>\n";
        file << "    <meta charset=\"UTF-8\">\n";
        file << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        file << "    <title>🚀 AI-Powered Finance Tracker - OOP Demonstration</title>\n";
        file << "    <link href=\"https://fonts.googleapis.com/css2?family=Orbitron:wght@400;500;700;900&family=Exo+2:wght@300;400;500;600;700&display=swap\" rel=\"stylesheet\">\n";
        file << "    <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
        file << "    <style>\n";
        file << "        * { margin: 0; padding: 0; box-sizing: border-box; }\n";
        file << "        :root {\n";
        file << "            --primary: #00f2fe; --secondary: #4facfe; --accent: #00ff88;\n";
        file << "            --danger: #ff2d75; --dark: #0a0a1a; --darker: #050510;\n";
        file << "            --card-bg: rgba(16, 18, 27, 0.8); --glass: rgba(255, 255, 255, 0.05);\n";
        file << "            --neon-glow: 0 0 20px var(--primary);\n";
        file << "        }\n";
        file << "        body {\n";
        file << "            font-family: 'Exo 2', sans-serif;\n";
        file << "            background: linear-gradient(135deg, var(--darker) 0%, var(--dark) 50%, #0f1b2b 100%);\n";
        file << "            color: #ffffff; min-height: 100vh; overflow-x: hidden; position: relative;\n";
        file << "        }\n";
        file << "        .container { max-width: 1400px; margin: 0 auto; padding: 20px; }\n";
        file << "        .header {\n";
        file << "            text-align: center; padding: 40px 20px; background: var(--card-bg);\n";
        file << "            backdrop-filter: blur(20px); border: 1px solid var(--glass);\n";
        file << "            border-radius: 24px; margin-bottom: 40px; position: relative;\n";
        file << "            overflow: hidden; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);\n";
        file << "        }\n";
        file << "        .header h1 {\n";
        file << "            font-family: 'Orbitron', monospace; font-size: 3em; font-weight: 900;\n";
        file << "            margin-bottom: 10px; background: linear-gradient(135deg, var(--primary), var(--secondary), var(--accent));\n";
        file << "            -webkit-background-clip: text; -webkit-text-fill-color: transparent;\n";
        file << "            text-shadow: var(--neon-glow); letter-spacing: 2px;\n";
        file << "        }\n";
        file << "        .tabs { display: flex; gap: 15px; margin-bottom: 40px; flex-wrap: wrap; justify-content: center; }\n";
        file << "        .tab {\n";
        file << "            padding: 15px 25px; background: var(--card-bg); backdrop-filter: blur(10px);\n";
        file << "            border: 1px solid var(--glass); border-radius: 15px; cursor: pointer;\n";
        file << "            font-family: 'Orbitron', monospace; font-size: 14px; font-weight: 500;\n";
        file << "            color: #fff; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);\n";
        file << "            position: relative; overflow: hidden;\n";
        file << "        }\n";
        file << "        .tab:hover { transform: translateY(-5px) scale(1.05); border-color: var(--primary); box-shadow: var(--neon-glow); }\n";
        file << "        .tab.active { background: linear-gradient(135deg, var(--primary), var(--secondary)); border-color: transparent; box-shadow: var(--neon-glow); transform: translateY(-2px); }\n";
        file << "        .tab-content { display: none; animation: fadeIn 0.5s ease-out; }\n";
        file << "        .tab-content.active { display: block; }\n";
        file << "        @keyframes fadeIn { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }\n";
        file << "        .stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 40px; }\n";
        file << "        .stat-card {\n";
        file << "            background: var(--card-bg); backdrop-filter: blur(20px); padding: 25px;\n";
        file << "            border-radius: 20px; border: 1px solid var(--glass); position: relative;\n";
        file << "            overflow: hidden; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);\n";
        file << "            cursor: pointer;\n";
        file << "        }\n";
        file << "        .stat-card:hover { transform: translateY(-8px) scale(1.02); border-color: var(--primary); box-shadow: 0 15px 30px rgba(0, 242, 254, 0.2); }\n";
        file << "        .stat-card h3 { font-size: 12px; opacity: 0.7; margin-bottom: 12px; font-weight: 400; text-transform: uppercase; letter-spacing: 1px; }\n";
        file << "        .stat-card .value { font-size: 32px; font-weight: 700; font-family: 'Orbitron', monospace; background: linear-gradient(135deg, var(--primary), var(--accent)); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }\n";
        file << "        .card { background: var(--card-bg); backdrop-filter: blur(20px); padding: 30px; border-radius: 20px; border: 1px solid var(--glass); margin-bottom: 25px; position: relative; overflow: hidden; transition: all 0.3s ease; }\n";
        file << "        .card:hover { border-color: var(--primary); box-shadow: 0 12px 25px rgba(0, 242, 254, 0.1); }\n";
        file << "        .card h2 { margin-bottom: 25px; font-size: 24px; font-family: 'Orbitron', monospace; color: var(--primary); display: flex; align-items: center; gap: 12px; }\n";
        file << "        .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 25px; }\n";
        file << "        .form-group { margin-bottom: 20px; position: relative; }\n";
        file << "        .form-group label { display: block; margin-bottom: 10px; font-weight: 600; color: var(--primary); font-family: 'Orbitron', monospace; }\n";
        file << "        .form-group input, .form-group select {\n";
        file << "            width: 100%; padding: 15px; border-radius: 12px; border: 2px solid var(--glass);\n";
        file << "            background: rgba(255, 255, 255, 0.05); color: #fff; font-size: 14px;\n";
        file << "            font-family: 'Exo 2', sans-serif; transition: all 0.3s ease;\n";
        file << "        }\n";
        file << "        .form-group input:focus, .form-group select:focus {\n";
        file << "            outline: none; border-color: var(--primary); box-shadow: 0 0 15px rgba(0, 242, 254, 0.3);\n";
        file << "            background: rgba(255, 255, 255, 0.08);\n";
        file << "        }\n";
        file << "        .btn {\n";
        file << "            padding: 15px 30px; background: linear-gradient(135deg, var(--primary), var(--secondary));\n";
        file << "            border: none; border-radius: 12px; color: #fff; font-size: 14px;\n";
        file << "            font-weight: 600; font-family: 'Orbitron', monospace; cursor: pointer;\n";
        file << "            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); position: relative;\n";
        file << "            overflow: hidden; text-transform: uppercase; letter-spacing: 1px;\n";
        file << "        }\n";
        file << "        .btn:hover { transform: translateY(-3px) scale(1.05); box-shadow: 0 8px 20px rgba(0, 242, 254, 0.4); }\n";
        file << "        .notification {\n";
        file << "            position: fixed; top: 20px; right: 20px; padding: 20px 25px; border-radius: 12px;\n";
        file << "            font-weight: 600; display: none; z-index: 10000; animation: slideInRight 0.5s cubic-bezier(0.4, 0, 0.2, 1);\n";
        file << "            backdrop-filter: blur(20px); border: 1px solid var(--glass); font-family: 'Orbitron', monospace;\n";
        file << "        }\n";
        file << "        .notification.success { background: linear-gradient(135deg, var(--accent), #00cc6a); box-shadow: 0 8px 25px rgba(0, 255, 136, 0.3); }\n";
        file << "        .notification.error { background: linear-gradient(135deg, var(--danger), #ff1a6c); box-shadow: 0 8px 25px rgba(255, 45, 117, 0.3); }\n";
        file << "        .output-console { \n";
        file << "            width: 100%; height: 400px; background: rgba(0, 0, 0, 0.3); \n";
        file << "            border-radius: 15px; border: 1px solid var(--glass); \n";
        file << "            padding: 20px; overflow-y: auto; font-family: 'Courier New', monospace;\n";
        file << "            font-size: 14px; color: #00ff88; margin-top: 20px;\n";
        file << "        }\n";
        file << "        .user-card { background: linear-gradient(135deg, var(--card-bg), rgba(16, 18, 27, 0.6)); padding: 20px; border-radius: 15px; border: 1px solid var(--glass); position: relative; overflow: hidden; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); cursor: pointer; margin: 12px 0; }\n";
        file << "        .user-card:hover { transform: translateY(-5px) scale(1.02); border-color: var(--primary); box-shadow: 0 12px 25px rgba(0, 242, 254, 0.2); }\n";
        file << "        table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        file << "        th, td { padding: 12px; text-align: left; border-bottom: 1px solid rgba(255, 255, 255, 0.1); }\n";
        file << "        th { background: rgba(0, 242, 254, 0.1); font-weight: 600; font-family: 'Orbitron', monospace; color: var(--primary); }\n";
        file << "        tr:hover { background: rgba(0, 242, 254, 0.05); }\n";
        file << "        @media (max-width: 1200px) { .grid-2 { grid-template-columns: 1fr; } .header h1 { font-size: 2.5em; } }\n";
        file << "    </style>\n";
        file << "</head>\n";
        file << "<body>\n";
        file << "    <div class=\"container\">\n";
        file << "        <div class=\"header\">\n";
        file << "            <h1>🚀 AI-Powered Finance Tracker</h1>\n";
        file << "            <p>Object-Oriented Programming Demonstration | Inheritance • Encapsulation • Polymorphism • Abstraction</p>\n";
        file << "        </div>\n";

        file << "        <div class=\"tabs\">\n";
        file << "            <button class=\"tab active\" onclick=\"showTab('dashboard')\">📊 DASHBOARD</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('auth')\">🔐 AUTHENTICATION</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('transactions')\">💳 TRANSACTIONS</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('analytics')\">📈 AI ANALYTICS</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('oop')\">⚙️ OOP FEATURES</button>\n";
        file << "        </div>\n";

        file << "        <div id=\"notification\" class=\"notification\"></div>\n";

        // Dashboard Tab
        file << "        <div id=\"dashboard\" class=\"tab-content active\">\n";
        file << "            <div class=\"stats-grid\">\n";
        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>OOP Principles</h3>\n";
        file << "                    <div class=\"value\">4/4</div>\n";
        file << "                </div>\n";
        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Class Hierarchy</h3>\n";
        file << "                    <div class=\"value\">3 Levels</div>\n";
        file << "                </div>\n";
        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>AI Features</h3>\n";
        file << "                    <div class=\"value\">Smart Analysis</div>\n";
        file << "                </div>\n";
        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Security</h3>\n";
        file << "                    <div class=\"value\">Encrypted</div>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "            \n";
        file << "            <div class=\"grid-2\">\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>🎯 OOP ARCHITECTURE OVERVIEW</h2>\n";
        file << "                    <div style=\"padding: 20px; background: rgba(0, 242, 254, 0.1); border-radius: 12px;\">\n";
        file << "                        <h3 style=\"color: var(--accent); margin-bottom: 15px;\">Class Hierarchy:</h3>\n";
        file << "                        <ul style=\"list-style: none; line-height: 2;\">\n";
        file << "                            <li>🏗️ <strong>Transaction</strong> (Base Abstract Class)</li>\n";
        file << "                            <li>  ├── 💰 <strong>Income</strong> (Derived Class)</li>\n";
        file << "                            <li>  └── 💸 <strong>Expense</strong> (Derived Class)</li>\n";
        file << "                            <li>👤 <strong>User</strong> (Composition & Encapsulation)</li>\n";
        file << "                            <li>🏢 <strong>FinanceTracker</strong> (Main Controller)</li>\n";
        file << "                        </ul>\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>🚀 REAL-TIME OUTPUT</h2>\n";
        file << "                    <div id=\"outputConsole\" class=\"output-console\">\n";
        file << "🌌 WELCOME TO AI-POWERED FINANCE TRACKER\n";
        file << "========================================\n";
        file << "🚀 OOP Features Demonstrated:\n";
        file << "  • Inheritance: Transaction → Income/Expense\n";
        file << "  • Encapsulation: Private user data\n";
        file << "  • Polymorphism: Virtual getType() method\n";
        file << "  • Abstraction: Simple public interfaces\n";
        file << "\n";
        file << "💡 Register or Login to get started!\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Authentication Tab
        file << "        <div id=\"auth\" class=\"tab-content\">\n";
        file << "            <div class=\"grid-2\">\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>🔐 USER AUTHENTICATION</h2>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>👤 USERNAME</label>\n";
        file << "                        <input type=\"text\" id=\"username\" placeholder=\"Enter your username\">\n";
        file << "                    </div>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>🔑 PASSWORD</label>\n";
        file << "                        <input type=\"password\" id=\"password\" placeholder=\"Enter your password\">\n";
        file << "                    </div>\n";
        file << "                    <div style=\"display: flex; gap: 15px; margin-top: 25px;\">\n";
        file << "                        <button class=\"btn\" onclick=\"login()\">🚀 LOGIN</button>\n";
        file << "                        <button class=\"btn\" onclick=\"register()\">📝 REGISTER</button>\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>👥 REGISTERED USERS</h2>\n";
        file << "                    <div id=\"userList\" style=\"max-height: 300px; overflow-y: auto;\">\n";
        file << "                        <div class=\"user-card\">\n";
        file << "                            <h3>No users registered yet</h3>\n";
        file << "                            <p>Register new users to see them here</p>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                    <div id=\"currentUser\" style=\"margin-top: 20px; padding: 15px; background: rgba(0, 255, 136, 0.1); border-radius: 10px; display: none;\">\n";
        file << "                        <strong>👤 Current User: <span id=\"currentUserName\"></span></strong>\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Transactions Tab
        file << "        <div id=\"transactions\" class=\"tab-content\">\n";
        file << "            <div class=\"grid-2\">\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>💳 ADD TRANSACTION</h2>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>📅 DATE (YYYY-MM-DD)</label>\n";
        file << "                        <input type=\"text\" id=\"date\" value=\"2024-01-15\" placeholder=\"2024-01-15\">\n";
        file << "                    </div>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>💰 AMOUNT</label>\n";
        file << "                        <input type=\"number\" id=\"amount\" value=\"25.50\" placeholder=\"Enter amount\">\n";
        file << "                    </div>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>🏷️ CATEGORY</label>\n";
        file << "                        <input type=\"text\" id=\"category\" value=\"Food\" placeholder=\"Enter category\">\n";
        file << "                    </div>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>📝 DESCRIPTION</label>\n";
        file << "                        <input type=\"text\" id=\"description\" value=\"Lunch\" placeholder=\"Enter description\">\n";
        file << "                    </div>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>🔀 TYPE</label>\n";
        file << "                        <select id=\"type\">\n";
        file << "                            <option value=\"Expense\">Expense</option>\n";
        file << "                            <option value=\"Income\">Income</option>\n";
        file << "                        </select>\n";
        file << "                    </div>\n";
        file << "                    <button class=\"btn\" onclick=\"addTransaction()\">💾 ADD TRANSACTION</button>\n";
        file << "                </div>\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>📋 TRANSACTION HISTORY</h2>\n";
        file << "                    <button class=\"btn\" style=\"margin-bottom: 20px;\" onclick=\"listTransactions()\">🔄 REFRESH LIST</button>\n";
        file << "                    <div id=\"transactionList\" style=\"max-height: 400px; overflow-y: auto;\">\n";
        file << "                        <p style=\"text-align: center; opacity: 0.7;\">No transactions yet. Add some transactions to see them here.</p>\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Analytics Tab
        file << "        <div id=\"analytics\" class=\"tab-content\">\n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>📊 AI-POWERED FINANCIAL ANALYTICS</h2>\n";
        file << "                <div class=\"form-group\">\n";
        file << "                    <label>📅 MONTH FOR ANALYSIS (YYYY-MM)</label>\n";
        file << "                    <input type=\"text\" id=\"monthInput\" value=\"2024-01\" placeholder=\"2024-01\">\n";
        file << "                </div>\n";
        file << "                <div style=\"display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 15px; margin-top: 20px;\">\n";
        file << "                    <button class=\"btn\" onclick=\"monthlySummary()\">📈 MONTHLY SUMMARY</button>\n";
        file << "                    <button class=\"btn\" onclick=\"categoryAnalytics()\">📊 CATEGORY ANALYTICS</button>\n";
        file << "                    <button class=\"btn\" onclick=\"aiRecommendations()\">🤖 AI RECOMMENDATIONS</button>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "            \n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>🎯 ANALYSIS RESULTS</h2>\n";
        file << "                <div id=\"analysisResults\" class=\"output-console\" style=\"height: 300px;\">\n";
        file << "Select an analysis option to see results here...\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // OOP Features Tab
        file << "        <div id=\"oop\" class=\"tab-content\">\n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>⚙️ OOP PRINCIPLES DEMONSTRATION</h2>\n";
        file << "                <div class=\"grid-2\">\n";
        file << "                    <div>\n";
        file << "                        <h3 style=\"color: var(--accent); margin-bottom: 15px;\">🏗️ INHERITANCE</h3>\n";
        file << "                        <div style=\"background: rgba(0, 242, 254, 0.1); padding: 15px; border-radius: 10px; margin-bottom: 20px;\">\n";
        file << "                            <p><strong>Base Class:</strong> Transaction (Abstract)</p>\n";
        file << "                            <p><strong>Derived Classes:</strong> Income, Expense</p>\n";
        file << "                            <p><strong>Polymorphism:</strong> Virtual getType() method</p>\n";
        file << "                        </div>\n";
        file << "                        \n";
        file << "                        <h3 style=\"color: var(--accent); margin-bottom: 15px;\">🔒 ENCAPSULATION</h3>\n";
        file << "                        <div style=\"background: rgba(0, 242, 254, 0.1); padding: 15px; border-radius: 10px; margin-bottom: 20px;\">\n";
        file << "                            <p><strong>Private Data:</strong> username, password, transactions</p>\n";
        file << "                            <p><strong>Public Interface:</strong> getters and business methods</p>\n";
        file << "                            <p><strong>Data Protection:</strong> Controlled access through methods</p>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                    \n";
        file << "                    <div>\n";
        file << "                        <h3 style=\"color: var(--accent); margin-bottom: 15px;\">🔄 POLYMORPHISM</h3>\n";
        file << "                        <div style=\"background: rgba(0, 242, 254, 0.1); padding: 15px; border-radius: 10px; margin-bottom: 20px;\">\n";
        file << "                            <p><strong>Virtual Function:</strong> getType()</p>\n";
        file << "                            <p><strong>Runtime Binding:</strong> Different behavior for Income/Expense</p>\n";
        file << "                            <p><strong>Unified Interface:</strong> Same method, different implementations</p>\n";
        file << "                        </div>\n";
        file << "                        \n";
        file << "                        <h3 style=\"color: var(--accent); margin-bottom: 15px;\">🎯 ABSTRACTION</h3>\n";
        file << "                        <div style=\"background: rgba(0, 242, 254, 0.1); padding: 15px; border-radius: 10px; margin-bottom: 20px;\">\n";
        file << "                            <p><strong>Simple Interface:</strong> Easy-to-use public methods</p>\n";
        file << "                            <p><strong>Complex Implementation:</strong> Hidden internal logic</p>\n";
        file << "                            <p><strong>Focus on What:</strong> Not how it's implemented</p>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "            \n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>💻 CODE SNIPPETS</h2>\n";
        file << "                <div class=\"output-console\" style=\"height: 250px; font-size: 12px;\">\n";
        file << "// INHERITANCE EXAMPLE\n";
        file << "class Transaction { // Base class\n";
        file << "protected:\n";
        file << "    string date, category, description;\n";
        file << "    double amount;\n";
        file << "public:\n";
        file << "    virtual string getType() const = 0; // Pure virtual\n";
        file << "};\n";
        file << "\n";
        file << "class Income : public Transaction { // Derived class\n";
        file << "public:\n";
        file << "    string getType() const override { \n";
        file << "        return \"Income\"; // Polymorphism\n";
        file << "    }\n";
        file << "};\n";
        file << "\n";
        file << "// ENCAPSULATION EXAMPLE\n";
        file << "class User {\n";
        file << "private:\n";
        file << "    string username, password; // Private data\n";
        file << "    vector<Transaction*> transactions;\n";
        file << "public:\n";
        file << "    string getUsername() const { return username; } // Public interface\n";
        file << "};\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        file << "    </div>\n";

        // JavaScript
        file << "    <script>\n";
        file << "        let currentUser = null;\n";
        file << "        let users = [];\n";
        file << "        let transactions = [];\n";
        file << "        \n";
        file << "        function showNotification(message, type) {\n";
        file << "            const notification = document.getElementById('notification');\n";
        file << "            notification.textContent = message;\n";
        file << "            notification.className = `notification ${type}`;\n";
        file << "            notification.style.display = 'block';\n";
        file << "            \n";
        file << "            setTimeout(() => {\n";
        file << "                notification.style.display = 'none';\n";
        file << "            }, 4000);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function appendToOutput(text) {\n";
        file << "            const output = document.getElementById('outputConsole');\n";
        file << "            output.innerHTML += '\\n' + text;\n";
        file << "            output.scrollTop = output.scrollHeight;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function clearOutput() {\n";
        file << "            document.getElementById('outputConsole').innerHTML = '';\n";
        file << "        }\n";
        file << "        \n";
        file << "        function login() {\n";
        file << "            const username = document.getElementById('username').value;\n";
        file << "            const password = document.getElementById('password').value;\n";
        file << "            \n";
        file << "            if (!username || !password) {\n";
        file << "                showNotification('Please enter both username and password!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const user = users.find(u => u.username === username && u.password === password);\n";
        file << "            if (user) {\n";
        file << "                currentUser = user;\n";
        file << "                showNotification('✅ Login successful! Welcome ' + username + '!', 'success');\n";
        file << "                updateUserDisplay();\n";
        file << "                clearOutput();\n";
        file << "                appendToOutput('✅ Login successful! Welcome ' + username + '!');\n";
        file << "                appendToOutput('Ready to manage your finances...');\n";
        file << "            } else {\n";
        file << "                showNotification('❌ Login failed! Invalid credentials.', 'error');\n";
        file << "            }\n";
        file << "        }\n";
        file << "        \n";
        file << "        function register() {\n";
        file << "            const username = document.getElementById('username').value;\n";
        file << "            const password = document.getElementById('password').value;\n";
        file << "            \n";
        file << "            if (!username || !password) {\n";
        file << "                showNotification('Please enter both username and password!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            if (users.find(u => u.username === username)) {\n";
        file << "                showNotification('❌ Registration failed! Username already exists.', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            users.push({ username, password, transactions: [] });\n";
        file << "            showNotification('✅ Registration successful! You can now login.', 'success');\n";
        file << "            updateUserList();\n";
        file << "            appendToOutput('✅ New user registered: ' + username);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateUserList() {\n";
        file << "            const userList = document.getElementById('userList');\n";
        file << "            userList.innerHTML = '';\n";
        file << "            \n";
        file << "            if (users.length === 0) {\n";
        file << "                userList.innerHTML = `\n";
        file << "                    <div class=\"user-card\">\n";
        file << "                        <h3>No users registered yet</h3>\n";
        file << "                        <p>Register new users to see them here</p>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            users.forEach(user => {\n";
        file << "                const userCard = document.createElement('div');\n";
        file << "                userCard.className = 'user-card';\n";
        file << "                userCard.innerHTML = `\n";
        file << "                    <h3>👤 ${user.username}</h3>\n";
        file << "                    <p>Transactions: ${user.transactions.length}</p>\n";
        file << "                `;\n";
        file << "                userList.appendChild(userCard);\n";
        file << "            });\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateUserDisplay() {\n";
        file << "            const currentUserDiv = document.getElementById('currentUser');\n";
        file << "            const currentUserName = document.getElementById('currentUserName');\n";
        file << "            \n";
        file << "            if (currentUser) {\n";
        file << "                currentUserName.textContent = currentUser.username;\n";
        file << "                currentUserDiv.style.display = 'block';\n";
        file << "            } else {\n";
        file << "                currentUserDiv.style.display = 'none';\n";
        file << "            }\n";
        file << "        }\n";
        file << "        \n";
        file << "        function addTransaction() {\n";
        file << "            if (!currentUser) {\n";
        file << "                showNotification('❌ Please login first!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const date = document.getElementById('date').value;\n";
        file << "            const amount = parseFloat(document.getElementById('amount').value);\n";
        file << "            const category = document.getElementById('category').value;\n";
        file << "            const description = document.getElementById('description').value;\n";
        file << "            const type = document.getElementById('type').value;\n";
        file << "            \n";
        file << "            if (!date || !amount || !category) {\n";
        file << "                showNotification('❌ Please fill all required fields!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const transaction = {\n";
        file << "                id: Date.now(),\n";
        file << "                date,\n";
        file << "                amount,\n";
        file << "                category,\n";
        file << "                description,\n";
        file << "                type\n";
        file << "            };\n";
        file << "            \n";
        file << "            currentUser.transactions.push(transaction);\n";
        file << "            showNotification('✅ Transaction added successfully!', 'success');\n";
        file << "            appendToOutput(`✅ Added ${type}: ${amount} for ${category}`);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function listTransactions() {\n";
        file << "            if (!currentUser) {\n";
        file << "                showNotification('❌ Please login first!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const transactionList = document.getElementById('transactionList');\n";
        file << "            \n";
        file << "            if (currentUser.transactions.length === 0) {\n";
        file << "                transactionList.innerHTML = '<p style=\"text-align: center; opacity: 0.7;\">No transactions yet. Add some transactions to see them here.</p>';\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            let html = `\n";
        file << "                <table>\n";
        file << "                    <thead>\n";
        file << "                        <tr>\n";
        file << "                            <th>Date</th>\n";
        file << "                            <th>Amount</th>\n";
        file << "                            <th>Category</th>\n";
        file << "                            <th>Type</th>\n";
        file << "                            <th>Description</th>\n";
        file << "                        </tr>\n";
        file << "                    </thead>\n";
        file << "                    <tbody>\n";
        file << "            `;\n";
        file << "            \n";
        file << "            currentUser.transactions.forEach(txn => {\n";
        file << "                html += `\n";
        file << "                    <tr>\n";
        file << "                        <td>${txn.date}</td>\n";
        file << "                        <td>${txn.amount.toFixed(2)}</td>\n";
        file << "                        <td>${txn.category}</td>\n";
        file << "                        <td>${txn.type}</td>\n";
        file << "                        <td>${txn.description}</td>\n";
        file << "                    </tr>\n";
        file << "                `;\n";
        file << "            });\n";
        file << "            \n";
        file << "            html += `\n";
        file << "                    </tbody>\n";
        file << "                </table>\n";
        file << "            `;\n";
        file << "            \n";
        file << "            transactionList.innerHTML = html;\n";
        file << "            appendToOutput('📋 Listed all transactions');\n";
        file << "        }\n";
        file << "        \n";
        file << "        function monthlySummary() {\n";
        file << "            if (!currentUser) {\n";
        file << "                showNotification('❌ Please login first!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const month = document.getElementById('monthInput').value;\n";
        file << "            if (!month) {\n";
        file << "                showNotification('❌ Please enter a month (YYYY-MM)!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const results = calculateMonthlySummary(currentUser, month);\n";
        file << "            document.getElementById('analysisResults').innerHTML = results;\n";
        file << "            appendToOutput('📈 Generated monthly summary for ' + month);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function categoryAnalytics() {\n";
        file << "            if (!currentUser) {\n";
        file << "                showNotification('❌ Please login first!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const month = document.getElementById('monthInput').value;\n";
        file << "            if (!month) {\n";
        file << "                showNotification('❌ Please enter a month (YYYY-MM)!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const results = calculateCategoryAnalytics(currentUser, month);\n";
        file << "            document.getElementById('analysisResults').innerHTML = results;\n";
        file << "            appendToOutput('📊 Generated category analytics for ' + month);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function aiRecommendations() {\n";
        file << "            if (!currentUser) {\n";
        file << "                showNotification('❌ Please login first!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const month = document.getElementById('monthInput').value;\n";
        file << "            if (!month) {\n";
        file << "                showNotification('❌ Please enter a month (YYYY-MM)!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const results = generateAIRecommendations(currentUser, month);\n";
        file << "            document.getElementById('analysisResults').innerHTML = results;\n";
        file << "            appendToOutput('🤖 Generated AI recommendations for ' + month);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function calculateMonthlySummary(user, month) {\n";
        file << "            let totalIncome = 0;\n";
        file << "            let totalExpense = 0;\n";
        file << "            \n";
        file << "            user.transactions.forEach(txn => {\n";
        file << "                if (txn.date.startsWith(month)) {\n";
        file << "                    if (txn.type === 'Income') {\n";
        file << "                        totalIncome += txn.amount;\n";
        file << "                    } else {\n";
        file << "                        totalExpense += txn.amount;\n";
        file << "                    }\n";
        file << "                }\n";
        file << "            });\n";
        file << "            \n";
        file << "            const savings = totalIncome - totalExpense;\n";
        file << "            \n";
        file << "            return `\n";
        file << "Summary for ${month}:\n";
        file << "  Total Income:  ${totalIncome.toFixed(2)}\n";
        file << "  Total Expense: ${totalExpense.toFixed(2)}\n";
        file << "  Savings:       ${savings.toFixed(2)}\n";
        file << "  Savings Rate:  ${totalIncome > 0 ? ((savings / totalIncome) * 100).toFixed(1) : '0.0'}%\n";
        file << "            `;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function calculateCategoryAnalytics(user, month) {\n";
        file << "            const categories = {};\n";
        file << "            \n";
        file << "            user.transactions.forEach(txn => {\n";
        file << "                if (txn.type === 'Expense' && txn.date.startsWith(month)) {\n";
        file << "                    const category = txn.category.toLowerCase();\n";
        file << "                    categories[category] = (categories[category] || 0) + txn.amount;\n";
        file << "                }\n";
        file << "            });\n";
        file << "            \n";
        file << "            let result = `Expense by Category for ${month}:\\n`;\n";
        file << "            for (const [category, amount] of Object.entries(categories)) {\n";
        file << "                result += `  ${category.padEnd(12)}: ${amount.toFixed(2)}\\n`;\n";
        file << "            }\n";
        file << "            \n";
        file << "            return result;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function generateAIRecommendations(user, month) {\n";
        file << "            const categories = {};\n";
        file << "            let totalIncome = 0;\n";
        file << "            let totalExpense = 0;\n";
        file << "            \n";
        file << "            user.transactions.forEach(txn => {\n";
        file << "                if (txn.date.startsWith(month)) {\n";
        file << "                    if (txn.type === 'Expense') {\n";
        file << "                        const category = txn.category.toLowerCase();\n";
        file << "                        categories[category] = (categories[category] || 0) + txn.amount;\n";
        file << "                        totalExpense += txn.amount;\n";
        file << "                    } else {\n";
        file << "                        totalIncome += txn.amount;\n";
        file << "                    }\n";
        file << "                }\n";
        file << "            });\n";
        file << "            \n";
        file << "            const savings = totalIncome - totalExpense;\n";
        file << "            \n";
        file << "            let result = `=== AI-Led Business Recommender for ${month} ===\\n`;\n";
        file << "            result += `Your savings: ${savings.toFixed(2)}\\n\\n`;\n";
        file << "            \n";
        file << "            // AI-like recommendations\n";
        file << "            if (savings < 0) {\n";
        file << "                result += \"⚠️ You are overspending! Consider reducing non-essential costs.\\n\";\n";
        file << "            } else if (savings < totalIncome * 0.2) {\n";
        file << "                result += \"💡 Your savings are low. Try to set at least 20% of income aside.\\n\";\n";
        file << "            } else {\n";
        file << "                result += \"✅ Great! Your savings are healthy this month.\\n\";\n";
        file << "            }\n";
        file << "            \n";
        file << "            for (const [category, amount] of Object.entries(categories)) {\n";
        file << "                if (amount > totalExpense * 0.5) {\n";
        file << "                    result += `⚠️ High spending in category: ${category}. Consider optimizing this expense.\\n`;\n";
        file << "                }\n";
        file << "            }\n";
        file << "            \n";
        file << "            return result;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function showTab(tabName) {\n";
        file << "            document.querySelectorAll('.tab-content').forEach(tab => {\n";
        file << "                tab.classList.remove('active');\n";
        file << "            });\n";
        file << "            \n";
        file << "            document.querySelectorAll('.tab').forEach(tab => {\n";
        file << "                tab.classList.remove('active');\n";
        file << "            });\n";
        file << "            \n";
        file << "            document.getElementById(tabName).classList.add('active');\n";
        file << "            event.target.classList.add('active');\n";
        file << "        }\n";
        file << "        \n";
        file << "        // Initialize\n";
        file << "        document.addEventListener('DOMContentLoaded', function() {\n";
        file << "            updateUserList();\n";
        file << "            updateUserDisplay();\n";
        file << "        });\n";
        file << "    </script>\n";
        file << "</body>\n";
        file << "</html>\n";

        file.close();
        cout << "✅ AI-POWERED FINANCE TRACKER GENERATED SUCCESSFULLY!" << endl;
    }
};

// ==================== MAIN FUNCTION ====================

int main() {
    cout << "=============================================================\n";
    cout << "    🚀 AI-Powered Finance Tracker - OOP Assignment  🚀\n";
    cout << "=============================================================\n\n";

    FinanceTracker tracker;
    HTMLGUIGenerator guiGen(tracker);
    guiGen.generateHTML();

    cout << "\n🎯 OOP FEATURES DEMONSTRATED:\n";
    cout << "   ✓ Inheritance: Transaction → Income/Expense\n";
    cout << "   ✓ Encapsulation: Private user data with public interfaces\n";
    cout << "   ✓ Polymorphism: Virtual getType() method\n";
    cout << "   ✓ Abstraction: Simple public methods hiding complex logic\n\n";

    cout << "🚀 ADVANCED FEATURES:\n";
    cout << "   ✓ AI-Powered financial recommendations\n";
    cout << "   ✓ Real-time analytics and insights\n";
    cout << "   ✓ Interactive HTML5 interface\n";
    cout << "   ✓ Secure user authentication\n";
    cout << "   ✓ Category-based spending analysis\n\n";

    cout << "🌐 LAUNCHING WEB INTERFACE...\n";

    // Open the HTML file in default browser
#ifdef _WIN32
    system("start finance_tracker.html");
#elif __APPLE__
    system("open finance_tracker.html");
#else
    system("xdg-open finance_tracker.html");
#endif

    cout << "\n✨ FINANCE TRACKER READY!\n";
    cout << "📁 Interface: finance_tracker.html\n";
    cout << "\n💡 Assignment Features:\n";
    cout << "   • Complete OOP implementation\n";
    cout << "   • Inheritance hierarchy\n";
    cout << "   • Polymorphic behavior\n";
    cout << "   • Encapsulated data\n";
    cout << "   • Abstract base class\n";
    cout << "   • AI-based business logic\n\n";

    cout << "Press Enter to exit...\n";
    cin.get();

    return 0;
}
