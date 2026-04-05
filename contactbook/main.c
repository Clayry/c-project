#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define NAME_LEN 100
#define PHONE_LEN 20
#define EMAIL_LEN 100
#define DATA_FILE "contacts.dat"

typedef struct Contact {
  char name[NAME_LEN];
  char phone[PHONE_LEN];
  char email[EMAIL_LEN];
  struct Contact *prev;
  struct Contact *next;
} Contact;

typedef enum { ACTION_DELETE, ACTION_EDIT } ActionType;

typedef struct UndoRecord {
  ActionType action;
  char name[NAME_LEN];
  char phone[PHONE_LEN];
  char email[EMAIL_LEN];
  struct UndoRecord *below;
} UndoRecord;

static Contact *head = NULL;
static Contact *tail = NULL;
static int count = 0;
static UndoRecord *undoTop = NULL;

static Contact *alloc_contact(const char *name, const char *phone,
                              const char *email) {
  Contact *c = (Contact *)malloc(sizeof(Contact));
  if (!c) {
    fprintf(stderr, "[ERROR] Out of memory allocating contact.\n");
    exit(EXIT_FAILURE);
  }
  strncpy(c->name, name, NAME_LEN - 1);
  c->name[NAME_LEN - 1] = '\0';
  strncpy(c->phone, phone, PHONE_LEN - 1);
  c->phone[PHONE_LEN - 1] = '\0';
  strncpy(c->email, email, EMAIL_LEN - 1);
  c->email[EMAIL_LEN - 1] = '\0';
  return c;
}

static void free_contact(Contact *c) { free(c); }

static void undo_push(ActionType action, const char *name, const char *phone,
                      const char *email) {
  UndoRecord *rec = (UndoRecord *)malloc(sizeof(UndoRecord));
  if (!rec) {
    fprintf(stderr, "[WARN] Could not record undo state (OOM).\n");
    return;
  }
  rec->action = action;
  strncpy(rec->name, name, NAME_LEN - 1);
  rec->name[NAME_LEN - 1] = '\0';
  strncpy(rec->phone, phone, PHONE_LEN - 1);
  rec->phone[PHONE_LEN - 1] = '\0';
  strncpy(rec->email, email, EMAIL_LEN - 1);
  rec->email[EMAIL_LEN - 1] = '\0';
  rec->below = undoTop;
  undoTop = rec;
}

static int undo_pop(UndoRecord *out) {
  if (!undoTop)
    return 0;
  *out = *undoTop;
  UndoRecord *old = undoTop;
  undoTop = undoTop->below;
  free(old);
  return 1;
}

static void undo_destroy(void) {
  while (undoTop) {
    UndoRecord *tmp = undoTop;
    undoTop = undoTop->below;
    free(tmp);
  }
}

void add_contact(const char *name, const char *phone, const char *email) {
  for (Contact *cur = head; cur; cur = cur->next) {
    if (strcasecmp(cur->name, name) == 0) {
      printf("[WARN] A contact named \"%s\" already exists.\n", name);
      break;
    }
  }
  Contact *c = alloc_contact(name, phone, email);

  if (!head) {
    head = tail = c;
  } else {
    c->prev = tail;
    tail->next = c;
    tail = c;
  }
  count++;
  printf("[OK] Contact \"%s\" added (total: %d).\n", name, count);
}

static Contact *find_contact(const char *name) {
  for (Contact *cur = head; cur; cur = cur->next) {
    if (strcmp(cur->name, name) == 0)
      return cur;
  }
  return NULL;
}

static void unlink_node(Contact *node) {
  if (node->prev)
    node->prev->next = node->next;
  else
    head = node->next;

  if (node->next)
    node->next->prev = node->prev;
  else
    tail = node->prev;

  node->prev = node->next = NULL;
  count--;
}

void delete_contact(const char *name) {
  Contact *c = find_contact(name);
  if (!c) {
    printf("[NOT FOUND] No contact named \"%s\".\n", name);
    return;
  }
  undo_push(ACTION_DELETE, c->name, c->phone, c->email);

  unlink_node(c);
  free_contact(c);
  printf("[OK] Contact \"%s\" deleted. (Undo available)\n", name);
}

void search_contact(const char *name) {
  Contact *c = find_contact(name);
  if (!c) {
    printf("[NOT FOUND] No contact named \"%s\". \n", name);
    return;
  }
  printf("\n┌─────────────────────────────────┐\n");
  printf("│  Name  : %-22s │\n", c->name);
  printf("│  Phone : %-22s │\n", c->phone);
  printf("│  Email : %-22s │\n", c->email);
  printf("└─────────────────────────────────┘\n");
}

