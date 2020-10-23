#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

#define FIFO_FILE "/tmp/fifo"

int main(void)
{
	int fd;
	int number;

	if(-1 == (fd = open(FIFO_FILE, O_WRONLY)))
	{
		perror("open() Error");
		exit(1);
	}
	
	number = 0;
	
	write(fd, number ,sizeof(int));
	close(fd);
}
