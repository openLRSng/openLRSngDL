// plurt - small program to output data at given rate (bytes/s)

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>

unsigned char packet[32];
unsigned char head[2];

long interval = 1000; //ms
long pktsize = 0;

struct timespec now,last;
struct timespec sleeptime;

int main(int argc, char **argv) {
  if (argc != 4) {
    exit(1);
  }

  pktsize = strtol(argv[2],NULL,0);
  if ((pktsize < 1) || (pktsize >32)) exit(2);

  interval = strtol(argv[3],NULL,0);
  if ((interval < 1) || (interval>10000)) exit(2);

  int fd = open(argv[1],O_RDWR|O_NONBLOCK);
  if (fd < 0) exit(3);

  fcntl(1,F_SETFL,O_NONBLOCK);

  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 1000000LL;

  for (int i=1; i < pktsize; i++)
    packet[i] = i;

  head[0] = 0xf0;
  head[1] = pktsize;

  clock_gettime(CLOCK_MONOTONIC, &last);
  while (1) {
    clock_gettime(CLOCK_MONOTONIC, &now);
    long long nsecs = 1000000000LL * (now.tv_sec - last.tv_sec);
    nsecs += now.tv_nsec - last.tv_nsec;

    if (nsecs > 1000000LL * interval) {
      write(fd,head,2);
      write(fd,packet,pktsize);
      packet[0]++;
      last = now;
    }

    unsigned char c;
    while (1==read(fd,&c,1)) {
      printf("%s%02x ", (c == 0xf0)?"\n":"", c);
    }

    if (1==read(1,&c,1)) {
      if ((c=='+') && (interval<1000)) interval+=10;
      if ((c=='-') && (interval>19)) interval-=10;
      printf("\ninterval set to %ld\n",interval);
    }
    nanosleep(&sleeptime,NULL);
  }
  return 0;
}
