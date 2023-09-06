#include "FluidCube.hpp"
#include <stdlib.h>
#include <math.h>

FluidCube::FluidCube(int size, int diffusion, int viscosity, float dt) {
    this->_size = size;
    this->_dt = dt;
    this->_diff = (float) diffusion;
    this->_visc = (float) viscosity;

    int N = size;
    this->_s = (float*) calloc(N * N * N, sizeof(float));
    this->_density = (float*) calloc(N * N * N, sizeof(float));

    this->_Vx = (float*) calloc(N * N * N, sizeof(float));
    this->_Vy = (float*) calloc(N * N * N, sizeof(float));
    this->_Vz = (float*) calloc(N * N * N, sizeof(float));

    this->_Vx0 = (float*) calloc(N * N * N, sizeof(float));
    this->_Vy0 = (float*) calloc(N * N * N, sizeof(float));
    this->_Vz0 = (float*) calloc(N * N * N, sizeof(float));
}

FluidCube::~FluidCube() {
    free(this->_s);
    free(this->_density);

    free(this->_Vx);
    free(this->_Vy);
    free(this->_Vz);

    free(this->_Vx0);
    free(this->_Vy0);
    free(this->_Vz0);
}

void FluidCube::AddDyeDensity(int x, int y, int z, float amount) 
{
    int N = _size;
    _density[INDEX_3TO1(x, y, z)] += amount;
}

void FluidCube::AddVelocity(int x, int y, int z, 
    float amountX, float amountY, float amountZ) 
{
    int N = _size;
    int index = INDEX_3TO1(x, y, z);

    _Vx[index] += amountX;
    _Vy[index] += amountY;
    _Vz[index] += amountZ;
}

void FluidCube::Step() 
{
    // Diffuse the velocities in each direction
    Diffuse(1, _Vx0, _Vx, _visc, _dt, 4, _size);
    Diffuse(2, _Vy0, _Vy, _visc, _dt, 4, _size);
    Diffuse(3, _Vz0, _Vz, _visc, _dt, 4, _size);

    // Ensure that the fluid stays incompressable
    Project(_Vx0, _Vy0, _Vz0, _Vx, _Vy, 4, _size);

    // Move velocities around so velocities themselves move
    Advect(1, _Vx, _Vx0, _Vx0, _Vy0, _Vz0, _dt, _size);
    Advect(2, _Vy, _Vy0, _Vx0, _Vy0, _Vz0, _dt, _size);
    Advect(3, _Vz, _Vz0, _Vx0, _Vy0, _Vz0, _dt, _size);

    // Ensure that the fluid stays incompressable
    Project(_Vx, _Vy, _Vz, _Vx0, _Vy0, 4, _size);

    // Diffuse the dye in the simulation
    Diffuse(0, _s, _density, _diff, _dt, 4, _size);
    // Move the dye around according to the velocities
    Advect(0, _density, _s, _Vx, _Vy, _Vz, _dt, _size);
}

void FluidCube::Diffuse(int b, float* x, float* x0, float diff, float dt, int iter, int N)
{
    float a = dt * diff * (N - 2) * (N - 2);
    LinearSolve(b, x, x0, a, 1 + 6 * a, iter, N);
}

void FluidCube::Project(float* velocX, float* velocY, float* velocZ, float* p, float* div, int iter, int N)
{
    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                div[INDEX_3TO1(i, j, k)] = -0.5f * (
                    velocX[INDEX_3TO1(i + 1, j, k)]
                    - velocX[INDEX_3TO1(i - 1, j, k)]
                    + velocY[INDEX_3TO1(i, j + 1, k)]
                    - velocY[INDEX_3TO1(i, j - 1, k)]
                    + velocZ[INDEX_3TO1(i, j, k + 1)]
                    - velocZ[INDEX_3TO1(i, j, k - 1)]
                    ) / N;
                p[INDEX_3TO1(i, j, k)] = 0;
            }
        }
    }
    SetBoundary(0, div, N);
    SetBoundary(0, p, N);
    LinearSolve(0, p, div, 1, 6, iter, N);

    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                velocX[INDEX_3TO1(i, j, k)] -= 0.5f * (p[INDEX_3TO1(i + 1, j, k)]
                    - p[INDEX_3TO1(i - 1, j, k)]) * N;
                velocY[INDEX_3TO1(i, j, k)] -= 0.5f * (p[INDEX_3TO1(i, j + 1, k)]
                    - p[INDEX_3TO1(i, j - 1, k)]) * N;
                velocZ[INDEX_3TO1(i, j, k)] -= 0.5f * (p[INDEX_3TO1(i, j, k + 1)]
                    - p[INDEX_3TO1(i, j, k - 1)]) * N;
            }
        }
    }
    SetBoundary(1, velocX, N);
    SetBoundary(2, velocY, N);
    SetBoundary(3, velocZ, N);
}

