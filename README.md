# PintOS
#### This is a variant of the Pintos Operating System for use as part of Operating Systems module at the University of the West of England (UWE).
---
#### Implementations done in this project to PintOS operating system in Linux Environment included:
- Tokenistaion of user argument input from terminal
- Stack management and argument alignment
- Implementation and integration of system calls
- Exploitation of system calls implemented
---
## Tokenization and Stack Management
* This was essential to allow user programs to access system calls, this was done by parsing and splitting arguments into tokens
* Extended the `src/userprog/process.c` file:
  * used the function `start_process()` to parse and tokenize arguments, then passing tokenized arguments to `passing_arg()`
      ```c
      //to get count of argc for the size of the *token[] array
      for (fn_copy = strtok_r(file_name_, " ", &save_ptr); fn_copy != NULL; fn_copy = strtok_r(NULL, " ", &save_ptr)){
        argc++;
      }  
    //parsing the file_name and delimitting the white spaces using strtok_r 
    //this is done also to give access to the file name for other functions
    for (fn_copy = strtok_r(file_name, " ", &save_ptr); fn_copy != NULL; fn_copy = strtok_r(NULL, " ", &save_ptr)){
      token[i] = fn_copy;
      i++;
    }
      ```
  * `passing_arg()` passes said arugments to stack, where stack alignment also occurs
    ```c
    //for loop to pass arguments to the stack pointer
    //saving pointer addresses in addr array
    for (i = argc - 1; i >= 0; i--) {
      len = strlen(argv[i]) + 1; 
      *esp -= len; 
      memcpy(*esp, argv[i], len);
      addr[i] = *esp;
    }
  
    //aligning to 4 bytes using 0's
    word_align = (uintptr_t)*esp % 4;
    *esp -= word_align;
    memset(*esp, 0, word_align);
    *esp -= 4;
    memset(*esp, 0, 4);
  
    //writing addresses that point to each argument
    for (j = argc - 1; j >= 0; j--){
      *esp -= sizeof(char*);
      memcpy(*esp, &addr[j], sizeof(char*));
    }
    ```

## System Calls (focused on `remove` system call)
* Three system calls were implemented in total: `create`, `open` and `remove`
* I only implemented the `remove` system call, found in the `src/filesys/filesys.c` and `src/filesys/directory/c`
   * In `filesys.c`:
     ```c
     bool
     filesys_remove (const char *name) { 
       struct dir *dir = dir_open_root ();
       bool success = dir != NULL && dir_remove (dir, name);
       dir_close (dir); 
       printf("\nfilesys.c: return value: %d\n", success);
       if(success == 0){
         printf("filesys.c: Removal failed");
       }
       return success;
     }
     ```
* The main objective of the `remove` system call is to remove a file from a directory in the system

#### Exploitation of `remove` system call
* Was completed with memory exploitation using shellcode which is implemented in `src/examples/rm_shellcode.c`

