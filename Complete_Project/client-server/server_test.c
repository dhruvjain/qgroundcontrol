#include <stdio.h>

#include <errno.h>

#include <string.h>

#include <stdlib.h>

#include <pthread.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <fcntl.h>

#include <termios.h>

#include <sys/ioctl.h>

void error(char* msg){

perror(msg);
exit(0);
}

int main(int argc,char* argv[]){

  int sock,length,fromlen,n;
  struct sockaddr_in server;
  struct sockaddr_in from,from2;
  char buf[1024];

  if(argc<2){
  fprintf(stderr,"ERRoR:No port provided");
  exit(0);
  }

  sock=socket(AF_INET,SOCK_DGRAM,0);

  if(sock<0){
   error("Opening port");
  }

  length=sizeof(server);

  bzero(&server,length);
  server.sin_family = AF_INET;

  server.sin_addr.s_addr = INADDR_ANY;

  server.sin_port = htons(atoi(argv[1]));

  if (-1 == bind(sock,(struct sockaddr *)&server,length))

{

      perror("error bind failed");

      close(sock);

      exit(1);
} 

fromlen= sizeof(struct sockaddr_in);
char buffer[255];


        

        n=recvfrom(sock,buf,1024,0,(struct sockadd*)&from,&fromlen);
        if(n<0){
           error("recvfrom");
          
        }
        write(1,"Received a datagram:",21);
        write(1,buf,n);
        if(buf[0]=='p' && buf[1]=='a' && buf[2]=='s' && buf[3]=='s' && buf[4]=='w' && buf[5]=='o' && buf[6]=='r' && buf[7]=='d')
        printf("correct \n");
        else
            return;
        
       
        printf("Enter you no. of uavs available \n");
        fgets(buffer,255,stdin);
        from2=from;
        n=sendto(sock,buffer,strlen(buffer),0,(struct sockaddr *)&from2,fromlen);
        if(n<0){
               error("send to");
          }
         n=recvfrom(sock,buf,1024,0,(struct sockadd*)&from,&fromlen);
        if(n<0){
           error("recvfrom");
          
        }
        write(1,"Received a uav id :",21);
        write(1,buf,n);
        

     
}
