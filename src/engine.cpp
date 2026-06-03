#include <glad/gl.h>
#include "../include/engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

// ============================================================================
// PHYSICS UPDATE LOOP - Core simulation for all particles
// ============================================================================
void melonyx::update(float dt) {
    if (!paused) {
        // Continuously spawn new particles at regular intervals
        continuousSpawn(dt);

        // Update each particle in the simulation
        for (auto it = particles.begin(); it != particles.end(); ) {
            // Decrement particle lifespan based on elapsed time
            it->lifespan -= dt;

            // Remove particle if lifespan has expired
            if (it->lifespan <= 0.0f) {
                it = particles.erase(it);  // Erase and get next iterator
            } else {
                // Physics integration: Update velocity and position
                // v(t+dt) = v(t) + a*dt
                it->velocity += it->acceleration * dt;
                
                // Position update: p(t+dt) = p(t) + v(t+dt)*dt
                // (Using updated velocity for semi-implicit Euler integration)
                it->position += it->velocity * dt;
                
                ++it;  // Move to next particle
            }
        }
    }
}

// ============================================================================
// CONTINUOUS SPAWNING - Fountain effect with configurable spawn rate
// ============================================================================
void melonyx::continuousSpawn(float dt) {
    // Accumulate time since last spawn burst
    spawnTimer += dt;
    
    // Check if it's time to spawn a new batch of particles
    while (spawnTimer >= spawnInterval) {
        spawnTimer -= spawnInterval;  // Reset timer for next batch
        
        // Spawn multiple particles per batch
        for (int i = 0; i < spawnCount; ++i) {
            // Spawn position: Bottom-center of screen (elevated spawning point)
            glm::vec3 pos(0.0f, -280.0f, 0.0f);
            
            // Randomized initial velocity for fountain spread
            // X and Z: ±10 m/s horizontal spread
            // Y: +6 to +9 m/s upward for fountain arc
            glm::vec3 vel(
                (rand() % 200 - 100) * 0.1f,   // X velocity: -10 to +10
                (rand() % 30 + 60) * 1.0f,     // Y velocity: 60 to 90
                (rand() % 200 - 100) * 0.1f    // Z velocity: -10 to +10
            );
            
            // Randomized particle color (full RGB spectrum)
            glm::vec3 col(
                rand() % 100 / 100.0f,  // Red: 0.0 to 1.0
                rand() % 100 / 100.0f,  // Green: 0.0 to 1.0
                rand() % 100 / 100.0f   // Blue: 0.0 to 1.0
            );
            
            // Randomized lifespan: 1 to 10 seconds
            float life = 1.0f + (rand() % 9);
            
            // Randomized radius: 2 to 10 meters
            float rad = 2.0f + (rand() % 9);

            // Create and add particle to engine
            addParticle(Particle(pos, vel, life, rad, col));
        }
    }
}

// ============================================================================
// SPHERE MESH GENERATION - UV sphere using sector/stack algorithm
// ============================================================================
SphereMesh melonyx::generateSphereMesh(float radius, int sectorCount, int stackCount) {
    SphereMesh mesh;
    std::vector<glm::vec3>& vertices = mesh.vertices;
    std::vector<uint32_t>& indices = mesh.indices;

    // Generate vertices using spherical coordinates
    for (int i = 0; i <= stackCount; ++i) {
        // Stack angle: from +π/2 (top) to -π/2 (bottom)
        float stackAngle = glm::pi<float>() / 2.0f - i * glm::pi<float>() / stackCount;
        float xy = radius * std::cos(stackAngle);  // Radius at this latitude
        float z = radius * std::sin(stackAngle);   // Z coordinate (height)

        // Generate vertices around the horizontal ring at this latitude
        for (int j = 0; j <= sectorCount; ++j) {
            // Sector angle: 0 to 2π around the sphere
            float sectorAngle = j * 2.0f * glm::pi<float>() / sectorCount;
            float x = xy * std::cos(sectorAngle);  // X on the horizontal circle
            float y = xy * std::sin(sectorAngle);  // Y on the horizontal circle
            
            // Store vertex (reordered for proper orientation)
            vertices.push_back(glm::vec3(x, z, y));
        }
    }

    // Generate triangle indices from vertices
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);      // First vertex of current stack
        int k2 = k1 + sectorCount + 1;       // First vertex of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // Generate two triangles per quad
            
            // First triangle: k1, k2, k1+1 (skip top cap)
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            // Second triangle: k1+1, k2, k2+1 (skip bottom cap)
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return mesh;
}

// ============================================================================
// GPU MESH SETUP - Initialize VAO, VBO, and EBO for rendering
// ============================================================================
void melonyx::setupMeshVAO(SphereMesh& mesh) {
    // Create GPU resources: Vertex Array Object, Vertex Buffer, Index Buffer
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    // Bind VAO (captures subsequent buffer bindings and attribute setup)
    glBindVertexArray(mesh.VAO);
    
    // Upload vertex position data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);

    // Upload index data to GPU (defines triangle connectivity)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    // Configure vertex attribute pointer: position attribute (location 0)
    // 3 floats per vertex, tightly packed in memory
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind buffers (VAO still retains the binding)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

