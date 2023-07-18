#include "multi_product_lib.h"
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
    std::cout << "Enter transition times matrix (" << size << "x" << size << "), this represents the time it takes to switch between routes:\n";
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    for (size_t i = 0; i < size; ++i) {
        std::cout << "Enter transition "<<i+1<<"-row (" << size << "):\n";
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



int main() {
    while (true) {
        int numRoutes = get_and_confirm<int>([]() -> int {
            int x;
            while (!(std::cin >> x)) { // if the extraction fails
                std::cin.clear(); // clears the error state
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discards the input
                std::cout << "Invalid input. Please enter a number: ";
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // removes the '\n' character from the input buffer
            return x;
            }, "Enter the number of routes of the hoist: ");


        std::vector<Route> routes;
        for (int i = 0; i < numRoutes; i++) {
            routes.push_back(get_and_confirm<Route>(get_route, "Enter details for route " + std::to_string(i + 1) + ":\n"));
        }

        auto mmatrix = get_and_confirm<std::vector<std::vector<int>>>([&numRoutes]() { return get_matrix(numRoutes); }, "Enter details for the transition times matrix:\n");
        try {

            RobotSchedule schedule(routes, mmatrix);
            /*RobotSchedule a({
            {1, { { 3, 1 }, { 0, 2 }, { 1, 3 } }, { 10 } },
            {0, { { 1, 1 }, { 1, 2 }, { 3, 3 } }, { 10 } } ,
            {0, { { 1, 1 }, { 0, 2 }, { 2, 3 },{1,5} ,{3,6},{2,7},{3,8} }, { 10,20 } }
           // {0, { { 2, 1 }, { 2, 4 }, { 3, 3 } }, { 9 } }
            },
            
            { { 1, 2, 5},
              { 3, 4, 6},
              { 7, 8, 9} });*/
            matrix<series> A, B;
            int numRoutes;
            std::vector<int> vec;
            do {
                cout << "Enter the schedule, this is a space-separated list of numbers representing the index of the route where the hoist is going to cycle, for example \"1 2\" would mean (1 2)^w" << endl;
                vec = get_and_confirm<std::vector<int>>(get_vector, "The list of routes is: ");

                numRoutes = vec.size();
                if (numRoutes == 0) {
                    cout << "The schedule cannot be empty. Please enter at least one route." << endl;
                }
            } while (numRoutes == 0);

            std::tie(A, B) = schedule.getBigAandBMatrix(vec);

            auto c = A.star();
            cout << "now this is A:" << endl << A << endl << endl;
            //cout << "now this is A*:" << endl << c << endl << endl;

            std::vector<int> initialVector;
            do {
                cout << "Write the initial u vector numerically, this should be the same length as the schedule you entered" << endl;
                initialVector = get_and_confirm<std::vector<int>>(get_vector, "The u vector is: ");

                if (initialVector.size() != numRoutes) {
                    cout << "The u vector size must match the schedule size." << endl;
                }
            } while (initialVector.size() != numRoutes);

            cout << "value of x[0]" << c * (B * intVector2MaxPlus(initialVector)) << endl;

            break;

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            std::cerr << "Please try again.\n";
        }
    }

    return 0;
}