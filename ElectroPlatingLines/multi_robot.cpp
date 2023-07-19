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


/*
struct robot_arm //merged the robot arm struct (can take unendless amount of robot arm information instead of one struct for all the robot arms
{    
    std::vector<int> robot_movement_time;  //robot movement times without carrying a piece
    std::vector<int> robot_transport_time;  //robot movement times with carrying piece
    std::vector<int> robot_processing_time;  //
    std::vector<tuple<int,int>> robot_route;  //
};
*/
//RobotRoute(int initialTank, std::vector<std::tuple<int, int>> modeArray, std::vector<int> processingTimes, int numberOfTanks);


//RobotRoute(int initialTank, std::vector<std::tuple<int, int>> modeArray, std::vector<int> processingTimes, int numberOfTanks);
//initialTank

struct shared_tank //struct to represent limited amount of shared tanks
{
    int time; 
    unsigned int places;//For now we only support 0 or 1.
};
typedef std::tuple<std::vector<Route>, std::vector<std::vector<int>>> schedule;

std::ostream& operator<<(std::ostream& out, const shared_tank& tank) {
    out << "Time: " << tank.time << ", Places: " << tank.places;
    return out;
}


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

std::vector<int> get_vector() {
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream stream(line);
        std::vector<int> vec;
        int num;
        bool valid = true;

        while (stream >> num) {
            vec.push_back(num);
        }

        if (stream.fail() && !stream.eof()) { // stream fails but is not at the end, so it means invalid data was read
            valid = false;
        }

        if (valid) {
            return vec;
        }
        else {
            std::cout << "Invalid input. Please enter a sequence of numbers separated by spaces.\n";
            // clean error state
            stream.clear();
        }
    }
}

shared_tank get_shared_tank() {
    int time;
    unsigned int places;

    while (true) {
        std::cout << "Please give in the holding time in digits and press enter: ";
        if (!(std::cin >> time)) { // if the extraction fails
            std::cin.clear(); // clears the error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discards the input
            std::cout << "Invalid input. Please enter a number: ";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // removes the '\n' character from the input buffer
            break;
        }
    }

    while (true) {
        std::cout << "Please give in the number of inital tokens (0 or 1) inside: ";
        if (!(std::cin >> places) || (places != 0 && places != 1)) { // if the extraction fails or the number isn't 0 or 1
            std::cin.clear(); // clears the error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discards the input
            std::cout << "Invalid input. Please enter either 0 or 1: ";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // removes the '\n' character from the input buffer
            break;
        }
    }

    return { time, places };
}

template<typename T>
T get_number() {
    T num;

    while (true) {
        if (!(std::cin >> num)) { // if the extraction fails
            std::cin.clear(); // clears the error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discards the input
            std::cout << "Invalid input. Please enter a number: ";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // removes the '\n' character from the input buffer
            break;
        }
    }

    return num;
}




Route get_route() {
    size_t id;
    while (true) {
        std::cout << "Enter initial tank index (where the hoist begins): ";
        if (!(std::cin >> id)) { // if the extraction fails
            std::cin.clear(); // clears the error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discards the input
            std::cout << "Invalid input. Please enter a number: ";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // removes the '\n' character from the input buffer
            break;
        }
    }


    std::vector<std::tuple<size_t, int>> transitions;
    size_t from;
    int time;
    while (true) {
        std::string line;
        std::cout << "Enter the next tank index of the hoist and how much time it would take (as numbers) separated with space, or simply press enter if there are no new tanks to visit:";
        std::getline(std::cin, line);
        if (line.empty()) {
            break;
        }
        std::istringstream stream(line);
        while (!(stream >> from >> time)) { // if the extraction fails
            std::cout << "Invalid input. Please enter two numbers separated by a space or a new-line to break: ";
            std::getline(std::cin, line);
            if (line.empty()) {
                goto after;
            }
            stream.clear();
            stream.str(line);
        }
        transitions.push_back({ from, time });
    }
after:
    std::cout << "Enter processing times of the route separated by space: ";
    std::vector<int> sequence = get_vector();

    return { id, transitions, sequence };
}

