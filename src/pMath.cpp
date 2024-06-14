#include "pMath.hpp"

double pMath::RK4(double h, double r_n, std::function<double(pData)> func)
{
    //Temporary modified pendulum states to feed into RK4 calculations
    pData dfRK4;

    //Prepare modified states
    dfRK4.t = theta; dfRK4.p = phi; dfRK4.t2 = theta2; dfRK4.p2 = phi2;
    //Feed into selected pendulum function
    double k1 = func(dfRK4);

    dfRK4.t = theta + h*k1/2; dfRK4.p = phi + h*k1/2; dfRK4.t2 = theta2 + h*k1/2; dfRK4.p2 = phi2 + h*k1/2;
    double k2 = func(dfRK4);

    dfRK4.t = theta + h*k2/2; dfRK4.p = phi + h*k2/2; dfRK4.t2 = theta2 + h*k2/2; dfRK4.p2 = phi2 + h*k2/2;
    double k3 = func(dfRK4);

    dfRK4.t = theta + h*k3; dfRK4.p = phi + h*k3; dfRK4.t2 = theta2 + h*k3; dfRK4.p2 = phi2 + h*k3;
    double k4 = func(dfRK4);

    //Return new value of specific state
    return r_n + h*(k1 + 2*k2 + 2*k3 + k4)/6;
}

//dtheta = phi
//Our four ODES 
double pMath::pen1_1(pData dfRK4)
{
    return dfRK4.p;
}
double pMath::pen1_2(pData dfRK4)
{
    double dtheta = dfRK4.t - dfRK4.t2;
    double M = mass1 + mass2;
    return (-sin(dtheta)*(mass2*len1*pow(dfRK4.p, 2)*cos(dtheta)+mass2*len2*pow(dfRK4.p2, 2)) - gravity*(M*sin(dfRK4.t)-mass2*sin(dfRK4.t2)*cos(dtheta)))
    /(len1*(mass1+mass2*pow(sin(dtheta), 2)));
}
double pMath::pen2_1(pData dfRK4)
{
    return dfRK4.p2;
}
double pMath::pen2_2(pData dfRK4)
{
    double dtheta = dfRK4.t - dfRK4.t2;
    double M = mass1 + mass2;
    return (sin(dtheta)*(M*len1*pow(dfRK4.p, 2)+mass2*len2*pow(dfRK4.p2, 2)*cos(dtheta)) + gravity*(M*sin(dfRK4.t)*cos(dtheta)-M*sin(dfRK4.t2)))
    /(len2*(mass1+mass2*pow(sin(dtheta), 2)));
}

pMath::pMath(double in1, double in2)
{
    start_time = std::chrono::steady_clock::now();
    now = 0;
    theta = in1;
    phi = 0;
    theta2 = in2;
    phi2 = 0;
}

std::shared_ptr<pData> pMath::simulate()
{
    using namespace std;

    auto ptr = make_shared<pData>();
    then = now;
    now = chrono::duration<double>(chrono::steady_clock::now() - start_time).count();
    auto h = then - now;

    //Pendulum 1
    thetapp = RK4(h, theta, [&](pData dfRK4) -> double {return pen1_1(dfRK4);});
    phipp = RK4(h, phi, [&](pData dfRK4) -> double {return pen1_2(dfRK4);});

    //Pendulum 2
    thetapp2 = RK4(h, theta2, [&](pData dfRK4) -> double {return pen2_1(dfRK4);});
    phipp2 = RK4(h, phi2, [&](pData dfRK4) -> double {return pen2_2(dfRK4);});

    //Now that the calculations are done we can discard old states
    theta = thetapp;
    phi = phipp;
    theta2 = thetapp2;
    phi2 = phipp2;

    //Store states into export structure
    ptr->t = theta;
    ptr->p = phi;
    ptr->t2 = theta2;
    ptr->p2 = phi2;

    //Return export structure
    return ptr;
}