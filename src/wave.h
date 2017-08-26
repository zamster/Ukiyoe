#ifndef WAVE_H_
#define WAVE_H_

#include <vector>
#include <list>
using namespace std;

#include "voxel.h"

struct Vector3f
{
    float   x, y, z;

    inline Vector3f()
    {
        x = y = z = 0.0f;
    }

    inline Vector3f(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    inline Vector3f(float xyz)
    {
        x = y = z = xyz;
    }

    inline Vector3f(const float *xyzArr)
    {
        x = xyzArr[0];
        y = xyzArr[1];
        z = xyzArr[2];
    }

    inline operator const float *() const
    {
        return ((const float *)&x);
    }

    inline float &operator[](unsigned int idx)
    {
        return (*(((float *)&x) + idx));
    }

    inline void operator +=(float s)
    {
        x += s;
        y += s;
        z += s;
    }

    inline void operator +=(const Vector3f &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    inline void operator -=(float s)
    {
        x -= s;
        y -= s;
        z -= s;
    }

    inline void operator -=(const Vector3f &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    inline void operator *=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    inline void operator *=(const Vector3f &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    inline void operator /=(float s)
    {
        float   inv = 1.0f / s;

        x *= inv;
        y *= inv;
        z *= inv;
    }

    inline void operator /=(const Vector3f &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
    }
};


inline Vector3f operator +(const Vector3f &v, float s)
{
    return (Vector3f(v.x + s, v.y + s, v.z + s));
}

inline Vector3f operator +(float s, const Vector3f &v)
{
    return (Vector3f(s + v.x, s + v.y, s + v.z));
}

inline Vector3f operator +(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x + v.x, u.y + v.y, u.z + v.z));
}

inline Vector3f operator -(const Vector3f &v, float s)
{
    return (Vector3f(v.x - s, v.y - s, v.z - s));
}

inline Vector3f operator -(float s, const Vector3f &v)
{
    return (Vector3f(s - v.x, s - v.y, s - v.z));
}

inline Vector3f operator -(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x - v.x, u.y - v.y, u.z - v.z));
}

inline Vector3f operator *(const Vector3f &v, float s)
{
    return (Vector3f(v.x * s, v.y * s, v.z * s));
}

inline Vector3f operator *(float s, const Vector3f &v)
{
    return (Vector3f(s * v.x, s * v.y, s * v.z));
}

inline Vector3f operator *(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x * v.x, u.y * v.y, u.z * v.z));
}

inline Vector3f operator /(const Vector3f &v, float s)
{
    float   inv = 1.0f / s;

    return (Vector3f(v.x * inv, v.y * inv, v.z * inv));
}

inline Vector3f operator /(float s, const Vector3f &v)
{
    return (Vector3f(s / v.x, s / v.y, s / v.z));
}

inline Vector3f operator /(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.x / v.x, u.y / v.y, u.z / v.z));
}

inline Vector3f operator -(const Vector3f &v)
{
    return (Vector3f(-v.x, -v.y, -v.z));
}

inline float dot(const Vector3f &u, const Vector3f &v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

inline Vector3f cross(const Vector3f &u, const Vector3f &v)
{
    return (Vector3f(u.y * v.z - v.y * u.z,
                     u.z * v.x - u.x * v.z,
                     u.x * v.y - u.y * v.x));
}

inline float length(const Vector3f &v)
{
    return ((float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline Vector3f normalize(const Vector3f &v)
{
    return (v / length(v));
}


struct Particle
{
    int id;
    float mass;
    float density;
    Vector3f position;
    Vector3f velocity;
    Vector3f force;
    Vector3f color_gradient;
    float color_laplacian;

    Vector3f viscosity_force;
    Vector3f pressure_force;

    Particle()
    {
        mass = 1.0f;
    }
};

struct GridElement
{
    list<Particle> particles;
};

struct FluidMaterial
{

    const float gas_constant;
    const float mu;
    const float rest_density;
    const float sigma;
    const float point_damping;

    FluidMaterial(
        float gas_constant,
        float mu,
        float rest_density,
        float sigma,
        float point_damping)
        : gas_constant(gas_constant),
          mu(mu),
          rest_density(rest_density),
          sigma(sigma),
          point_damping(point_damping)
    {
    }
};

class SphFluidSolver
{
public:
    const int grid_width;
    const int grid_height;
    const int grid_depth;

    const float core_radius;
    const float timestep;

    const FluidMaterial material;

    GridElement *grid_elements;
    GridElement *sleeping_grid_elements;

    SphFluidSolver(
        float domain_width,
        float domain_height,
        float domain_depth,
        float core_radius,
        float timestep,
        FluidMaterial material)
        : grid_width((int) (domain_width / core_radius) + 1),
          grid_height((int) (domain_height / core_radius) + 1),
          grid_depth((int) (domain_depth / core_radius) + 1),
          core_radius(core_radius),
          timestep(timestep),
          material(material)
    {

    }

    void update(void(*inter_hook)() = NULL, void(*post_hook)() = NULL);

    void init_particles(Particle *particles, int count);

    template <typename Function>
    void foreach_particle(Function function)
    {
        for (int k = 0; k < grid_depth; k++)
        {
            for (int j = 0; j < grid_height; j++)
            {
                for (int i = 0; i < grid_width; i++)
                {
                    GridElement &grid_element = grid_elements[grid_width * (k * grid_height + j) + i];

                    list<Particle> &plist = grid_element.particles;
                    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
                    {
                        function(*piter);
                    }
                }
            }
        }
    }

private:

    float kernel(const Vector3f &r, const float h);

    Vector3f gradient_kernel(const Vector3f &r, const float h);

    float laplacian_kernel(const Vector3f &r, const float h);

    Vector3f gradient_pressure_kernel(const Vector3f &r, const float h);

    float laplacian_viscosity_kernel(const Vector3f &r, const float h);

    void add_density(Particle &particle, Particle &neighbour);

    void sum_density(GridElement &grid_element, Particle &particle);

    void sum_all_density(int i, int j, int k, Particle &particle);

    void update_densities(int i, int j, int k);

    void add_forces(Particle &particle, Particle &neighbour);

    void sum_forces(GridElement &grid_element, Particle &particle);

    void sum_all_forces(int i, int j, int k, Particle &particle);

    void update_forces(int i, int j, int k);

    void update_particle(Particle &particle);

    void update_particles(int i, int j, int k);

    void reset_particle(Particle &particle);

    void reset_particles();

    void insert_into_grid(int i, int j, int k);

    void update_grid();

    void update_densities();

    void update_forces();

    void update_particles();

    GridElement &grid(int i, int j, int k);

    GridElement &sleeping_grid(int i, int j, int k);

    int grid_index(int i, int j, int k);

    void add_to_grid(GridElement *target_grid, Particle &particle);
};

class Wave
{
public:
    Wave(float _x, float _y, float _z);
    ~Wave();

    float xPos, yPos, zPos;
    vector<Voxel> voxels;
    void update();
};

#endif