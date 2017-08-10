### Reflections on the project

#### Steering drift
By setting the steering value to 0 radians and taking around 20 readings. It is observed that the steering drift is .44 degrees. 

#### twiddle
We use the twiddle python code done in the previous exercise to get the values of parameters. 
Table below has parameter values for various speeds. 
We set the steering drift at .44
<table>
<tr>
<th>Speed(Miles/hour)</th>
<th>Tau-p</th>
<th>Tau-d</th>
<th>Tau-i</th>
<th>Total error</th>
</tr>
<tr>
<td>10</td>
<td>3</td>
<td>0</td>
<td>0</td>
<td>60.82</td>
</tr>
</table>

#### Approach

#### best p values using twiddle

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

6) target speed 60 miles/hr
