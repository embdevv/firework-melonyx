#include <glad/gl.h>
#include "../include/engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

/**
 * @brief Update the particle simulation by one timestep.
 *
 * If not paused, spawns new particles via continuousSpawn, then iterates
 * all active particles. Each particle's lifespan is decremented by dt.
 * Particles whose lifespan reaches zero are removed. Remaining particles
 * are updated using Euler integration: velocity is updated by acceleration,
 * then position is updated by the new velocity.
 *
 * @param dt Delta time in seconds (typically 1/60)
 */
void melonyx::update(float dt) {
    if (!paused) {
        // Spawn new particles at the configured rate
        continuousSpawn(dt);

        for (auto it = particles.begin(); it != particles.end(); ) {
            // Deduct elapsed time from particle lifespan
            it->lifespan -= dt;

            if (it->lifespan <= 0.0f) {
                // Remove expired particle from simulation
                it = particles.erase(it);
            }
            else {
                // Apply gravity: update velocity then position (Euler integration)
                it->velocity += it->acceleration * dt;
                it->position += it->velocity * dt;
                ++it;
            }
        }
    }
}

/**
 * @brief Spawn new particles at regular intervals to simulate a fountain.
 *
 * Accumulates elapsed time and spawns batches of particles whenever the
 * accumulated time exceeds the spawn interval. Each particle is given:
 * - A fixed spawn position near the bottom of the screen
 * - A random velocity with upward bias and wide horizontal spread
 * - A random RGB color
 * - A random lifespan between 1 and 10 seconds
 * - A random radius between 2 and 10 meters (per spec)
 *
 * @param dt Delta time in seconds
 */
void melonyx::continuousSpawn(float dt) {
    spawnTimer += dt;

    while (spawnTimer >= spawnInterval) {
        spawnTimer -= spawnInterval;

        for (int i = 0; i < spawnCount; ++i) {
            // Spawn at elevated point near the bottom of the 800m world
            glm::vec3 pos(0.0f, -280.0f, 0.0f);

            // Random velocity: wide horizontal spread, upward bias
            glm::vec3 vel(
                (rand() % 200 - 100) * 0.5f,   ///< X: -50 to +50 m/s
                (rand() % 100 + 50) * 0.8f,   ///< Y: 40 to 120 m/s upward
                (rand() % 200 - 100) * 0.5f    ///< Z: -50 to +50 m/s
            );

            // Random color in RGB 0-1 range
            glm::vec3 col(
                rand() % 100 / 100.0f,
                rand() % 100 / 100.0f,
                rand() % 100 / 100.0f
            );

            float life = 1.0f + (rand() % 9);   ///< Lifespan: 1-10 seconds
            float rad = 2.0f + (rand() % 9);   ///< Radius: 2-10 meters

            addParticle(Particle(pos, vel, life, rad, col));
        }
    }
}