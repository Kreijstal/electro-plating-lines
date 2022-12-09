#include "ElectroPlatingLines.h"
#include <algorithm>

//initializes a matrix of dimensions n_transitions x n_transitions. This matrix is matrix A of a max-plus linear system, written in the domain of daters
//matrix<series> A_matrix(info_products inputs);
vector<matrix<series>> A_multi_product(vector<info_products> inputs) {
	int linputs = inputs.size();
	vector<matrix<series>> A_matrices;
	transform(inputs.begin(), inputs.end(), std::back_inserter(A_matrices), A_matrix);
	return A_matrices;
}

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
	//relax assumption that pieces are not going to all the tanks
	//a for loop might not work because we want to save the pieces on the container, how long are pieces stored in a container, try optimizing A for each schedule
	//Assumptions to relax
	//Every piece has to go through all deposits once
	//Every deposit has capacity one
	//Every piece has to start from the beginning and end in the end
	//How can I find A star of a switched linear system, given a schedule
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

	for_each(A.begin(), A.end(), [&A](matrix<series>& value) {
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
	//gd r(1,5);
	//r = gd(1, 3);
	//matrix<series> U(1,1);
	//U(0, 0) = ((poly) r+gd(1, infinit));
	//std::cout << "\n r= " << r << std::endl;
	//poly a = (poly)r + gd(0,4);
	//a.isE();
	//std::cout << "\n a= " << a.star() << std::endl;

	getchar();

	return 1;

}
