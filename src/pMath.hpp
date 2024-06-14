#include <cmath>
#include <chrono>
#include <functional>
#include <memory>

//Struct storing the relative (to the anchor point) positions of both pendulums as t (theta)
//p (phi) is the time derivative of t
//This is used for exporting our states from the thread
//This is also used to pass modified states into RK4 function calls
struct pData {
    double t;
	double p;

	double t2;
	double p2;
};

class pMath {
    private:
        //Stores the state of the system
        double theta;
        double phi;
        double theta2;
        double phi2;

        //Temporarily stores the new states of the system so that we dont replace the old states in the middle of our calculation
        //I cant remeber the logic of the pp suffix 
        double thetapp;
        double phipp;
        double thetapp2;
        double phipp2;

        //Time variables used to time the last calcution
        std::chrono::steady_clock::time_point start_time;
        double then;
        double now;
        
        //Constants of the simulation
        const double gravity = 9.8;
        const double len1 = 1;
        const double len2 = 1;
        const double mass1 = 1;
        const double mass2 = 1;

        //Runge Kutta 4 function
        double RK4(double h, double r_n, std::function<double(pData)> func);

        //Our pendulum ODES
        double pen1_1(pData dfRK4);
        double pen1_2(pData dfRK4);
        double pen2_1(pData dfRK4);
        double pen2_2(pData dfRK4);
    public:
        //Feed initial conditions to math model
        pMath(double in1, double in2);

        //Repesents one time unit pendulum calculation
        //Passed to generic thread creator
        std::shared_ptr<pData> simulate();
};