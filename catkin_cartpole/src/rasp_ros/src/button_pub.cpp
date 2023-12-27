#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <iostream>		// Include all needed libraries here
#include <ctime>
#include <wiringPi.h>

using namespace std;		// No need to keep using “std”

bool controlFlag = false;

const int btnPin = 0;
int btnState = HIGH;	// Button State
int lastBtnState = HIGH;	// Last Button State
clock_t time_begin;
double btn_react_time = 0.5;


int main(int argc, char** argv)
{
	wiringPiSetup();			// Setup the library
	pinMode(btnPin, INPUT);		// Configure Button Pin as an input

	ros::init(argc, argv, "button_publisher");

	ros::NodeHandle nh;
	ros::Publisher btn_pub = nh.advertise<geometry_msgs::Twist>("hoverboard_velocity_controller/cmd_vel", 1000);

	//ros::Rate loop_rate(10);
	geometry_msgs::Twist twist_msg;
	
	cout << "Button is under detection!" << endl;

	time_begin = clock();

	// Main program loop
	while(ros::ok())
	{
		btnState = digitalRead(btnPin);
		cout << "Button State is: " << btnState << endl;	
	
		clock_t curr_time = clock();
		double duration = static_cast<double>(curr_time - time_begin) / CLOCKS_PER_SEC;
		// Check if the button is pressed
		if (btnState == LOW && lastBtnState == HIGH) {
			time_begin = clock();
		}

        // Check if the button is released
        else if (btnState == HIGH && lastBtnState == LOW) {
           
			cout << "Button Released!" << endl;
			time_begin = clock();

			if (duration >=  btn_react_time) { 
				controlFlag ^= 1; 	// Change control flag
			}
        }

		lastBtnState = btnState;

		if(controlFlag) {
			twist_msg.linear.x = 0.2;
			btn_pub.publish(twist_msg);
			ROS_INFO("Published Twist message: linear=%.2f, angular=%.2f",
                 twist_msg.linear.x, twist_msg.angular.z);		
		
		}else {
			twist_msg.linear.x = 0.;
			btn_pub.publish(twist_msg);
			ROS_INFO("The robot is stopped now.");		
		}
		ros::spinOnce();
		//loop_rate.sleep();

	}
	return 0;
}
