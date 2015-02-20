// plurt - small program to output data at given rate (bytes/s)

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>

char datablock[] =
  "#######  #######   #####   #######  ######      #     #######     #     \r\n"
  "   #     #        #     #     #     #     #    # #       #       # #    \r\n"
  "   #     #        #           #     #     #   #   #      #      #   #   \r\n"
  "   #     #####     #####      #     #     #  #     #     #     #     #  \r\n"
  "   #     #              #     #     #     #  #######     #     #######  \r\n"
  "   #     #        #     #     #     #     #  #     #     #     #     #  \r\n"
  "   #     #######   #####      #     ######   #     #     #     #     #  \r\n";

char getbyte() {
  static unsigned int index=0;
  if (++index >= sizeof(datablock)) index = 0;
  return datablock[index];
}

int bps = 0;

long long nanobytes = 0;

struct timespec now,last;

struct timespec sleeptime;

int main(int argc, char **argv) {
  if (argc != 3) {
    exit(1);
  }
  bps = strtol(argv[2],NULL,0);
  if (!bps) exit(2);

  int fd = open(argv[1],O_RDWR|O_NONBLOCK);
  if (fd<0) exit(3);

  fcntl(1,F_SETFL,O_NONBLOCK);

  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 1000000000LL / bps *4;

  clock_gettime(CLOCK_MONOTONIC, &last);
  while (1) {
    clock_gettime(CLOCK_MONOTONIC, &now);
    long long nsecs = 1000000000LL * (now.tv_sec - last.tv_sec);
    nsecs += now.tv_nsec - last.tv_nsec;
    last=now;

    nanobytes+=nsecs*bps;
    while (nanobytes > 1000000000LL) {
      nanobytes-=1000000000LL;
      char c = getbyte();
      write(fd,&c,1);
    }
    char c;
    while (1==read(fd,&c,1)) write(0,&c,1);

    if (1==read(1,&c,1)) {
      if (c=='+') bps+=10;
      if ((c=='-') && (bps>10)) bps-=10;
      printf("\nBPS set to %d\n",bps);
    }
    nanosleep(&sleeptime,NULL);
  }
  return 0;
}
