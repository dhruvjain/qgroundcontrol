#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdlib.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <fcntl.h>

#include <termios.h>

#include <sys/ioctl.h>

#define PORT 8000

char target_ip[100]="192.168.0.113";
int main(){

   
   int sock,length,n;
   struct sockaddr_in server,from;

   struct hostent *hp;

   char* buffer;
   buffer=(char*)malloc(256*sizeof(char));

   char printbuff[INET_ADDRSTRLEN];
  

   sock=socket(AF_INET,SOCK_DGRAM,0);
 
   if(sock<0){
        perror("socket");
   }


   server.sin_family = AF_INET;

   server.sin_addr.s_addr = inet_addr(target_ip);

   server.sin_port = htons(PORT);
   
   length=sizeof(struct sockaddr_in);

   printf("Please enter the message \n");

   bzero(buffer,256);
   fgets(buffer,255,stdin);

   n=sendto(sock,buffer,strlen(buffer),0,&server,length);
   
   inet_ntop(server.sin_family, &server.sin_addr.s_addr, printbuff, sizeof(printbuff));
   
   if(n<0){
    error("Sendto");
   }
  
   //n=recvfrom(sock,buffer,256,0,&from,&length);
  
   
 
   char command[5000]="sudo mavproxy.py --master=/dev/ttyUSB2 --baudrate=57600 --out ";

   
   strcat(command,printbuff);

   

   strcat(command,":");
   strcat(command,"1234");
   
   printf("%s\n",command);
   
   system(command);
   


}
