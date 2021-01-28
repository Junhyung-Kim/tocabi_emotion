#include "tocabi_emotion/tocabi_emotion.h"

int main(int argc, char **argv)
{

	ros::init(argc, argv, "tocabi_emotion_node");
	ros::NodeHandle nh;

	tocabi_emotion_sub = nh.subscribe("tocabi/emotion", 1, emotionCallback);
	hand_force_pub = nh.advertise<std_msgs::Float64MultiArray>("/tocabi/handforce" , 1);
	ros::Rate loop_rate(2);

	for(int i = 0; i < 9; i++)
	{
		force_msg.data.push_back(0.0);
	}

	for(int i = 0; i < 9; i++)
	{
		force_msg_prev.data.push_back(0.0);
	}


	fd = open(serialport, O_RDWR | O_NOCTTY);  //FACE
	fd1 = open(serialport1, O_RDWR | O_NOCTTY| O_NDELAY); //FT
	if(fd<0)
	{
		//printf("serial port1 is error \n");
	}
	else
	{
	//	fcntl(fd, F_SETFL, 0);
		//printf("port1 is open.\n");
	}

	if(fd1<0)
	{
		printf("serial port2 is error \n");
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
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


	tcflush(fd1, TCIFLUSH);

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
		   //  printf("Write port1 error \n");
	   	}

		char read_buf[60];
		a  = read(fd1, read_buf, sizeof(read_buf));

		if(a<0)
	    	{
		    printf("Read port2 error \n");
	    	}

		int row = 0;
		std::string read_data(read_buf);
		std::vector<std::vector<double>> vect(3);
		std::stringstream ss(read_data);
		double num;
		std::string read_line;

		while(std::getline(ss, read_line))
		{
			std::stringstream ss1(read_line);

			//std::cout << "row "  << read_line << std::endl;
			while(ss1>>num) {	

			//std::cout << "num "  << num << std::endl;
				vect[row].push_back(num);   
				if (ss1.peek() == ',')
					ss1.ignore();
				//std::cout << "col" << std::endl;
			}
			row++;
		}

		for(int i=0; i<row; i++)
		{	
			for(std::size_t j = 0; j < vect[i].size(); j++)
			{
				//std::cout << "vector " << i << ", " << j<< " " <<vect[i][j] << std::endl;
			}
		}
	
		for(int i = 0; i < 5; i++)
		{
			if(vect[0].size() == 5)
			{
				force_msg.data[i] = vect[0][i];
			}
			else if(vect[1].size() == 5)
			{
				force_msg.data[i] = vect[1][i];
			}
			else if(vect[2].size() == 5)
			{
				force_msg.data[i] = vect[2][i];
			}
			else
			{
				force_msg.data = force_msg_prev.data;
			}
		}

		for(int j = 0; j < 5; j++)
			std::cout << "rosmsg " << j << " " << force_msg.data[j] << std::endl;


		hand_force_pub.publish(force_msg);

		force_msg_prev.data = force_msg.data;


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
