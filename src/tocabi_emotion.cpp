#include "tocabi_emotion/tocabi_emotion.h"

int main(int argc, char **argv)
{

    ros::init(argc, argv, "tocabi_emotion_node");
    ros::NodeHandle nh;

	tocabi_emotion_sub = nh.subscribe("/tocabi/emotion", 1, emotionCallback);
	ros::Rate loop_rate(10);

	fd = open(serialport, O_RDWR | O_NOCTTY );

	if(fd<0)
	{
		printf("serial port is error");
		return fd;
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("port is open.\n");
	}

    struct termios port_settings;      
	cfsetispeed(&port_settings, B115200);    
	cfsetospeed(&port_settings, B115200);

	port_settings.c_cflag &= ~PARENB;    
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;
	
	tcsetattr(fd, TCSANOW, &port_settings); 

	while (ros::ok())	
	{
        cmd[0] = (char)em;
	    a = write(fd,cmd, sizeof(cmd));

        if(a<0)
	    {
		    printf("write error");
			return a;
	    }

		ros::spinOnce();
		loop_rate.sleep();
	}

	close(fd);
    return 0;
}

void emotionCallback(const std_msgs::Int64::ConstPtr &msg)
{
    em = msg->data;
}