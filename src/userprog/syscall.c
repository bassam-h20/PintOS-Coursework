#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include <string.h>

//arrow operator reference
//Mulani, S. (2022) Arrow operator in C - All you need to know! Available from: https://www.digitalocean.com/community/tutorials/arrow-operator-c-plus-plus [Accessed 24 December 2022]. 


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

  //if statemenets check the bool result of the functions in filesys.c, if returned false
  //  and return an error message
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
  printf("\nSys Number: %d\n\n", sys_code);

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
      unsigned initial_size = (*(unsigned*) (f->esp + 8));

      //validating if user virtual address lies within PHYS_BASE range of user virtual address space
      //validating user-provided pointer address and if file name is NULL
      //referenced from threads/vaddr.h
      if(!is_user_vaddr((const void*)f -> esp + 1) || file == NULL)
      {
        f -> eax = false;
        printf("\nError occured, pointer address invalid or file name is null\n");
        break;
      }
      

      f->eax = filesys_create(file, initial_size) ;


      //if statemenets check the bool result of the functions in filesys.c, if returned false
      //  and return an error message
      if (filesys_create(file, initial_size) == true)
      {
        printf("\nfile: '%s' created successfully\n", file);
        break;
      }
      if(filesys_create(file, initial_size) == false)
      {
        printf("\nError occured.\nfile either already exists, or internal memory allocation failed\n");
        f -> eax = false;
        break;
      }
      
        
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

      printf("\naddress valid.\n");
      f->eax = filesys_open(file);

      //if statemenets check the bool result of the functions in filesys.c, if returned false
      //  and return an error message
      if(filesys_open(file) == true)
        {
          printf("\nfile: %s opened successfully.\n", file);
          break;
        }
      if(filesys_open(file) == false)
      {
        printf("\nfile has not been opened, error occured\n");
        f -> eax = false;
        break;
      }
       
    }

    //included SYS_EXIT in order to exit after system calls are done executing
    case SYS_EXIT:
    { 
      printf("\nSYS EXIT\n");
      thread_exit();
    }
  

  }

  

  printf("\n\nsystem call\n\n");
  
}