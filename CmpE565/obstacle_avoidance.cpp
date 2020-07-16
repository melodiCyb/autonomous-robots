#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_listener.h>


class ObstacleAvoidance
{

    ros::Subscriber scanSubs;
    ros::Publisher velPub;
    tf::TransformListener transformListener;
    tf::StampedTransform robotTransform;
    ros::NodeHandle nh;

public:
    ObstacleAvoidance()
    {
        scanSubs = nh.subscribe("/scan", 10, &ObstacleAvoidance::laserCallback, this);
        velPub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 1);
        transformListener.waitForTransform("odom", "base_link",
                ros::Time::now(), ros::Duration(2.0));
    }

    void laserCallback(const sensor_msgs::LaserScan& input_scan)
    {
        
	// inspect laser data
        std::cout << "ranges size: " << input_scan.ranges.size() << std::endl;
        std::cout << "max range: " << input_scan.range_max << std::endl;
        std::cout << "min range: " << input_scan.range_min << std::endl;
        std::cout << "min angle: " << input_scan.angle_min << std::endl;
        std::cout << "max angle: " << input_scan.angle_max << std::endl;
        std::cout << "min increment: " << input_scan.angle_increment << std::endl;
        
        float min_val = 0.5;
        geometry_msgs::Twist moveCmd;
 
        /*
        Detect obstacles by inspecting laser scans
        */
       
        //std::cout<<input_scan.ranges[i]<<" ";
	std::cout << "ranges 200: " << input_scan.ranges[200] << std::endl;
	std::cout << "ranges 250: " << input_scan.ranges[250] << std::endl;
	std::cout << "ranges 300: " << input_scan.ranges[300] << std::endl;
	if (input_scan.ranges[200] > min_val && input_scan.ranges[250] > min_val &&
    input_scan.ranges[300] > min_val)
	{
		moveCmd.linear.x = 2.5;
		moveCmd.angular.z = 0.0;
		
	}
	else 
	{
		moveCmd.linear.x = 0.0;
   		moveCmd.angular.z = 0.5 ;
		   		
	}

        // inspect robot pose
        transformListener.lookupTransform("world","base_link",
                ros::Time(0),robotTransform);
        float robotX = robotTransform.getOrigin().x();
        float robotY = robotTransform.getOrigin().y();
        float robotYaw = tf::getYaw(robotTransform.getRotation());
        std::cout << "robot pose: x,y,yaw: " << robotX << "," << robotY << "," << robotYaw
                << std::endl;

  
       velPub.publish(moveCmd);
}
   };

int main(int argc,char **argv)
{
    std::cout << "Obstacle Avoidance Start..." << std::endl;
    ros::init(argc,argv,"obstacle_avoidance");
    ObstacleAvoidance tp;
    ros::spin();
    printf("Obstacle Avoidance Finish...");
}

