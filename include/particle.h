#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @class Particle
 * @brief Represents a single particle in the physics engine.
 * 
 * A particle maintains position, velocity, and acceleration for physics simulation.
 * It has a limited lifespan and a visual radius for rendering.
 */
class Particle {
public:
    glm::vec3 position;         ///< Current 3D position
    glm::vec3 velocity;         ///< Velocity vector (m/s)
    glm::vec3 acceleration;     ///< Acceleration vector (always gravity -10 m/s² on Y)
    glm::vec3 color;            ///< RGB color (0-1 range)
    float lifespan;             ///< Remaining time before removal (seconds)
    float radius;               ///< Visual radius for rendering (meters)
    bool active = true;         ///< Whether particle is active

    /**
     * @brief Construct a new Particle.
     * @param pos Initial position
     * @param vel Initial velocity
     * @param life Lifespan in seconds
     * @param rad Radius for rendering
     * @param col Color (RGB 0-1)
     */
    Particle(const glm::vec3& pos, const glm::vec3& vel,
             float life, float rad, const glm::vec3& col)
        : position(pos), velocity(vel), lifespan(life),
          radius(rad), color(col), acceleration(0.0f, -10.0f, 0.0f) {}

    /**
     * @brief Get the model transformation matrix for this particle.
     * @return Model matrix: translate to position, scale by radius
     */
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(radius));
        return model;
    }
};