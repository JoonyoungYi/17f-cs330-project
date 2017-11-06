#include "threads/palloc.h"
#include "vm/frame.h"
#include "vm/page.h"


/* Initializes the vm allocator. */
void
frame_init (void)
{
  palloc_init ();
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
  return palloc_get_page (flags);
}

/* Frees the page at PAGE. */
void
frame_free_page (void *page)
{
  return palloc_free_page (page);
}
