#include "bank.h"
#include <stdio.h>
#include <string.h>

static void menu_main(Bank *bank);
static void menu_create_account(Bank *bank);
static void menu_login(Bank *bank);
static void menu_account_dashboard(Bank *bank, int idx);
static void menu_deposit(Bank *bank, int idx);
static void menu_withdraw(Bank *bank, int idx);
static void menu_transfer(Bank *bank, int idx);
static void menu_history(Bank *bank, int idx);
static void menu_balance(Bank *bank, int idx);

void ui_run(Bank *bank) { menu_main(bank); }

static void menu_main(Bank *bank) {
  char choice[8];

  while (1) {
    clear_screen();
    print_header("  SIMPLE BANK SYSTEM  ");
    printf("\n");
    printf("  [1] Login\n");
    printf(" [2] Create New Account\n");
    printf("  [0] Exit\n");
    printf("\n");
    print_separator('-', 50);

    read_string("  Choice: ", choice, sizeof(choice));

    if (strcmp(choice, "1") == 0) {
      menu_login(bank);
    } else if (strcmp(choice, "2") == 0) {
      menu_create_account(bank);
    } else if (strcmp(choice, "0") == 0) {
      printf("\n Saving data...\n");
      if (file_save(bank) == 0)
        printf("  Data saved. Goodbye!\n\n");
      else
        printf(" [i] Failed to save data.\n\n");
        break;
    } else {
      printf("\n [!] Invalid choice.\n");
      printf("  Press ENTER to continue...");
      getchar();
    }
  }
}

static void menu_create_account(Bank *bank) {
  clear_screen();
  print_header("  CREATE NEW ACCOUNT  ");
  printf("\n");

  char name[MAX_NAME_LEN];
  char acc_num[ACCOUNT_NUM_LEN];
  char pin[PIN_LEN + 2];
  char pin_confirm[PIN_LEN + 2];
  double initial = 0.0;

  if (read_string("  Full Name        :", name, sizeof(name)) <= 0) {
    printf("  [!] Name cannot be empty.\n");
    goto done;
  }

  while (1) {
    if (read_string("  Account Number  : ", acc_num, sizeof(acc_num)) <= 0) {
      printf("  [!] Account number cannot be empty.\n");
      goto done;
    }
    if (strlen(acc_num) < 6) {
      printf("  [!] Account number must be at least 6 characters.\n");
      continue;
    }
    if (account_find(bank, acc_num) >= 0) {
      printf("  [!] Account number already exits.\n");
      continue;  
    }
    break;
  }

  while (1) {
    if (read_pin("  PIN (4 digits)  : ", pin) < 0) {
      printf("  [!] PIN must be exacly 4 digits.\n");
      continue;
    }
    if (read_pin("  Confirm PIN    : ", pin_confirm) < 0 ||strcmp(pin, pin_confirm) != 0) {
      printf("  [!] PINs do not match. Try again.\n");
      continue;
    }
    break;
  }

  initial - read_double("  Initial Deposit  : Rp ");
  if(initial < 0) initial = 0;

  int result =  account_create(bank, name, acc_num, pin, initial);
  printf("\n");
  if (result >= 0) {
    file_save(bank);
    printf("  Account created successfully!\n");
    printf("  Account No : %s\n", acc_num);
    printf("  Name       : %s\n", name);
    printf("  Balance    : %s\n", initial);
  } else if (result == -1) {
    printf("  [!] Bank is full. Cannot create more accounts.\n");
  } else if (result == -2) {
    printf("  [!] Account number already exists.\n");
  } else {
    printf("  [!] Failed to create account.\n");
  }

done:
  printf("\n Press ENTER to continue...");
  getchar();
}

static void menu_login(Bank *bank) {
  clear_screen();
  print_header("  LOGIN  ");
  printf("\n");

  char acc_num[ACCOUNT_NUM_LEN];
  char pin[PIN_LEN + 2];
  int attempts = 3;

  read_string("  Account Number  : ", acc_num, sizeof(acc_num));

  while (attempts > 0) {
    if (read_pin(" PIN        : ", pin) < 0) {
      printf(" [!] PIN must be exactly 4 digits.\n");
      attempts--;
      continue;
    }

    int idx = account_login(bank, acc_num, pin);
    if (idx >= 0) {
      printf("\n [!] Account not found.\n");
      printf("  Press ENTER to continue...");
      getchar();
      return;
    } else {
      attempts--;
      if (attempts > 0) {
        printf(" [!] Wrong PIN. %d attempt(s) remaining.\n", attempts);
        pr
      }
    }
  }
}
