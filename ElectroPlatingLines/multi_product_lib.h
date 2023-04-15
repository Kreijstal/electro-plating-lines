#pragma once
#include "etvo.h"
#include <tuple>
#include <queue>
#include <unordered_set>
#include <variant>
#include <unordered_map>
#include "tuple_hash.h"
using namespace std;
using namespace etvo; 

enum class IO {
    INPUT,
    OUTPUT
};

class TransitionTank { //this defines any tanks transitions EXCEPT FOR INPUT AND OUTPUT TANKS (transitions in the middle so to say).
public:
    int index;  //in cpp terms (starts with 0)
    IO io;   //input and output of the TANKs transition NOT the whole schedule
};

namespace std {
    template <>
    struct hash<TransitionTank> {
        size_t operator()(const TransitionTank& tank) const
        {
            return (std::hash<size_t>()(tank.index) << 1) & std::hash<IO>()(tank.io);
        }
    };
}

/*The Transition class is defined as a variant that can either be an IO object
 * or a TransitionTank object. The IO class is an enumeration with two possible values:
 * INPUT and OUTPUT. The TransitionTank class has two public members: an index integer,
 * and an io variable of type IO.*/
typedef std::variant<IO, TransitionTank> Transition; //it shows if Transition is IO or middle transition

class TransitionMode { //this defines a transition mode and specifies which transition in which mode we are talking about
public:
    int index;
    Transition transition;
};

inline bool operator==(const TransitionTank& lhs, const TransitionTank& rhs); //checks if these two are equal
inline bool operator==(const Transition& lhs, const Transition& rhs);

std::ostream& operator<<(std::ostream& os, const IO& io); //print all the classes implemented before to see if they work these 4 lines
std::ostream& operator<<(std::ostream& os, const TransitionTank& tank);
std::ostream& operator<<(std::ostream& os, const Transition& a);
std::ostream& operator<<(std::ostream& out, const TransitionMode& mode);


template <typename... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t)
{
    os << "(";
    std::apply([&os](const auto&... args) {
        ((os << args << ", "), ...);
        }, t);
    os << "\b\b)";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& out,
    const std::vector<T>& vec)
{
    out << "[";
    bool first = true;
    for (const auto& item : vec) {
        if (!first) {
            out << ", ";
        }
        out << item;
        first = false;
    }
    out << "] ";
    return out;
}

