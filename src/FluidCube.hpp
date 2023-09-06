#ifndef FLUID_CUBE
#define FLUID_CUBE

// Converts a 3D position into a 1D position in a large array. This makes it
// easier to handle 3D positions while maintaining locality.
#define INDEX_3TO1(x, y, z) ((x) + (y) * N + (z) * N * N)

// TODO: Define the cube in a data-oriented way, which optimizes cache locality
// Do this using a index into an array of the struct's values instead
/**
* 
* 
*/
class FluidCube {
private:
    int size;
    float dt;
    float diff;
    float visc;

    float* s;
    float* density;

    float* Vx;
    float* Vy;
    float* Vz;

    float* Vx0;
    float* Vy0;
    float* Vz0;

public:
    FluidCube(int size, int diffusion, int viscosity, float dt);
    ~FluidCube();

    void AddDyeDensity(int x, int y, int z, float amount);

    void AddVelocity(int x, int y, int z, 
        float amountX, float amountY, float amountZ);
};


#endif // !FLUID_CUBE