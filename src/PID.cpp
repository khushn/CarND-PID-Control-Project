#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Kd, double Ki) {
	this->Kp = Kp;
	this->Kd = Kd;
	this->Ki = Ki;
	square_error = 0.;
	first_time = true;
	d_error=0.;
}

void PID::UpdateError(double cte) {

	if (!first_time) {
		d_error = cte - p_error;
	}

	p_error = cte;

	i_error += cte;
	square_error += cte*cte;
	
}

double PID::TotalError() {
	return square_error;
}

double PID::calculate_steer(){
	double steer = -Kp * p_error - Kd * d_error - Ki * i_error;
	return steer; // based on the convention in this case
}

