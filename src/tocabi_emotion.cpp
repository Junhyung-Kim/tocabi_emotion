#include "tocabi_emotion/tocabi_emotion.h"

int main(int argc, char **argv)
{

    ros::init(argc, argv, "tocabi_emotion_node");
    ros::NodeHandle nh;

	tocabi_emotion_sub = nh.subscribe("tocabi/emotion", 1, emotionCallback);
	hand_force_pub = nh.advertise<std_msgs::Float64MultiArray>("/tocabi/handforce" , 1);
	ros::Rate loop_rate(1000);

	fd = open(serialport, O_RDWR | O_NOCTTY);  //FACE
	fd1 = open(serialport1, O_RDWR | O_NOCTTY); //FT
	if(fd<0)
	{
		printf("serial port1 is error \n");
	}
	else
	{
	//	fcntl(fd, F_SETFL, 0);
		printf("port1 is open.\n");
	}

	if(fd1<0)
	{
		printf("serial port2 is error \n");
	}
	else
	{
	//	fcntl(fd, F_SETFL, 0);
		printf("port2 is open.\n");
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
		    printf("Write port1 error \n");
	    }

		char read_buf[90];
		a  = read(fd1, read_buf, sizeof(read_buf));

		if(a<0)
	    {
		    printf("Read port2 error \n");
	    }

		std::string read_data(read_buf);
		std::cout << "read data : " << read_data << std::endl;

		std::vector<double> vect;
		std::stringstream ss(read_data);
		double num;	

		while(ss>>num) {
			vect.push_back(num);    
			if (ss.peek() == ',')
				ss.ignore();
		}

	/*	for (std::size_t i = 0; i < vect.size(); i++)
			std::cout << "vector " << vect[i] << std::endl;
	*/
		for(int i = 0; i < 5; i++)
		{
			force_msg.data[i] = vect[i];
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
	callback = true;
}