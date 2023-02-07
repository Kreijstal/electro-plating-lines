#include <iostream>
#include <exception>
#include "etvo.h"
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <tuple>
#include <variant>
#include <algorithm>
#include <functional>
#include <cinttypes>
#include <cstddef>
#include <queue>
#include <unordered_set>
using namespace std;  //this imports the standard library, every cout and vector 
using namespace etvo; //just a library :P every matrix and series. all elements from the library
// code from https://stackoverflow.com/a/55113454/10000823
namespace std {
    namespace {

        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     http://stackoverflow.com/questions/4948780

        template <class T>
        inline void
            hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
            static void apply(size_t& seed, Tuple const& tuple)
            {
                HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
                hash_combine(seed, std::get<Index>(tuple));
            }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple, 0>
        {
            static void apply(size_t& seed, Tuple const& tuple)
            {
                hash_combine(seed, std::get<0>(tuple));
            }
        };
    }

    template <typename... TT>
    struct hash<std::tuple<TT...>>
    {
        size_t operator()(std::tuple<TT...> const& tt) const
        {
            size_t seed = 0;
            HashValueImpl<std::tuple<TT...>>::apply(seed, tt);
            return seed;
        }
    };
}
enum class IO
{
    INPUT,
    OUTPUT
};

namespace std {
    template <>
    struct hash<IO>
    {
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

class TransitionTank
{
public:
    int index;
    IO io;
};

namespace std {
    template <>
    struct hash<TransitionTank>
    {
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




inline bool operator==(const TransitionTank& lhs, const TransitionTank& rhs)
{
    return lhs.index == rhs.index && lhs.io == rhs.io;
}

inline bool operator==(const Transition& lhs, const Transition& rhs)
{
    if (lhs.index() != rhs.index()) {
        return false;
    }

    if (lhs.index() == 0) {
        return std::get<IO>(lhs) == std::get<IO>(rhs);
    }
    else {
        return std::get<TransitionTank>(lhs) == std::get<TransitionTank>(rhs);
    }
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

std::ostream& operator<<(std::ostream& os, const TransitionTank& tank)
{
    // Print the index and io values of the TransitionTank object.
    os << "Tank (index: " << tank.index << ", io: " << tank.io << ")";
    // Return the std::ostream object to allow chaining.
    return os;
}
std::ostream& operator<<(std::ostream& os, const Transition& a)
{
    if (auto* io = std::get_if<IO>(&a)) {
        // a is an IO value
        // os << static_cast<int>(*io);
        os << static_cast<IO>(*io);
    }
    else if (auto* tank = std::get_if<TransitionTank>(&a)) {
        // a is a TransitionTank value
        os << static_cast<TransitionTank>(*tank);
    }
    else {
        // a is not a valid value
        os << "Invalid value";
    }
    return os;
}
std::ostream& operator<<(std::ostream& out, const TransitionMode& mode) {
    out << "TransitionMode: index=" << mode.index << ", transition=" << mode.transition;
    return out;
}
/*std::ostream& operator<<(std::ostream& os,
    const std::tuple<Transition, Transition>& t)
{
    os << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ")";
    return os;
}*/
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
    out << "Vector: [";
    bool first = true;
    for (const auto& item : vec) {
        if (!first) {
            out << ", ";
        }
        out << item;
        first = false;
    }
    out << "]" << std::endl;
    return out;
}

template <typename Key, typename Value>
std::ostream& operator<<(std::ostream& out,
    const std::unordered_map<Key, Value>& map)
{
    out << "Unordered map: {" << std::endl;
    for (const auto& item : map) {
        out << "  " << item.first << ": " << item.second << std::endl;
    }
    out << "}" << std::endl;
    return out;
}
/*This is the signature for a C++ function named getMatrixIndex that takes two
 * arguments: a std::variant object named t and an int named matrixlength. The
 * std::variant object t can hold either a value of type IO or a value of type
 * TransitionTank. The function returns an int.*/
int
getMatrixIndex(Transition t, int matrixlength)
{
    // Use the std::visit function to access the IO enum class based on the value
    // of the container variant
    return std::visit([&matrixlength](auto&& arg) -> int {
        using T = std::decay_t<decltype(arg)>;
        if
            constexpr (std::is_same_v<T, IO>)
        {
            switch (arg) {
            case IO::INPUT:
                return 0;
                break;
            case IO::OUTPUT:
                return 2 * matrixlength - 1;
                break;
            }
            //	std::cout << "IO enum class" << std::endl;
        }
        else if
            constexpr (std::is_same_v<T, TransitionTank>)
        {
            // Use the TransitionTank class here
            int a = 1;
            a += arg.index * 2;
            return arg.io == IO::OUTPUT ? a + 1 : a;
        }
        }, t);
    return 0;
}
enum class lr
{
    left,
    right
};
/*typedef struct {
	int initialTank;
	vector<tuple<int, int>> modeArray;
	vector<int>	procesingTimes;
} mode;*/
Transition
t_convert(int p, lr lr, int out)
{
    if (p == 0)
        return IO::INPUT;
    if (p == out)
        return IO::OUTPUT;
    return TransitionTank{ p - 1, lr == lr::left ? IO::INPUT : IO::OUTPUT };
}
#define IS_EVEN(n) ((n) % 2 == 0)

class Mode {
public:
    // Constructor
    //let initialTank=0 be input tank and initialTank=numberOfTanks+1 the output Tank
    Mode(int initialTank, vector<tuple<int, int>> modeArray, vector<int> processingTimes, int numberOfTanks) : initialTank(initialTank), modeArray(modeArray), processingTimes(processingTimes), numOfTanks(numberOfTanks){
        countainerRequirements.resize(numOfTanks);
        finalContainersTokenCount.resize(numOfTanks);
        processingTimesQueue.clear();
        processingTimesQueue.resize(numberOfTanks);
        queue<int> q;
        for (auto& i : processingTimes) {
            q.push(i);
        }
        std::fill(countainerRequirements.begin(), countainerRequirements.end(), 0);
        std::fill(finalContainersTokenCount.begin(), finalContainersTokenCount.end(), 0);
        for (int index = 0; index < modeArray.size(); ++index) {
            int bef;
            int aft= std::get<0>(modeArray[index]);
            if (index==0) {
                bef = initialTank;
            }
            else {
                bef = std::get<0>(modeArray[index-1]);
            }
            if (bef > numOfTanks + 1 || aft > numOfTanks + 1) {
                throw std::invalid_argument("numOfTanks can't be smaller than the actual tanks given on modeArray");
            }
            //in,out
            A0_matrix[std::make_tuple(t_convert(aft, IS_EVEN(index)?lr::left: lr::right, numOfTanks + 1),t_convert(bef, IS_EVEN(index) ? lr::right: lr::left, numOfTanks + 1))] = std::get<1>(modeArray[index]);
            if (IS_EVEN(index)) {
                if (bef == numOfTanks + 1)
                    throw std::invalid_argument("You can't take a piece from the output deposit");
                if (aft == 0)
                    throw std::invalid_argument("You can't put a piece on the input deposit");
                if (bef != 0) {
		    finalContainersTokenCount[bef - 1]--;
		    int popped;
                    if (processingTimesQueue[bef - 1].empty()) {
			//we assume object already exists
                        //throw runtime_error("Queue is empty");
			cout << "processing time not found, assuming zero."<<endl;
			popped=0;
                    }else {
                    popped = processingTimesQueue[bef - 1].front();
			    processingTimesQueue[bef - 1].pop();
		    }
			    A0_matrix[std::make_tuple(t_convert(bef, lr::right, numOfTanks + 1),t_convert(bef, lr::left, numOfTanks + 1))] = popped;
                }
                if (aft != numOfTanks + 1) {
                    finalContainersTokenCount[aft - 1]++;
                    if (q.empty()) {
                        throw runtime_error("q Queue was empty");
                    }
                    processingTimesQueue[aft - 1].push(q.front());
                    q.pop();
                }
                for (int i = 0; i < finalContainersTokenCount.size(); i++) {
                    countainerRequirements[i] = -min(-countainerRequirements[i], finalContainersTokenCount[i]);
                }
            }
        }

        validate();
    }