std::vector<std::vector<int>> get_matrix(size_t size) {
    if (size == 1) {
        std::cout << "As there is only one route, please enter the time for it to go back to its initial position (loop back):\n";
    }
    else {
        std::cout << "Please give in the transition times matrix (" << size << "x" << size << "), this represents the time it takes to switch between routes:\n";
    }
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    for (size_t i = 0; i < size; ++i) {
        std::cout << "Enter transition " << i + 1 << "-row (" << size << "):\n";
        matrix[i] = get_vector();
    }
    return matrix;
}

template<typename T>
T get_and_confirm(const std::function<T()>& parser, const std::string& prompt) {
    T result;
    std::string confirmation;
    do {
        std::cout << prompt;
        result = parser();
        std::cout << "You entered: " << result << ". Is this correct? (yes/no) ";
        std::getline(std::cin, confirmation);
        std::transform(confirmation.begin(), confirmation.end(), confirmation.begin(), ::tolower);
        while (confirmation != "yes" && confirmation != "y" && confirmation != "no" && confirmation != "n") {
            std::cout << "Invalid input. Please enter \"yes\" or \"no\": ";
            std::getline(std::cin, confirmation);
            std::transform(confirmation.begin(), confirmation.end(), confirmation.begin(), ::tolower);
        }
    } while (confirmation == "no" || confirmation == "n");

    return result;
}


template <typename T>
T get_non_zero_number() {
    T number;
    while (true) {
        std::cin >> number;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number: ";
        }
        else if (number == 0) {
            std::cout << "Number must be non-zero. Please enter a number: ";
        }
        else {
            return number;
        }
    }
}




RobotOrchestra get_robot_orchestra() {
    std::vector<shared_tank> tanks;
    std::vector<schedule> schedules;
    size_t num_tanks, num_schedules;

    num_tanks = get_and_confirm<size_t>(get_number<size_t>, "Please enter the number of shared tanks: ");

    for (size_t i = 0; i < num_tanks; ++i) {
        std::cout << "For shared tank " << i + 1 << ",\n";
        tanks.push_back(get_and_confirm<shared_tank>(get_shared_tank, " "));
    }


    num_schedules = num_tanks+1;
    std::cout << "The number of robot hoists is: "<<num_schedules<<std::endl;

    for (size_t i = 0; i < num_schedules; ++i) {
        std::cout << "For hoist number " << i + 1 << "\n";
        std::vector<Route> routes;
        std::vector<std::vector<int>> mTransTimes;
        size_t num_routes;

        num_routes = get_and_confirm<size_t>(get_non_zero_number<size_t>, "Please give in the number of routes for the current hoist:");

        for (size_t j = 0; j < num_routes; ++j) {
            std::cout << "Enter route "<< j+1<<" and keep in mind that the indices of tanks are relative to each robot, meaning it starts by zero:\n";
            routes.push_back(get_and_confirm<Route>(get_route, ""));
        }

        size_t matrix_size;
        //std::cout << "Enter the size of the transition times matrix: ";
        matrix_size = num_routes;//get_and_confirm<size_t>(get_number<size_t>, "Enter a number: ");
        mTransTimes = get_and_confirm<std::vector<std::vector<int>>>(std::bind(get_matrix, matrix_size), "Enter a matrix: ");

        schedules.push_back({ routes, mTransTimes });
    }

    return { tanks, schedules };
}

int main() {

    //matrix<series> stackMatricesDiagonally(vector<matrix<series>> vms);
    //gd i2gd(int a);
    try {
        //we have one shared tank because 2-1=1 (number of robots-1=number shared tanks)
                   /* std::vector<schedule> robots = {
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

                    RobotOrchestra d(tanks, robots);*/
                    RobotOrchestra d = get_robot_orchestra();

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

                            cout << "The complete A matrix is shown below: " << endl;
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

