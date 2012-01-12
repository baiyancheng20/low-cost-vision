#include <cratedemo/GridCrate.hpp>
#include <cratedemo/MiniBall.hpp>
#include <iostream>
#include <cratedemo/initialCrateContentDemo1.hpp>
#include <cratedemo/CrateDemo.hpp>
#include <ros/ros.h>
#include <unistd.h>
#include <iostream>

using namespace cratedemo;

class Demo : public CrateDemo
{
public:	
	Demo(ros::NodeHandle& hNode,
		CrateContentMap& crateContentMap):CrateDemo(hNode,
		"enableGripper" ,
		"stop",
		"moveTo" ,
		"deltaError" ,
		"visDummy",crateContentMap) {}

	void onNewCrate(Crate& crate) {
		ROS_INFO("onNewCrate called %s, on location:\tx:\t%f\ty:\t%f",crate.getName().c_str(),crate.position.x,crate.position.y);
		CrateMap::iterator it1 = crates.find("GC4x4MB_1");
		if(it1 == crates.end()) { return; }

		CrateMap::iterator it2 = crates.find("GC4x4MB_2");
		if(it2 == crates.end()) { return; }

		

		for(size_t i = 0; i < 4*4; i++)
		{
			moveObject(*(it1->second), i, *(it2->second), i);
		}
		ROS_INFO("Demo done...");
		//CrateDance(crate);
	}

	void onCrateMove(Crate& crate)
	{
		ROS_INFO("onCrateMove called %s",crate.getName().c_str());
	}

	void onCrateRemoved(Crate& crate)
	{
		ROS_INFO("onCrateRemoved called %s",crate.getName().c_str());
	}
};

using namespace std;

int main(int argc, char** argv) {
	ros::init(argc, argv, "demoNode");
	ros::NodeHandle nodeHandle;
	Demo d(nodeHandle,initializeCrateContent1());
	while(ros::ok()){
		d.update();
		usleep(1000/60);
	}
	cout << "ros is not OK" << endl;
	return 0;
}
