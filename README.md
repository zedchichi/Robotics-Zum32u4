# Robotics-Zum32u4
Use a Pololu Zumo 32U4 - robot to simulate an automated parcel delivery service. The scenario for this assignment is that the robot is trying to locate the house where a parcel is to be delivered. 

Open the files with the Arduino IDE

Plug in the Zumo 32u4 into the 	USB Port on your laptop.
Select the Arduino Leonardo on the port selection.

Upload the First file to run scenario 1.
Upload the Second file to run scenario 2.
Upload the Third file to run scenario 3.

Start the Zumo by switching the On switch.

If there is an LCD screen the zumo will show a PRESS A.

Press the A button and the Zumo will start the uploaded Scenario, the Yellow LED light will turn OFF and then the robot will start moving immediately.

If there is no LCD Screen attached on the Zumo, once the YelloW LED light turns ON, then PRESS A to start the uploaded scenario, the Yellow LED light will turn OFF and then the robot will start moving immediately.

#Scenario 1
The robot will make sure that no delivery has been made.

If no delivery has been made then the robot will navigate the maze trying to find a house while also recording the path. 

Once a house is detected the robot will perform the delivery by playing a sound. 

After the delivery the robot will, reverse the path it took, turn around and follow the reversed path back to its starting position.

#Scenario 2
The robot will make sure that no delivery has been made.

If no delivery has been made then the robot will navigate the maze trying to find a house while also recording the path.

The robot will record the number of houses it has found and keep going around the maze till it finds more houses.

For every house the robot has found it will play a different sound and also display what number house it has found.

Once it has found at least 2 houses the robot will generate the return path start to go back to the start position via the return path. 

Note: The robot assumes that there are only 2 houses it has to delivery to.

#Scenario 3
The robot will make sure that no delivery has been made.

If no delivery has been made then the robot will start to explore the maze, while also detecting houses and performing deliveries.

As the robot is exploring the maze and performing delievry the path is being recorded.

For this scenario, the robot will keep exploring till the number of houses it has detected all the houses.

Once all the houses have been detected, the  robot will optimise the path that has been recorded to try an find the shortest path.

It will then follow the optimised path around the maze.


