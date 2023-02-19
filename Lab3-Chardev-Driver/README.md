- Module program `SimpleChardevStatic.c`
    - One has to create a character special file in /dev with the command mknod and the major number 190
    - This module writes in the kernel log a message indicating the type of file operation performed on the file of the module : open, close, read and wite

