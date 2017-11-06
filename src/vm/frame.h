#include <list.h>

// supplemenatry page table entry
struct frame
  {
    void* kpage;
    struct spte *spte;
    struct list_elem list_elem;
  };