    // Getters and setters
    int getInitialTank() const { return initialTank; }
    void setInitialTank(int value) { initialTank = value; }

    vector<tuple<int, int>> getModeArray() const { return modeArray; }
    void setModeArray(vector<tuple<int, int>> value) {
        modeArray = value;
        update();
    }

    vector<int> getProcessingTimes() const { return processingTimes; }
    void setProcessingTimes(vector<int> value) {
        processingTimes = value;
        update();   
    }
    void update() {
        //initialTank
        //modeArray
        validate();
    }
    // Validation
    void validate() {
        if (modeArray.size() % 2 == 0) {
            throw std::invalid_argument("modeArray must have an uneven length");
        }
        int lastCount = 0;
        for (int i = 0; i < modeArray.size(); i++) {
            if (std::get<0>(modeArray[i]) == numOfTanks + 1)
                lastCount++;
        }
        cout << (processingTimes.size()+ lastCount) * 2 - 1 <<"="<< modeArray.size() << endl;
        if ((processingTimes.size() + lastCount) * 2 - 1 != modeArray.size()) {
            throw std::invalid_argument("processingTimes must have a valid length (there is 1/2 as many processing times as transitions)");
        }
    }

    matrix<series> getA0matrix() {
    }
    vector<queue<int>> processingTimesQueue;
    vector<int> finalContainersTokenCount;
    vector<int> countainerRequirements;
    std::unordered_map<tuple<Transition, Transition>, int> A0_matrix;
private:

