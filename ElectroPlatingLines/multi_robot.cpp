//#include "ElectroPlatingLines.h"
#include "multi_product_lib.h"
#include <iostream>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <algorithm>
#include <functional>
#include <cinttypes>
#include <cstddef>
#include <numeric>


struct robot_arm //merged the robot arm struct (can take unendless amount of robot arm information instead of one struct for all the robot arms
{    
    std::vector<int> robot_movement_time;  //robot movement times without carrying a piece
    std::vector<int> robot_transport_time;  //robot movement times with carrying piece
    std::vector<int> robot_processing_time;  //
    std::vector<tuple<int,int>> robot_route;  //
};

//RobotRoute(int initialTank, std::vector<std::tuple<int, int>> modeArray, std::vector<int> processingTimes, int numberOfTanks);


//RobotRoute(int initialTank, std::vector<std::tuple<int, int>> modeArray, std::vector<int> processingTimes, int numberOfTanks);
//initialTank

struct shared_tank //struct to represent limited amount of shared tanks
{
    int time; 
    unsigned int places;//For now we only support 0 or 1.
};
typedef std::tuple<std::vector<Route>, std::vector<std::vector<int>>> schedule;

class RobotOrchestra
{
public:
    std::vector<shared_tank> shared_tanks;
    std::vector<RobotSchedule> vecRobotModeCollection;

    // Constructor for RobotOrchestra
    RobotOrchestra(
        std::vector<shared_tank> tanks,
        std::vector<schedule> schedules
        ): shared_tanks(tanks)
    {
        for (const auto& [vmode, mTransTimes] : schedules) {
            // Create a RobotSchedule object and push it into the vecRobotModeCollection vector
            vecRobotModeCollection.emplace_back(vmode, mTransTimes);
        }
    }
};

//RobotSchedule(std::vector<std::tuple<int, std::vector<std::tuple<int, int>>, std::vector<int>>> modes, vector<vector<int>> mTransTimes);
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
// seperate input that takes care of the shared place?
// one robot has the input of the shared tank and the other does not
// another possibility>
//   another file where you can aufruf these functions
//   shared files for things we share. w juan 
int main() {

    //matrix<series> stackMatricesDiagonally(vector<matrix<series>> vms);
    //gd i2gd(int a);
    try {
        //we have one shared tank because 2-1=1 (number of robots-1=number shared tanks)
        std::vector<schedule> robots = {
    {
        {
            {0, { { 2, 1 }, { 2, 2 }, { 1, 3 }, { 1, 4 }, { 3, 4 } }, { 5, 6 } }
        },
        { { 7 } }
    },
    {
        {
            {0, { { 1, 1 }, { 1, 2 }, { 2, 3 }, { 2, 4 }, { 3, 4 } }, { 5, 6 } }
        },
        { { 7 } }
    }
        };

        std::vector<shared_tank> tanks = { {78, 0} };

        RobotOrchestra d(tanks, robots);
        d.vecRobotModeCollection[0].vMode[0].A0_matrix;
        (d.vecRobotModeCollection[0].numOfTanks + 1) * 2;
        //d.vecRobotModeCollection.map(_=>_.vMode[0]);
        std::vector<matrix<series>> result;

        for (auto& item : d.vecRobotModeCollection) {
            result.push_back(std::get<0>(item.getBigAandBMatrix({0})));
        }
        matrix<series> thebigoutputmatrix = stackMatricesDiagonally(result);
        int p = 0;
        for (size_t i = 0; i < d.vecRobotModeCollection.size() - 1; ++i) {
            const RobotSchedule& robotSchedule = d.vecRobotModeCollection[i];
            int numOfTanks = robotSchedule.numOfTanks;

            // Calculate the position p and update it for the next iteration
            p += (numOfTanks + 1) * 2;

            // Iterate over the routes in the current RobotSchedule
            for (const RobotRoute& route : robotSchedule.vMode) {
                // Get the shared tank information
                int tankTime = d.shared_tanks[i].time;
                unsigned int tankPlaces = d.shared_tanks[i].places;

                cout << "this is tankTime" << tankTime << " tankPlaces" << tankPlaces << "position " << p << "," << p - 1 << endl;
                thebigoutputmatrix(p, p - 1) = gd(tankPlaces, tankTime);
            }
        }



       
        cout << thebigoutputmatrix;
    }
    catch (std::exception& e) {
        std::cerr << endl
            << "An error has been detected in the program, the program will exit now." << endl
            << "error message:" << e.what() << endl;
        return 1;
    }
    
    return 0;
}