template <typename Key, typename Value>
std::ostream& operator<<(std::ostream& out,
    const std::unordered_map<Key, Value>& map)
{
    out << "{" << std::endl;
    for (const auto& item : map) {
        out << "  " << item.first << ": " << item.second << std::endl;
    }
    out << "}" << std::endl;
    return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out,
    const std::queue<T>& q)
{
    std::queue<T> temp(q);
    out << "Queue: [";
    bool first = true;
    while (!temp.empty()) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << temp.front();
        temp.pop();
    }
    out << "]";
    return out;
}
template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p)
{
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

/*This is the signature for a C++ function named getMatrixIndex that takes two
* arguments: a std::variant object named tand an int named matrixlength.The
* std::variant object t can hold either a value of type IO or a value of type
* TransitionTank.The function returns an int.*/
size_t getMatrixIndex(Transition t, size_t matrixlength);
enum class take_deposit {
    take,
    deposit
};
/**
 * @brief Converts an integer to a Transition object, which can be either an IO enum or a TransitionTank struct.
 *
 * This function takes three parameters: an integer tank_ID representing the numeric input (meaning the tank number),
 * an enum take_or_deposit indicating, whether it's an input or output transition, and an integer out/last_tank. It maps the numeric input to a Transition
 * object based on the tank and whether it's an input or output transition. The function returns a Transition
 * object that can be either an IO (input/output) or a TransitionTank (with a tank index and an IO status).
 *
 * @param tank_ID Integer representing the numeric input. #(where the transitions begin)#
 * @param lr Enum of type take_or_deposit, where take_or_deposit::take represents an input transition and lr::deposit represents an output transition.
 * @param out Integer representing the number of output tank.
 * @return Transition object which can be either an IO enum (IO::INPUT or IO::OUTPUT) or a TransitionTank struct.
 */
Transition t_convert(size_t tank_ID, take_deposit take_or_deposit, size_t last_tank);
/**
 * @brief Converts an integer to a gd struct.
 *
 * tank_ID = is the tank where the hoist begins. (begin_tank)
 * lr = left/right (left=input,right=output)
 * This function takes an integer as input and creates a gd struct with the provided integer value and a default
 * gamma value of zero. The main purpose of this function is to simplify the creation of gd objects by allowing
 * the user to provide only the integer value, while the function takes care of setting the default gamma value.
 *
 * @param a Integer value to be used as the second component of the gd struct.
 * @return gd struct with the gamma value set to zero and the provided integer value as the second component.
 */
gd i2gd(int a);
#define IS_EVEN(n) ((n) % 2 == 0)

/**
 * @brief Stacks a vector of matrices diagonally into a single matrix.
 *
 * This function takes a vector of matrices and stacks them diagonally into a single matrix. The resulting matrix
 * has its dimensions computed as the sum of the rows and columns of all the input matrices. Each input matrix
 * is placed diagonally in the resulting matrix, with no overlapping between matrices.
 *
 * @param vms Vector of matrices with elements of type series.
 * @return A matrix of type series with the input matrices stacked diagonally.
 */
matrix<series> stackMatricesDiagonally(vector<matrix<series>> vms);

template <typename T>
std::vector<T> create_vector(T element, int length) {
    // create an empty vector of size length
    std::vector<T> vec(length);
    // fill the vector with element
    std::fill(vec.begin(), vec.end(), element);
    return vec;
}


 /**
  * @class RobotMode
  * @brief Represents a mode in a Switched Linear Dual Inequalities (SLDI) model based on a Timed Event Graph (TEG).
  *
  * This class takes information from a P-TEG, such as the tanks used, the transportation/movement time, and the
  * processing time, and exposes the A0 matrix through an unordered_map attribute. It also provides information
  * about tokens that remained on the TEG as a vector of queues.
  * 
  *  This class models a processing mode in a system with multiple tanks.
 * It keeps track of transportation and movement times between tanks,
 * processing times for each tank, and the initial and final states
 * of the system.
 *
 * The class provides methods to set and retrieve various attributes,
 * such as initial tank, mode array, and processing times. It also
 * performs validation on the input data and calculates the minimum
 * number of tokens required in each container during the process.
  */
class RobotMode {
public:
    /**
 * @brief Constructs a RobotMode object.
 *
 * @param initialTank An integer representing the initial tank.
 *        0 indicates the input tank, and numberOfTanks + 1 indicates the output tank.
 * @param modeArray A vector of tuples, where the first element is the tank number
 *        (0 for the input tank and numberOfTanks + 1 for the output tank),
 *        and the second element is either the transportation or movement time.
 * @param processingTimes A vector of integers representing the processing times
 *        for each tank.
 * @param numberOfTanks An integer representing the total number of tanks,
 *        excluding the input and output tanks.
 *
 * @throws std::invalid_argument If modeArray has an even length, or if the
 *         processing times vector has an invalid length, or if the tank number
 *         is greater than the specified number of tanks.
 */
    RobotMode(int initialTank, std::vector<std::tuple<int, int>> modeArray, std::vector<int> processingTimes, int numberOfTanks);
    int getInitialTank() const;
    void setInitialTank(int value);
    vector<tuple<int, int>> getModeArray() const;
    void setModeArray(vector<tuple<int, int>> value);
    vector<int> getProcessingTimes() const;
    void setProcessingTimes(vector<int> value);
    void update();
    /**
     * @brief Validates the mode array and processing times to ensure they follow the expected constraints.
     */
    void validate();
    vector<queue<int>> processingTimesQueues;
    vector<int> finalContainersTokenCount;
    vector<int> countainerRequirements;
    std::unordered_map<tuple<Transition, Transition>, int> A0_matrix;
    int lastTank;
private:
    /**
 * @brief Processes the modeArray and updates various data structures within the class.
 *
 * The loopOverTupleWithTanksAndTimes() function iterates over the modeArray, which is a vector
 * of tuples where the first element is the tank number (0 for the input tank and
 * numOfTanks + 1 for the output tank), and the second element is either the transportation
 * or movement time. The function updates the processingTimesQueues, A0_matrix,
 * finalContainersTokenCount, and countainerRequirements data structures.
 *
 * The function distinguishes transportation operations from movement operations by checking the index of the current tuple in the modeArray. If the index is even, the operation is considered a transportation operation; if the index is odd, the operation is considered a movement operation.
 * 
 * 
 * The high-level algorithm is as follows:
 * 1. Iterate over the tuples in the modeArray.
 * 2. For each tuple, check if the index is even or odd.
 *    - If the index is even (transportation operation):
 *      - Update the A0_matrix with the transportation time.
 *      - Adjust the finalContainersTokenCount vector based on the transportation operation.
 *      - Update the processing times for the corresponding tanks.
 *    - If the index is odd (movement operation), update the A0_matrix with the movement time.
 * 3. Calculate the minimum number of tokens required in each container based on the
 *    finalContainersTokenCount vector.
 *
 * The function assumes that the input data is valid and throws exceptions if certain
 * conditions are not met, such as an invalid number of tanks, an invalid length of
 * modeArray, or if processing times are not found for a given operation.
 */
    void loopOverTupleWithTanksAndTimes();
    void processTransportation(unsigned int index, unsigned int previousTank, unsigned int currentTank, queue<int>& q);
    void processMovement(unsigned int index, unsigned int previousTank, unsigned int currentTank);
    void updateContainerRequirements();
    int numOfTanks;
    int initialTank;

    vector<tuple<int, int>> modeArray;
    vector<int> processingTimes;
};

matrix<series> createVectorInETVO(int length);
matrix<series> intVector2MaxPlus(vector<int> i);
matrix<series> getXfromMatrix(matrix<series>& ms);
void printSize(matrix<series>& a);
void printMatrix(matrix<series>& a);

typedef std::tuple<int, std::vector<std::tuple<int, int>>, std::vector<int>> mode;
class RobotSchedule {
public:
    // Constructor
    
        /*the mTransTimes is the matrix that represents the switching times between the modes.*/
    RobotSchedule(std::vector<mode> modes, vector<vector<int>> mTransTimes);

    // Attributes
    vector<RobotMode> vMode;
    vector<vector<int>> mTransTimes;
    //vector<matrix<series>> B_matrices;
    std::unordered_map<tuple<Transition, Transition>, series> A0_matrices;
    int numOfTanks;
    vector<tuple<int, int>> A1data;
    //vector<matrix<series>> C_matrices;
    // Methods
    //vector<matrix<series>> getAMatrices() { return A_matrices; };
    //int method2(int x);
    //
    //std::string method3(std::string str);
    vector<int> initialVector();
    vector<int> addB(vector<int> in, int mode);
    std::tuple<matrix<series>, matrix<series>> getBigAandBMatrix(vector<int> schedule);
    std::unordered_map<tuple<Transition, Transition>, int> getA1(int prevMode, int currMode);
    vector<int> multiplyWithA1(vector<int> transitions, int prevMode, int currMode);

    vector<int> multiplyWithAstarMatrix(vector<int> transitions, int mode);

private:
    std::unordered_map<tuple<int, int>, std::unordered_map<tuple<Transition, Transition>, int>> A1cache;
   
   

    template <typename T>
    vector<T> readIndexes(vector<int>& L, vector<T>& B);
    template <typename T>
    void writeIndexes(vector<int>& L, vector<T>& B, vector<T> values);
    int getMaxValue(const std::vector<mode>& vec);
    
};
/**
* @brief Converts a matrix of series objects to a standard vector of integers.
*
* This function iterates through the given matrix of series objects, extracting the
* integer values from each series object and inserting them into a standard vector.
* If a series object cannot be converted to an integer, the function throws a runtime_error.
*
* @param i A matrix<series> object representing the input matrix of series objects.
* @return A vector<int> containing the integer values extracted from the input matrix.
* @throws runtime_error If a series object cannot be converted to an integer.
*/
vector<int> etvoVector2stdVector(matrix<series> i);
/**
* @brief Converts a map of transition pairs and integers to a matrix of series objects.
*
* This function creates a matrix of series objects using the given map of transition pairs
* and integers, and a vector of transitions for indexing. It uses a lambda function to find
* the index of a transition in the indexT vector.
*
* @param indexT A vector of Transition objects, used for indexing.
* @param Amap An unordered_map containing pairs of Transition objects as keys and integers as values.
* @return A matrix<series> object, where the elements are series objects created from the input map.
* @throws std::runtime_error If a Transition couldn't be found in the indexT vector.
*/
matrix<series> etvoAMatrix(vector<Transition> indexT, unordered_map<tuple<Transition, Transition>, int> Amap);
/**
 * @brief Checks if the given matrix is square.
 *
 * This function checks if the given matrix has an equal number of rows and columns.
 *
 * @param matrix A reference to a vector of vector of integers representing the matrix.
 * @return A boolean value indicating if the matrix is square or not.
 */
bool isSquare(vector<vector<int>>& matrix);
template <typename T>
vector<T> RobotSchedule::readIndexes(vector<int>& L, vector<T>& B)
{
    //cout << "readIndexes, indexes:" << L << " vector:" << B << endl;
    vector<T> result;
    for (auto index : L) {
        result.push_back(B[index]);
    }
    return result;
}
template <typename T>
void RobotSchedule::writeIndexes(vector<int>& L, vector<T>& B, vector<T> values)
{
    int index = 0;
    for (auto& i : L) {
        B[i] = values[index++];
    }
}