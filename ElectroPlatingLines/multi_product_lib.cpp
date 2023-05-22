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
#include "multi_product_lib.h"


inline bool operator==(const ProcessingStationTransition& lhs, const ProcessingStationTransition& rhs)
{
    return lhs.index == rhs.index && lhs.io == rhs.io;
}

inline bool operator==(const TankTransitionRelation& lhs, const TankTransitionRelation& rhs)
{
    if (lhs.index() != rhs.index()) {
        return false;// if the variants hold different types, they are not equal
    }

    if (lhs.index() == 0) {
        return std::get<IO>(lhs) == std::get<IO>(rhs);
    }
    else {
        return std::get<ProcessingStationTransition>(lhs) == std::get<ProcessingStationTransition>(rhs);
    }
}
inline bool operator==(const Transition& lhs, const Transition& rhs)
{
    return lhs.repetitionIndex == rhs.repetitionIndex && lhs.transition == rhs.transition;
}

std::ostream& operator<<(std::ostream& os, const IO& io)
{
    // Switch on the IO value and print the corresponding string representation.
    switch (io) {
    case IO::INPUT:
        os << "INPUT";
        break;
    case IO::OUTPUT:
        os << "OUTPUT";
        break;
    }
    // Return the std::ostream object to allow chaining.
    return os;
}

std::ostream& operator<<(std::ostream& os, const ProcessingStationTransition& tank)
{
    // Print the stationIndex and io values of the ProcessingStationTransition object.
    os << "Tank (index: " << tank.index << ", io: " << tank.io << ")";
    // Return the std::ostream object to allow chaining.
    return os;
}
std::ostream& operator<<(std::ostream& os, const TankTransitionRelation& a)
{
    if (auto* io = std::get_if<IO>(&a)) {
        // a is an IO value
        // os << static_cast<int>(*io);
        os << static_cast<IO>(*io);
    }
    else if (auto* tank = std::get_if<ProcessingStationTransition>(&a)) {
        // a is a ProcessingStationTransition value
        os << static_cast<ProcessingStationTransition>(*tank);
    }
    else {
        // a is not a valid value
        os << "Invalid value";
    }
    return os;
}


std::ostream& operator<<(std::ostream& out, const Transition& tr)
{
    out << "Transition: tankrep=" << tr.repetitionIndex << ", transition=" << tr.transition;
    return out;
}

std::ostream& operator<<(std::ostream& out, const TransitionMode& mode)
{
    out << "TransitionMode: index=" << mode.index << ", transition=" << mode.transition;
    return out;
}


/*std::ostream& operator<<(std::ostream& os,
  const std::tuple<Transition, Transition>& t)
  {
  os << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ")";
  return os;
  }*/

  /**
   * @brief Get the matrix stationIndex for a given Transition object and matrix length.
   *
   * @param t A std::variant object which can hold either a value of type IO or a value of type ProcessingStationTransition.
   * @param matrixlength An int representing the length of the matrix.
   * @return int The matrix stationIndex based on the given Transition object and matrix length.
   *
   * This function calculates the matrix stationIndex based on the given Transition object (t) and matrix length (matrixlength).
   * The Transition object (t) is a std::variant that can hold either a value of type IO or a value of type ProcessingStationTransition.
   */
size_t getMatrixIndex(TankTransitionRelation t, size_t matrixlength)
{
    // Use the std::visit function to access the IO enum class based on the value
    // of the container variant
    return std::visit([&matrixlength](auto&& arg) -> size_t {
        using T = std::decay_t<decltype(arg)>;
        if
            constexpr (std::is_same_v<T, IO>)
        {
            switch (arg) {
            case IO::INPUT:
                return 0;
                break;
            case IO::OUTPUT:
                return matrixlength - 1;
                break;
            }
            //	std::cout << "IO enum class" << std::endl;
        }
        else if
            constexpr (std::is_same_v<T, ProcessingStationTransition>)
        {
            // Use the ProcessingStationTransition class here
            size_t a = 1;
            a += arg.index * 2;
            return arg.io == IO::OUTPUT ? a + 1 : a;
        }
        }, t);
    return 0;
}

