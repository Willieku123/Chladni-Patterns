# Chladni-Patterns
Developed by Willie Ku and Supreme Leader Chen KB.
## What is Chladni patterns?
Chladni patterns is a physics phenomenon created by applying constant vibration to a flexible plate.  

This project simulates the phenomenon by modeling the plate as two layers of grids, each vertex connected with adjacent verticies by a spring following Hooke's law. 
## Usage
We provide two inplementation of the simulation:
### Python
Requires python 2.7 and PIL. VPython 6 is needed for visualizing the simulation in 3D, manually delete `from visual import` if not desired. Runs pretty slow regardless lol.
Tested on Windows.
### C++
Runs faster. 
Requires OpenCV and CMake. Initialize CMake stuff with: 

    cmake . -DOpenCV_DIR="/path/to/opencv/build/folder"

To compile executable:

    make

Tested on MacOS, should work on Linux, probably also Windows. I'm not really sure. 


