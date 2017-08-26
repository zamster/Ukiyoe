#include "wave.h"

#include <sys/time.h>

#include <iostream>

#define WIDTH       20 * 3
#define HEIGHT      15 * 3
#define DEPTH       10 * 2

FluidMaterial material(1000.0f, 0.1f, 1.2f, 1.0f, 1.0f);
SphFluidSolver solver(WIDTH, HEIGHT, DEPTH, 1.5f, 0.01f, material);
const float gravity = 15.0f;
const float scale = 1.0f;
float collision_restitution = 1.1f;

Vector3f gravity_direction;

Wave::Wave(float _x, float _y, float _z)
{
    xPos = _x;
    yPos = _y;
    zPos = _z;

    voxels.clear();

    gravity_direction.x = 0;
    gravity_direction.y = -1;
    gravity_direction.z = 0;
    gravity_direction = normalize(gravity_direction);

    Particle *particles = new Particle[8192];

    int count = 8192;
    Particle *particle_iter = particles;
    while (true)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            for (int k = 0; k < DEPTH; k++)
            {
                for (int i = 0; i < WIDTH; i++)
                {
                    if (count-- == 0)
                    {
                        solver.init_particles(particles, 8192);
                        return;
                    }

                    particle_iter->position.x = i / scale;
                    particle_iter->position.y = j / scale;
                    particle_iter->position.z = k / scale;
                    particle_iter++;
                }
            }
        }
    }
}

void add_gravity_force(Particle &particle)
{
    particle.force += gravity * gravity_direction * particle.density;
}

void add_global_forces()
{
    solver.foreach_particle(add_gravity_force);
}

void handle_particle_collision_cube(Particle &particle)
{
    static float alpha = 0;
    float test = WIDTH + sin(alpha * 3.14 / 180) * 30 - particle.position.y * particle.position.y / 80;
    alpha += 0.00005;

    if (alpha >= 360)
    {
        alpha = 0;
    }

    float &px = particle.position.x;
    float &py = particle.position.y;
    float &pz = particle.position.z;

    float &vx = particle.velocity.x;
    float &vy = particle.velocity.y;
    float &vz = particle.velocity.z;

    if (px < 0 || px > test / scale)
    {
        px = min(max(px, 0.0f), (float) test / scale);
        vx *= -collision_restitution;
    }
    if (py < 0 || py > HEIGHT / scale)
    {
        py = min(max(py, 0.0f), (float) HEIGHT / scale);
        vy *= -collision_restitution;
    }
    if (pz < 0 || pz > DEPTH / scale)
    {
        pz = min(max(pz, 0.0f), (float) DEPTH / scale);
        vz *= -collision_restitution;
    }
}

void handle_particle_collision_cylinder(Particle &particle) {
    Vector3f mid = Vector3f(WIDTH, 0.0f, DEPTH) / 2.0f;
    Vector3f distance = Vector3f(particle.position.x, 0.0f, particle.position.z) - mid;

    if (length(distance) >= WIDTH / 2) {
        distance = normalize(distance);

        particle.position.x = (mid + WIDTH / 2 * distance).x;
        particle.position.z = (mid + WIDTH / 2 * distance).z;

        particle.velocity -= 2.0f * dot(particle.velocity, distance) * distance;
    }

    if (particle.position.y >= HEIGHT - 1) {
        particle.position.y = HEIGHT - 1;
        particle.velocity.y *= -collision_restitution;
    } else if (particle.position.y < 0.0f) {
        particle.position.y = 0.0f;
        particle.velocity.y *= -collision_restitution;
    }
}

void handle_collisions()
{
    solver.foreach_particle(handle_particle_collision_cube);
}