size_t getMatrixIndex(Transition t, size_t matrixlength)
{
    return getMatrixIndex(t.transition, matrixlength) + (t.repetitionIndex * matrixlength);
}


TankTransitionRelation
t_convert(size_t tank_ID, take_deposit take_or_deposit, size_t out)
{
    if (tank_ID == 0)
        return IO::INPUT;
    if (tank_ID == out)
        return IO::OUTPUT;
    return ProcessingStationTransition{ tank_ID - 1, take_or_deposit == take_deposit::take ? IO::OUTPUT : IO::INPUT };
}

gd i2gd(int a)
{
    return gd(0, a);
}
#define IS_EVEN(n) ((n) % 2 == 0)


matrix<series> stackMatricesDiagonally(vector<matrix<series>> vms) {
    //cout << "stackingMatrices diagonally!" << endl;
    // Get the sizes of each matrix as pairs of columns and rows
    vector<std::pair<int, int>> sizes;
    std::transform(vms.begin(), vms.end(), std::back_inserter(sizes),
        [](const matrix<series>& ms) {
            return std::make_pair(ms.GetRow(), ms.GetColumn());
        });
    // Compute the total size as the sum of pairs
    std::pair<int, int> totalSize = std::accumulate(sizes.begin(), sizes.end(),
        std::make_pair(0, 0),
        [](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
            return std::make_pair(p1.first + p2.first, p1.second + p2.second);
        });
    //cout << "total size is:" << totalSize << endl;
    // Create a new matrix with the total size
    matrix<series> A(totalSize.first, totalSize.second);
    // Iterate over each matrix and its stationIndex in the vector using a traditional for loop
    int i = 0; // Row offset for stacking matrices
    int j = 0; // Column offset for stacking matrices
    for (std::size_t idx = 0; idx < vms.size(); ++idx) {
        const auto& ms = vms[idx];
        // Iterate over each row and column of the current matrix 
        for (std::size_t k = 0; k < ms.GetRow(); k++) {
            for (std::size_t l = 0; l < ms.GetColumn(); l++) {
                // Copy the element from the current matrix to the new matrix 
                A(i + k, j + l) = ms(k, l);
            }
        }
        // Update the offsets based on the size of the current matrix 
        i += ms.GetRow();
        j += ms.GetColumn();
    }
    return A;
}


// Constructor
//let initialTank=0 be input tank and initialTank=numberOfTanks+1 the output Tank
//A routeWithTimes is a vector of tuples where the first element is the Tank as a number, where 0 is the input tank and numberOfTanks+1 is the output tank.
//And the second element is either the transportation or movement time.
RobotRoute::RobotRoute(size_t initialTank, vector<tuple<size_t, int> > modeArray, vector<int> processingTimes, size_t numberOfTanks)
    :// initialTank(initialTank)
    processingTimes(processingTimes)
    , numOfTanks(numberOfTanks)
{
    
    //cout << "hello z" << endl;
    countainerRequirements.resize(numOfTanks);
    finalContainersTokenCount.resize(numOfTanks);

    vector<TankTransitionRelation> routeWithTimesLoc;
    //push first transition
    auto initialTransLoc = t_convert(initialTank, take_deposit::take, numOfTanks + 1);
    routeWithTimesLoc.push_back(initialTransLoc);
    initialTransition = Transition{ 0,initialTransLoc };
    for (size_t i = 0; i < modeArray.size(); ++i) {
        size_t tank_ID;
        int time;
        std::tie(tank_ID, time) = modeArray[i];
        if (tank_ID > numOfTanks + 1) {
            throw std::invalid_argument("numOfTanks must be greater than the actual tanks given on modeArray");
        }
        TankTransitionRelation transitionLoc = t_convert(
            tank_ID,
            i % 2 ? take_deposit::take : take_deposit::deposit,
            numOfTanks + 1
        );
        size_t countTransition = std::count(routeWithTimesLoc.begin(), routeWithTimesLoc.end(), transitionLoc);
        //if (countTransitionsInTransitionLocVec(routeWithTimesLoc, transition)>1) {
        //    throw std::runtime_error("Using the same Tank more than once in a single route, is currently not supported.");
        //}
        routeWithTimesLoc.push_back(transitionLoc);
        routeWithTimes.emplace_back(Transition{ countTransition ,transitionLoc }, time);
        numMatrixRepeats = std::max(numMatrixRepeats, countTransition);
        if (i == modeArray.size() - 1) {
            lastTransition = Transition{ countTransition ,transitionLoc };
        }

    }
    numMatrixRepeats++;//The count was previously on the data seen so far, since we added one, then we add one here, as well.
    std::fill(countainerRequirements.begin(), countainerRequirements.end(), 0);
    std::fill(finalContainersTokenCount.begin(), finalContainersTokenCount.end(), 0);
    loopOverTupleWithTanksAndTimes();   
    validate();
}

