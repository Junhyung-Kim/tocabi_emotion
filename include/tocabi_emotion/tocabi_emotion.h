#include "ros/ros.h"
#include "std_msgs/Int64.h"
#include "std_msgs/Float64MultiArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>

char serialport[]= "/dev/ttyACM0";
char serialport1[]= "/dev/ttyACM1";

int fd, fd1, a;
struct termios port_settings; 

ros::Subscriber tocabi_emotion_sub;
ros::Publisher hand_force_pub;

std_msgs::Float64MultiArray force_msg;

int em;
char cmd[] = {'0'};

void emotionCallback(const std_msgs::Int64::ConstPtr &msg);