void Wave::update()
{
    for (int i = 0; i < 2; ++i)
    {
        solver.update(add_global_forces, handle_collisions);
    }

    voxels.clear();

    for (int k = 0; k < solver.grid_depth; k++)
    {
        for (int j = 0; j < solver.grid_height; j++)
        {
            for (int i = 0; i < solver.grid_width; i++)
            {
                GridElement &grid_element = solver.grid_elements[solver.grid_width * (k * solver.grid_height + j) + i];

                list<Particle> &plist = grid_element.particles;
                for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
                {
                    Vector3f p = scale * piter -> position;
                    Voxel tmp;
                    tmp.scale = 32 / (piter -> density * 100);
                    tmp.pos = glm::vec3(xPos + p.x, yPos + p.y, zPos + p.z);
                    // tmp.color = glm::vec4(piter -> color_gradient.x, piter -> color_gradient.y, piter -> color_gradient.z, 1);
                    tmp.color = glm::vec4(31, 71, 136, 255) / 128.0f * length(piter -> color_gradient);
                    // if (length(piter -> color_gradient) > 0.5f)
                    // {
                        // tmp.color = glm::vec4(235, 246, 247, 255) / 255.0f;
                    // }
                    voxels.push_back(tmp);
                }
            }
        }
    }
}

Wave::~Wave()
{
}

#define PI_FLOAT                3.14

#define SQR(x)                  ((x) * (x))
#define CUBE(x)                 ((x) * (x) * (x))
#define POW6(x)                 (CUBE(x) * CUBE(x))
#define POW9(x)                 (POW6(x) * CUBE(x))

inline float SphFluidSolver::kernel(const Vector3f &r, const float h)
{
    return 315.0f / (64.0f * PI_FLOAT * POW9(h)) * CUBE(SQR(h) - dot(r, r));
}

inline Vector3f SphFluidSolver::gradient_kernel(const Vector3f &r, const float h)
{
    return -945.0f / (32.0f * PI_FLOAT * POW9(h)) * SQR(SQR(h) - dot(r, r)) * r;
}

inline float SphFluidSolver::laplacian_kernel(const Vector3f &r, const float h)
{
    return   945.0f / (32.0f * PI_FLOAT * POW9(h))
             * (SQR(h) - dot(r, r)) * (7.0f * dot(r, r) - 3.0f * SQR(h));
}

inline Vector3f SphFluidSolver::gradient_pressure_kernel(const Vector3f &r, const float h)
{
    if (dot(r, r) < SQR(0.001f))
    {
        return Vector3f(0.0f);
    }

    return -45.0f / (PI_FLOAT * POW6(h)) * SQR(h - length(r)) * normalize(r);
}

inline float SphFluidSolver::laplacian_viscosity_kernel(const Vector3f &r, const float h)
{
    return 45.0f / (PI_FLOAT * POW6(h)) * (h - length(r));
}

inline void SphFluidSolver::add_density(Particle &particle, Particle &neighbour)
{
    if (particle.id > neighbour.id)
    {
        return;
    }

    Vector3f r = particle.position - neighbour.position;
    if (dot(r, r) > SQR(core_radius))
    {
        return;
    }

    float common = kernel(r, core_radius);
    particle.density += neighbour.mass * common;
    neighbour.density += particle.mass * common;
}

void SphFluidSolver::sum_density(GridElement &grid_element, Particle &particle)
{
    list<Particle> &plist = grid_element.particles;
    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
    {
        add_density(particle, *piter);
    }
}

inline void SphFluidSolver::sum_all_density(int i, int j, int k, Particle &particle)
{
    for (int z = k - 1; z <= k + 1; z++)
    {
        for (int y = j - 1; y <= j + 1; y++)
        {
            for (int x = i - 1; x <= i + 1; x++)
            {
                if (   (x < 0) || (x >= grid_width)
                        || (y < 0) || (y >= grid_height)
                        || (z < 0) || (z >= grid_depth))
                {
                    continue;
                }

                sum_density(grid(x, y, z), particle);
            }
        }
    }
}

void SphFluidSolver::update_densities(int i, int j, int k)
{
    GridElement &grid_element = grid(i, j, k);

    list<Particle> &plist = grid_element.particles;
    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
    {
        sum_all_density(i, j, k, *piter);
    }
}

