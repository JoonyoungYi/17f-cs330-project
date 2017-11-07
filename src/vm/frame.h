#include <list.h>
#include "threads/palloc.h"
#include "lib/kernel/hash.h"

// frame entry
struct frame
  {
    void *kpage;
    struct spte *spte;
    struct list_elem list_elem;
    struct hash_elem hash_elem;
  };

void frame_init (void);
void *frame_get_page (enum palloc_flags flags);
void frame_free_page (void *page);
