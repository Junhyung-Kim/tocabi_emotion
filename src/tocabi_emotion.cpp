#include "tocabi_emotion/tocabi_emotion.h"

int main(int argc, char **argv)
{

    ros::init(argc, argv, "tocabi_emotion_node");
    ros::NodeHandle nh;

	tocabi_emotion_sub = nh.subscribe("tocabi/emotion", 1, emotionCallback);
	ros::Rate loop_rate(1000);

	fd = open(serialport, O_RDWR | O_NOCTTY);  //FACE
	fd1 = open(serialport1, O_RDWR | O_NOCTTY); //FT

	if(fd<0)
	{
		printf("serial port is error \n");
	}
	else
	{
	//	fcntl(fd, F_SETFL, 0);
		printf("port is open.\n");
	}
 
    struct termios port_settings;      
	cfsetispeed(&port_settings, B115200);    
	cfsetospeed(&port_settings, B115200);
	cfsetispeed(&port_settings1, B115200);    
	cfsetospeed(&port_settings1, B115200);

	port_settings.c_cflag &= ~PARENB;    
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;

	port_settings1.c_cflag &= ~PARENB;    
	port_settings1.c_cflag &= ~CSTOPB;
	port_settings1.c_cflag &= ~CSIZE;
	port_settings1.c_cflag |= CS8;
	port_settings1.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	port_settings1.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	port_settings1.c_lflag &= ~ICANON;
	port_settings1.c_lflag &= ~ECHO; // Disable echo
	port_settings1.c_lflag &= ~ECHOE; // Disable erasure
	port_settings1.c_lflag &= ~ECHONL; // Disable new-line echo
	port_settings1.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	port_settings1.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	port_settings1.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	port_settings1.c_oflag &= ~OPOST;
	port_settings1.c_oflag &= ~ONLCR; 

	port_settings1.c_cc[VTIME] = 10;
	port_settings1.c_cc[VMIN] = 0;
	
	tcsetattr(fd, TCSANOW, &port_settings); 
	
	while (ros::ok())	
	{
		if(callback == true)
		{
			if(em == 1)
			{
				char cmd1[] = {'1'};
				a = write(fd, cmd1, sizeof(cmd1));
			}
			else if(em == 2)
			{
				char cmd2[] = {'2'};
				a = write(fd, cmd2, sizeof(cmd2));
			}
			else if(em == 3)
			{
				char cmd3[] = {'3'};
				a = write(fd, cmd3, sizeof(cmd3));
			}
			else if(em == 4)
			{
				char cmd4[] = {'4'};
				a = write(fd, cmd4, sizeof(cmd4));
			}
			else
			{
				char cmd5[] = {'0'};
				a = write(fd, cmd5, sizeof(cmd5));
			}
			callback = false;
		}
		
        if(a<0)
	    {
		    printf("write error \n");
	    }
		ros::spinOnce();
		loop_rate.sleep();
	}

	close(fd);
	close(fd1);
    return 0;
}

void emotionCallback(const std_msgs::Int64::ConstPtr &msg)
{
    em = msg->data;
	callback = true;
}