vector<int> RobotRoute::getProcessingTimes() const { return processingTimes; }
void RobotRoute::setProcessingTimes(vector<int> value)
{
    processingTimes = value;
    update();
}
void RobotRoute::update()
{
    //initialTank
    //routeWithTimes
    validate();
}
// Validation
void RobotRoute::validate()
{
    if (routeWithTimes.size() % 2 == 0) {
        throw std::invalid_argument("modeArray must have an odd length");
    }
    int lastCount = 0;
    for (size_t i = 0; i < routeWithTimes.size(); i++) {
        if (std::get<0>(routeWithTimes[i]).transition == IO::OUTPUT)
            lastCount++;
    }
    //cout << (processingTimes.size() + lastCount) * 2 - 1 << "=" << routeWithTimes.size() << endl;
    if ((processingTimes.size() + lastCount) * 2 - 1 != routeWithTimes.size()) {
        throw std::invalid_argument("processingTimes must have a valid length, equal to half the number of transitions, excluding output transitions");
    }
}

void RobotRoute::updateContainerRequirements() {
    for (size_t i = 0; i < finalContainersTokenCount.size(); i++) {
        countainerRequirements[i] = -min(-countainerRequirements[i], finalContainersTokenCount[i]);
    }
}

void RobotRoute::processMovement(size_t index, Transition previousTransition, Transition currentTransition) {
    // Update A0_matrix with movement time
    //this is a movement
    auto ind = std::make_tuple(currentTransition, previousTransition);
    // cout << "movement " << ind << endl;
    A0_matrix[ind] =
        std::get<1>(routeWithTimes[index]);
}


void RobotRoute::loopOverTupleWithTanksAndTimes() {
    processingTimesQueues.clear();
    processingTimesQueues.resize(numOfTanks);
    queue<int> q;
    for (auto& i : processingTimes) {
        q.push(i);
    }
    for (size_t processingStation = 0; processingStation < routeWithTimes.size(); ++processingStation) {
        Transition previousTank;
        Transition currentTank = std::get<0>(routeWithTimes[processingStation]);

        if (processingStation == 0) {
            previousTank = initialTransition;
        }
        else {
            previousTank = std::get<0>(routeWithTimes[processingStation - 1]);
        }


        if (IS_EVEN(processingStation)) { // If even, it is a transportation operation
            processTransportation(processingStation, previousTank, currentTank, q);
        }
        else { // If odd, it is a movement operation
            processMovement(processingStation, previousTank, currentTank);
        }
    }
    updateContainerRequirements();
}

