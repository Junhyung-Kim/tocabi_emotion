#include "ros/ros.h"
#include "std_msgs/Int64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>

char serialport[]= "/dev/tty0";

int fd, a;
struct termios port_settings; 

ros::Subscriber tocabi_emotion_sub;

int em;
char cmd[] = {'0'};

void emotionCallback(const std_msgs::Int64::ConstPtr &msg);
