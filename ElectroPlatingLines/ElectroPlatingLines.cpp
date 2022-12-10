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
enum class transition {
	IO,
	tank
};
int getMatrixIndex(std::variant<IO, Tank> t,int matrixlength) {

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
				return matrixlength - 1;
				break;
			}
		//	std::cout << "IO enum class" << std::endl;
		}
		else if constexpr (std::is_same_v<T, Tank>) {
			// Use the IOClass class here
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

// Use a trailing return type to explicitly specify the return type of the lambda function
auto transition_convert(tuple<int, int> tup, int out) -> tuple<std::variant<IO, Tank>, std::variant<IO, Tank>> {
	// Change the return type of the lambda function to std::variant<IO, Tank>
	auto t_co = [&out](int p, lr lr) -> std::variant<IO, Tank> {
		if (p == 0)
			return IO::INPUT;
		if (p == out)
			return IO::OUTPUT;
		return Tank{ p-1, lr == lr::left ? IO::INPUT : IO::OUTPUT };
	};

	int left, right;
	std::variant<IO, Tank> a, b;
	std::tie(left, right) = tup;

	// Call the lambda function and assign the result to the variants a and b
	a = t_co(left, lr::left);
	b = t_co(right, lr::right);

	// Return the tuple containing the two variants
	return std::tie(a, b);
}

vector<tuple<std::variant<IO, Tank>, std::variant<IO, Tank>>> vec_transition_convert(vector<tuple<int, int>> inputs) {
	int linputs = inputs.size();
	vector<tuple<std::variant<IO, Tank>, std::variant<IO, Tank>>> tranconverted;
	transform(inputs.begin(), inputs.end(), std::back_inserter(tranconverted), std::bind(&transition_convert,std::placeholders::_1, linputs));
	return tranconverted;
}
matrix<series> A_matrix(info_products inputs) {
	//the returned matrix gives us the graph 
	//where the order of transitions are:
	//input (input deposit,output deposit)_i output
	vector<tuple<int,int>> input=inputs.route;
	vector<int> taup=inputs.transportationTime;
	vector<int> tau=inputs.movementTime;
	vector<int> l=inputs.processingTime;
	assert((void("The vector containing the time of pieces in containers doesn't match with the input"), input.size() - 1 == l.size()));
	assert((void("The length of the vector containing the time of the hoist without a piece must be equal to the length of the input"), input.size() == tau.size()));
	//taup is the time of the hoist with a piece
	//tau is the time of the hoist without a piece
	//l is the time a piece has to be on the tank
	//input Length
	auto pinput = vec_transition_convert(input);
	int inpLen = input.size();
	auto minput = ([&inpLen](auto inputs)->vector<tuple<int, int>> {
		vector<tuple<int, int>> tranconverted;
		transform(inputs.begin(), inputs.end(), std::back_inserter(tranconverted), [&inpLen](auto p)->tuple<int, int> {
			std::variant<IO, Tank> left, right;
			std::tie(left, right) = p;
			int a = getMatrixIndex(right, inpLen);
			int b= getMatrixIndex(left, inpLen);
			return std::tie(a, b);
			});
		return tranconverted;
		})(pinput);
	//int N = T + 1;
	//Number of transitions
	int t = 2 * inpLen ;

	matrix<series> A(t, t);//A(to,from)
	//this for loop sets all elements in matrix A related to the movement of the hoist while grabbing a piece
	//for (int i = 0;i < inpLen;i++) {
		//input->input_1
		//output_{i-1}->input_i
		//A(2 * i + 1, 2 * i) = gd(0, taup[i]);
	//}
	A(getMatrixIndex(Tank{ 0,IO::INPUT }, t), getMatrixIndex(IO::INPUT, t)) = gd(0, taup[0]);
	for (int i = 1;i < inpLen;i++) {
		A(getMatrixIndex(Tank{ i,IO::INPUT }, t), getMatrixIndex(Tank{ i-1,IO::OUTPUT }, t)) = gd(0, taup[i]);
	}
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
