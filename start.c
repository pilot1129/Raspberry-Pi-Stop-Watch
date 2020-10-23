#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringPi.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>

#define FIFO_FILE 	"/tmp/fifo"
#define BUFF_SIZE 1024

int controlNumber = 0; //FND control
const int FndselectPin[6] = {4, 17, ,18, 27, 22, 23};
const int FndPin[8] = {6, 12, 13, 16, 19, 20, 26, 21};
const int FndFont[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

void Init()
{
	int i ;
	if(wiringPiSetupGpio() == -1)
	{
		printf("wiringPiSetupGpio() error\n");
		exit(-1);
	}
	
	for(i = 0; i<6; i++)
	{
		pinMode(FndSelectPin[i], OUTPUT);
		digitalWrite(FndSelectPin[i], HIGH); // Select핀 OFF
	}
	
	for(i=0; i<8, i++)
	{
		pinMode(FndPin[i], OUTPUT);
		digitalWrite(FndPin[i], LOW);
	}
}

void FndSelect(int position)
{
	int i;
	for(i = 0; i<6; i++)
	{
		if(i == position)
			digitalWrite(FndSelectPin[i], LOW);
		else
			digitalWrite(FndSelectPin[i], HIGH);
	}
}
void FndDisplay(int position, int num)
{
	int i, j;
	int flag = 0;
	int shift = 0x01;
	for(i = 0; i<8; i++)
	{
		flag = (FndFont[num] & shift);
		digitalWrite(FndPin[i], flag);
		shift <<=1;
	}
	if(position==2)
		digitalWrite(FndPin[7], 1);
	FndSelect(position);
}

void *t_function1(void *data) // FND control
{
	int pos;
	int data[6] ={0};
	int lasttime = 0;
	int time = 0;
	int stoptime = 0;
	while(1)
	{	
		if(controlNumber == 0) // clear
		{
			for(int i = 0; i<6; i++)
			{
				data[i] = 0;
			}
		}
		else if(controlNumber ==1) // start
		{
			lasttime = millis();
			controlNumber = 3;
		}
		else if(controlNumber == 2) // stop
		{
			//stoptime = time
		}
		else if(controlNumber == 3) // else(ing)
		{
			time = millis() - lasttime;
		}

		data[0] = time%10;
		data[1] = time%100/10;
		data[2] = time/100%10;
		data[3] = time/1000%10;
		data[4] = time/10000%10;
		data[5] = time/100000%10;

		for(pos = 0; pos<6; pos++)
		{
			FndDisplay(pos, data[pos]);
			delay(1);
		}
	}
}
void *t_function2(void *data) // IPC
{
	int fd;
	char buff[BUFF_size];
	if(-1 == mkfifo(FIFO_FILE, 0666))
	{
		perror("mkfifo() Error");
		exit(1);
	}
	if(-1 == (fd = open(FIFO_FILE, O_RDWR)))
	{
		perror("open() Error");
		exit(1);
	}

	while(1)
	{
		memset(buff, 0, BUFF_SIZE);
		read(fd, buff, BUFF_SIZE);	
		controlNumber = atoi(buff);
	}
	close(fd);
}

int main(void)
{
	int pid;
	pid = fork(); // fork

	if(pid>0) // parent
	{
		Init();
	}
	else if(pid == 0) // child
	{
		pthread_t thread[2];
		pthread_create(&thread[0], NULL, &t_function1, NULL);
		pthread_create(&thread[1], NULL, &t_function2, NULL);
		
	}
	else if(pid == -1)
	{
		perror("fork error :");
		exit(0);
	}

	return 0;
}	
