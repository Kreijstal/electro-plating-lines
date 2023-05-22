#include "multi_product_lib.h"
int main()  
{
    try {
    
        RobotSchedule a({
            {1, { { 3, 1 }, { 0, 2 }, { 1, 3 } }, { 10 } },
            {0, { { 1, 1 }, { 1, 2 }, { 3, 3 } }, { 10 } } ,
            {0, { { 1, 1 }, { 0, 2 }, { 2, 3 },{1,5} ,{3,6},{2,7},{3,8} }, { 10,20 } }
           // {0, { { 2, 1 }, { 2, 4 }, { 3, 3 } }, { 9 } }
            },
            
            { { 1, 2, 5},
              { 3, 4, 6},
              { 7, 8, 9} });
              
        /*RobotSchedule a({
            {1, { { 2, 1 }, { 0, 2 }, { 1, 3 } }, { 10 } },
            {0, { { 1, 1 }, { 1, 2 }, { 2, 3 } }, { 10 } }
            },
            { { 1, 2 },
              { 3, 4 } }); 
              */
        matrix<series> A, B;
        cout << a.vMode[0].A0_matrix << "A MATRIX first thing end" << endl;

        std::tie(A, B) =a.getBigAandBMatrix({1,2});

        auto c = A.star();
        cout << "now this is A:" << endl << A << "END" << endl;
        cout << "now this is A*:" << endl << c << "END" << endl;
        cout<<c* (B * intVector2MaxPlus({ 0,0 }))<<endl;
       
        cout << "hello world" << endl;
        
    }
    catch (std::exception& e) {
        std::cerr << endl
            << "An error has been detected in the program, the program will exit now." << endl
            << "error message:" << e.what() << endl;
        return 1;
    }
}
