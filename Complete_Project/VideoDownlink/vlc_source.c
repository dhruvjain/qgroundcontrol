#include<stdlib.h>
#include<stdio.h>
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

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


int main(){

   int sock,length,n;
   struct sockaddr_in server,from;
   struct hostent *hp;
   char buffer[256];
   
   FILE *fp;
   sock=socket(AF_INET,SOCK_DGRAM,0);
 
   if(sock<0){
             error("socket");
   }
   server.sin_family=AF_INET;

  hp=gethostbyname("192.168.1.142");

  if(hp==0){

   error("unknown host");
  }

  bcopy((char *)hp->h_addr,(char *)&server.sin_addr,hp->h_length);

  server.sin_port=htons(8000);
  length=sizeof(struct sockaddr_in);


  printf("Please enter the message \n");

  bzero(buffer,256);
  fgets(buffer,255,stdin);

  n=sendto(sock,buffer,strlen(buffer),0,&server,length);

  if(n<0){
  error("Sendto");
  }

  

  fp = popen(" cvlc v4l2:// :v4l2-vdev=\"/dev/video0\" --sout '#transcode{vcodec=mp4v,acodec=mpga,vb=800,ab=128}:standard  {access=http,mux=asf,dst=:8080}' --no-sout-audio", "r");

 pclose(fp);

return 0;

}
