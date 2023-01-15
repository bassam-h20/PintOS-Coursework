#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include <string.h>

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


bool remove (const char *file)
{
  
  printf("\nstarting filesys_remove(file).\n");
  bool result = filesys_remove(file);
  printf("\nfile name  = '%s'\n", file);

  if(result == true)
  {
    printf("\nFile = '%s' has been remove successfully\n", file);
  }

  else if(result == false)
  {
    printf("\nFile removal has failed.\n");
  }

  return result;

}


bool create (const char *file, unsigned initial_size)
{
   //using synchronization constructs:


 printf ("before filesys_create()\n");
  bool b = filesys_create(*file, initial_size);
  printf ("after filesys_create()\n");
 // }
  printf("%s\n %d", file, &initial_size);

  return b;

}



static void
syscall_handler (struct intr_frame *f )
{

  int sys_code = *(int*)f -> esp;
  printf("\nSys Number: %d\n", sys_code);

  switch(sys_code)
  {
    
    case SYS_REMOVE:
    {

      printf("\nSYS_REMOVE initiated.\n");
      char *file = (*(char**)(f -> esp + 4));
      //validating if user virtual address lies within PHYS_BASE range of user virtual addresses
      //validating user-provided pointer address and if file name is NULL
      printf("\naddress validation and NULL check process initiated\n");
      if(!is_user_vaddr((const void*)f -> esp + 1) || file == NULL)
      {
        f -> eax = false;
        printf("\nError occured, pointer address invalid or file name is null\n");
        break;
      }

      
      printf("\nvalidated successfully.\n");
      printf("\nfile name = '%s'\n", file);
      f->eax = remove(file);

      break;
      printf("\ncase SYS_REMOVE completed.\n");
    }


    case SYS_CREATE:
    {
      char *file =  (*(char**)(f->esp+ 4));
      if(!is_user_vaddr((const void*)f -> esp + 1) || file == NULL)
      {
        f -> eax = false;
        printf("\nError occured, pointer address invalid or file name is null\n");
        break;
      }
      unsigned initial_size = (*(unsigned*) (f->esp + 8));

      f->eax = filesys_create(file, initial_size) ;
      break;  
    }


    case SYS_OPEN:
    {
      //struct thread *t = thread_current();

      char *file = (*(char**)(f->esp+ 4));
      printf("\nvalidating user virtual address\n");
      if(!is_user_vaddr((const void*)f -> esp + 1) || file == NULL || !pagedir_get_page(thread_current()->pagedir, file))
      {
        f -> eax = false;
        printf("\nError occured, pointer address invalid or file name is null\n");
        break;
      }

      /*void *page = pagedir_get_page(t->pagedir, file);
      if (page == NULL)
      {
        f -> eax = false;
        printf("\nError occured, virtual address not valid\n");
        break;
      }*/
      
      f->eax = filesys_open(file);

      if (file == NULL)
        {
          printf("\nFile does not exist in the root directory\n");
          f -> eax = false;
        }
      else
        { 
          printf("\nFile exists in the root directory, proceeding with the open system call\n");
          f -> eax = false;
        }

        int a = f->eax;  

        printf("\nfile: %s opened successfully.\n", file);
      
      break;
    }

    case SYS_EXIT:
    { 
      printf("\nSYS EXIT\n");
      thread_exit();
    }
  }

  

  printf("\nsystem call\n");
  
}