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

#include "/home/dhruv/Desktop/telemetry_work/mavlink/include/mavlink/v1.0/common/mavlink.h"

/* Should probably allow this to be passed in on the command line */

const char * ser_path = "/dev/ttyUSB0";

#define SRV_IP "192.168.0.112" // loopback

#define LOCAL_PORT 5760    // Ardupilot default port

#define MAVLINK_LOCAL_UDP_PORT 14551  // Default UDP port for Mavlink 

#define MAVLINK_REMOTE_UDP_PORT 14550 // QGC listens to this

struct thread_args {

  int ser_fd;

  int udp_sock;

};

struct sockaddr_in gcAddr; 

struct sockaddr_in locAddr;

void forward_udp_2_serial(struct thread_args *r2)

{

  int serial_fd = r2->ser_fd;

  int udp_socket = r2->udp_sock;

  char rcv_buff[1024];

  struct sockaddr_in fromAddr;

  int fromLen;

  char printbuf[INET_ADDRSTRLEN];

  mavlink_status_t msg_status;

  mavlink_message_t msg;

  int i;

  for (;;) {

    fromLen = sizeof(gcAddr);

    int recSize = recvfrom(udp_socket, rcv_buff, sizeof(rcv_buff),0,(struct sockaddr *)&fromAddr, &fromLen);

    if (recSize == -1) {

      perror("Error recvfrom ");

    }

    inet_ntop(fromAddr.sin_family, &fromAddr.sin_addr.s_addr, printbuf, sizeof(printbuf));

    //    printf("received 0x%x bytes from from %s\n", recSize, printbuf);

    for (i = 0; i < recSize; ++i) {

      int n = write(serial_fd, &rcv_buff[i], 1);

      if (n == -1) {

perror("Error writing to serial port ");

      }

      if (n != 1) {

printf("write to serial link returned %d\n", n);

      }

      //      printf("%02x ", (unsigned char)rcv_buff[i]);

      if (mavlink_parse_char(MAVLINK_COMM_0, rcv_buff[i], &msg, &msg_status)) {

// Packet received

printf("UDP -> Serial: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n", 

      msg.sysid, msg.compid, msg.len, msg.msgid);

      }

    }

  }

}

void forward_serial_2_udp(struct thread_args *args)

{

  char recvd_byte;

  mavlink_status_t msg_status;

  mavlink_message_t msg;

  int rc;

  static const int FIXED_HEADER_LEN = 

    MAVLINK_NUM_HEADER_BYTES +   // 6

    MAVLINK_NUM_CHECKSUM_BYTES;  // 2

  int udp_sock_flags = 0;

  int serial_fd = args->ser_fd;

  int udp_socket = args->udp_sock;
  printf("serial_fd=%d, udp_socket=%d \n",serial_fd,udp_socket);

  for (;;) {
   
    int n = read(serial_fd, &recvd_byte, 1); // read a char at a time
   
    if( n==-1) {

      perror("Error reading serial port");

    }

    if( n==0 ) {

      usleep( 10 * 1000 ); // wait 10 msec try again

      continue;
    }

    if (mavlink_parse_char(MAVLINK_COMM_0, recvd_byte, &msg, &msg_status)) {

      int j;

      // Packet received
    
      printf("Serial -> UDP: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n",

    msg.sysid, msg.compid, msg.len, msg.msgid);

#ifdef DEBUG

      for (j = 0; j < msg.len + FIXED_HEADER_LEN; j++) {

printf("0x%02x ", *((uint8_t*)&msg.magic + j) & 0x00ff);

if (((j+1) % 8) == 0) printf("\n");

      }

#endif

      // Now write the packet out to the UDP socket

      rc = sendto(udp_socket, 

  (uint8_t*)&msg.magic, 

  msg.len + FIXED_HEADER_LEN,

  udp_sock_flags,

  (const struct sockaddr *)&gcAddr, 

  sizeof(gcAddr));

      if (rc == -1) {

perror("Error writing UDP port ");

      }

    }

  }

}

int init_serial_port(int ser_port)

