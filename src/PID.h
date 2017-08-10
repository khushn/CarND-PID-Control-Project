#ifndef PID_H
#define PID_H

#include <uWS/uWS.h>

const int NUM_PARAMS=3;

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  double square_error;

  double best_error;
  double best_p[NUM_PARAMS];
  /*
  * Coefficients
  */ 

  double p[NUM_PARAMS];
  double dp[NUM_PARAMS];

  bool first_time;

  uWS::WebSocket<uWS::SERVER> ws_;

  int iterations_;

  bool restart_mode;
  /*
  * Constructor
  */
  PID(double p[3]);

  /*
  * Destructor.
  */
  virtual ~PID();


  void reset_vars();

  /*
  * Update the PID error variables given cross track error.
  * the method used in training (or twiddle) mode
  */
  int UpdateErrorTraining(double cte, double speed, double angle);

  /*
  * The method used in drive mode
  */
  int UpdateErrorDrive(double cte, double speed, double angle);

  /*
  * Calculate the total PID error.
  */
  double TotalError();

  /*
  * calculate the steering angle needed
  */
  double calculate_steer(double speed);

  /*
  Calculate throttle based on current and desired speed
  * Again a simple PID controller (just the P portion may be sufficient)
  */
  double calculate_throttle(double speed, double desired_speed, double steer_value, double cte);

  /**
  Thanks to this forum discussion. Got the idea from there: 
  https://discussions.udacity.com/t/twiddle-application-in-pid-controller/243427/21
  */
  void SetServer(uWS::WebSocket<uWS::SERVER> ws);
  void Restart();

  int twiddle_check();

  bool check_best_error();

  // vars used by twiddle
  int twiddle_n;
  int twiddle_fn_pos;
  
};

#endif /* PID_H */
