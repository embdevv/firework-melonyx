#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

/**
 * @class Camera
 * @brief Manages camera view and projection matrices with orbital movement.
 * 
 * Supports both orthographic and perspective projections.
 * The camera orbits around the world center (0, 0, 0) at a fixed height (5.0).
 */
class Camera {
private:
    float distance = 20.0f;             ///< Distance from orbital center
    float angle = 0.0f;                 ///< Orbital angle around Y-axis (radians)

public:
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 20.0f);  ///< Current camera position
    bool isPerspective = true;          ///< True for perspective, false for orthographic

    /**
     * @brief Rotate camera around the world center.
     * @param dx Horizontal rotation input (-1 to 1 scale)
     * @param dy Vertical distance change input (-1 to 1 scale)
     */
    void rotateAroundPoint(float dx, float dy) {
        angle += dx * 0.02f;
        distance = glm::max(5.0f, distance - dy * 0.1f);
        updatePosition();
    }

    /**
     * @brief Update camera position based on angle and distance.
     */
    void updatePosition() {
        position.x = distance * std::cos(angle);
        position.z = distance * std::sin(angle);
        position.y = 5.0f;
    }

    /**
     * @brief Get the view matrix (look from camera at world center).
     * @return View transformation matrix
     */
    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }

    /**
     * @brief Get the projection matrix (perspective or orthographic).
     * @return Projection transformation matrix
     */
    glm::mat4 getProjectionMatrix() {
        if (isPerspective)
            return glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    }
};