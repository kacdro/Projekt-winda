# Projekt-winda

Elevator Simulation Project
This project is a simulation of an elevator system implemented in C++ using the ImGui library for the user interface. It provides a visual representation of an elevator and allows users to interact with it.

Features

Multiple floors: 
The elevator supports multiple floors, and users can select their desired floor for the elevator to move to.
Passenger management: Users can simulate passengers entering and exiting the elevator on different floors.
Queue management: The elevator intelligently manages its queue of floors to visit based on passenger requests.

Visualization: 
The elevator's current position, direction, and passenger status are visually represented in the user interface.

Requirements:
CMake 3.5 or higher
C++17 compatible compiler
ImGui library (already included as a submodule in the project)

Libraries used in project:
inlcude OpenGL glad GLFW imgui stb 
include algorithm
include chrono
include cmath

Usage:
Press the "Add Passenger" button to simulate a passenger requesting the elevator on a specific floor.
Select a floor from the drop-down menu to request the elevator to move to that floor.
The elevator's current position, direction, and passenger information are displayed in the user interface.





