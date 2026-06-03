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
            } else {
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
            glm::vec3 pos(0.0f, 0.0f, 0.0f);
            glm::vec3 vel(
                (rand() % 200 - 100) / 100.0f,
                (rand() % 200 + 100) / 100.0f,
                (rand() % 200 - 100) / 100.0f
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

SphereMesh melonyx::generateSphereMesh(float radius, int sectorCount, int stackCount) {
    SphereMesh mesh;
    std::vector<glm::vec3>& vertices = mesh.vertices;
    std::vector<uint32_t>& indices = mesh.indices;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2.0f - i * glm::pi<float>() / stackCount;
        float xy = radius * std::cos(stackAngle);
        float z = radius * std::sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2.0f * glm::pi<float>() / sectorCount;
            float x = xy * std::cos(sectorAngle);
            float y = xy * std::sin(sectorAngle);
            vertices.push_back(glm::vec3(x, z, y));
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return mesh;
}

void melonyx::setupMeshVAO(SphereMesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

