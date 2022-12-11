#include "ElectroPlatingLines.h"
#include <variant>
#include <iostream>
#include <functional>
#include <cinttypes>
#include <cstddef>

//code from https://stackoverflow.com/a/55113454/10000823
class hash_tuple {
	template<class T>
	struct component {
		const T& value;
		component(const T& value) : value(value) {}
		uintmax_t operator,(uintmax_t n) const {
			n ^= std::hash<T>()(value);
			n ^= n << (sizeof(uintmax_t) * 4 - 1);
			return n ^ std::hash<uintmax_t>()(n);
		}
	};

public:
	template<class Tuple>
	size_t operator()(const Tuple& tuple) const {
		return std::hash<uintmax_t>()(
			std::apply([](const auto& ... xs) { return (component(xs), ..., 0); }, tuple));
	}
};
enum class IO {
	INPUT,
	OUTPUT
};

namespace std {
	template<>
	struct hash<IO> {
		size_t operator()(const IO& io) const {
			if (io == IO::INPUT) {
				return std::hash<int>()(0);
			}
			else {
				return std::hash<int>()(1);
			}
		}
	};
}

class Tank {
public:
	int index;
	IO io;
};

namespace std {
	template<>
	struct hash<Tank> {
		size_t operator()(const Tank& tank) const {
			return std::hash<int>()(tank.index) ^ std::hash<IO>()(tank.io);
		}
	};
}

