#include "FluidCube.hpp"
#include <stdlib.h>

FluidCube::FluidCube(int size, int diffusion, int viscosity, float dt) {
    this->size = size;
    this->dt = dt;
    this->diff = (float) diffusion;
    this->visc = (float) viscosity;

    int N = size;
    this->s = (float*) calloc(N * N * N, sizeof(float));
    this->density = (float*) calloc(N * N * N, sizeof(float));

    this->Vx = (float*) calloc(N * N * N, sizeof(float));
    this->Vy = (float*) calloc(N * N * N, sizeof(float));
    this->Vz = (float*) calloc(N * N * N, sizeof(float));

    this->Vx0 = (float*) calloc(N * N * N, sizeof(float));
    this->Vy0 = (float*) calloc(N * N * N, sizeof(float));
    this->Vz0 = (float*) calloc(N * N * N, sizeof(float));
}

FluidCube::~FluidCube() {
    free(this->s);
    free(this->density);

    free(this->Vx);
    free(this->Vy);
    free(this->Vz);

    free(this->Vx0);
    free(this->Vy0);
    free(this->Vz0);
}

void FluidCube::AddDyeDensity(int x, int y, int z, float amount) 
{
    int N = this->size;
    this->density[INDEX_3TO1(x, y, z)] += amount;
}

void FluidCube::AddVelocity(int x, int y, int z, 
    float amountX, float amountY, float amountZ) 
{
    int N = this->size;
    int index = INDEX_3TO1(x, y, z);

    this->Vx[index] += amountX;
    this->Vy[index] += amountY;
    this->Vz[index] += amountZ;
}