{

  struct termios ser_attr;

  // Issue TIOCEXCL ioctl to prevent additional opens except by root-owned processes.

  if (ioctl(ser_port, TIOCEXCL) == -1) {

    printf("Error setting TIOCEXCL on %s - %s(%d).\n",

  ser_path, strerror(errno), errno);

    exit(EXIT_FAILURE);

  }

  // Set the baud rate

  // Get the current options and save them so we can restore the default settings later.

  if (tcgetattr(ser_port, &ser_attr) == -1) {

    printf("Error getting tty attributes %s - %s(%d).\n",

  ser_path, strerror(errno), errno);

    exit(EXIT_FAILURE);

  }

  cfmakeraw(&ser_attr);

  cfsetspeed(&ser_attr, 115200);// Cause the new options to take effect immediately.

  // 8N1

  ser_attr.c_cflag &= ~PARENB;

  ser_attr.c_cflag &= ~CSTOPB;

  ser_attr.c_cflag &= ~CSIZE;

  ser_attr.c_cflag |= CS8;

  // no flow control

  ser_attr.c_cflag &= ~CRTSCTS;

  ser_attr.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines

  ser_attr.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

  ser_attr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw

  ser_attr.c_oflag &= ~OPOST; // make raw

  // see: http://unixwiz.net/techtips/termios-vmin-vtime.html

  ser_attr.c_cc[VMIN]  = 0;

  ser_attr.c_cc[VTIME] = 20;

  if (tcsetattr(ser_port, TCSANOW, &ser_attr) == -1) {

    printf("Error setting tty attributes %s - %s(%d).\n",

  ser_path, strerror(errno), errno);

    exit(EXIT_FAILURE);

  }

  if (fcntl(ser_port, F_SETFL, 0)) {

    printf("Error setting tty non-blocking %s - %s(%d).\n",

  ser_path, strerror(errno), errno);

    exit(EXIT_FAILURE);

  }

}

const char help[] = "--help";

int main(int argc, char* argv[])

{

  pthread_t thread1, thread2;

  struct thread_args args1, args2;

  char target_ip[100];

  int sock;

  int ser_port;

  // Check if --help flag was used
  printf("%d\n",argc);

  if ((argc == 2) && (strcmp(argv[1], help) == 0)) {

    printf("\n");

    printf("\tUsage:\n\n");

    printf("\t");

    printf("%s", argv[0]);

    printf(" <ip address of QGroundControl>\n");

    printf("\tDefault for localhost: udp-server 127.0.0.1\n\n");

    exit(EXIT_FAILURE);

  }

    
  printf("%s\n",ser_path);
  ser_port = open(ser_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
  printf("%d \n",ser_port);

  if (ser_port == -1) {

    /*

     * Could not open the port.

     */

    perror("open_port: Unable to open serial port ");

    exit(EXIT_FAILURE);

  }

  init_serial_port(ser_port);

  // Change the target ip if parameter was given

  strcpy(target_ip, SRV_IP);

  // TODO Make sure that argv[1] is a valid IP address syntax

  if (argc == 2) {

    strcpy(target_ip, argv[1]);

  }

    

  memset(&locAddr, 0, sizeof(locAddr));

  locAddr.sin_family = AF_INET;

  locAddr.sin_addr.s_addr = INADDR_ANY;

  locAddr.sin_port = htons(MAVLINK_LOCAL_UDP_PORT);

  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);    

  if (sock == -1) {

    perror("Failed to open udp socket ");

   exit(EXIT_FAILURE);

  }

  /* Bind the socket to port 14551 

     - necessary to receive packets from qgroundcontrol */ 

  if (-1 == bind(sock,(struct sockaddr *)&locAddr, sizeof(struct sockaddr)))

    {

      perror("error bind failed");

      close(sock);

      exit(EXIT_FAILURE);

    } 

    

  memset(&gcAddr, 0, sizeof(gcAddr));

  gcAddr.sin_family = AF_INET;

  gcAddr.sin_addr.s_addr = inet_addr(target_ip);

  gcAddr.sin_port = htons(MAVLINK_REMOTE_UDP_PORT);

    

  args1.ser_fd = ser_port;

  args1.udp_sock = sock;

  pthread_create(&thread1, NULL, (void*) forward_serial_2_udp,

(void*) &args1);

  args2.ser_fd = ser_port;

  args2.udp_sock = sock;

  pthread_create(&thread2, NULL, (void*) forward_udp_2_serial,

(void*) &args2);

  

  /* Once the threads are up and running, we want to be careful never to

   * close the serial port, as that will result in a restart of the APM 

   * card, which will crash the copter.

   *

   * So, let's just wait here forever in a busy loop.

   */

  pthread_join(thread1, NULL);

  pthread_join(thread2, NULL);

  for (;;) {

    sleep(1);

  }

  

  return (-1);

}

 
