#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include "particle.h"
#include "tiny_obj_loader.h"

/**
 * @class ObjMesh
 * @brief Loads and stores a mesh from a .obj file for GPU rendering.
 */
class ObjMesh {
public:
    std::vector<float> vertices;    ///< Interleaved position + normal data
    std::vector<unsigned> indices;  ///< Triangle indices
    uint32_t VAO = 0;               ///< Vertex Array Object
    uint32_t VBO = 0;               ///< Vertex Buffer Object
    uint32_t EBO = 0;               ///< Element Buffer Object

    /**
     * @brief Load mesh data from a .obj file.
     * @param path Path to the .obj file
     * @return True if loaded successfully
     */
    bool load(const std::string& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
            std::cerr << "Failed to load OBJ: " << err << "\n";
            return false;
        }

        for (auto& shape : shapes) {
            for (auto& idx : shape.mesh.indices) {
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
                if (idx.normal_index >= 0) {
                    vertices.push_back(attrib.normals[3 * idx.normal_index + 0]);
                    vertices.push_back(attrib.normals[3 * idx.normal_index + 1]);
                    vertices.push_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                else {
                    vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);
                }
                indices.push_back((unsigned)indices.size());
            }
        }
        return true;
    }

    /**
     * @brief Upload mesh data to GPU buffers.
     */
    void upload() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
};

/**
 * @class melonyx
 * @brief The core particle physics engine (named "Melonyx").
 *
 * Manages particle simulation, continuous spawning for fountain effect,
 * rendering via OBJ mesh, and pause functionality.
 */
class melonyx {
private:
    std::vector<Particle> particles;    ///< Active particles in simulation
    bool paused = false;                ///< Pause state
    float spawnTimer = 0.0f;            ///< Time accumulator for spawning
    const float spawnInterval = 0.05f;  ///< Spawn period (20 batches/sec)
    int spawnCount = 10;                ///< Particles per spawn batch

public:
    ObjMesh sphereMesh;                 ///< Shared sphere mesh for rendering

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
};