void RobotRoute::processTransportation(size_t stationIndex, Transition previousTransition, Transition currentTransition, queue<int>& q) {
    //We know stationIndex is always even
    // Update A0_matrix with transportation time
    auto s = std::make_tuple(currentTransition, previousTransition);
    //cout << "set transport" << s << "thingy" << routeWithTimes[stationIndex] << endl;
    A0_matrix[s] =
        std::get<1>(routeWithTimes[stationIndex]);

    if (previousTransition.transition == IO::OUTPUT)
        throw std::invalid_argument("You can't take a piece from the output deposit");

    if (currentTransition.transition == IO::INPUT)
        throw std::invalid_argument("You can't put a piece on the input deposit");

    if (previousTransition.transition != IO::INPUT) {
        auto tankindex = std::get<ProcessingStationTransition>(previousTransition.transition).index;
        finalContainersTokenCount[tankindex]--;
        int popped;
        if (processingTimesQueues[tankindex].empty()) {
            // We don't assume the object already exists
            // throw runtime_error("Queue is empty");
            cout << "processing time not found." << endl;
        }
        else {
            //Get the next value from the processing time vector
            popped = processingTimesQueues[tankindex].front();
            processingTimesQueues[tankindex].pop();
            auto s = std::make_tuple(Transition{ previousTransition.repetitionIndex,ProcessingStationTransition{ tankindex ,IO::OUTPUT} }, Transition{ previousTransition.repetitionIndex,
                ProcessingStationTransition{ tankindex ,IO::INPUT } });
            if (A0_matrix.find(s) != A0_matrix.end()) {
                /*cout << "previousTransition" << previousTransition << "g" << A0_matrix[s] << "WHAT" << popped << "  ," << s << endl;
                throw std::runtime_error("Attempting to set an already existing place (this is an error with this library itself)");*/
                A0_matrix[s] = max(A0_matrix[s], popped);
            }
            //cout << "set processing" << s<<endl;
            A0_matrix[s] = popped;
        }
    }

    if (currentTransition.transition != IO::OUTPUT) {
        auto currdumindex = std::get<ProcessingStationTransition>(currentTransition.transition).index;
        finalContainersTokenCount[currdumindex]++;
        if (q.empty()) {
            //cout << "repetitionIndex:"<<stationIndex<<",previousTransition:"<<previousTransition<< ",currentTransition:"<<currentTransition<<endl;
            throw runtime_error("q Queue was empty, this probably means you didn't write your Route correctly, or there were not enough processing times given for this route.");
        }
        processingTimesQueues[currdumindex].push(q.front());
        q.pop();
    }
}

matrix<series> getXfromMatrix(matrix<series>& ms) {
    size_t size = ms.GetRow();
    matrix<series> out(size, 1);
    //out(0, 0) = i2gd(0);
    return out;
}
void printSize(matrix<series>& a) {
    std::cout << "(" << a.GetRow() << "," << a.GetColumn() << ")" << std::endl;
}

void printMatrix(matrix<series>& a) {
    for (size_t i = 0;i < a.GetRow();i++) {
        for (size_t j = 0;j < a.GetColumn();j++) {
            series& t = a(i, j);
            std::cout << "[" << i << "," << j << "] = P:" << t.getP() << ",Q:" << t.getQ() << ",R:" << t.getR() << std::endl;
        }
    }
}



matrix<series> createVectorInETVO(int length) {
    matrix<series> out(length, 1);
    return out;
}
matrix<series> intVector2MaxPlus(vector<int> i)
{
    matrix<series> out(i.size(), 1);
    for (size_t index = 0; index < i.size(); ++index) {
        out(index, 0) = i2gd(i[index]);
    }
    return out;
}

