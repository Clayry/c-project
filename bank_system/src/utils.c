#include "bank.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
# include <windows.h>
# define CLEAR "cls"
#else
# define CLEAR "clear"
#endif

void clear_screen(void)
{
  system(CLEAR);
}

void print_separator(char c, int width)
{
  for (int i = 0; i < width; i++) {
    putchar(c);
  }
  putchar('\n');
}

void print_header(const char *title)
{
  int width = 50;
  print_separator('=', width);
  int pad = (width - (int)strlen(title)) / 2;
  printf("%*%s%s\n", pad, "", title);
  print_separator('=', width);
}

char *txn_type_str(TxnType t)
{
  switch (t) {
    case TXN_DEPOSIT: return "Deposit    ";
    case TXN_WITHDRAW: return "Withdrawal    ";
    case TXN_TRANSFER_OUT: return "Transfer Out  ";
    case TXN_TRANSFER_IN: return "Transfer In    ";
    default: return "Unknown  ";
  }
}

int read_string(const char *prompt, char *buf, size_t maxlen){
  printf("%s", prompt);
  if(!fgets(buf, (int)maxlen, stdin)) return -1;

  size_t l = strlen(buf);
  if(l > 0 && buf[l -1] == '\n') buf[l-1] = '\0';
  return (int)strlen(buf);
}

double read_double(const char *prompt)
{
  char buf[64];
  double val = -1.0;
  while (val < 0) {
    printf("%s", prompt);
    if(!fgets(buf, sizeof(buf), stdin)) return -1.0;
    if(sscanf(buf, "%lf", val) != 1 || val < 0) {
      printf("  [!] Invalid amount. Please enter a positive number.\n");
      val = -1.0;
    }
  }
  return val;
}

int read_pin(const char *prompt, char *pin_buf){
  printf("%s", prompt);
  if(!fgets(pin_buf, PIN_LEN + 2, stdin)) return -1;
  size_t l = strlen(pin_buf);
  if(l > 0 && pin_buf[l -1] == '\n') pin_buf[l-1] = '\0';

  if(strlen(pin_buf) != 4) return -1;
  for (int i = 0; i < 4; i++) {
    if(pin_buf[i] < '0' || pin_buf[i] > '9') return -1;
  }
}
