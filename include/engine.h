#pragma once
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include "particle.h"

/**
 * @struct SphereMesh
 * @brief Stores mesh data for a sphere: vertices, indices, and GPU resources.
 */
struct SphereMesh {
    std::vector<glm::vec3> vertices;    ///< Vertex positions
    std::vector<uint32_t> indices;      ///< Triangle indices
    uint32_t VAO = 0;                   ///< Vertex Array Object
    uint32_t VBO = 0;                   ///< Vertex Buffer Object
    uint32_t EBO = 0;                   ///< Element Buffer Object
};

/**
 * @class melonyx
 * @brief The core particle physics engine (named "Melonyx").
 * 
 * Manages particle simulation, continuous spawning for fountain effect,
 * rendering mesh generation, and pause functionality.
 */
class melonyx {
private:
    std::vector<Particle> particles;    ///< Active particles in simulation
    bool paused = false;                ///< Pause state
    float spawnTimer = 0.0f;            ///< Time accumulator for spawning
    const float spawnInterval = 0.05f;  ///< Spawn period (20/sec)
    int spawnCount = 10;                ///< Particles per spawn

public:
    SphereMesh sphereMesh;              ///< Shared sphere mesh for rendering

    /**
     * @brief Set the spawn rate (particles per second).
     * @param particlesPerSecond Target spawn rate
     */
    void setSpawnRate(int particlesPerSecond) {
        spawnCount = particlesPerSecond / 20;
        if (spawnCount < 1) spawnCount = 1;
    }

    /**
     * @brief Add a particle to the engine.
     * @param p Particle to add
     */
    void addParticle(const Particle& p) { particles.push_back(p); }
    
    /**
     * @brief Toggle pause state.
     */
    void togglePause() { paused = !paused; }
    
    /**
     * @brief Update simulation by one timestep.
     * @param dt Delta time in seconds
     */
    void update(float dt);
    
    /**
     * @brief Spawn new particles at regular intervals.
     * @param dt Delta time in seconds
     */
    void continuousSpawn(float dt);
    
    /**
     * @brief Get all active particles.
     * @return Const reference to particle vector
     */
    const std::vector<Particle>& getParticles() const { return particles; }
    
    /**
     * @brief Check if simulation is paused.
     * @return True if paused
     */
    bool isPaused() const { return paused; }
    
    /**
     * @brief Generate a UV sphere mesh.
     * @param radius Sphere radius (default 1.0)
     * @param sectorCount Longitude segments (default 32)
     * @param stackCount Latitude segments (default 32)
     * @return Generated mesh
     */
    static SphereMesh generateSphereMesh(float radius = 1.0f, int sectorCount = 32, int stackCount = 32);
    
    /**
     * @brief Setup GPU resources (VAO, VBO, EBO) for mesh.
     * @param mesh Mesh to setup
     */
    static void setupMeshVAO(SphereMesh& mesh);
};