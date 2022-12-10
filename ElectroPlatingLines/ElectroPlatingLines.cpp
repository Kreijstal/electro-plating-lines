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
		os << "Tank " << tank->index << " (" << tank->io << ")";
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

matrix<series> A_matrix(info_products inputs) {
	//the returned matrix gives us the graph 
	//where the order of transitions are:
	//input (input deposit,output deposit)_i output
	vector<tuple<int,int>> input=inputs.route;
	vector<int> taup=inputs.transportationTime;
	vector<int> tau =inputs.movementTime;
	vector<int> l   =inputs.processingTime;
	assert((void("The vector containing the time of pieces in containers doesn't match with the input"), input.size() - 1 == l.size()));
	assert((void("The length of the vector containing the time of the hoist without a piece must be equal to the length of the input"), input.size() == tau.size()));
	//taup is the time of the hoist with a piece
	//tau is the time of the hoist without a piece
	//l is the time a piece has to be on the tank
	//input Length
	auto pinput = vec_transition_convert(input);
	int inpLen = input.size();
	auto finput = [&inpLen](auto p)->tuple<int, int> {
		Transition left, right;
		std::tie(left, right) = p;
		int a = getMatrixIndex(right, inpLen);
		int b = getMatrixIndex(left, inpLen);
		return std::tie(a, b);
	};
	auto minput = ([&finput](auto inputs)->vector<tuple<int, int>> {
		vector<tuple<int, int>> tranconverted;
		transform(inputs.begin(), inputs.end(), std::back_inserter(tranconverted), finput);
		return tranconverted;
		})(pinput);
	//int N = T + 1;
	//Number of transitions
	int t = 2 * inpLen;

	matrix<series> A(t, t);//A(to,from)
	//this for loop sets all elements in matrix A related to the movement of the hoist while grabbing a piece
	//for (int i = 0;i < inpLen;i++) {
		//input->input_1
		//output_{i-1}->input_i
		//A(2 * i + 1, 2 * i) = gd(0, taup[i]);
	//}
	 for (int i = 0;i < inpLen;i++) {
		//cout << "ayy (" << get<1>(minput[i + 1]) << "," << get<0>(minput[i]) << ") = (" << 2 * i + 1 << "," << 2 * i << ")" << endl;
	  	A(get<1>(minput[(i + 1)%inpLen]),  get<0>(minput[i])) = gd(0, taup[i]);
		// A(2 * i + 1, 2 * i) = gd(0, taup[i]);
	}
	 //A(get<1>(minput[0]), get<0>(minput[inpLen - 1])) = gd(0, taup[inpLen - 1]);
	//cout << inpLen - 1 <<";" << input[inpLen - 1] << ";" << transition_convert(input[inpLen - 1],5)<<';'<< finput(transition_convert(input[inpLen - 1], 5)) << endl;
	//cout << "ayy (" << get<1>(minput[0]) << "," << get<0>(minput[inpLen - 1]) << ") = (" << 2 *(0)  + 1 << "," << 2 * (0) << ")" << endl;
	/*A(getMatrixIndex(Tank{0,IO::INPUT}, t), getMatrixIndex(IO::INPUT, t)) = gd(0, taup[0]);
	for (int i = 1;i < inpLen;i++) {
		A(getMatrixIndex(Tank{ i,IO::INPUT }, t), getMatrixIndex(Tank{ i-1,IO::OUTPUT }, t)) = gd(0, taup[i]);
	}*/
	//this loop sets all elements in matrix A related to the movement of the hoist without grabbing a piece, except for the movement from tank x(T+1) to tank x'(T+1) = 0 (input tank), which is set in the following step
	for (int i = 0;i < inpLen-1;i++) {
		//0-> 0.left	
		//1-> 0.right
		//2-> 1.left
		//3-> 1.right
		//right,left
		//cout << "(" << 2 * get<1>(input[i]) << "," << 2 * get<0>(input[i]) - 1 << ")=(" <<
		//	get<1>(minput[i]) <<","<< get<0>(minput[i])<<")" << endl;
		A(get<0>(minput[i]),get<1>(minput[i])) = gd(0, tau[i]);
	}
	//difference here is that the place has an initial token 
	A(get<0>(minput.back()),get<1>(minput.back())) = gd(1, tau[inpLen-1]);
	
	//the following part is for the processing time in the tanks, it is a bit more complicated as we need to put tokens (representing pieces initially in a tank) in the right spot, based only on the schedule
	for (int i = 0;i < inpLen-1;i++) {
		//printf("i:%d\n", i);
		int flag = 0;
		for (int j = 0;j <= i;j++) {
			//printf("i:%d j:%d\n", i, j);
			//printf("%d == %d\n", input[i].right, input[j].left);
			if (get<1>(input[i]) == get<0>(input[i])) {
				flag = 1;
				A(2 * get<1>(input[i]), 2 * get<1>(input[i]) - 1) = gd(0, l[i]);
			}
		}
		//printf("flagtest %d\n", flag);
		if (flag != 1) {
			//printf("left: %d right:%d \n", 2 * input[i].right, 2 * input[i].right - 1);
			A(2 * get<1>(input[i]), 2 * get<1>(input[i]) - 1) = gd(1, l[i]);
		}
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