matrix<series> etvoAMatrix(vector<Transition> indexT, unordered_map<tuple<Transition, Transition>, int> Amap)
{
    auto t2i = [indexT](Transition t) -> int {
        auto it = find(indexT.begin(), indexT.end(), t);
        if (it != indexT.end()) {
            return distance(indexT.begin(), it);
        }
        else {
            throw std::runtime_error("Transition couldn't be found in indexT");
        }
    };
    matrix<series> A(indexT.size(), indexT.size());
    for (auto& [key, value] : Amap) {
        A(t2i(std::get<0>(key)), t2i(std::get<1>(key))) = i2gd(value);
    }
    return A;
}

vector<int> etvoVector2stdVector(matrix<series> i)
{
    vector<int> a;//out(1, i.size());
    int size = i.GetRow();
    for (int index = 0; index < size; ++index) {
        //cout << "access here:"<<stationIndex<<" size:"<<size<<endl;
        auto s = i(index, 0);
        if (!s.isPoly() || s.getQ().getn() != 1 || s.getQ().getpol(0).getg() != 0)throw runtime_error("series can't be converted to int");
        a.push_back(s.getQ().getpol(0).getd());
    }
    return a;
}


bool isSquare(vector<vector<int>>& matrix)
{
    size_t n = matrix.size();
    for (size_t i = 0; i < n; i++) {
        if (matrix[i].size() != n) {
            return false;
        }
    }
    return true;
}

vector<Transition> getAllTransitionsFromHashmap(std::unordered_map<tuple<Transition, Transition>, int>& map) {
    vector<Transition> allValues; //list of used transitions

    unordered_set<Transition> seen;
    for (auto& [key, value] : map) {
        if (!seen.count(std::get<0>(key))) {
            allValues.push_back(std::get<0>(key));
            seen.insert(std::get<0>(key));
        }
        if (!seen.count(std::get<1>(key))) {
            allValues.push_back(std::get<1>(key));
            seen.insert(std::get<1>(key));
        }
    }
    return allValues;
}
//std::vector<std::tuple<size_t, std::vector<std::tuple<size_t, int> >, std::vector<int> > >
size_t RobotSchedule::getMaxValue(const std::vector<Route>& vec)
{
    size_t maxValue = 0;

    for (const auto& tuple : vec) {
        // Get the second element of the tuple, which is a vector<tuple<int, int>>
        const auto& innerVec = std::get<1>(tuple);

        // Find the maximum value in the inner vector
        size_t maxInnerValue = 0;
        for (const auto& innerTuple : innerVec) {
            maxInnerValue = std::max(maxInnerValue, std::get<0>(innerTuple));
        }

        // Update the maximum value if necessary
        maxValue = std::max(maxValue, maxInnerValue);
    }

    return maxValue;
}


