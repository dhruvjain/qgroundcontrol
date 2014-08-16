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

#define MAVLINK_REMOTE_UDP_PORT 14550 

int main(){

int n,udp_socket,fromlen;
fromlen=sizeof(struct sockaddr_in); 
udp_socket = socket(AF_INET, SOCK_DGRAM, 0);  
 struct sockaddr_in locAddr,fromAddr,address,gcAddr;

       locAddr.sin_family = AF_INET;
                                            
       locAddr.sin_addr.s_addr = INADDR_ANY;


       locAddr.sin_port = htons(1234); // port at which uavs send data   .

    
       if (udp_socket == -1) {

        perror("Failed to open udp socket ");

        pthread_exit(0);

       }

       if (-1 == bind(udp_socket,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))

        {

        perror("error bind failed");

        close(udp_socket);

        pthread_exit(0);
        } 

char rcv_buff[1024];

       gcAddr.sin_family = AF_INET;

       gcAddr.sin_addr.s_addr = inet_addr("192.168.0.112");

       gcAddr.sin_port = htons(MAVLINK_REMOTE_UDP_PORT);

      for (;;) {

        
        int recSize = recvfrom(udp_socket, rcv_buff, sizeof(rcv_buff),0,(struct sockaddr *)&fromAddr, &fromlen);  // receiving from UAV
        int n=sendto(udp_socket,rcv_buff,strlen(rcv_buff),0,(struct sockaddr *)&gcAddr,fromlen);
        
        if (recSize == -1) {
  
        perror("Error recvfrom ");
        }
        // inet_ntop(fromAddr.sin_family, &fromAddr.sin_addr.s_addr, printbuf, sizeof(printbuf));
        //printf("%s\n",printbuf);
       
       
    }

}
