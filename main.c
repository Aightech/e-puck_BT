#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void printBIN(char *str,int n)
{
  printf("(BIN)\t |");
  int i=0;
  while(i<n)
    printf("%d|",(int)str[i++]);
  printf("\n");
}

void printSHORT(char *str,int n)
{
  printf("(SHORT)\t |");
  int i=0;
  char msb, lsb;
  int combined;
  while(i<n)
    {
      lsb=str[i];
      msb=str[i+1];
      combined = (msb << 8 ) | (lsb & 0xff);
      printf("%d|",combined);
      i+=2;
    }
  printf("\n");
}

int init_connection(const char *path)
{
	int i,fd = open(path, O_RDWR | O_NOCTTY);
	if(fd < 0)
	{
		printf("couldn't connect ...\n");
		for(i=0;i<3;i++,sleep(1))
		{
			if((fd = open(path, O_RDWR | O_NOCTTY))>-1)
				return fd;
			printf("couldn't connect ...\n");
		}
	}
	return fd;
}

int main (int argc, char *args []) {

  char c;
  int epuck = init_connection("/dev/rfcomm0");
  char str[9]="N\n\0";
  int i=0;

  //if an argument is passed
  if(argc>1)
    {
      if(args[1][0]=='-')//BINARY mode
	{
	  while(args[1][1+i]!='\0')
	    str[i-1]=-args[1][1+i++];//get the opposite
	  str[i++]='\0';//end by null character
	}
      else//ASCII mode 
	{
	  str[0]=args[1][0];//just the first argument is passed
	  str[1]='\n';//end by new line
	  i=2;
	}
    }
  else//if no argument is passed then require Motors and IR
    {
      str[0]=-'E';
      str[1]=-'N';
      str[2]=0x00;
    }
  
  char rep[1000];
  while (1)
    {
      //SENDING
      printf("#########################\n");
      printf("## >>\t\tSending :\n");
      printf("(ASCII)\t %s\n",str);
      printBIN(str,i);
      
      int n = write(epuck,str,i);
      printf("(INFO)\t %d bytes sent\n",n);
      
      sleep(1);

      //READING
      n = read(epuck, rep, 1000);
      printf("## <<\t\tReceiving :\n");
      printf("(ASCII)\t %s\n",rep);
      printBIN(rep,n);
      printSHORT(rep,n);
      printf("(INFO)\t %d bytes received\n", n);
  
      sleep(1);
    }

  close(epuck);

  return 0;
}


  