std::unordered_map<tuple<Transition, Transition>, int> RobotSchedule::getA1(int prevMode, int currMode) {
    auto tup = std::make_tuple(prevMode, currMode);
    if (A1cache.find(tup) == A1cache.end()) {
        std::unordered_map<tuple<Transition, Transition>, int> A1_matrix;
        //in order to create A1 we need to take care of three cases
        //case 1 when going from last tank in previous Route to first tank in current Route
        auto tofrom = std::make_tuple(vMode[currMode].initialTransition,
            vMode[prevMode].lastTransition);
        // cout << "from last tank 1st Route to 1st tank second Route:" <<                 tofrom << endl;
        A1_matrix[tofrom] = (mTransTimes[currMode][prevMode]);
        //case 3 pass through

        vector<Transition> allValues = getAllTransitionsFromHashmap(A1_matrix);

        for (const auto& av : allValues) {//diagonal
            auto s = std::make_tuple(av, av);
            if (A1_matrix.find(s) == A1_matrix.end()) {

                A1_matrix[s] = 0;
            }
            else {
                A1_matrix[s] = max(A1_matrix[s], 0);
            }

        }

        //case 2 passing pieces that are "leftovers"
        //how to pass leftover pieces, let's think about this for a moment.
        //modes have "inputs" (countainerRequirements) and outputs (finalContainersTokenCount)
        //the length of the queue should be the same as the ouput 
        //we create a copy of the queue 
        // Copy the original vector
        //std::vector<std::queue<int>> copiedVector;
        int i = 0;
        for (const auto& q : vMode[prevMode].processingTimesQueues) {
            std::queue<int> copiedQueue = q;
            if (copiedQueue.size() > 1)
                //Only 1 leftover for now TODO: allow more leftovers
                throw std::runtime_error("Output of previous Mode has more than 1 piece in a tank");
            else if (copiedQueue.size() == 1) {
                auto s = std::make_tuple(Transition{ 0,t_convert(i + 1, take_deposit::take, numOfTanks + 1) }, Transition{ 0,
                    t_convert(i + 1, take_deposit::deposit, numOfTanks + 1) });
                //cout << "From To " <<s << endl;
                if (A1_matrix.find(s) == A1_matrix.end()) {

                    A1_matrix[s] = (copiedQueue.front());
                }
                else {
                    A1_matrix[s] = max(A1_matrix[s], copiedQueue.front());
                }
            }
            //copiedVector.push_back(copiedQueue);
            i++;
        }
        A1cache[tup] = A1_matrix;
        return A1_matrix;
    }
    else {
        return A1cache[tup];
    }
}
/* vector<int> RobotSchedule::multiplyWithA1(vector<int> transitions, int prevMode, int currMode) {
     //Here we need to take queues that are not empty!
     //define the a1 matrix;
     std::unordered_map<tuple<Transition, Transition>, int> A1_matrix;
     //in order to create A1 we need to take care of three cases
     //case 1 when going from last tank in previous Route to first tank in current Route
     A1_matrix[std::make_tuple(t_convert(vMode[currMode].getInitialTank(), take_deposit::deposit, numOfTanks + 1),
         t_convert(vMode[prevMode].lastTank, take_deposit::take, numOfTanks + 1))] = (mTransTimes[currMode][prevMode]);
     //case 3 pass through
     vector<Transition> allValues; //list of used transitions

     unordered_set<Transition> seen;
     for (auto& [key, value] : A1_matrix) {
         if (!seen.count(std::get<0>(key))) {
             allValues.push_back(std::get<0>(key));
             seen.insert(std::get<0>(key));
         }
         if (!seen.count(std::get<1>(key))) {
             allValues.push_back(std::get<1>(key));
             seen.insert(std::get<1>(key));
         }
     }

     for (const auto& av : allValues) {
         A1_matrix[std::make_tuple(av, av)] = (0);
     }

     //case 2 passing pieces that are "leftovers"
     //how to pass leftover pieces, let's think about this for a moment.
     //modes have "inputs" (countainerRequirements) and outputs (finalContainersTokenCount)
     //I have no idea how to handle outputs of more than 1
     //the length of the queue should be the same as the ouput
     //we create a copy of the queue
     // Copy the original vector
     //std::vector<std::queue<int>> copiedVector;
     int i = 0;
     for (const auto& q : vMode[prevMode].processingTimesQueues) {
         std::queue<int> copiedQueue = q;
         if (copiedQueue.size() > 1)
             throw std::runtime_error("Output of previous Mode has more than 1 piece in a tank");
         else if (copiedQueue.size() == 1) {
             auto s = std::make_tuple(t_convert(i + 1, take_deposit::deposit, numOfTanks + 1),
                 t_convert(i + 1, take_deposit::take, numOfTanks + 1));
             if (A1_matrix.find(s) == A1_matrix.end()) {

                 A1_matrix[s] = (copiedQueue.front());
             }
             else {
                 A1_matrix[s] = max(A1_matrix[s], copiedQueue.front());
             }
         }
         //copiedVector.push_back(copiedQueue);
         i++;
     }
     //A1_matrix

     for (auto& [key, value] : A1_matrix) {
         if (!seen.count(std::get<0>(key))) {
             allValues.push_back(std::get<0>(key));
             seen.insert(std::get<0>(key));
         }
         if (!seen.count(std::get<1>(key))) {
             allValues.push_back(std::get<1>(key));
             seen.insert(std::get<1>(key));
         }
     }

     int matrixlength = (numOfTanks + 1) * 2;
     vector<int> allIndex; //list of matrix indices of all used transitions
     std::transform(allValues.begin(), allValues.end(), std::back_inserter(allIndex), [matrixlength](const auto& t) {
         return getMatrixIndex(t, matrixlength);
         });
     matrix<series> A1 = etvoAMatrix(allValues, A1_matrix);
     cout << A1 << endl;
     vector<int> vNTransitions = readIndexes(allIndex, transitions);
     cout << allIndex << endl;
     cout << intVector2MaxPlus(vNTransitions) << endl;
     writeIndexes<int>(allIndex, transitions, etvoVector2stdVector(A1 * intVector2MaxPlus(vNTransitions)));
     return transitions;
 }
 */
 /*vector<int> RobotSchedule::multiplyWithAstarMatrix(vector<int> transitions, int Route)
 {
     //optimization try to figure out the pertaining transitions..
     //only makes sense to do square matrix.
     int matrixlength = (numOfTanks + 1) * 2;
     if (matrixlength != transitions.size()) {
         std::string errorMessage = "number of transitions does not match with Schedule, it should have at least " + std::to_string(matrixlength) + " transitions. Transitions vector is instead " + std::to_string(transitions.size()) + ".";
         throw std::invalid_argument(errorMessage);
     }
     RobotRoute mobj = vMode[Route];
     vector<Transition> allValues; //list of used transitions
     {
         unordered_set<Transition> seen;
         for (auto& [key, value] : mobj.A0_matrix) {
             if (!seen.count(std::get<0>(key))) {
                 allValues.push_back(std::get<0>(key));
                 seen.insert(std::get<0>(key));
             }
             if (!seen.count(std::get<1>(key))) {
                 allValues.push_back(std::get<1>(key));
                 seen.insert(std::get<1>(key));
             }
         }
     }

     matrix<series> A = etvoAMatrix(allValues, mobj.A0_matrix);
     //cout << A << endl;
     A = A.star();
     //cout<<"starred" << A << endl;
     vector<int> allIndex; //list of matrix indices of all used transitions
     std::transform(allValues.begin(), allValues.end(), std::back_inserter(allIndex), [matrixlength](const auto& t) {
         return getMatrixIndex(t, matrixlength);
         });
     vector<int> vNTransitions = readIndexes(allIndex, transitions);
     //cout << etvoVector2stdVector(A * intVector2MaxPlus(vNTransitions)) << endl;
     writeIndexes<int>(allIndex, transitions, etvoVector2stdVector(A * intVector2MaxPlus(vNTransitions)));
     return transitions;
 }
 */