/*The Transition class is defined as a variant that can either be an IO object or a Tank object. The IO class is an enumeration with two possible values: INPUT and OUTPUT. The Tank class has two public members: an index integer, and an io variable of type IO.*/
typedef std::variant<IO, Tank> Transition;
std::ostream& operator<<(std::ostream& os, const IO& io) {
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

std::ostream& operator<<(std::ostream& os, const Tank& tank) {
	// Print the index and io values of the Tank object.
	os << "Tank (index: " << tank.index << ", io: " << tank.io << ")";
	// Return the std::ostream object to allow chaining.
	return os;
}
std::ostream& operator<<(std::ostream& os, const Transition& a) {
	if (auto* io = std::get_if<IO>(&a)) {
		// a is an IO value
		//os << static_cast<int>(*io);
		os << static_cast<IO>(*io);
	}
	else if (auto* tank = std::get_if<Tank>(&a)) {
		// a is a Tank value
		os << static_cast<Tank>(*tank);
	}
	else {
		// a is not a valid value
		os << "Invalid value";
	}
	return os;
}
std::ostream& operator<<(std::ostream& os, const std::tuple<Transition, Transition>& t) {
	os << "(" << std::get<0>(t) << ", " << std::get<1>(t) << ")";
	return os;
}

std::ostream& operator<<(std::ostream& out, const std::vector<tuple<Transition, Transition>>& vec) {
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
/*This is the signature for a C++ function named getMatrixIndex that takes two arguments: a std::variant object named t and an int named matrixlength. The std::variant object t can hold either a value of type IO or a value of type Tank. The function returns an int.*/
int getMatrixIndex(Transition t,int matrixlength) {

	// Use the std::visit function to access the IO enum class based on the value of the container variant
	return 
	std::visit([&matrixlength](auto&& arg)->int {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, IO>) {
			switch (arg) {
			case IO::INPUT:
				return 0;
				break;
			case IO::OUTPUT:
				return 2*matrixlength - 1;
				break;
			}
		//	std::cout << "IO enum class" << std::endl;
		}
		else if constexpr (std::is_same_v<T, Tank>) {
			// Use the Tank class here
			int a=1;
			a += arg.index * 2;
			return arg.io == IO::OUTPUT ? a + 1 : a;
		}
		}, t);
	return 0;
}
enum class lr {
	left,
	right
};

/*
The t_convert function is a helper function for the fromTankNumber2Transition function.It takes an integer, an enumeration value of type lr, and an integer representing the total number of tanks, and returns a Transition object.
It converts the integer from the Schedule into the transition that is to be fired.
*/
Transition t_convert(int p, lr lr, int out) {
	if (p == 0)
		return IO::INPUT;
	if (p == out)
		return IO::OUTPUT;
	return Tank{ p - 1, lr == lr::left ? IO::INPUT : IO::OUTPUT };
}

auto fromTankNumber2Transition(tuple<int, int> tup, int out) -> tuple<Transition, Transition> {
	int left, right;
	Transition a, b;
	std::tie(left, right) = tup;

	// Call the function and assign the result to the variants a and b
	a = t_convert(left, lr::left, out);
	b = t_convert(right, lr::right, out);

	// Return the tuple containing the two variants
	return std::make_tuple(a, b);
}

vector<tuple<Transition, Transition>> vec_fromTankNumber2Transition(vector<tuple<int, int>> inputs) {
	int linputs = inputs.size();
	vector<tuple<Transition, Transition>> transitionTuples;
	transform(inputs.begin(), inputs.end(), std::back_inserter(transitionTuples), std::bind(&fromTankNumber2Transition,std::placeholders::_1, linputs));
	return transitionTuples;
}

std::ostream& operator<<(std::ostream& os, const std::tuple<int, int>& t)
{
	// Extract the elements of the tuple
	int x, y;
	std::tie(x, y) = t;

	// Print the tuple to the output stream
	os << "(" << x << ", " << y << ")";

	return os;
}
/*This function uses a lambda expression to define an anonymous function that takes a tuple of two integers by reference and returns the maximum value of the two integers. It uses the std::max function to calculate the maximum value, and the std::get function to access the elements of the tuple.*/
auto max_tuple = [](const std::tuple<int, int>& tup) -> int {
	return std::max(std::get<0>(tup), std::get<1>(tup));
};

// Helper function to convert a tuple of Transition objects into a tuple of two integers
tuple<int, int> fromTransition2Index(tuple<Transition, Transition> p, int inpLen) {
	Transition left, right;
	std::tie(left, right) = p;
	int a = getMatrixIndex(right, inpLen);
	int b = getMatrixIndex(left, inpLen);
	return std::make_tuple(a, b);
}

// Helper function to convert a vector of Transition objects into a vector of tuple<int, int> objects
vector<tuple<int, int>> vec_fromTransition2Index(vector<tuple<Transition, Transition>> inputs) {
	int inpLen = inputs.size();
	vector<tuple<int, int>> tranconverted;
	transform(inputs.begin(), inputs.end(), std::back_inserter(tranconverted), [&inpLen](auto p) {
		return fromTransition2Index(p, inpLen);
		});
	return tranconverted;
}


/*
//the returned matrix gives us the graph
where the order of transitions are:
strctInpts.route (strctInpts.route deposit,output deposit)_i output
strctInpts.transportationTime is the time of the hoist with a piece
strctInpts.movementTime is the time of the hoist without a piece
strctInpts.processingTime is the time a piece has to be on the tank
*/
matrix<series> A_matrix(info_products strctInpts) {
	// Check if strctInpts.route sizes match
	assert(strctInpts.route.size() - 1 == strctInpts.processingTime.size() && "The vector containing the time of pieces in containers doesn't match with the input");
	assert(strctInpts.route.size() == strctInpts.movementTime.size() && "The length of the vector containing the time of the hoist without a piece must be equal to the length of the input");

	// Compute length of strctInpts.route
	auto transitionTuples = vec_fromTankNumber2Transition(strctInpts.route);
	int inpLen = strctInpts.route.size();
	// Get max tank
	int maxTankNumber;
	{
		std::vector<int> maxTankValues;
		std::transform(strctInpts.route.begin(), strctInpts.route.end(), std::back_inserter(maxTankValues), max_tuple);
		maxTankNumber = *std::max_element(maxTankValues.begin(), maxTankValues.end());
	}
	// Convert Transition objects to tuple of ints
	auto vecIntTransitionConverted = vec_fromTransition2Index(transitionTuples);

	// Initialize matrix mtxA
	//2 * inpLen is the number of transitions.
	matrix<series> mtxA(2 * inpLen, 2 * inpLen);

	// Set elements related to the movement of the hoist while grabbing a piece
	for (int i = 0; i < inpLen; i++) {
		mtxA(get<1>(vecIntTransitionConverted[(i + 1) % inpLen]), get<0>(vecIntTransitionConverted[i])) = gd(0, strctInpts.transportationTime[i]);
	}

	// Set elements related to the movement of the hoist without grabbing a piece
	for (int i = 0; i < inpLen - 1; i++) {
		std::apply(mtxA, vecIntTransitionConverted[i]) = gd(0, strctInpts.movementTime[i]);
	}

	// Set element related to the movement from the last tank to the first tank
	std::apply(mtxA, vecIntTransitionConverted.back()) = gd(1, strctInpts.movementTime[inpLen - 1]);

	// Set elements related to the processing times
	for (int i = 0;i < inpLen - 1;i++) {
		auto tupTransitionProcess = fromTankNumber2Transition(std::tie(get<1>(strctInpts.route[i]), get<1>(strctInpts.route[i])), maxTankNumber);
		auto tupIntS=fromTransition2Index(tupTransitionProcess,inpLen);
		bool flag = !std::any_of(strctInpts.route.begin(), strctInpts.route.begin() + 1 + i, [i, &strctInpts](auto pair) { return get<1>(strctInpts.route[i]) == get<0>(pair); });
		//cout << "flag:" << flag << ";tupTransitionProcess:" << tupTransitionProcess <<";tupIntS:" << tupIntS <<";mtxA(tupIntS):" << std::apply(mtxA, tupIntS) << endl;
		std::apply(mtxA, tupIntS) = std::apply(mtxA, tupIntS)+gd(flag, strctInpts.processingTime[i]);
	}
	//Note that from matrix mtxA we can construct the TEG, except for strctInpts.route(tupIntS) and output(tupIntS). In order to find out strctInpts and outputs we need matrices B and C
	return mtxA;
}

matrix<series> B_matrix(int T) {
		
	//int T = strctInpts.route.size()/2  - 1;
	int t = 2 * T + 2;

	matrix<series> B(t, 1);

	B(0, 0) = gd(0, 1);

	return B;
}

matrix<series> C_matrix(int T) {
	int t = 2 * T + 2;

	matrix<series> C(1, t);

	C(0, t - 1) = gd(0, 1);

	return C;
}
/*void myfunction(matrix<series> i) {  // function:
	int idx = &value - &data[0];
	cout << "mtxA = " << i;
}*/
