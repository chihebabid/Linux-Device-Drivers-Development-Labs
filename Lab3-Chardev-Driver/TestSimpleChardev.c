#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
int main(void)
{
 /* First, you need run "mknod /dev/mydev c 202 0" to create /dev/mydev */
 int my_dev = open("/dev/mydev", 0);
 if (my_dev < 0) {		
 perror("Fail to open device file: /dev/mydev");
} else {		
 printf("File opened!\n");
 getchar();		
 close(my_dev);
 printf("File closed!\n");
}
return 0;
}
