Programs illustrate how to write simple chardev modules, and implementing some file operations. For each module, the major number is allocated statically. Thus, one has to create a file in `/dev/`with major number specified in the program.

- Module program `SimpleChardevStatic.c`
    - One has to create a character special file in /dev with the command mknod and the major number 190
    - This module writes in the kernel log a message indicating the type of file operation performed on the file of the module : open, close, read and wite
- Module program `SimpleReadWrite.c`
    - This module allows to write/read a message into/from device file
