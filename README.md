# opengl-waterflow
This is a project on the rendering and simulation of water in real-time using particles as the representation for water. The rendering for the water is fully complete, however, the physics simulation is not yet complete. Currently, only gravity affects the particles and there is no particle iteraction. If you would like to see the project for yourself, you can download the source code and build using cmake. Once the source code is downloaded / copied using git, enter the file `build` and type `cmake -A Win32 ..` or `cmake ..` depending on if you use Windows or not. The binaries can be found in `build/Release` and `build/Debug`.

## References and Resources
### Simulation and Rendering
[Fluid Simulation for Dummies](https://mikeash.com/pyblog/fluid-simulation-for-dummies.html), by Mike Ash.
[Ten Minute Physics](https://www.youtube.com/@TenMinutePhysics) YouTube channel
[Animating Sand As a Fluid](https://www.cs.ubc.ca/~rbridson/docs/zhu-siggraph05-sandfluid.pdf), by Yongning Zhu and Robert Bridson
[GDC 2010 - Screen Space Fluid Rendering for Games](https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf), by Simon Green
[A Gentle Introduction to Realtime Fluid Simulation](https://shahriyarshahrabi.medium.com/gentle-introduction-to-fluid-simulation-for-programmers-and-technical-artists-7c0045c40bac), by Shahriar Shahrabi
### Setup for CMake with Opengl and other libraries: 
[How to setup OpenGL project with CMake](https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake/), by Tomasz Galaj
