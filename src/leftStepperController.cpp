#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

#include <wiringPi.h>

#define leftStepperDirectionPin 7
#define leftStepperStepPin 11

float rightStepperSpeed;
float leftStepperSpeed; // in m/s
float leftStepperRPM;
double leftStepperDelay;

const float distanceBetweenTwoWheels = 0.313; // in m
const float wheelsRaduis = 0.04325; // in m

void cmd_velCallback (const geometry_msgs::Twist twist)
{

    rightStepperSpeed = (twist.angular.z * M_PI * distanceBetweenTwoWheels / 2) + twist.linear.x; // this is in m/s
    leftStepperSpeed =  (twist.linear.x * 2) - rightStepperSpeed;

    leftStepperRPM = leftStepperSpeed / (M_PI * wheelsRaduis);

    // calculate the delay for the rpm in msec
    leftStepperDelay = abs( 600 / (4 * leftStepperRPM) );

    // Set the directions
    if(leftStepperSpeed < 0)
    {
      digitalWrite(leftStepperDirectionPin, LOW);

    } // end if

    else // leftStepperSpeed >= 0
    {
      digitalWrite(leftStepperDirectionPin, HIGH);

    } // end else

   ROS_INFO("Left stepper delay: %f\n", leftStepperDelay);


    	// Move the motor
	digitalWrite(leftStepperStepPin, LOW);
	usleep(leftStepperDelay);
	digitalWrite(leftStepperStepPin, HIGH);
	usleep(leftStepperDelay);
	digitalWrite(leftStepperStepPin, LOW);
	usleep(leftStepperDelay);
	digitalWrite(leftStepperStepPin, HIGH);

} // end cmd_velCallback

int main(int argc, char **argv)
{
	ros::init(argc, argv, "leftStepperController");

	ros::NodeHandle nodeHandle;

	ros::Subscriber cmd_velSubscriber = nodeHandle.subscribe("cmd_vel", 10, cmd_velCallback);


	wiringPiSetupSys();

	system("gpio export 7 output");
	//pinMode(rightStepperDirectionPin, OUTPUT);

	system("gpio export 11 output");
	//pinMode(rightStepperStepPin, OUTPUT);

	ros::Rate r(1.0);
	while(nodeHandle.ok())
		ros::spinOnce();
	r.sleep();

} // end main