void edit_contact(const char *name, const char *new_phone,
                  const char *new_email) {
  Contact *c = find_contact(name);
  if (!c) {
    printf("[NOT FOUND] No contact named \"%s\".\n", name);
    return;
  }
  undo_push(ACTION_EDIT, c->name, c->phone, c->email);

  if (new_phone && new_phone[0] != '\0')
    strncpy(c->phone, new_phone, PHONE_LEN - 1);
  if (new_email && new_email[0] != '\0')
    strncpy(c->email, new_email, EMAIL_LEN - 1);

  printf("[OK] Contact \"%s\" updated. (Undo available)\n", name);
}

void display_contacts(void) {
  if (!head) {
    printf("[INFO] The contact book is empty.\n");
    return;
  }
  printf("\n%-4s  %-25s  %-15s  %s\n", "No.", "Name", "Phone", "Email");
  printf("%-4s  %-25s  %-15s  %s\n", "----", "-------------------------",
         "---------------", "----------------------------------------");
  int i = 1;
  for (Contact *cur = head; cur; cur = cur->next, i++) {
    printf("%-4d %-25s %-15s %s\n", i, cur->name, cur->phone, cur->email);
  }
  printf("\n Total: %d contact(s)\n", count);
}

typedef int (*CmpFn)(const Contact *, const Contact *);

static int cmp_name(const Contact *a, const Contact *b) {
  return strcmp(a->name, b->name);
}

static int cmp_phone(const Contact *a, const Contact *b) {
  return strcmp(a->phone, b->phone);
}

static Contact *split_list(Contact *node) {
  Contact *slow = node;
  Contact *fast = node->next;
  while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
  }
  Contact *second = slow->next;
  slow->next = NULL;
  if (second)
    second->prev = NULL;
  return second;
}

static Contact *merge(Contact *a, Contact *b, CmpFn cmp) {
  if (!a)
    return b;
  if (!b)
    return a;

  Contact *merged;
  if (cmp(a, b) <= 0) {
    merged = a;
    merged->next = merge(a, b->next, cmp);
  } else {
    merged = b;
    merged->next = merge(a, b->next, cmp);
  }
  if (merged->next)
    merged->next->prev = merged;
  return merged;
}

static Contact *merge_sort(Contact *node, CmpFn cmp) {
  if (!node || !node->next)
    return node;

  Contact *second = split_list(node);
  node = merge_sort(node, cmp);
  second = merge_sort(second, cmp);
  return merge(node, second, cmp);
}

static void sort_contacts(CmpFn cmp, const char *label) {
  if (!head) {
    printf("[INFO] Nothing to sort.\n");
    return;
  }
  head = merge_sort(head, cmp);

  head->prev = NULL;
  Contact *cur = head;
  while (cur->next)
    cur = cur->next;
  tail = cur;

  printf("[OK] Contacts sorted by %s.\n", label);
}

void sort_by_name(void) { sort_contacts(cmp_name, "name"); }
void sort_by_phone(void) { sort_contacts(cmp_phone, "phone"); }

void undo_last(void) {
  UndoRecord rec;
  if (!undo_pop(&rec)) {
    printf("[INFO] Nothing to undo.\n");
    return;
  }

  if (rec.action == ACTION_DELETE) {
    Contact *c = alloc_contact(rec.name, rec.phone, rec.email);
    if (!head) {
      head = tail = c;
    } else {
      c->prev = tail;
      tail->next = c;
      tail = c;
    }
    count++;
    printf("[UNDO] Contact \"%s\" restored successfully.\n", rec.name);

  } else {
    Contact *c = find_contact(rec.name);
    if (!c) {
      printf("[UNDO] Could not found \"%s\" to restore - "
             "it may have been deleted after the edit.\n",
             rec.name);
      return;
    }
    strncpy(c->phone, rec.phone, PHONE_LEN - 1);
    strncpy(c->email, rec.email, EMAIL_LEN - 1);
    printf("[UNDO] Contact \"%s\" reverted to previous values.\n", rec.name);
  }
}

