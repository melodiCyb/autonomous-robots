#include <iostream>
#include <math.h>
#include <tuple>
#include "Core" // eigen lib
#include "LU"   // eigen lib

using namespace std;
using namespace Eigen;

float measurements[6] = { 1, 2, 3, 4, 5, 6 };


tuple<MatrixXf, MatrixXf> kalman_filter(MatrixXf x, MatrixXf P, MatrixXf u, MatrixXf F, MatrixXf H, MatrixXf R, MatrixXf I)
{

    for (int n = 0; n < sizeof(measurements) / sizeof(measurements[0]); n++) {
        
       // measurement update
       MatrixXf z(1,1);
       MatrixXf y(1,1);
       z << measurements[n]; // observation
       y << z - (H * x);  // measurement residual
       MatrixXf S(1,1);
       S << H * P * H.transpose() + R;
       
       MatrixXf K(2,1);
       K << P * H.transpose() * S.inverse(); // Kalman gain
       
       x = x + (K * y); // posterior state
       P = (I - (K * H)) * P; // posterior  covariance
       
    
       // prediction
       x = F * x + u;
       P = F * P * F.transpose();
        
    }

    return make_tuple(x, P);
}

int main()
{

    MatrixXf x(2, 1); // initial state (location and velocity)
    x << 0,
         0;
    MatrixXf P(2, 2);// initial uncertainty
    P << 1000, 0,
         0, 1000;
    MatrixXf u(2, 1);// external motion
    u << 0,
         0;
    MatrixXf F(2, 2);// next State func
    F << 1, 1,
         0, 1;
    MatrixXf H(1, 2);// measurement func
    H << 1,
         0;
    MatrixXf R(1, 1); // measurement uncertainity
    R << 1;
    MatrixXf I(2, 2);// identity matrix
    I << 1, 0,
         0, 1;

    tie(x, P) = kalman_filter(x, P, u, F, H, R, I);
    cout << "x= " << x << endl;
    cout << "P= " << P << endl;

    return 0;
}
