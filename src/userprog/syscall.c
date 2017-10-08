#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"

static void syscall_handler (struct intr_frame *);
/* Projects 2 and later. */
void halt (void);
void exit (int status);
tid_t exec (const char *file);
int wait (tid_t tid);
int open (const char *file);
void close (int fd);
int read (int fd, void *buffer, unsigned length);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int read_argument (const unsigned int *esp);

/* */
void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Check if the ptr(address) is right address to prevent the page falut. */
void
check_ptr_validation (void *ptr)
{
  // // printf (">> check_ptr_validation: start\n");
  if (((unsigned int) ptr) <= 0x08048000 || is_kernel_vaddr(ptr))
    exit (-1);
  // printf (">> check_ptr_validation: end\n");

  if (pagedir_get_page (thread_current()->pagedir, ptr) == NULL)
    {
      // printf (">> check_user_ptr_validation: exit\n");
      exit (-1);
    }
}

/* */
static void
syscall_handler (struct intr_frame *f)
{
  // printf (">> syscall_handler: start\n");
	/* get stack pointer from interrupt */
	unsigned int *esp = (unsigned int*) (f->esp);
	/* get system call number from stack */
	/* Current, esp indicates to system call numer */

  // printf (">> syscall_handler: 0x%x\n", f);
  // printf (">> syscall_handler: 0x%x\n", f->esp);
	check_ptr_validation (esp);

  // printf (">> syscall_handler: syscall_number get\n");
	unsigned int syscall_number = *esp;
	/* check if the address in the esp refer to right location */
  // // printf (">> syscall_handler: esp -> %d\n", esp);
  // printf (">> syscall_handler: *esp -> %d\n", *esp);

  // printf (">> syscall_handler: start switch\n");
  // printf (">> syscall_handler: syscall_number -> %d\n", syscall_number);
	/* connect each system call by its number
		number is defined in syscall-nr.h */
  int fd;
  void *buffer;
  unsigned length;
	switch (syscall_number)
    {
  		case SYS_HALT:                   /* Halt the operating system. */
        {
          halt ();
    			break;
        }
      case SYS_EXIT:					         /* Terminate this process. */
        {
          exit ((int) read_argument (esp + 1));
      	  break;
        }
      case SYS_EXEC:                   /* Start another process. */
        {
          f->eax = exec ((const char*) read_argument (esp + 1));
        	break;
        }
      case SYS_WAIT:                   /* Wait for a child process to die. */
        {
          f->eax = wait ((tid_t) read_argument (esp + 1));
      	  break;
        }
      case SYS_CREATE:                 /* Create a file. */
        {
          f->eax = create ((const char*) read_argument (esp + 1),
                           (unsigned) read_argument (esp + 2));
        	break;
        }
      case SYS_REMOVE:                 /* Delete a file. */
        {
          f->eax = remove ((const char*) read_argument (esp + 1));
        	break;
        }
      case SYS_OPEN:                   /* Open a file. */
        {
          f->eax = open ((const char*) read_argument (esp + 1));
          break;
        }
      case SYS_FILESIZE:               /* Obtain a file's size. */
      	{
          break;
        }
      case SYS_READ:                   /* Read from a file. */
      	{
          fd = (int) read_argument (esp + 1);
          buffer = (void*) read_argument (esp + 2);
          length = (unsigned) read_argument (esp + 3);
          f->eax = read (fd, buffer, length);
          break;
        }
      case SYS_WRITE:                  /* Write to a file. */
        {
          // // printf (">> syscall_handler: case SYS_WRITE\n");
          fd = (int) read_argument (esp + 1);
          // // printf (">> syscall_handler: case SYS_WRITE fd -> %d\n", fd);
          buffer = (void*) read_argument (esp + 2);
          // // printf (">> syscall_handler: case SYS_WRITE buffer\n");
          length = (unsigned) read_argument (esp + 3);
          // // printf (">> syscall_handler: case SYS_WRITE length -> %d\n", length);
          f->eax = write (fd, buffer, length);
          break;
        }
      case SYS_SEEK:                   /* Change position in a file. */
      	{
          break;
        }
      case SYS_TELL:                   /* Report current position in a file. */
      	{
          break;
        }
      case SYS_CLOSE:                  /* Close a file. */
      	{
          close ((const char*) read_argument (esp + 1));
          break;
        }
  	}

  // // printf (">> system call! : syscall_number(SYS_EXIT) -> %d\n", SYS_EXIT);
  // // printf (">> system call! : syscall_number(SYS_WRITE) -> %d\n", SYS_WRITE);
  // thread_exit ();
}

/* */
int
read_argument (const unsigned int *esp)
{
	/* To execute a system call, we need argument in the stack,
	Read that arguments in the esp */
	check_ptr_validation (esp);
  return (int) *esp;
}

/* */
void
halt (void)
{
  // printf (">> halt: start\n");
  power_off ();
  // printf (">> halt: end\n");
}

/* */
void
exit (int status)
{
	struct thread *t = thread_current ();
	printf ("%s: exit(%d)\n", t->name, status);
  t->exit_status = status;
	thread_exit ();
}

/* */
int
open (const char *file)
{
  // printf (">> open: start\n");
  if (file == NULL)
    return -1;
  check_ptr_validation (file);

  struct file *f = filesys_open (file);
  if (f == NULL)
    return -1;

  int fd = process_add_file (f);
  if (fd != -1)
    return fd;

  return -1;
}

/* */
void
close (int fd)
{
  if (fd >= 0)
    process_remove_file (fd);
  else
    exit (-1);
}

/* */
bool
create (const char *file, unsigned initial_size)
{
  // printf (">> create: start\n");
  // // printf (">> create: file -> 0x%x\n", file);
  check_ptr_validation (file);
  return filesys_create (file, initial_size);
}

/* */
bool
remove (const char *file)
{
  check_ptr_validation (file);
  return filesys_remove (file);
}

/* */
tid_t
exec (const char *file)
{
  // printf (">> exec: start\n");
  check_ptr_validation (file);
  tid_t tid = process_execute (file);
  struct thread* chld = get_child_thread (tid);
  if (chld == NULL)
    return -1;

  while (chld->load_status == 0)
    {
      // if (chld->status != 1 && chld->status != 2)
      //   // printf (">> process_wait: chld->status -> %d\n", chld->status);
      thread_yield ();
    }

  if (chld->load_status != 1)
    return -1;
  return tid;
}

/* */
int
wait (tid_t tid)
{
  return process_wait(tid);
}

/* */
int
read (int fd, void *buffer, unsigned length)
{
  ASSERT (fd > 1);

  struct thread_file *tf = get_thread_file (fd);
  if (tf == NULL)
    return -1;

  struct file *f = tf->f;
  if (f == NULL)
    return -1;

  int size = file_read (f, buffer, length);
  return size;
}

/* */
int
write (int fd, const void *buffer, unsigned length)
{
  ASSERT (fd == 1);
  check_ptr_validation (buffer);

  // printf (">> write: fd -> %d\n", fd);
  putbuf (buffer, length);
  return length;
}
