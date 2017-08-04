#include "PID.h"
#include <chrono>
#include <thread>
#include <math.h>

using namespace std;

/*
* TODO: Complete the PID class.
*/

const double DP_TOL = .2;
const int RESTART_STEPS = 200;
const int MIN_STEPS_ERR = 40;
const double MAX_CTE = 2;

PID::PID(double p[NUM_PARAMS]) {
	best_error = numeric_limits<double>::max();
	for(int i=0; i<NUM_PARAMS; i++) {
		this->p[i] = p[i];
	}
	dp[0] =1;
	dp[1] =1;
	dp[2] =1;
	reset_vars();
	twiddle_n = 0;
	twiddle_fn_pos=0;
	restart_mode = false;
}

PID::~PID() {}


void PID::reset_vars() {
	square_error = 0.;
	first_time = true;
	d_error=0.;
	iterations_ = 0;	
}

int PID::UpdateErrorDrive(double cte, double speed, double angle) {



	if (!first_time) {
		d_error = cte - p_error;
	}

	p_error = cte;

	i_error += cte;

	
	square_error += cte*cte;
		

	return 0;
}

int PID::UpdateErrorTraining(double cte, double speed, double angle) {

	if (restart_mode) {
		// check if still nor restarted. As there is some lag in the simulator
		if (fabs(cte) > MAX_CTE && speed > 5 && angle > 0) {
			cout << "Still in restart mode" << endl;
			return -2;
		} else {
			cout << "restart done" << endl;
			restart_mode = false;
		}
	}

	if(++iterations_ > RESTART_STEPS) {
		// We restart the simulator after every certain no. of steps
		Restart();		
		return -1;
	}

	// If car swerves too much at any point
	if (fabs(cte) > MAX_CTE) {
		cout << "car swerved too much! cte: " << cte << endl;
		Restart();
		return -2;
	}

	

	if (!first_time) {
		d_error = cte - p_error;
	}

	p_error = cte;

	i_error += cte;

	if (iterations_ > MIN_STEPS_ERR) {
		square_error += cte*cte;
		if (best_error != -1 && square_error > best_error) {
			cout << "error exceeded best so far " << square_error << "(" << best_error << ")" << endl;
			Restart();
			return -2;
		}
	}

	return 0;
}

double PID::TotalError() {
	return square_error;
}

double PID::calculate_steer(){
	double steer = -p[0] * p_error - p[1] * d_error - p[2] * i_error;
	return steer; // based on the convention in this case
}


void PID::SetServer(uWS::WebSocket<uWS::SERVER> ws){
	ws_ = ws;
}

void PID::Restart(){
	std::string reset_msg = "42[\"reset\",{}]";
	ws_.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);

	this_thread::sleep_for(chrono::milliseconds(100));
	
	
	cout << "==============================================" << endl;
	cout << "restarting after " << iterations_ << " steps" << endl;	
	cout << "square_error: " << square_error << endl;
	cout << "(Best so far. error: " << best_error << " )" << endl;
	cout << "debug: Next code: " << twiddle_fn_pos << endl;

	
	twiddle_check();
	reset_vars();
	cout << "Next p: " << p[0] << ", " << p[1] << ", " << p[2] << ")" << endl;

	iterations_=0;
  	restart_mode=true;
}

int PID::twiddle_check() {

	

	switch(twiddle_fn_pos) {

		case 0: {
			double sum_dp = dp[0]+dp[1]+dp[2];
			cout << "sum_dp: " << sum_dp << endl;
			if ( sum_dp < DP_TOL){
				cout << "Done twiddle." << endl;
				cout << "Best error: " << best_error << endl;
				cout << "Best values of p: " << p[0] << ", " << p[1] << ", " << p[2] << endl;
				cout << "Exiting" << endl;
				exit(0);
			}

			twiddle_n = 0;
			twiddle_fn_pos=1;
			twiddle_check();
			
			break;
		}
		case 1: {			

			// The first two steps of for loop
			p[twiddle_n] += dp[twiddle_n];				
			twiddle_fn_pos=2;
			break;
		}
		case 2: {
			bool ret = check_best_error();
			if(ret) {
				dp[twiddle_n]*= 1.1;				
				twiddle_n++;
				if (twiddle_n == NUM_PARAMS-1) {
					twiddle_fn_pos = 0;					
				} else {
					twiddle_fn_pos=1;
				}
				twiddle_check();
			} else {
				p[twiddle_n] -= 2 * dp[twiddle_n];
				twiddle_fn_pos=3;
			}
			break;
		}
		case 3: {
		// the else part
			bool ret = check_best_error();
			if(ret) {
				dp[twiddle_n]*= 1.1;				
			} else {
				p[twiddle_n] +=dp[twiddle_n];
				dp[twiddle_n]*= 0.9;
			}
			
			twiddle_n++;
			if (twiddle_n == NUM_PARAMS-1) {
				twiddle_fn_pos = 0;				
				
			} else {
				twiddle_fn_pos=1;
			}
			twiddle_check();
			break;
		}
	}
	return twiddle_fn_pos;
}

bool PID::check_best_error(){
	if ( iterations_ >= RESTART_STEPS && best_error > square_error) {
		best_error = square_error;
		cout << "New best error: " << best_error << endl;
		cout << "corresponding p: " << p[0] << ", " << p[1] << ", " << p[2] << endl;
		return true;
	} else {
		return false;
	}
}
