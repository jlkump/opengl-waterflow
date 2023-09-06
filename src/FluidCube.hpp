#ifndef FLUID_CUBE
#define FLUID_CUBE

// Converts a 3D position into a 1D position in a large array. This makes it
// easier to handle 3D positions while maintaining locality.
#define INDEX_3TO1(x, y, z) ((x) + (y) * N + (z) * N * N)

// TODO: Define the cube in a data-oriented way, which optimizes cache locality
// Do this using a index into an array of the struct's values instead
// Kinda realized this is already happening with the initialization of arrays within the struct :/
/**
* 
* 
*/
class FluidCube {
private:
    int _size;
    float _dt;
    float _diff;
    float _visc;

    float* _s;
    float* _density;

    float* _Vx;
    float* _Vy;
    float* _Vz;

    float* _Vx0;
    float* _Vy0;
    float* _Vz0;

public:
    FluidCube(int size, int diffusion, int viscosity, float dt);
    ~FluidCube();

    void AddDyeDensity(int x, int y, int z, float amount);

    void AddVelocity(int x, int y, int z, 
        float amountX, float amountY, float amountZ);

    void Step();

    static void Diffuse(int b, float *x, float *x0, float diff, float dt, int iter, int N);

    static void Project(float* velocX, float* velocY, float* velocZ, float* p, float* div, int iter, int N);

    static void Advect(int b, float *d, float *d0, float *velocX, float *velocY, float *velocZ, float dt, int N);

    // This will set the fluid values at boundaries of
    // the box, ensuring that the fluid does not leak out
    // When the boundary is not at the edge of the box, but
    // between a cell's neighbor, then we just produce
    // a velocity opposite
    // Otherwise we produce a velocity equal to 

    // b determines what kind of boundary condition we are dealing with
    // b == 1 is X
    // b == 2 is Y
    // b == 3 is Z
    // b == 4 is true boundary
    static void SetBoundary(int b, float* x, int N);

    static void LinearSolve(int b, float *x, float *x0, float a, float c, int iter, int N);
};


#endif