void save_contacts(void) {
  if (!head) {
    printf("[INFO] Nothing to save.\n");
    return;
  }
  FILE *fp = fopen(DATA_FILE, "w");
  if (!fp) {
    perror("[ERROR] Cannot open file for writing");
    return;
  }
  for (Contact *cur = head; cur; cur = cur->next) {
    fprintf(fp, "%s\n%s\n%s\n\n", cur->name, cur->phone, cur->email);
  }
  fclose(fp);
  printf("[OK] %d contact(s) saved to \"%s\".\n", count, DATA_FILE);
}

void load_contacts(void) {
  FILE *fp = fopen(DATA_FILE, "r");
  if (!fp)
    return;

  char name[NAME_LEN], phone[PHONE_LEN], email[EMAIL_LEN];
  int loaded = 0;

  while (fgets(name, sizeof name, fp) && fgets(phone, sizeof phone, fp) &&
         fgets(email, sizeof email, fp)) {
    name[strcspn(name, "\n")] = '\0';
    phone[strcspn(phone, "\n")] = '\0';
    email[strcspn(email, "\n")] = '\0';

    if (name[0] == '\0')
      continue;

    Contact *c = alloc_contact(name, phone, email);
    if (!head) {
      head = tail = c;
    } else {
      c->prev = tail;
      tail->next = c;
      tail = c;
    }
    count++;
    loaded++;

    char tmp[8];
    (void)fgets(tmp, sizeof tmp, fp);
  }

  fclose(fp);
  if (loaded > 0)
    printf("[OK] Loaded %d contact(s) from \"%s\".\n", loaded, DATA_FILE);
}

void destroy_list(void) {
  Contact *cur = head;
  while (cur) {
    Contact *nxt = cur->next;
    free_contact(cur);
    cur = nxt;
  }
  head = tail = NULL;
  count = 0;
  undo_destroy();
  printf("[OK] Contact book cleared and memory freed.\n");
}

static int read_line(const char *prompt, char *buf, int size) {
  printf("%s", prompt);
  fflush(stdout);
  if (!fgets(buf, size, stdin))
    return 0;
  buf[strcspn(buf, "\n")] = '\0';
  return 1;
}

static void print_menu(void) {
  printf("\n╔══════════════════════════════╗\n");
  printf("║      CONTACT BOOK MENU       ║\n");
  printf("╠══════════════════════════════╣\n");
  printf("║  1. Add contact              ║\n");
  printf("║  2. Delete contact           ║\n");
  printf("║  3. Search contact           ║\n");
  printf("║  4. Edit contact             ║\n");
  printf("║  5. Display all contacts     ║\n");
  printf("║  6. Sort by name             ║\n");
  printf("║  7. Sort by phone            ║\n");
  printf("║  8. Save to file             ║\n");
  printf("║  9. Undo last action         ║\n");
  printf("║  0. Exit                     ║\n");
  printf("╚══════════════════════════════╝\n");
  printf("  Choice: ");
  fflush(stdout);
}

int main(void) {
  printf("=== Contact Book (Doubly Linked List) ===\n");

  load_contacts();

  char choice[8];
  char name[NAME_LEN], phone[PHONE_LEN], email[EMAIL_LEN];

  while (1) {
    print_menu();

    if (!fgets(choice, sizeof choice, stdin))
      break;
    choice[strcspn(choice, "\n")] = '\0';

    switch (choice[0]) {
    case '1':
      read_line(" Name  : ", name, NAME_LEN);
      read_line(" Phone : ", phone, PHONE_LEN);
      read_line(" Email : ", email, EMAIL_LEN);
      add_contact(name, phone, email);
      break;

    case '2':
      read_line(" Name to delete: ", name, NAME_LEN);
      search_contact(name);
      break;

    case '3':
      read_line(" Name to search: ", name, NAME_LEN);
      search_contact(name);
      break;

    case '4':
      read_line(" Name to edit : ", name, NAME_LEN);
      read_line(" New phone (Enter to keep): ", phone, PHONE_LEN);
      read_line(" New email (Enter to keep): ", email, EMAIL_LEN);
      edit_contact(name, phone, email);
      break;

    case '5':
      display_contacts();
      break;

    case '6':
      sort_by_name();
      break;

    case '7':
      sort_by_phone();
      break;

    case '8':
      save_contacts();
      break;

    case '9':
      undo_last();
      break;

    case '0':
      destroy_list();
      printf("Goodbye!\n");
      return 0;

    default:
      printf("[WARN] Unknown option '%s'.\n", choice);
    }
  }

  destroy_list();
  return 0;
}
