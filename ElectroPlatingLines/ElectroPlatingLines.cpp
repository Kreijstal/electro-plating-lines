#include "ElectroPlatingLines.h"


matrix<series> A_matrix(info_products inputs) {
	vector<route_struct> input=inputs.route;
	vector<int> taup=inputs.transportationTime;
	vector<int> tau=inputs.mvoementTime;
	vector<int> l=inputs.processingTime;
	assert((void("The vector containing the time of pieces in containers doesn't match with the input"), input.size() - 1 == l.size()));
	assert((void("The length of the vector containing the time of the hoist without a piece must be equal to the length of the input"), input.size() == tau.size()));
	//taup is the time of the hoist with a piece
	//tau is the time of the hoist without a piece
	//l is the time a piece has to be on the tank
	//Number of tanks
	int T = input.size() - 1;
	//int N = T + 1;
	//Number of transitions
	int t = 2 * T + 2;

	matrix<series> A(t, t);
	//this for loop sets all elements in matrix A related to the movement of the hoist while grabbing a piece
	for (int i = 0;i <= T;i++) {
		A(2 * i + 1, 2 * i) = gd(0, taup[i]);
	}
	//this loop sets all elements in matrix A related to the movement of the hoist without grabbing a piece, except for the movement from tank x(T+1) to tank x'(T+1) = 0 (input tank), which is set in the following step
	for (int i = 0;i < T;i++) {
		//0-> 0.left
		//1-> 0.right
		//2-> 1.left
		//3-> 1.right
		A(2 * input[i].right, 2 * input[i].left - 1) = gd(0, tau[i]);
	}
	//difference here is that the place has an initial token 
	A(2 * input[T].right, 2 * input[T].left - 1) = gd(1, tau[T]);

	//the following part is for the processing time in the tanks, it is a bit more complicated as we need to put tokens (representing pieces initially in a tank) in the right spot, based only on the schedule
	for (int i = 0;i < T;i++) {
		//printf("i:%d\n", i);
		int flag = 0;
		for (int j = 0;j <= i;j++) {
			//printf("i:%d j:%d\n", i, j);
			//printf("%d == %d\n", input[i].right, input[j].left);
			if (input[i].right == input[j].left) {
				flag = 1;
				A(2 * input[i].right, 2 * input[i].right - 1) = gd(0, l[i]);
			}
		}
		//printf("flagtest %d\n", flag);
		if (flag != 1) {
			//printf("left: %d right:%d \n", 2 * input[i].right, 2 * input[i].right - 1);
			A(2 * input[i].right, 2 * input[i].right - 1) = gd(1, l[i]);
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
