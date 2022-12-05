// Run program: Ctrl + F5 or Debug > Start Without Deb
// 
// ugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <iostream>
#include <exception>
#include "etvo.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cassert>

using namespace std;  //this imports the standard library, every cout and vector 
using namespace etvo; //just a library :P every matrix and series. all elements from the library

typedef struct {      //represents a tuple of two integers
	int left;
	int right;
} route_struct;            //tuple is a container of two elements of any set 

typedef struct {
	vector<route_struct> route;
	vector<int> transportationTime;
	vector<int> mvoementTime;
	vector<int> processingTime;
} info_products;
//initializes a matrix of dimensions n_transitions x n_transitions. This matrix is matrix A of a max-plus linear system, written in the domain of daters
matrix<series> A_matrix(info_products inputs);
vector<matrix<series>> A_multi_product(vector<info_products> inputs) {
	int linputs = inputs.size();
	vector<matrix<series>> A_matrices;
	transform(inputs.begin(), inputs.end(), std::back_inserter(A_matrices), A_matrix);
	return A_matrices;
}

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
int main() {

	//int T = 3;
	//int t = 2 * T + 2;

	//vector<int> input{ 1,3,4,1,2,4,5,2,3,5,6,0 }; // Is this correct?
	//vector<int> input{ 1,3,4,1,2,2,3,0 }; // from Davide's paper

	//info_products p= {{ {1,2},{3,3},{4,1},{2,4},{5,0} };
	//vector<tuple_2> input2{ {1,2},{3,3},{4,1},{2,4},{5,0} };
	//vector<int> input{ 1,2,3,1,2,3,4,0 };
	//input(0) = 1;
	//input(1) = 2;
	//input(2) = 3;
	//input(3) = 1;
	//input(4) = 2;
	//input(5) = 3;
	//input(6) = 4;
	//input(7) = 0;
	vector<info_products> p = { 
		{ { {1,2},{3,3},{4,1},{2,4},{5,0} },//input
	{ 2, 3, 5, 7,8 },//taup
	{ 11, 13, 17, 19, 20 },//tau
	{ 23, 29, 31, 37 } },
	{ { {1,2},{3,3},{4,1},{2,4},{5,0} },//input
	{ 2, 3, 5, 7,8 },//taup
	{ 11, 13, 17, 19, 20 },//tau
	{ 23, 29, 31, 37 } }
	};//l


	

	vector<matrix<series>> A = A_multi_product(p);

	//int length = p.route.size() - 1;
	//matrix<series> B = B_matrix(length);
	//matrix<series> C = C_matrix(length);

	//matrix<series> H = C * A.star() * B;
	
	for_each(A.begin(), A.end(), [&A](matrix<series> &value) {
		int idx = &value - &A[0];
		cout << idx << "A = " << value;
	});

	//cout << "A = " << A;
	//cout << "H = " << H << endl;

	//for (int i=0; i < t; i++) {
	//	for (int j=0; j < t; j++) {
	//		
	//		cout << A(i, j) << " ";
	//		
	//	}
	//}
	gd r(1,5);
	//r = gd(1, 3);
	//matrix<series> U(1,1);
	//U(0, 0) = ((poly) r+gd(1, infinit));
	//std::cout << "\n r= " << r << std::endl;
	poly a = (poly)r + gd(0,4);
	a.isE();
	std::cout << "\n a= " << a.star() << std::endl;

	getchar();

	return 1;

}