    int numOfTanks;
    int initialTank;
    vector<tuple<int, int>> modeArray;
    vector<int> processingTimes;
    
};


class Schedule {
public:
    // Constructor
    Schedule(vector<Mode> vMode, vector<vector<int>> mTransTimes) :vMode(vMode), mTransTimes(mTransTimes) {};
    Schedule(std::vector<std::tuple<int, std::vector<std::tuple<int, int>>, std::vector<int>>> modes, vector<vector<int>> mTransTimes) : mTransTimes(mTransTimes) {
    if(!(isSquare(mTransTimes)&&mTransTimes.size()==modes.size()))
        throw std::invalid_argument("processingTimes must have a valid length (there is 1/2 as many processing times as transitions)");
    int n = getMaxValue(modes);
    numOfModes=n;
		std::transform(modes.begin(), modes.end(), std::back_inserter(vMode), [n](const auto& t) {
		int initialTank;
		std::vector<std::tuple<int, int>> modeArray;
		std::vector<int> processingTimes;

		std::tie(initialTank, modeArray, processingTimes) = t;
		return Mode(initialTank, modeArray, processingTimes, n);
		});
	    };

	    // Attributes
	    vector<Mode> vMode;
	    vector<vector<int>> mTransTimes;
	    //vector<matrix<series>> B_matrices;
	    std::unordered_map<tuple<Transition, Transition>, series> A0_matrices;
	    int numOfModes;
	    vector<tuple<int,int>> A1data;
	    //vector<matrix<series>> C_matrices;
	    // Methods
	    //vector<matrix<series>> getAMatrices() { return A_matrices; };
	    //int method2(int x);
	    // 
	    //std::string method3(std::string str);
	    vector<int> initialVector(){
		    vector<int> a;
		    a.resize((numOfModes+1)*2);
		    //initially the initial vector starts with the initial times, so the time is set to -Infinity
		    std::fill(a.begin(), a.end(), _infinit);
	    }
	    vector<int> multiplyWithAstarMatrix(vector<int> transitions,int mode){
		    //optimization try to figure out the pertaining transitions..
		    //only makes sense to do square matrix.
		    if((numOfModes+1)*2==transitions.size){
			throw std::invalid_argument("number of transitions does not match with Schedule, it should have at least ${(numOfModes+1)*2}");
		    }
		    Mode mobj=vMode[mode];
		    vector<Transition> allValues;
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



	    }
	private:
	    template <typename T>
		    vector<T> readIndexes(vector<int>& L, vector<T>& B) {
			    vector<T> result;
			    for (auto index : L) {
				    result.push_back(B[index]);
			    }
			    return result;
		    }
	    template <typename T>
		    void writeIndexes(vector<int>& L, vector<T>& B, vector<T>& values) {
			    int index = 0;
			    for (auto& i : L) {
				    B[i] = values[index++];
			    }
		    }
	    int getMaxValue(const std::vector<std::tuple<int, std::vector<std::tuple<int, int>>, std::vector<int>>>& vec) {
		    int maxValue = 0;

		for (const auto& tuple : vec) {
		    // Get the second element of the tuple, which is a vector<tuple<int, int>>
		    const auto& innerVec = std::get<1>(tuple);

		    // Find the maximum value in the inner vector
		    int maxInnerValue = 0;
		    for (const auto& innerTuple : innerVec) {
			maxInnerValue = std::max(maxInnerValue, std::get<0>(innerTuple));
		    }

		    // Update the maximum value if necessary
		    maxValue = std::max(maxValue, maxInnerValue);
		}

		return maxValue;
	    }
	    bool isSquare(vector<vector<int>> matrix) {
		int n = matrix.size();
		for (int i = 0; i < n; i++) {
		    if (matrix[i].size() != n) {
			return false;
		    }
		}
		return true;
	    }

	    //vector<matrix<series>> A_matrices;
	};
	gd i2gd(int a) {
	    return gd(0, a);
	}

	int main() {
	    std::vector<std::tuple<int, int>> modeArray = { {2, 1},{0,2},{1, 3} };
	    std::vector<int> processingTimes = { 10 };
	    Mode mode(1, modeArray, processingTimes, 1);
	    cout << mode.A0_matrix << endl;
	    cout << mode.countainerRequirements << endl;
	    cout << mode.finalContainersTokenCount << endl;
	    //matrix<poly> A(2,2);
	    //A(0, 0) = i2gd(1);
	    //A(0, 1) = i2gd(2);
	    //A(1, 0) = i2gd(3);  
	    //A(1, 1) = i2gd(4);
	    //cout << A << endl;
		cout << "hello world" << endl;
		}