void FluidCube::Advect(int b, float* d, float* d0, float* velocX, float* velocY, float* velocZ, float dt, int N)
{
    float i0, i1, j0, j1, k0, k1;

    float dtx = dt * (N - 2);
    float dty = dt * (N - 2);
    float dtz = dt * (N - 2);

    float s0, s1, t0, t1, u0, u1;
    float tmp1, tmp2, tmp3, x, y, z;

    float Nfloat = N;
    float ifloat, jfloat, kfloat;
    int i, j, k;

    for (k = 1, kfloat = 1; k < N - 1; k++, kfloat++) {
        for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
            for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
                tmp1 = dtx * velocX[INDEX_3TO1(i, j, k)];
                tmp2 = dty * velocY[INDEX_3TO1(i, j, k)];
                tmp3 = dtz * velocZ[INDEX_3TO1(i, j, k)];
                x = ifloat - tmp1;
                y = jfloat - tmp2;
                z = kfloat - tmp3;

                if (x < 0.5f) x = 0.5f;
                if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
                i0 = floorf(x);
                i1 = i0 + 1.0f;
                if (y < 0.5f) y = 0.5f;
                if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
                j0 = floorf(y);
                j1 = j0 + 1.0f;
                if (z < 0.5f) z = 0.5f;
                if (z > Nfloat + 0.5f) z = Nfloat + 0.5f;
                k0 = floorf(z);
                k1 = k0 + 1.0f;

                s1 = x - i0;
                s0 = 1.0f - s1;
                t1 = y - j0;
                t0 = 1.0f - t1;
                u1 = z - k0;
                u0 = 1.0f - u1;

                int i0i = i0;
                int i1i = i1;
                int j0i = j0;
                int j1i = j1;
                int k0i = k0;
                int k1i = k1;

                d[INDEX_3TO1(i, j, k)] =

                    s0 * (t0 * (u0 * d0[INDEX_3TO1(i0i, j0i, k0i)]
                        + u1 * d0[INDEX_3TO1(i0i, j0i, k1i)])
                        + (t1 * (u0 * d0[INDEX_3TO1(i0i, j1i, k0i)]
                            + u1 * d0[INDEX_3TO1(i0i, j1i, k1i)])))
                    + s1 * (t0 * (u0 * d0[INDEX_3TO1(i1i, j0i, k0i)]
                        + u1 * d0[INDEX_3TO1(i1i, j0i, k1i)])
                        + (t1 * (u0 * d0[INDEX_3TO1(i1i, j1i, k0i)]
                            + u1 * d0[INDEX_3TO1(i1i, j1i, k1i)])));
            }
        }
    }
    SetBoundary(b, d, N);
}

