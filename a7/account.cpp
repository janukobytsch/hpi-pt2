#include <iostream>
#include <cassert>
#include <list>
#include <cstdlib>
#include <string>

using namespace std;


typedef struct LogEntry
{
    std::string keyword;
    double amount;

    LogEntry(std::string kw, double a) : keyword(kw), amount(a) {};
} LogEntry;


class Account {
public:
    Account(long accountNo, double balance, double limit, bool locked);

    virtual ~Account();

    long accountNo() const { return accountNo_; } 

    virtual void setLimit(double limit) { limit_ = limit; }
    double getLimit() const { return limit_; }

    virtual bool credit(double amount);
    virtual bool debit(double amount);
    double balance() const { return balance_; } 

    void setLock(bool locked) { locked_ = locked; } 
    bool getLock() const { return locked_; }

private: 
    long accountNo_; 
    double balance_; 
    double limit_; 
    bool locked_;
};

Account::Account(long accountNo, double balance, double limit, bool locked) : 
    accountNo_{accountNo}, 
    balance_{balance}, 
    limit_(limit), 
    locked_{locked} { 
}

Account::~Account() { 
}

bool Account::credit(double amount) {
    assert(amount>=0.0); 

    // cannot use locked account
    if(locked_) return false; 

    balance_ = balance_ + amount; 
    return true; 
}

bool Account::debit(double amount) {
    assert(amount>=0.0); 

    // cannot use locked account
    if(locked_) return false; 
    
    // check if limit is hit
    if(balance_-amount<limit_) return false; 

    // change balance
    balance_ = balance_ - amount; 
    return true; 
}

class LoggedAccount : public Account
{
public:
    typedef Account super;

    LoggedAccount(long accountNo, double balance, double limit, bool locked)
        : super(accountNo, balance, limit, locked) {
            addLogEntry(LogEntry("initial balance", balance));
        }

    std::list<LogEntry> getLog() const { return _log; }
    bool debit(double amount);
    void setLimit(double limit);
    bool credit(double amount);

private:
    void setLog(std::list<LogEntry> log) { _log = log; }
    void addLogEntry(LogEntry entry);

    std::list<LogEntry> _log;
};

void LoggedAccount::addLogEntry(LogEntry entry) {
    if (!_log.empty()) {
        _log.pop_back();
    }
    _log.push_back(entry);
    _log.push_back(LogEntry("current balance", balance()));
}

bool LoggedAccount::debit(double amount) {
    addLogEntry(LogEntry("debit", amount));
    super::debit(amount);
}

void LoggedAccount::setLimit(double limit) {
    addLogEntry(LogEntry("limit change", limit));
    super::setLimit(limit);
}

bool LoggedAccount::credit(double amount) {
    addLogEntry(LogEntry("credit", amount));
    return super::credit(amount);
}

int main(int argc, char** argv) {
  Account A1(19920, 0.0, -1000.0, false); 
  LoggedAccount A2(20020, 0.0, -1000.0, false);

  A1.credit(500.0);
  A2.credit(500.0);
  A2.debit(100.0);
  A2.setLimit(-2000.);

  for (const auto& x : A2.getLog()) {
      cout << x.keyword << ": " << x.amount << endl;
  }

  return 0; 
}
