#include "bank.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void account_add_txn(Account *acc, TxnType type, double amount, const char *note)
{
  if (acc->txn_count >= MAX_TRANSACTIONS) {
    memmove(&acc->history[0], &acc->history[1], sizeof(Transaction)*(MAX_TRANSACTIONS -1));
    acc->txn_count = MAX_TRANSACTIONS - 1;
  }

  Transaction *t = &acc->history[acc->txn_count++];
  t->type = type;
  t->amount=amount;
  t->balance_after=acc->balance;
  t->timestamp=time(NULL);
  strncpy(t->note, note, sizeof(t->note)-1);
  t->note[sizeof(t->note) - 1];
}

int account_create(Bank *bank, const char *name, const char *acc_num, const char *pin, double initial_balance){
  if(bank->count >= MAX_ACCOUNTS) return -1;
  if(account_find(bank, acc_num)>= 0) return -2;
  if(initial_balance < 0) return -3;

  Account *a = &bank->accounts[bank->count++];
  memset(a, 0, sizeof(*a));

  strncpy(a->name, name, MAX_NAME_LEN - 1);
  strncpy(a->account_num, acc_num, ACCOUNT_NUM_LEN - 1);
  strncpy(a->pin_hash, pin, PIN_LEN - 1);
  a->balance = initial_balance;
  a->txn_count = 0;

  if (initial_balance > 0) {
    account_add_txn(a, TXN_DEPOSIT, initial_balance, "Initial Deposit");
  }

  return bank->count - 1;
}

int account_find(Bank *bank, const char *acc_num){
  for (int i = 0; i < bank->count; i++) {
    if(strcmp(bank->accounts[i].account_num, acc_num)==0) return i;
  }
  return -1;
}

int account_login(Bank *bank, const char *acc_num, const char *pin){
  int idx = account_find(bank, acc_num);
  if(idx < 0) return -1;
  if(strcmp(bank->accounts[idx].pin_hash, pin) != 0) return -2;
  return idx;
}

int account_deposit(Bank *bank, int idx, double amount){
  if(idx< 0 || idx >= bank->count) return -1;
  if(amount <= 0) return -2;

  bank->accounts[idx].balance += amount;
  account_add_txn(&bank->accounts[idx], TXN_DEPOSIT, amount, "Cash deposit");
  return 0;
}

int account_withdraw(Bank *bank, int idx, double amount) {
  if(idx < 0 || idx >= bank->count) return -1;
  if(amount <= 0) return -2;
  if(bank->accounts[idx].balance < amount) return -3;

  bank->accounts[idx].balance -= amount;
  account_add_txn(&bank->accounts[idx], TXN_WITHDRAW, amount, "Cash withdrawal");
  return 0;
}

int account_transfer(Bank *bank, int from_idx, const char *to_acc_num, double amount) {
  if(from_idx < 0 || from_idx >= bank->count) return -1;
  if(amount <= 0) return -2;
  if(bank->accounts[from_idx].balance < amount) return -3;

  int to_idx = account_find(bank, to_acc_num);
  if(to_idx < 0) return -4;
  if(to_idx == from_idx) return -5;

  char note[64];

  bank->accounts[from_idx].balance -= amount;
  snprintf(note, sizeof(note), "Transfer to %s", to_acc_num);
  account_add_txn(&bank->accounts[from_idx], TXN_TRANSFER_OUT, amount, note);

  bank->accounts[to_idx].balance += amount;
  snprintf(note, sizeof(note), "Transfer from %s",bank->accounts[from_idx].account_num);
  account_add_txn(&bank->accounts[to_idx], TXN_TRANSFER_IN, amount, note);

  return 0;
}
