#pragma once
#include "etvo.h"
#include <tuple>
#include <queue>
#include <unordered_set>
#include <variant>
#include <unordered_map>
#include "tuple_hash.h"
using namespace std; //this imports the standard library, every cout and vector
using namespace etvo; //just a library :P every matrix and series. all elements from the library

enum class IO {
    INPUT,
    OUTPUT
};
class TransitionTank {
public:
    int index;
    IO io;
};
namespace std {
    template <>
    struct hash<IO> {
        size_t operator()(const IO& io) const
        {
            if (io == IO::INPUT) {
                return std::hash<int>()(0);
            }
            else {
                return std::hash<int>()(1);
            }
        }
    };
}


namespace std {
    template <>
    struct hash<TransitionTank> {
        size_t operator()(const TransitionTank& tank) const
        {
            return (std::hash<int>()(tank.index) << 1) & std::hash<IO>()(tank.io);
        }
    };
}

/*The Transition class is defined as a variant that can either be an IO object
 * or a TransitionTank object. The IO class is an enumeration with two possible values:
 * INPUT and OUTPUT. The TransitionTank class has two public members: an index integer,
 * and an io variable of type IO.*/
typedef std::variant<IO, TransitionTank> Transition;
class TransitionMode {
public:
    int index;
    Transition transition;
};
inline bool operator==(const TransitionTank& lhs, const TransitionTank& rhs);
inline bool operator==(const Transition& lhs, const Transition& rhs);
std::ostream& operator<<(std::ostream& os, const IO& io);
std::ostream& operator<<(std::ostream& os, const TransitionTank& tank);
std::ostream& operator<<(std::ostream& os, const Transition& a);
std::ostream& operator<<(std::ostream& out, const TransitionMode& mode);
template <typename... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t);
template <typename T>
std::ostream& operator<<(std::ostream& out,
    const std::vector<T>& vec);
template <typename Key, typename Value>
std::ostream& operator<<(std::ostream& out,
    const std::unordered_map<Key, Value>& map);
template <typename T>
std::ostream& operator<<(std::ostream& out,
    const std::queue<T>& q);
template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p);
/*This is the signature for a C++ function named getMatrixIndex that takes two
* arguments: a std::variant object named tand an int named matrixlength.The
* std::variant object t can hold either a value of type IO or a value of type
* TransitionTank.The function returns an int.*/
int getMatrixIndex(Transition t, int matrixlength);
enum class lr {
    left,
    right
};
Transition t_convert(int p, lr lr, int out);
gd i2gd(int a);
#define IS_EVEN(n) ((n) % 2 == 0)

matrix<series> stackMatricesDiagonally(vector<matrix<series>> vms);

template <typename T>
std::vector<T> create_vector(T element, int length);

class Mode {
public:
    // Constructor
    //let initialTank=0 be input tank and initialTank=numberOfTanks+1 the output Tank
    //A modeArray is a vector of tuples where the first element is the Tank as a number, where 0 is the input tank and numberOfTanks+1 is the output tank.
    //And the second element is either the transportation or movement time.
    Mode(int initialTank, std::vector<std::tuple<int, int>> modeArray, std::vector<int> processingTimes, int numberOfTanks);
    int getInitialTank() const;
    void setInitialTank(int value);
    vector<tuple<int, int>> getModeArray() const;
    void setModeArray(vector<tuple<int, int>> value);
    vector<int> getProcessingTimes() const;
    void setProcessingTimes(vector<int> value);
    void update();
    // Validation
    void validate();
    vector<queue<int>> processingTimesQueues;
    vector<int> finalContainersTokenCount;
    vector<int> countainerRequirements;
    std::unordered_map<tuple<Transition, Transition>, int> A0_matrix;
    int lastTank;
private:
    void loopOverTupleWithTanksAndTimes();
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

class Schedule {
public:
    // Constructor
    /*Schedule(vector<Mode> vMode, vector<vector<int> > mTransTimes)
        : vMode(vMode)
        , mTransTimes(mTransTimes) {};*/
        /*the mTransTimes is the matrix that represents the switching times between the modes.*/
    Schedule(std::vector<std::tuple<int, std::vector<std::tuple<int, int>>, std::vector<int>>> modes, vector<vector<int>> mTransTimes);

    // Attributes
    vector<Mode> vMode;
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
    matrix<series> etvoAMatrix(vector<Transition> indexT, unordered_map<tuple<Transition, Transition>, int> Amap);

    vector<int> mV2iV(matrix<series> i);

    template <typename T>
    vector<T> readIndexes(vector<int>& L, vector<T>& B);
    template <typename T>
    void writeIndexes(vector<int>& L, vector<T>& B, vector<T> values);
    int getMaxValue(const std::vector<std::tuple<int, std::vector<std::tuple<int, int> >, std::vector<int> > >& vec);
    bool isSquare(vector<vector<int>>& matrix);
};