#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);
/* Projects 2 and later. */
void halt (void);
void exit (int status);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int read_argument(const unsigned int *esp);


void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

bool
is_valid_ptr(void* ptr)
{
  return (((unsigned int*) ptr) > 0x8048000 &&
          ((unsigned int*) ptr) < 0xc0000000);
}

static void
syscall_handler (struct intr_frame *f)
{
	/* get stack pointer from interrupt */
	unsigned int *esp = (unsigned int*) (f->esp);
	/* get system call number from stack */
	/* Current, esp indicates to system call numer */

	unsigned int syscall_number = *esp;
	/* check if the address in the esp refer to right location */
  // printf (">> syscall_handler: esp -> %d\n", esp);
  printf (">> syscall_handler: *esp -> %d\n", *esp);

	/* Check if the address in esp is right address to prevent the page falut*/
	if (!is_valid_ptr(esp))
		exit(-1);

  printf (">> syscall_handler: start switch\n");
	/* connect each system call by its number
		number is defined in syscall-nr.h */
	switch (syscall_number)
    {
  		case SYS_HALT:                   /* Halt the operating system. */
  			halt ();
  			break;
      case SYS_EXIT:					         /* Terminate this process. */
        exit ((int) read_argument (esp + 1));
      	break;
      case SYS_EXEC:                   /* Start another process. */

      	break;
      case SYS_WAIT:                   /* Wait for a child process to die. */
      	break;
      case SYS_CREATE:                 /* Create a file. */
      	f->eax = create ((const char*) read_argument (esp + 1),
                         (unsigned) read_argument (esp + 2));
      	break;
      case SYS_REMOVE:                 /* Delete a file. */
      	f->eax = remove ((const char*) read_argument (esp + 1));
      	break;
      case SYS_OPEN:                   /* Open a file. */
      	break;
      case SYS_FILESIZE:               /* Obtain a file's size. */
      	break;
      case SYS_READ:                   /* Read from a file. */
      	break;
      case SYS_WRITE:                  /* Write to a file. */
        printf(">> syscall_handler: case SYS_WRITE\n");
        int fd = *(int*) read_argument (esp + 1);
        printf(">> syscall_handler: case SYS_WRITE fd -> %d\n", fd);
        void *buffer = *(void**) read_argument (esp + 2);
        printf(">> syscall_handler: case SYS_WRITE buffer\n");
        unsigned length = *(unsigned*) read_argument (esp + 3);
        printf(">> syscall_handler: case SYS_WRITE length -> %d\n", length);
        f->eax = write (fd, buffer, length);
        break;
      case SYS_SEEK:                   /* Change position in a file. */
      	break;
      case SYS_TELL:                   /* Report current position in a file. */
      	break;
      case SYS_CLOSE:                  /* Close a file. */
      	break;
  	}

  printf (">> system call! : syscall_number(SYS_EXIT) -> %d\n", SYS_EXIT);
  printf (">> system call! : syscall_number(SYS_WRITE) -> %d\n", SYS_WRITE);
  printf (">> system call! : syscall_number -> %d\n", syscall_number);
  thread_exit ();
}

/* */
int
read_argument (const unsigned int *esp)
{
	/* To execute a system call, we need argument in the stack,
	Read that arguments in the esp */
	if (is_valid_ptr(esp))
    {
      printf(">> read_argument: if esp -> %d\n", esp);
      return (int) *esp;
    }
	else
    {
      printf(">> read_argument: else\n");
      exit(-1);
    }
}

/* */
void
halt (void)
{
	power_off();
}

/* */
void
exit (int status)
{
	struct thread *t = thread_current ();
	printf(">> %s: exit(%d)\n", t->name, status);
	// Should we change any struct's component here?
	thread_exit();
}

/* */
bool
create (const char *file, unsigned initial_size)
{
	return filesys_create (file, initial_size);
}

/* */
bool
remove (const char *file)
{
	return filesys_remove (file);
}

/* */
// pid_t
// exec (const char *file)
// {
//   return NULL;
// }


/* */
int
write (int fd, const void *buffer, unsigned length)
{
  printf(">> write: fd -> %d\n", fd);
  ASSERT (fd == 1);
  putbuf(buffer, length);
  return length;
}
