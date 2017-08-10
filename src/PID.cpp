#include "PID.h"
#include <chrono>
#include <thread>
#include <math.h>

using namespace std;

/*
* TODO: Complete the PID class.
*/

const double DP_TOL =  1; // .00001; is a good value to have for the I part
const int RESTART_STEPS = 600;
const int MIN_STEPS_ERR = 40;
const double MAX_CTE = 2;

const double DEGREE_TO_RADIANS=0.0174533;
const double MAX_THROTTLE_AT_SPEED = 5; // degrees

const int TWIDDLE_START_POS = 1;

PID::PID(double p[NUM_PARAMS]) {
	best_error = numeric_limits<double>::max();
	for(int i=0; i<NUM_PARAMS; i++) {
		this->p[i] = p[i];
	}
	dp[0] =0;  // 5; is a good value to start with for P
	dp[1] =50; // 5; // this is the best value for D 
	dp[2] =0; // .0001; // this is a good value to play with
	reset_vars();
	twiddle_n = TWIDDLE_START_POS;
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
	} else {
		first_time = false;
	}

	p_error = cte;

	i_error += cte;

	
	square_error += cte*cte;
		

	return 0;
}

int PID::UpdateErrorTraining(double cte, double speed, double angle) {

	if (restart_mode) {
		// check if still nor restarted. As there is some lag in the simulator
		// if (fabs(cte) > MAX_CTE && speed > 5 && angle > 0) {
		if (fabs(cte) > MAX_CTE || speed > 5) {
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
	} else {
		first_time = false;
	}

	//cout << "cte: " << cte << ", d_error: " << d_error << endl;
	p_error = cte;

	i_error += cte;

	if (iterations_ > MIN_STEPS_ERR) {
		square_error += cte*cte;
		if (best_error != -1 && square_error > best_error) {
			cout << "error exceeded best so far " << square_error << "(" << best_error << "; best_p= "
			<< best_p[0] << ", " << best_p[1] << ", " << best_p[2] << ")" << endl;
			Restart();
			return -2;
		}
	}

	return 0;
}

double PID::TotalError() {
	return square_error;
}

double PID::calculate_steer(double speed){
	double steer = -p[0] * p_error - p[1] * d_error - p[2] * i_error;

	// We introduce some control based on high speed
	// It applies only for significant turns > 7 degree (arbitrary)
	double steer_lim = MAX_THROTTLE_AT_SPEED * DEGREE_TO_RADIANS;
	if (fabs(steer) > steer_lim) {
		// and we apply for speed only above 10 miles/hr
		int mul=1;
		if (steer < 0)
			mul = -1;
		steer = fabs(steer) - fabs(steer)*(speed - 10)/speed;
		if (steer < steer_lim) 
			steer =  steer_lim;
		steer*=mul;
	}

	// debug
	//  apply the steer static correcrion 
	// .44 degrees to the right
	//steer -= 0.007679449;

	return steer; // based on the convention in this case
}

double PID::calculate_throttle(double speed, double desired_speed, double steer_value, double cte){
	const double MIN_SPEED_FOR_THROTTLE_CHECK = 10;
	double speed_cte = speed - desired_speed;
	double throttle = -.12 * speed_cte;

	
	// if cte is more and the steer value then don't throttle so much
	// (reduce throttle)
	if (speed > MIN_SPEED_FOR_THROTTLE_CHECK) {
		if (throttle > 0) {
			throttle += -1.75*fabs(steer_value) - 0.3 * fabs(cte);
		}
	}

	/**
	Alternate way
	throttle += -1.2*fabs(cte);
	**/
	

	return throttle;
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

			twiddle_n = TWIDDLE_START_POS;
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
		for(int i=0; i<NUM_PARAMS; i++){
			best_p[i] = p[i];
		}
		cout << "corresponding p: " << p[0] << ", " << p[1] << ", " << p[2] << endl;
		return true;
	} else {
		return false;
	}
}