vector<int> RobotSchedule::initialVector()
{
    vector<int> a;
    a.resize((numOfTanks + 1) * 2);
    //initially the initial vector starts with the initial times, so the time is set to -Infinity
    std::fill(a.begin(), a.end(), _infinit);
    return a;
}
vector<int> RobotSchedule::addB(vector<int> in, int mode) {
    //we do the dumb case and just edit the input to transition to fire at the output transition
        //int i=getMatrixIndex(t_convert(vMode[Route].getInitialTank(), take_or_deposit::right, numOfTanks + 1), (numOfTanks + 1)*2);
    in[0] = max(in[0], 0);
    return in;
}


RobotSchedule::RobotSchedule(std::vector<Route> modes, std::vector<std::vector<int>> mTransTimes)
    : mTransTimes(mTransTimes)
{
    if (!(isSquare(mTransTimes) && mTransTimes.size() == modes.size()))
        throw std::invalid_argument("mTransTimes should be a square matrix.");
    int n = getMaxValue(modes) - 1;
    numOfTanks = n;
    std::transform(modes.begin(), modes.end(), std::back_inserter(vMode), [n](const auto& t) {
        size_t initialTank;
        std::vector<std::tuple<size_t, int> > modeArray;
        std::vector<int> processingTimes;
        std::tie(initialTank, modeArray, processingTimes) = t;
        //cout << "RobotRoute creation" << endl;
        return RobotRoute(initialTank, modeArray, processingTimes, n);
        });
        
    maxNumMatrixRepeats = 0;

    for (const RobotRoute& mode : vMode) {
        maxNumMatrixRepeats = std::max(maxNumMatrixRepeats, mode.numMatrixRepeats);
    }
   
};

