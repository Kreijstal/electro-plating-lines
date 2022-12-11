#include "ElectroPlatingLines.h"
#include <variant>
#include <iostream>
#include <functional>

enum class IO {
	INPUT,
	OUTPUT
};

class Tank {
public:
	int index;
	IO io;
};

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

void printVectorPinput(const std::vector<tuple<Transition, Transition>>& vec) {
	std::cout << "Vector: [";
	bool first = true;
	for (const auto& item : vec) {
		if (!first) {
			std::cout << ", ";
		}
		std::cout << item;
		first = false;
	}
	std::cout << "]" << std::endl;
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
The t_convert function is a helper function for the transition_convert function.It takes an integer, an enumeration value of type lr, and an integer representing the total number of tanks, and returns a Transition object.
It converts the integer from the Schedule into the transition that is to be fired.
*/
Transition t_convert(int p, lr lr, int out) {
	if (p == 0)
		return IO::INPUT;
	if (p == out)
		return IO::OUTPUT;
	return Tank{ p - 1, lr == lr::left ? IO::INPUT : IO::OUTPUT };
}

auto transition_convert(tuple<int, int> tup, int out) -> tuple<Transition, Transition> {
	int left, right;
	Transition a, b;
	std::tie(left, right) = tup;

	// Call the function and assign the result to the variants a and b
	a = t_convert(left, lr::left, out);
	b = t_convert(right, lr::right, out);

	// Return the tuple containing the two variants
	return std::tie(a, b);
}

vector<tuple<Transition, Transition>> vec_transition_convert(vector<tuple<int, int>> inputs) {
	int linputs = inputs.size();
	vector<tuple<Transition, Transition>> tranconverted;
	transform(inputs.begin(), inputs.end(), std::back_inserter(tranconverted), std::bind(&transition_convert,std::placeholders::_1, linputs));
	return tranconverted;
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

/*
//the returned matrix gives us the graph
where the order of transitions are:
input (input deposit,output deposit)_i output
taup is the time of the hoist with a piece
tau is the time of the hoist without a piece
l is the time a piece has to be on the tank
*/
matrix<series> A_matrix(info_products inputs) {
	
	vector<tuple<int,int>> input=inputs.route;
	vector<int> taup=inputs.transportationTime;
	vector<int> tau =inputs.movementTime;
	vector<int> l   =inputs.processingTime;

    // Check if input sizes match
    assert(input.size() - 1 == l.size() && "The vector containing the time of pieces in containers doesn't match with the input");
    assert(input.size() == tau.size() && "The length of the vector containing the time of the hoist without a piece must be equal to the length of the input");
	
	// Compute length of input
	auto transitions = vec_transition_convert(input);
	int inpLen = input.size();

	//Get max tank
	std::vector<int> maxvals;
	std::transform(input.begin(), input.end(), std::back_inserter(maxvals), max_tuple);
	// Use std::max_element to get the maximum value in the maxVals vector
	auto maxTank = *std::max_element(maxvals.begin(), maxvals.end());
	
	/*The fromTransition2Index variable is a lambda function that takes a Transition object and returns a tuple of two integers. It converts a tuple of Transition objects into a tuple of two integers.
	*/
	auto fromTransition2Index = [&inpLen](auto p)->tuple<int, int> {
		Transition left, right;
		std::tie(left, right) = p;
		int a = getMatrixIndex(right, inpLen);
		int b = getMatrixIndex(left, inpLen);
		return std::tie(a, b);
	};
	auto transitionConverted = ([&fromTransition2Index](auto inputs)->vector<tuple<int, int>> {
		vector<tuple<int, int>> tranconverted;
		transform(inputs.begin(), inputs.end(), std::back_inserter(tranconverted), fromTransition2Index);
		return tranconverted;
		})(transitions);
	//int N = T + 1;
	//Number of transitions
	int t = 2 * inpLen;

	matrix<series> A(t, t);//A(to,from)
	//this for loop sets all elements in matrix A related to the movement of the hoist while grabbing a piece
	 for (int i = 0;i < inpLen;i++) {
	  	A(get<1>(transitionConverted[(i + 1)%inpLen]),  get<0>(transitionConverted[i])) = gd(0, taup[i]);
	}
	//this loop sets all elements in matrix A related to the movement of the hoist without grabbing a piece, except for the movement from tank x(T+1) to tank x'(T+1) = 0 (input tank), which is set in the following step
	for (int i = 0;i < inpLen-1;i++) {
		std::apply(A, transitionConverted[i]) = gd(0, tau[i]);
	}
	//difference here is that the place has an initial token 
	std::apply(A, transitionConverted.back()) = gd(1, tau[inpLen-1]);
	//the following part is for the processing time in the tanks, it is a bit more complicated as we need to put tokens (representing pieces initially in a tank) in the right spot, based only on the schedule
	for (int i = 0;i < inpLen - 1;i++) {
		auto transitio = transition_convert(std::tie(get<1>(input[i]), get<1>(input[i])), maxTank);
		auto s=fromTransition2Index(transitio);
		bool flag = !std::any_of(input.begin(), input.begin() + 1 + i, [i, &input](auto pair) { return get<1>(input[i]) == get<0>(pair); });
		//cout << "flag:" << flag << ";transitio:" << transitio <<";s:" << s <<";A(s):" << std::apply(A, s) << endl;
		std::apply(A, s) = std::apply(A, s)+gd(flag, l[i]);
	}
	//Note that from matrix A we can construct the TEG, except for input(s) and output(s). In order to find out inputs and outputs we need matrices B and C
	return A;
}

matrix<series> B_matrix(int T) {
		
	//int T = input.size()/2  - 1;
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
	cout << "A = " << i;
}*/
