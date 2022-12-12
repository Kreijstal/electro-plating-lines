#include "ElectroPlatingLines.h"

// Define a struct named "robot_arm_1" that represents a robot arm
struct robot_arm_1
{    // Define a member named "robot_1_movement_time" that is a vector of ints
    std::vector<int> robot_1_movement_time;
    std::vector<int> robot_1_transport_time;
    std::vector<int> robot_1_processing_time;
    std::vector<tuple<int,int>> robot_1_route;
};

struct robot_arm_2
{
    std::vector<int> robot_2_movement_time;
    std::vector<int> robot_2_transport_time;
    std::vector<int> robot_2_processing_time;
    std::vector<tuple<int, int>> robot_2_route;
};
// Create an instance of the "robot_arm_1" struct named "arm"

// 18. November 2022 Freitag. EN Gebaeude //
//switched max plus, define matrices. a for product a, b for product b. once u have them u can combine them. problem is getting the matrices. 
//robot follows the same route, and products stay there for the same time. 
//if they are both like that. they are undistinguishable. so the matrices would be the same. code would be adaptable to these schedules. 
// same routes but different waiting times. then you can change the schedule. still all the pieces go to the tanks, all the tanks but in different
// orders. open an issue for each step. once u implement it, you can proceed. first two are relatively easy 
// third case, more time consuming. 
// for loop that searches all the products. vector of matrices. 
//assertion of the same length. 2 vectors of everything. 
//vector of a schedule, m2. element of a structure juan created. each structure, scheduel and time each structure 
// struct c++ . tuples= order sets. (a ,b ,c ,d )each position has a different name. diff data types. must be consistent. 
// robot_info? if thats ok in c++ > up to you. 
// info_robots.route 
// info_robots.movement_time ?
// info_robots.transportation_time
// info_robots.processing_time
// route.struct {{_,_},{_,_}}
// input is info_robot1.route and info_robot2.route
// B, C are the same? in which cases are they not? assume they are the same
// possibily of having them different. more input deposits then output deposit, which by us is not the case
// seperate input that takes care of the shared place
// one robot has the input of the shared tank and the other does not
// another possibility>
//   another file where you can aufruf these functions
// shared files for things we share. w juan 
int main() {
    info_products robotarm1;
    robot_arm_1 arm;
    // Initialize the "robot_1_movement_time" member of "arm" with the specified vector of ints
    arm.robot_1_movement_time = { 1,2,3 };
    arm.robot_1_transport_time = { 4,5,6 };
    arm.robot_1_processing_time = { 7,8,9 };
    arm.robot_1_route = { {0,2},{2,1},{1,3} };
    robotarm1.route = arm.robot_1_route;
    robotarm1.processingTime= arm.robot_1_processing_time;
    robotarm1.transportationTime = arm.robot_1_transport_time;
    robotarm1.movementTime = arm.robot_1_movement_time;
    
    robot_arm_2 arm2;
    arm2.robot_2_movement_time = { 2, 3, 1, 5 };
    arm2.robot_2_transport_time = { 1, 2, 3, 4 };
    arm2.robot_2_processing_time = { 5, 4, 3, 2 };
    arm2.robot_2_route = { {3,5},{5,4},{4,6} };
    return 0;
}

