#include <list.h>
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "vm/frame.h"
#include "vm/page.h"

static struct hash frams_hash;
static struct list frame_list;

/* */
unsigned frame_hash (const struct hash_elem *f_, void *aux UNUSED)
{
  const struct frame *f = hash_entry (f_, struct frame, hash_elem);
  return hash_bytes (&f->kpage, sizeof f->kpage);
}

/* */
bool frame_less (const struct hash_elem *a_,
                 const struct hash_elem *b_,
                 void *aux UNUSED)
{
  const struct frame *a = hash_entry (a_, struct frame, hash_elem);
  const struct frame *b = hash_entry (b_, struct frame, hash_elem);
  return a->kpage < b->kpage;
}

/* Initializes the vm allocator. */
void
frame_init (void)
{
  hash_init (&frame_hash, frame_hash, frame_less, NULL);
  list_init (&frame_list);
}

/* Obtains a single free page and returns its kernel virtual
   address.
   If PAL_USER is set, the page is obtained from the user pool,
   otherwise from the kernel pool.  If PAL_ZERO is set in FLAGS,
   then the page is filled with zeros.  If no pages are
   available, returns a null pointer, unless PAL_ASSERT is set in
   FLAGS, in which case the kernel panics.
   frame_flags equals to palloc_flags */
void *
frame_get_page (enum palloc_flags flags)
{
  void* kpage = palloc_get_page (flags);
  if (kpage == NULL)
    {
      //TODO: swapping
      return NULL;
    }

  struct frame *frame = malloc(sizeof(struct frame));
  if (frame == NULL)
    {
      palloc_free_page (kpage);
      return NULL;
    }

  frame->kpage = kpage;
  hash_insert (&frame_hash, &frame->hash_elem);
  list_push_back (&frame_list, &frame->list_elem);

  return kpage;
}

struct frame *
frame_lookup (const void *page)
{
  struct frame f;
  struct hash_elem *e;

  f.kpage = page;
  e = hash_find (&frame_hash, &f.hash_elem);
  return e != NULL ? hash_entry (e, struct frame, hash_elem) : NULL;
}

/* Frees the page at PAGE. */
void
frame_free_page (void *page)
{
  struct frame *frame = frame_lookup (page);
  if (frame == NULL)
    {
      PANIC ("frame correspond to the page pointer doesn't exist.");
    }

  hash_delete (&frame_hash, &frame->hash_elem);
  list_remove (&frame->list_elem);
  free (frame);

  palloc_free_page (page);
}
