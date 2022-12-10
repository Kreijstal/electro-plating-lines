#pragma once
#include <iostream>
#include <exception>
#include "etvo.h"
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <tuple>
#include <algorithm>
using namespace std;  //this imports the standard library, every cout and vector 
using namespace etvo; //just a library :P every matrix and series. all elements from the library


typedef struct {
	vector<tuple<int,int>> route;
	vector<int> transportationTime;//Transportations time are ordered by deposits
	vector<int> movementTime;//the order of the movement times is the same order in which the hoist moves through the graph
	vector<int> processingTime;//the order of processing times is the order when the hoist grabs the item as it moves through the graph
} info_products;

matrix<series> A_matrix(info_products inputs);
vector<matrix<series>> A_multi_product(vector<info_products> inputs);
matrix<series> B_matrix(int T);
matrix<series> C_matrix(int T);