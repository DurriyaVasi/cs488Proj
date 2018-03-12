Compilation:

Run in cs488 directory
To run, use the foloowing commands:

premake4 gmake
make clean
make
./A3 ./Assets/puppet.lua	

Used computer gl34.student.cs.uwaterloo.ca


Manual:

Did not complete trackball rotation
For invalid undo/redo, print message to cerr that is shown in terminal.

Additional Documentation:

The file graphDiagram.jpg is include in the Assets folder, and has details of my puppet's hierarchial model.

I changed JointNode so it has 2 more properties to track the current angle, I made it rotate by the init angle on construction, I added my own rotation function that limits angles, and I added a reset function to reset joint to intial angle.

