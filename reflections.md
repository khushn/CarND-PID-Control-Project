### Reflections on the project


#### Approach

We use <i>twiddle</i>, with the simulator. The key code is modeled after the Python code done in the class. And is in the <code> twiddle_check() </code> method of PID class. It looks a bit complicated, because of the control exchange between simulater and itself. 

So there are two modes - Training and Drive. 

##### Training mode

In the training mode, the main program is run as: 

<code>./pid train</code>

In this mode the method called of class PID is <code> UpdateErrorTraining() </code>, which makes use of the <code> Restart() </code> method to: 
1. Send a Reset message to the simulator. Done after every RESTART_STEPS parameter in PID.cpp. Which brings the car in the simulator to the staring point. 
2. It also calls <code> twiddle_check() </code> which twiddles the parameters in the p[] array in PID class. 

###### Effect of P of the controller and how its chosen

We first want to get the P part, which is very important. So I did this by setting the other two params in the dp[] array to 0. Also TWIDDLE_START_POS was set to one, and twiddle_check() method loops were adjusted accordingly. 
I also tried it at various speeds of 10 miles/h, 20 miles/h,  40 m/h and going upto 60 m/hr. 
The P value for each speed was slightly different. This is because there is a greater <b>push</b> at higher speed and the car swerves more. 

<video width="320" height="240" controls>
<source src="/pid_train.mp4" type="video/mp4">
You can also see this short <a href = "/pid_train.mp4"> Video of training </a> on how twiddle is used with Reset.
</video>

###### Effect of D(ifferential) part in the PID

The differntial smoothens the oscillating behavior. When the car is run with just P part, it continously dances along the way. Where as with the D part, the values shown in table below it runs relatively smoothly.

###### Effect of I(ntegral) part in the PID

The integral part takes care of Car flaws like steering drift. 
By setting the steering value to 0 radians and taking around 20 readings. It is observed that the steering drift is .44 degrees. 
This was calculated the last, when we had stable P and I values. And it definitely improved the error.

###### PID table learnt using Twiddle

Below table shows all the values of PID at different speeds. 

<table>
<tr>
<th>Speed(Miles/hour)</th>
<th>Tau-p</th>
<th>Tau-d</th>
<th>Tau-i</th>
</tr>
<tr>
<td>10</td>
<td>5.05822</td>
<td>1.44926</td>
<td>0</td>
</tr>
<tr>
<td>20</td>
<td>0.945085</td>
<td>7.16176</td>
<td>0</td>
</tr>
</tr>
<tr>
<td>30</td>
<td>0.216085</td>
<td>5.63086</td>
<td>0</td>
</tr>
</tr>
<tr>
<td>40</td>
<td>0.945085</td>
<td>7.16176</td>
<td>0.000300551</td>
</tr>
</table>


##### Drive mode

In the drive mode, the main program is run without any arguments as: 

<code>./pid</code>

Before that of course the p[] values learnt in the training mode for the speed are applied. 
The car was seen to drive comfortably ar 40 miles/hr and also at 50 miles/hr. 

At 60 miles/hr. the car was seen to go off track, at a very curvy place at the end of the loop. 

The present code works fine for 40 miles/hr. (Since the simulator's record option wasn't working on Linux, I could take only one video with my phone to capture the training part)


##### Other Observations/Suggestions
The training for searching of PID values takes a significant amount of time. And it heats the laptop. So it would be better if we can find a way, for future students, to train using a command line version of the simulator. And just drive on the simulator. This will give options to run on cloud machines.


### Appendix

Some other notes made during the training part are below.

#### twiddle
At first, I tried to use the twiddle python code done in the previous exercise to get the values of parameters. But soon I realized PID values should be learnt in the actual system. An approximation, even at same speeds yields totally differnt results. 
The python code is in <a href="/python/twiddle.py"> twiddle.py </a>


#### Record of various best p values using twiddle, changing speed and no. of steps in simulation

1)
error: 0.119131 (speed 10 miles/hr)
corresponding p: 5.05822, 1.44926, 0

2) error 49.75 (speed 30 miles/hr
0.121577, 0, 0

3) speed: 20 miles/hr
  error: 6.37
  p: 0.945085, 7.16176, 0

4) speed 30 miles/hr
best so far 58.4071(58.3905; best_p= 0.216085, 5.63086, 0)
Using 600 steps

5) target speed 40 miles/hr
best so far 70.5631(70.4391; best_p= 0.445619, 6.58267, 0)

5b) 40 miles/hr (with 10th Aug changes) -- 600 steps
95.2339; best_p= 0.983737, 6.58267, 0 (Just P)
94.7971; best_p= 0.983737, 6.00883, 0 (ID)
64.8928; best_p= 0.983737, 6.00883, 0.000300551(Just I)
54.856;  best_p= 0.983737, 12.09, 0.000300551 (Just D)
37.2432; best_p= 1.93066, 12.09, 0.000300551 (Just P)

6) Trying again afresh: 
target: 40 miles/hr
142.015(141.39; best_p= 16.0899, 0, 0.000300551 (Just P)