inline void SphFluidSolver::add_forces(Particle &particle, Particle &neighbour)
{
    if (particle.id >= neighbour.id)
    {
        return;
    }

    Vector3f r = particle.position - neighbour.position;
    if (dot(r, r) > SQR(core_radius))
    {
        return;
    }

    /* Compute the pressure force. */
    Vector3f common = 0.5f * material.gas_constant
                      * ((particle.density - material.rest_density) + (neighbour.density - material.rest_density))
                      * gradient_pressure_kernel(r, core_radius);
    particle.force += -neighbour.mass / neighbour.density * common;
    particle.pressure_force += -neighbour.mass / neighbour.density * common;
    neighbour.force -= -particle.mass / particle.density * common;
    neighbour.pressure_force -= -particle.mass / particle.density * common;

    /* Compute the viscosity force. */
    common = material.mu * (neighbour.velocity - particle.velocity)
             * laplacian_viscosity_kernel(r, core_radius);
    particle.force += neighbour.mass / neighbour.density * common;
    particle.viscosity_force += neighbour.mass / neighbour.density * common;
    neighbour.force -= particle.mass / particle.density * common;
    neighbour.viscosity_force -= particle.mass / particle.density * common;

    /* Compute the gradient of the color field. */
    common = gradient_kernel(r, core_radius);
    particle.color_gradient += neighbour.mass / neighbour.density * common;
    neighbour.color_gradient -= particle.mass / particle.density * common;

    /* Compute the laplacian of the color field. */
    float value = laplacian_kernel(r, core_radius);
    particle.color_laplacian += neighbour.mass / neighbour.density * value;
    neighbour.color_laplacian += particle.mass / particle.density * value;
}

void SphFluidSolver::sum_forces(GridElement &grid_element, Particle &particle)
{
    list<Particle>  &plist = grid_element.particles;
    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
    {
        add_forces(particle, *piter);
    }
}

void SphFluidSolver::sum_all_forces(int i, int j, int k, Particle &particle)
{
    for (int z = k - 1; z <= k + 1; z++)
    {
        for (int y = j - 1; y <= j + 1; y++)
        {
            for (int x = i - 1; x <= i + 1; x++)
            {
                if (   (x < 0) || (x >= grid_width)
                        || (y < 0) || (y >= grid_height)
                        || (z < 0) || (z >= grid_depth))
                {
                    continue;
                }

                sum_forces(grid(x, y, z), particle);
            }
        }
    }
}

void SphFluidSolver::update_forces(int i, int j, int k)
{
    GridElement &grid_element = grid(i, j, k);
    list<Particle> &plist = grid_element.particles;
    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
    {
        sum_all_forces(i, j, k, *piter);
    }
}

inline void SphFluidSolver::update_particle(Particle &particle)
{
    if (length(particle.color_gradient) > 0.001f)
    {
        particle.force +=   -material.sigma * particle.color_laplacian
                            * normalize(particle.color_gradient);
    }

    Vector3f acceleration =   particle.force / particle.density
                              - material.point_damping * particle.velocity / particle.mass;
    particle.velocity += timestep * acceleration;

    particle.position += timestep * particle.velocity;
}

void SphFluidSolver::update_particles(int i, int j, int k)
{
    GridElement &grid_element = grid(i, j, k);

    list<Particle> &plist = grid_element.particles;
    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
    {
        update_particle(*piter);
    }
}

inline void SphFluidSolver::reset_particle(Particle &particle)
{
    particle.density = 0.0f;
    particle.force = Vector3f(0.0f);
    particle.viscosity_force = Vector3f(0.0f);
    particle.pressure_force = Vector3f(0.0f);
    particle.color_gradient = Vector3f(0.0f);
    particle.color_laplacian = 0.0f;
}

