#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

long get_difference_of_time(struct timespec begin, struct timespec end) {
  return (end.tv_sec - begin.tv_sec) * 1000 + (end.tv_nsec - begin.tv_nsec) / 1000 / 1000;
}
