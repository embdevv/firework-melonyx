#include <glad/gl.h>
#include "../include/engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

void melonyx::update(float dt) {
    if (!paused) {
        continuousSpawn(dt);

        for (auto it = particles.begin(); it != particles.end(); ) {
            it->lifespan -= dt;

            if (it->lifespan <= 0.0f) {
                it = particles.erase(it);
            }
            else {
                it->velocity += it->acceleration * dt;
                it->position += it->velocity * dt;
                ++it;
            }
        }
    }
}

void melonyx::continuousSpawn(float dt) {
    spawnTimer += dt;
    while (spawnTimer >= spawnInterval) {
        spawnTimer -= spawnInterval;
        for (int i = 0; i < spawnCount; ++i) {
            glm::vec3 pos(0.0f, -280.0f, 0.0f);
            glm::vec3 vel(
                (rand() % 200 - 100) * 0.5f,
                (rand() % 100 + 50) * 0.8f,
                (rand() % 200 - 100) * 0.5f
            );
            glm::vec3 col(
                rand() % 100 / 100.0f,
                rand() % 100 / 100.0f,
                rand() % 100 / 100.0f
            );
            float life = 1.0f + (rand() % 9);
            float rad = 2.0f + (rand() % 9);

            addParticle(Particle(pos, vel, life, rad, col));
        }
    }
}