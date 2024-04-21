# opengl-waterflow
This is a project on the rendering and simulation of water in real-time using particles as the representation for water. 

The rendering for the water is fully complete but is not incorperated with the simulation half of the project.

The physics simulation is done through compute shaders. Currently, only gravity affects the particles and there is no particle iteraction. 

## Building
If you would like to see the project for yourself, make sure to download / clone the project. The project uses CMake as a build tool, so make sure to have that as well. 
- Once the source code is downloaded / cloned using git, enter the file `build` and type `cmake -A Win32 ..` or `cmake ..` depending on if you use Windows or not.
- The binaries can be found in `build/Release` and `build/Debug`.

## Controls
To move aroun the scene, use WASD. Use Space and Shift to move vertically. The following are commands to activate various views:
- T: toggles the on-screen ms / frame timer
- G: toggles the grid for the sim
- V: toggles the grid-based velocities for the sim
- B: toggles the axis-aligned velocities for the sim
- O: toggles the origin
- N: toggles the particles for the sim
- P: toggles the simulation (starts paused)
- Comma / Period: cycles the simulation type between SEQ_GRID, SEQ_PARTICLE, and GPU_PARTICLE

Changing the number of particles and grid dimension is currently done manually in code.

## References and Resources
### Simulation and Rendering
[Fluid Simulation for Dummies](https://mikeash.com/pyblog/fluid-simulation-for-dummies.html), by Mike Ash.

[Ten Minute Physics](https://www.youtube.com/@TenMinutePhysics) YouTube channel

[Animating Sand As a Fluid](https://www.cs.ubc.ca/~rbridson/docs/zhu-siggraph05-sandfluid.pdf), by Yongning Zhu and Robert Bridson

[GDC 2010 - Screen Space Fluid Rendering for Games](https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf), by Simon Green

[A Gentle Introduction to Realtime Fluid Simulation](https://shahriyarshahrabi.medium.com/gentle-introduction-to-fluid-simulation-for-programmers-and-technical-artists-7c0045c40bac), by Shahriar Shahrabi


### Setup for CMake with Opengl and other libraries: 
[How to setup OpenGL project with CMake](https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake/), by Tomasz Galaj
