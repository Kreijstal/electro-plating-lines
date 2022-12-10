#include "ElectroPlatingLines.h"
// 18. November 2022 Freitag. EN Gebaeude //
//switched max plus, define matrices. a for product a, b for product b. once u have them u can combine them. problem is getting the matrices. 
//robot follows the same route, and products stay there for the same time. 
//if they are both like that. they are undistinguishable. so the matrices would be the same. code would be adaptable to these schedules. 
// same routes but different waiting times. then you can change the schedule. still all the pieces go to the tanks, all the tanks but in different
// orders. open an issue for each step. once u implement it, you can proceed. first two are relatively easy 
// third case, more time consuming. 
// for loop that searches all the products. vector of matrices. 
//assertion of the same length. 2 vectors of everything. 
//vector of a schedule, m2. element of a structure juan created. each structure, scheduel and time each structure 
// struct c++ . tuples= order sets. (a ,b ,c ,d )each position has a different name. diff data types. must be consistent. 
// robot_info? if thats ok in c++ > up to you. 
// info_robots.route 
// info_robots.movement_time ?
// info_robots.transportation_time
// info_robots.processing_time
// route.struct {{_,_},{_,_}}
// input is info_robot1.route and info_robot2.route
// B, C are the same? in which cases are they not? assume they are the same
// possibily of having them different. more input deposits then output deposit, which by us is not the case
// seperate input that takes care of the shared place
// one robot has the input of the shared tank and the other does not
// another possibility>
//   another file where you can aufruf these functions
// shared files for things we share. w juan 