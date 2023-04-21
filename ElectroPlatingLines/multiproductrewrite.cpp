#include "multi_product_lib.h"
int main()  
{
    try {
    
        RobotSchedule a({
            {1, { { 3, 1 }, { 0, 2 }, { 1, 3 } }, { 10 } },
            {0, { { 1, 1 }, { 1, 2 }, { 3, 3 } }, { 10 } } ,
            //{0, { { 1, 1 }, { 0, 2 }, { 2, 3 },{1,5} ,{3,6},{2,7},{3,8} }, { 10,20 } }
           //won't work,(yet) why?, because it reuses the input transition twice
            {0, { { 2, 1 }, { 2, 4 }, { 3, 3 } }, { 9 } }
            },
            { { 1, 2, 5},
              { 3, 4, 6},
              { 7, 8, 9} });
        //cout << "TEST" << endl;
        matrix<series> A, B;
        std::tie(A, B) =a.getBigAandBMatrix({2,2});
        //cout << "TEST" << endl;
        //matrix<series> x = getXfromMatrix(A);
        //g1.d15+(g2.d23)
        //series p(gd(0, 0), gd(1, 13), gd(1, 13));
        //series q(gd(0, 0));
        //auto d = p+q;
        //poly ay = gd(1, 5);
        //poly b = gd(0, 4);
        //std::vector<gd> as({ gd(0,4),gd(1,5) });
        //poly s(as);
       // s.setsimple
        //s.simpli();
        //cout <<"printing <:"<<(gd(0,0)<gd(1,13))<<(gd(0, 0) <= gd(1, 13))<< endl;
 
        //cout << series(gd(1, 2))+ series(gd(2, 10)) << endl;*/
        auto c = A.star();
        //c(0, 5).canon();
        cout << "now this is A:" << endl << A << "END" << endl;
        cout << "now this is A*:" << endl << c << "END" << endl;
        cout<<A.star()* (B * intVector2MaxPlus({ 0,0 }))<<endl;
        /*vector<int> x = a.initialVector();
        x = a.multiplyWithAstarMatrix(a.addB(x, 1), 0);
//        cout << a.vMode[0].processingTimesQueues << endl;
        cout<<x<<endl;
        cout << a.multiplyWithAstarMatrix(a.multiplyWithA1(x,0,1),1) << endl;*/

        //cout << a.multiplyWithA1(x,0,0) << endl;
        //matrix<poly> A(2,2);
        //A(0, 0) = i2gd(1);
        //A(0, 1) = i2gd(2);
        //A(1, 0) = i2gd(3);
        //A(1, 1) = i2gd(4);
        //cout << A << endl;
        cout << "hello world" << endl;
        
    }
    catch (std::exception& e) {
        std::cerr << endl
            << "An error has been detected in the program, the program will exit now." << endl
            << "error message:" << e.what() << endl;
        return 1;
    }
}
