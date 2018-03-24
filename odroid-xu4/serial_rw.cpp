#include "ros/ros.h"
#include "SerialCom.h"

int main(int argc, char **argv)
{
	SerialCom uart;

	ros::init(argc, argv, "mcu_interface");

	ros::NodeHandle nh;

	ros::Subscriber pwm_sub = nh.subscribe("/pwm", 1000, &SerialCom::callback, &uart);

	ros::spin();

	return 0;
}