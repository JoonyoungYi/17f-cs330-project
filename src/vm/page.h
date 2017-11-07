#include <list.h>
#include "lib/kernel/hash.h"

enum spte_status
  {
    INITIAL,
    ON_MEMORY,
    ON_DISK
  };

// supplemenatry page table
struct spt
  {
    struct hash hash;
    struct list list;
  };

// supplemenatry page table entry
struct spte
  {
    void* upage;
    void* kpage;
    struct hash_elem hash_elem;
    struct list_elem list_elem;
    enum spte_status status;
    bool dirty;
    struct thread *thread;
  };

struct spt* page_create_spt (struct thread* t);
void page_remove_spt ();
