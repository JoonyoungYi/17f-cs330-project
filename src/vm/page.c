#include <list.h>
#include "vm/frame.h"
#include "vm/page.h"
#include "lib/kernel/hash.h"

/* */
static unsigned spte_hash_func (const struct hash_elem *e_, void *aux UNUSED)
{
  const struct spte *e = hash_entry (e_, struct spte, hash_elem);
  return hash_bytes (&e->upage, sizeof e->upage);
}

/* */
static bool spte_less_func (const struct hash_elem *a_,
                            const struct hash_elem *b_,
                            void *aux UNUSED)
{
  const struct spte *a = hash_entry (a_, struct spte, hash_elem);
  const struct spte *b = hash_entry (b_, struct spte, hash_elem);
  return a->upage < b->upage;
}

/* */
struct spt* page_create_spt (struct thread* t)
{
  struct spt *spt = malloc (sizeof (struct spt));
  if (spt == NULL)
    {
      return NULL;
    }

  hash_init (&spt->hash, spte_hash_func, spte_less_func, NULL);
  list_init (&spt->list);
  spt->thread = t;
  return spt;
}

/* */
void page_remove_spt (struct *spt)
{
  //TODO: list clear
  //TODO: hash clear
  free (spt);
}
