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
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <cstdlib>
#include <pthread.h>
#include<thread>


using namespace std;
 

FILE *fp;
void error(char* msg){

perror(msg);
exit(0);
}
void  process()
{    
        system("nohup cvlc -Ihttp --file-logging --log-verbose=1 --logfile=\"vlc.log\" --vlm-conf vlm.conf & echo $! > vlc.pid");
        
        //char c[300]=" nohup cvlc -Ihttp --file-logging --log-verbose=1 --logfile=\"vlc.log\" --vlm-conf vlm.conf & echo $! > vlc.pid";
        //fp = popen(c,"r");
      
        
}



int main(){

  int sock,length,fromlen,n;
  
  struct sockaddr_in server;
  struct sockaddr_in from;
  char buf[1024];
  pthread_t thread[10];

  sock=socket(AF_INET,SOCK_DGRAM,0);

  if(sock<0){
   perror("Opening port");
  }

  length=sizeof(server);

  bzero(&server,length);
  server.sin_family = AF_INET;

  server.sin_addr.s_addr = INADDR_ANY;

  server.sin_port = htons(8000);

  if (-1 == bind(sock,(struct sockaddr *)&server,length))

{

      perror("error bind failed");

      close(sock);

      exit(1);
} 
char printbuff[INET_ADDRSTRLEN];
inet_ntop(server.sin_family, &server.sin_addr.s_addr, printbuff, sizeof(printbuff));
fromlen= sizeof(struct sockaddr_in);

char printbuf[INET_ADDRSTRLEN];
int vlcport=8095;
ofstream myfile;
string nuavs,filename;
myfile.open("/var/www/html/noofuavs.txt",ios::out);
int i=1;
std::fstream file;
file.open ("vlm.conf", std::fstream::in | std::fstream::out );

   while(1){
          
          n=recvfrom(sock,buf,1024,0,(struct sockaddr*)&from,(socklen_t*)&fromlen);
          printf("revf");
          if(n<0){
           perror("recvfrom");
          }


          //write(1,"Received a datagram:",21);
          //write(1,buf,n);

        
          inet_ntop(from.sin_family, &from.sin_addr.s_addr, printbuf, sizeof(printbuf));

   
         
          ostringstream convert; 
          convert <<i;     

          nuavs = convert.str(); 
          myfile<<nuavs;
          myfile.seekp(0, ios::beg);
  
          char ch2[10];
          sprintf(ch2,"%d",i);
          string ch(ch2);
          string s1="new channel"+ch+ " broadcast enabled";
          string ip(printbuf);
          string s2="setup channel"+ch+" input \"http://"+ip+":8080\"";
          string s3="setup channel"+ch+" output #http{mux=ts,dst=:8095/live"+ch+"}";
 
          string s4="setup channel"+ch+" option http-reconnect";
          string s5="setup channel"+ch+" option no-sout-rtp-sap option no-sout-standard-sap option ttl=1 option sout-keep";
          string s6="control channel"+ch+" play";
          file<<s1<<endl<<s2<<endl<<s3<<endl<<s4<<endl<<s5<<endl<<endl<<s6<<endl<<endl;
         usleep(4000000);
          std::thread first (process);   
          first.join();       

         
          //fp = popen(c,"r");
          
         // pthread_create(&thread[i], 0, process, (void *) i);
	  //pthread_detach(thread[i]);
 
          i++; 
         /* ostringstream convert;  
          int Number=i+1;
          convert << Number;     

          fileno = convert.str(); 
          filename="a";
          filename.append(fileno);
          filename.append(".asx");
          filen = strdup(filename.c_str());
          myfile.open (filen);
       

          string str="<ASX version=\"3.0\" \n <ENTRY> \n <REF HREF=";
          string addr="http://";
          addr.append(printbuff);
          addr.append(":");
          addr.append(port);
          str.append(addr);
          str.append(" /> \n</ENTRY> \n</ASX>");
          myfile <<str;
          myfile.close();
          
          ostringstream convert; 
          convert <<i+1;     

          nuavs = convert.str(); 
          myfile<<nuavs;
          myfile.seekp(0, ios::beg);*/
         
         
    }

}


