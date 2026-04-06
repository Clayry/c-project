#include "bank.h"
#include <stdio.h>
#include <string.h>

int main(void){
  Bank bank;
  memset(&bank, 0, sizeof(bank));

  int load_result = file_load(&bank);
  if (load_result == -1) {
    
  }else if (load_result < -1) {
    fprintf(stderr, "[!] Warning: data file may be corrupted (code %d).\n", load_result);
    fprintf(stderr, "  Starting with empty bank.\n");
    memset(&bank, 0, sizeof(bank));
  }

  ui_run(&bank);

  return 0;
}

