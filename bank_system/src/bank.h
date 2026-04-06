#ifndef BANK_H
#define BANK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Constants ─────────────────────────────────────────────────────────────── */
#define MAX_ACCOUNTS       100
#define MAX_NAME_LEN       64
#define ACCOUNT_NUM_LEN    10
#define PIN_LEN            5       /* 4-digit PIN + null terminator */
#define MAX_TRANSACTIONS   50
#define DATA_FILE          "bank_data.bin"

/* ── Transaction types ─────────────────────────────────────────────────────── */
typedef enum {
    TXN_DEPOSIT = 1,
    TXN_WITHDRAW,
    TXN_TRANSFER_OUT,
    TXN_TRANSFER_IN
} TxnType;

/* ── Transaction record ────────────────────────────────────────────────────── */
typedef struct {
    TxnType     type;
    double      amount;
    double      balance_after;
    time_t      timestamp;
    char        note[64];           /* e.g. "Transfer to 1234567890" */
} Transaction;

/* ── Account ───────────────────────────────────────────────────────────────── */
typedef struct {
    char        name[MAX_NAME_LEN];
    char        account_num[ACCOUNT_NUM_LEN];
    char        pin_hash[PIN_LEN];  /* stored as plain text for simplicity */
    double      balance;
    int         txn_count;
    Transaction history[MAX_TRANSACTIONS];
} Account;

/* ── Bank (in-memory state) ────────────────────────────────────────────────── */
typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int     count;
} Bank;

/* ── Function declarations ─────────────────────────────────────────────────── */

/* account.c */
int  account_create(Bank *bank, const char *name, const char *acc_num,
                    const char *pin, double initial_balance);
int  account_find(Bank *bank, const char *acc_num);
int  account_login(Bank *bank, const char *acc_num, const char *pin);
int  account_deposit(Bank *bank, int idx, double amount);
int  account_withdraw(Bank *bank, int idx, double amount);
int  account_transfer(Bank *bank, int from_idx, const char *to_acc_num, double amount);
void account_add_txn(Account *acc, TxnType type, double amount, const char *note);

/* file.c */
int  file_save(const Bank *bank);
int  file_load(Bank *bank);

/* ui.c */
void ui_run(Bank *bank);

/* utils.c */
void     clear_screen(void);
void     print_separator(char c, int width);
void     print_header(const char *title);
char    *txn_type_str(TxnType t);
void     format_time(time_t t, char *buf, size_t len);
int      read_string(const char *prompt, char *buf, size_t maxlen);
double   read_double(const char *prompt);
int      read_pin(const char *prompt, char *pin_buf);

#endif /* BANK_H */