void SphFluidSolver::reset_particles()
{
    for (int k = 0; k < grid_depth; k++)
    {
        for (int j = 0; j < grid_height; j++)
        {
            for (int i = 0; i < grid_width; i++)
            {
                GridElement &grid_element = grid(i, j, k);

                list<Particle> &plist = grid_element.particles;
                for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
                {
                    reset_particle(*piter);
                }
            }
        }
    }
}

inline void SphFluidSolver::insert_into_grid(int i, int j, int k)
{
    GridElement &grid_element = grid(i, j, k);

    list<Particle> &plist = grid_element.particles;
    for (list<Particle>::iterator piter = plist.begin(); piter != plist.end(); piter++)
    {
        add_to_grid(sleeping_grid_elements, *piter);
    }
}

void SphFluidSolver::update_grid()
{
    for (int k = 0; k < grid_depth; k++)
    {
        for (int j = 0; j < grid_height; j++)
        {
            for (int i = 0; i < grid_width; i++)
            {
                insert_into_grid(i, j, k);
                grid(i, j, k).particles.clear();
            }
        }
    }

    /* Swap the grids. */
    swap(grid_elements, sleeping_grid_elements);
}

void SphFluidSolver::update_densities()
{
    timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    for (int k = 0; k < grid_depth; k++)
    {
        for (int j = 0; j < grid_height; j++)
        {
            for (int i = 0; i < grid_width; i++)
            {
                update_densities(i, j, k);
            }
        }
    }

    gettimeofday(&tv2, NULL);
    int time = 1000 * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) / 1000;
    // printf("TIME[update_densities]: %dms\n", time);
}

void SphFluidSolver::update_forces()
{
    timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    for (int k = 0; k < grid_depth; k++)
    {
        for (int j = 0; j < grid_height; j++)
        {
            for (int i = 0; i < grid_width; i++)
            {
                update_forces(i, j, k);
            }
        }
    }

    gettimeofday(&tv2, NULL);
    int time = 1000 * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) / 1000;
    // printf("TIME[update_forces]   : %dms\n", time);
}

void SphFluidSolver::update_particles()
{
    timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    for (int k = 0; k < grid_depth; k++)
    {
        for (int j = 0; j < grid_height; j++)
        {
            for (int i = 0; i < grid_width; i++)
            {
                update_particles(i, j, k);
            }
        }
    }

    gettimeofday(&tv2, NULL);
    int time = 1000 * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) / 1000;
    // printf("TIME[update_particles]: %dms\n", time);
}

void SphFluidSolver::update(void(*inter_hook)(), void(*post_hook)())
{
    reset_particles();

    update_densities();
    update_forces();

    /* User supplied hook, e.g. for adding custom forces (gravity, ...). */
    if (inter_hook != NULL)
    {
        inter_hook();
    }

    update_particles();

    /* User supplied hook, e.g. for handling collisions. */
    if (post_hook != NULL)
    {
        post_hook();
    }

    update_grid();
}

void SphFluidSolver::init_particles(Particle *particles, int count)
{
    grid_elements = new GridElement[grid_width * grid_height * grid_depth];

    sleeping_grid_elements = new GridElement[grid_width * grid_height * grid_depth];

    for (int x = 0; x < count; x++)
    {
        particles[x].id = x;
        add_to_grid(grid_elements, particles[x]);
    }
}

inline GridElement &SphFluidSolver::grid(int i, int j, int k)
{
    return grid_elements[grid_index(i, j, k)];
}

inline GridElement &SphFluidSolver::sleeping_grid(int i, int j, int k)
{
    return sleeping_grid_elements[grid_index(i, j, k)];
}

inline int SphFluidSolver::grid_index(int i, int j, int k)
{
    return grid_width * (k * grid_height + j) + i;
}

inline void SphFluidSolver::add_to_grid(GridElement *target_grid, Particle &particle)
{
    int i = (int) (particle.position.x / core_radius);
    int j = (int) (particle.position.y / core_radius);
    int k = (int) (particle.position.z / core_radius);
    target_grid[grid_index(i, j, k)].particles.push_back(particle);
}
