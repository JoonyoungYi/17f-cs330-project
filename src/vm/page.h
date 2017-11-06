#include <list.h>
#include "lib/kernel/hash.h"

enum spte_status {
  INITIAL,
  ON_MEMORY,
  ON_DISK
};

// supplemenatry page table
struct spt
  {
    struct hash hash;
    struct list list;
  }

// supplemenatry page table entry
struct spte
  {
    void* upage;
    void* kpage;
    struct hash_elem elem;
    enum spte_status status;
    bool dirty;
    struct thread *thread;
  };
