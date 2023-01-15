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

//function is called inside the SYS_REMOVE
bool remove (const char *file)
{
  //calling the filesys_remove function from filesys/filesys.c
  printf("\nstarting filesys_remove(file).\n");
  bool result = filesys_remove(file);
  printf("\nfile name  = '%s'\n", file);

  //printing a message depending on the bool result returned from filesys_remove()
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
      //pointer is made to point at buffer + 4 due to word-alignment of bytes
      char *file = (*(char**)(f -> esp + 4));


      //validating if user virtual address lies within PHYS_BASE range of user virtual address space
      //validating user-provided pointer address and if file name is NULL
      //referenced from threads/vaddr.h
      printf("\naddress validation and NULL check process initiated\n");
      if(!is_user_vaddr((const void*)f -> esp + 1) || file == NULL)
      { 
        //return value (false in this case) stored in eax register
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
      //pointer is made to point at buffer + 4 due to word-alignment of bytes
      char *file =  (*(char**)(f->esp+ 4));
      

      //validating if user virtual address lies within PHYS_BASE range of user virtual address space
      //validating user-provided pointer address and if file name is NULL
      //referenced from threads/vaddr.h
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
      struct thread *t = thread_current();
      
      //pointer is made to point at buffer + 4 due to word-alignment of bytes
      char *file = (*(char**)(f->esp+ 4));
      printf("\nvalidating user virtual address\n");


      //validating if user virtual address lies within PHYS_BASE range of user virtual address space
      //validating user-provided pointer address and if file name is NULL
      //referenced from threads/vaddr.h
      if(!is_user_vaddr((const void*)f -> esp + 1) || file == NULL || !pagedir_get_page(thread_current()->pagedir, file))
      {
        f -> eax = false;
        printf("\nError occured, pointer address invalid or file name is null\n");
        break;
      }
      
      f->eax = filesys_open(file);

      //checking if file contents are NULL or not
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

    //included SYS_EXIT in order to exit after system calls are done executing
    case SYS_EXIT:
    { 
      printf("\nSYS EXIT\n");
      thread_exit();
    }
  

  }

  

  printf("\nsystem call\n");
  
}