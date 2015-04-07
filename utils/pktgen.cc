// plurt - small program to output data at given rate (bytes/s)

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>

unsigned char packet[48];
unsigned char head[2];

long interval = 1000; //ms
long pktsize = 0;

unsigned char appendcrc = 0;

struct timespec now,last;
struct timespec sleeptime;

unsigned short crc;

void CRC16_add(unsigned char c) // CCITT polynome
{
  int i;
  crc ^= (unsigned short)c << 8;
  for (i = 0; i < 8; i++) {
    if (crc & 0x8000) {
      crc = (crc << 1) ^ 0x1021;
    } else {
      crc <<= 1;
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("USAGE: pktgen <device> <pktsize> <interval> [crc]\n");
    exit(1);
  }

  pktsize = strtol(argv[2],NULL,0);
  if ((pktsize < 1) || (pktsize >48)) exit(2);

  interval = strtol(argv[3],NULL,0);
  if ((interval < 0) || (interval>10000)) exit(2);

  if (argc == 5)
    appendcrc = atoi(argv[4]);

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
      if (interval) {
        write(fd,head,2);
        write(fd,packet,pktsize);
        if (appendcrc) {
          crc = 0;
          // CRC16_add(head[0]);
          // CRC16_add(head[1]);
          for (int i=0; i<pktsize; i++)
            CRC16_add(packet[i]);
          unsigned char tmp[2];
          if (appendcrc == 2) crc^=0xdead;
          tmp[0] = crc >> 8;
          tmp[1] = crc & 0xff;
          write(fd,tmp,2);
        }
        packet[0]++;
      }
      last = now;
    }

    unsigned char c;
    while (1==read(fd,&c,1)) {
      printf("%s%02x ", (c == 0xf0)?"\n":"", c);
    }

    if (1==read(1,&c,1)) {
      if ((c=='+') && (interval<1000)) interval+=10;
      if (c=='-') {
        if (interval>10)
          interval-=10;
        else
          interval=0;
      }
      if (appendcrc && (c=='c')) {
        if (appendcrc == 2)
          appendcrc = 1;
        else
          appendcrc = 2;
      }
      printf("\ninterval=%ld appendcrc=%d\n",interval,appendcrc);
    }
    nanosleep(&sleeptime,NULL);
  }
  return 0;
}