void FluidCube::SetBoundary(int b, float* x, int N)
{
    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
            x[INDEX_3TO1(i, j, 0)] = b == 3 ? -x[INDEX_3TO1(i, j, 1)] : x[INDEX_3TO1(i, j, 1)];
            x[INDEX_3TO1(i, j, N - 1)] = b == 3 ? -x[INDEX_3TO1(i, j, N - 2)] : x[INDEX_3TO1(i, j, N - 2)];
        }
    }
    for (int k = 1; k < N - 1; k++) {
        for (int i = 1; i < N - 1; i++) {
            x[INDEX_3TO1(i, 0, k)] = b == 2 ? -x[INDEX_3TO1(i, 1, k)] : x[INDEX_3TO1(i, 1, k)];
            x[INDEX_3TO1(i, N - 1, k)] = b == 2 ? -x[INDEX_3TO1(i, N - 2, k)] : x[INDEX_3TO1(i, N - 2, k)];
        }
    }
    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            x[INDEX_3TO1(0, j, k)] = b == 1 ? -x[INDEX_3TO1(1, j, k)] : x[INDEX_3TO1(1, j, k)];
            x[INDEX_3TO1(N - 1, j, k)] = b == 1 ? -x[INDEX_3TO1(N - 2, j, k)] : x[INDEX_3TO1(N - 2, j, k)];
        }
    }
    x[INDEX_3TO1(0, 0, 0)] = 0.33f * (x[INDEX_3TO1(1, 0, 0)]
        + x[INDEX_3TO1(0, 1, 0)]
        + x[INDEX_3TO1(0, 0, 1)]);
    x[INDEX_3TO1(0, N - 1, 0)] = 0.33f * (x[INDEX_3TO1(1, N - 1, 0)]
        + x[INDEX_3TO1(0, N - 2, 0)]
        + x[INDEX_3TO1(0, N - 1, 1)]);
    x[INDEX_3TO1(0, 0, N - 1)] = 0.33f * (x[INDEX_3TO1(1, 0, N - 1)]
        + x[INDEX_3TO1(0, 1, N - 1)]
        + x[INDEX_3TO1(0, 0, N)]);
    x[INDEX_3TO1(0, N - 1, N - 1)] = 0.33f * (x[INDEX_3TO1(1, N - 1, N - 1)]
        + x[INDEX_3TO1(0, N - 2, N - 1)]
        + x[INDEX_3TO1(0, N - 1, N - 2)]);
    x[INDEX_3TO1(N - 1, 0, 0)] = 0.33f * (x[INDEX_3TO1(N - 2, 0, 0)]
        + x[INDEX_3TO1(N - 1, 1, 0)]
        + x[INDEX_3TO1(N - 1, 0, 1)]);
    x[INDEX_3TO1(N - 1, N - 1, 0)] = 0.33f * (x[INDEX_3TO1(N - 2, N - 1, 0)]
        + x[INDEX_3TO1(N - 1, N - 2, 0)]
        + x[INDEX_3TO1(N - 1, N - 1, 1)]);
    x[INDEX_3TO1(N - 1, 0, N - 1)] = 0.33f * (x[INDEX_3TO1(N - 2, 0, N - 1)]
        + x[INDEX_3TO1(N - 1, 1, N - 1)]
        + x[INDEX_3TO1(N - 1, 0, N - 2)]);
    x[INDEX_3TO1(N - 1, N - 1, N - 1)] = 0.33f * (x[INDEX_3TO1(N - 2, N - 1, N - 1)]
        + x[INDEX_3TO1(N - 1, N - 2, N - 1)]
        + x[INDEX_3TO1(N - 1, N - 1, N - 2)]);
}

void FluidCube::LinearSolve(int b, float* x, float *x0, float a, float c, int iter, int N)
{
    float cRecip = 1.0 / c;
    for (int k = 0; k < iter; k++) {
        for (int m = 1; m < N - 1; m++) {
            for (int j = 1; j < N - 1; j++) {
                for (int i = 1; i < N - 1; i++) {
                    x[INDEX_3TO1(i, j, m)] =
                        (x0[INDEX_3TO1(i, j, m)]
                            + a * (x[INDEX_3TO1(i + 1, j, m)]
                                + x[INDEX_3TO1(i - 1, j, m)]
                                + x[INDEX_3TO1(i, j + 1, m)]
                                + x[INDEX_3TO1(i, j - 1, m)]
                                + x[INDEX_3TO1(i, j, m + 1)]
                                + x[INDEX_3TO1(i, j, m - 1)]
                                )) * cRecip;
                }
            }
        }
        SetBoundary(b, x, N);
    }
}
