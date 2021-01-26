#include "tocabi_emotion/tocabi_emotion.h"

int main(int argc, char **argv)
{

    ros::init(argc, argv, "tocabi_emotion_node");
    ros::NodeHandle nh;

	tocabi_emotion_sub = nh.subscribe("/tocabi/emotion", 1, emotionCallback);
	hand_force_pub = nh.advertise<std_msgs::Float64MultiArray>("/tocabi/handforce" , 1);
	
	ros::Rate loop_rate(200);

	fd = open(serialport, O_RDWR | O_NOCTTY);  //FACE
	fd1 = open(serialport1, O_RDWR | O_NOCTTY); //FT

	if(fd<0)
	{
		printf("serial port1 is error");
		return fd;
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("serial port1 is open.\n");
	}

	if(fd<0)
	{
		printf("serial port2 is error");
		return fd;
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("serial port2 is open.\n");
	}

	for(int i = 0; i < 9; i++)
	{
		force_msg.data.push_back(0.0);
	}

    struct termios port_settings, port_settings1;      
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
	tcsetattr(fd1, TCSANOW, &port_settings1);

	double data[9] = {0,0,0,0,0,0,0,0,0}; 

	while (ros::ok())	
	{
        cmd[0] = (char)em;//(char)em;
	    a = write(fd,cmd, sizeof(cmd));
	    a = write(fd1,cmd, sizeof(cmd));

        if(a<0)
	    {
		    printf("write error");
			return a;
	    }

		char read_buf [256];
		char *ret_ptr;
		char *next_ptr;
		int num_bytes = read(fd1, &read_buf, sizeof(read_buf));

		if(num_bytes == -1)
		{
			printf("reading error");
			return 0;
		}

		// n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
		if (num_bytes > 0) 
		{
			ret_ptr = strtok_r(read_buf, ",", &next_ptr);
			int i=0;
			while(ret_ptr) 
			{
				data[i] = atof(ret_ptr);
				ret_ptr = strtok_r(NULL, ",", &next_ptr);
				i++;
			}
		}

		for(int i = 0; i < 9; i++)
		{
			force_msg.data[i] = data[i];
		}

		hand_force_pub.publish(force_msg);

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
}