#pragma once
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

matrix<series> A_matrix(info_products inputs);
vector<matrix<series>> A_multi_product(vector<info_products> inputs);
matrix<series> B_matrix(int T);
matrix<series> C_matrix(int T);