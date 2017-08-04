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

#### best p values using twiddle

1)
error: 0.119131 (speed 10 miles/hr)
corresponding p: 5.05822, 1.44926, 0

2) error 49.75 (speed 30 miles/hr
0.121577, 0, 0

3) speed: 20 miles/hr
  error: 6.11535
  p: 0.538877, 0.916183, 0