std::tuple<matrix<series>, matrix<series>> RobotSchedule::getBigAandBMatrix(std::vector<int> schedule) {
    size_t size = schedule.size();
    size_t lenf = (numOfTanks + 1) * 2;
    size_t len = maxNumMatrixRepeats * lenf;
    matrix<series> A(len * size, len * size);
    //cout << "matrix sixe of " << len * size << "^2" << endl;
    //cout << "BEF RETURN" << endl;
    for (size_t k, i = 0;i < size;i++) {
        //cout << "loop internal" << i << endl;
        k = (i + 1) % size;
        //if k=0 gamma=1
        auto t2i = [len, i, lenf](Transition t) -> size_t {
            return getMatrixIndex(t, lenf) + (i * len);
        };
        for (auto& [key, value] : vMode[schedule[i]].A0_matrix) {
            //cout << "accessing [" << t2i(std::get<0>(key)) << "," << t2i(std::get<1>(key)) << "] " << endl;
            auto s = std::make_tuple(t2i(std::get<0>(key)), t2i(std::get<1>(key)));
            //cout << "aa " << key << " , " << s << endl;
            std::apply(A, s) = i2gd(value);
        }
        //cout << "loop internalA" << i << endl;
        if (k != 0) {
            /*for (int I = 0;I < len;I++) {
                //cout << "accessing [" << I + (k * len) << "," << I + (i * len) <<"] I:"<<I<<" k:"<<k<<" len:"<<len << endl;
                A(I + (k * len), I + (i * len)) = i2gd(0);
            }*/
            for (auto& [key, value] : getA1(schedule[i], schedule[k])) {
                //cout << "accessing [" << t2i(std::get<0>(key)) + len << "," << t2i(std::get<1>(key)) << "] " << endl;
                A(t2i(std::get<0>(key)) + len, t2i(std::get<1>(key))) = i2gd(value);
                //cout << "weird" << endl;
            }
            //cout << "over" << endl;

        }
        else {
            //cout << "loop internalB" << i << endl;
            /*for (int I = 0;I < len;I++) {
                A(I, I + (i * len)) = gd(1, 0);
            }*/
            for (auto& [key, value] : getA1(schedule[i], schedule[k])) {
                // cout << "KEY:" << key <<" t2i(KEY)=("<< t2i(std::get<0>(key))<<","<< t2i(std::get<1>(key)) << ")" << ", converted key: (" << (t2i(std::get<0>(key)) + len) % (len * size) << "," << t2i(std::get<1>(key)) << ") val:" << value << endl;
                auto s = std::make_tuple((t2i(std::get<0>(key)) + len) % (len * size), t2i(std::get<1>(key)));
                std::apply(A, s) = gd(1, value);
            }

        }

    }
    //cout << "BEF RETURN" << endl;
    matrix<series> B = createVectorInETVO(len);
    //cout << "BEF RETURN" << endl;
    B(0, 0) = i2gd(0);
    //cout << "BEF RETURN" << endl;
    //cout <<" printining B here:" << stackMatricesDiagonally(create_vector(B, size)) << endl;
    return std::make_tuple(A, stackMatricesDiagonally(create_vector(B, size)));

}