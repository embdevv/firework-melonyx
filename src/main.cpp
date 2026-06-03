#include <iostream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../include/engine.h"
#include "../include/camera.h"

melonyx engine;
Camera cam;
GLuint shaderProgram = 0;

// ====== Shader Management ======
std::string readShaderFile(const std::string& filename) {
    // Try multiple paths relative to execution location
    const std::string paths[] = {
        filename,
        "../" + filename,
        "../../" + filename,
        "shaders/" + filename,
        "../shaders/" + filename,
        "../../shaders/" + filename
    };

    FILE* file = nullptr;
    for (const auto& path : paths) {
        file = fopen(path.c_str(), "r");
        if (file) break;
    }

    if (!file) {
        std::cerr << "Error: Could not open shader file: " << filename << "\n";
        return "";
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::string content(size, ' ');
    fread(&content[0], 1, size, file);
    fclose(file);
    return content;
}

bool compileShader(GLuint shader, const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << "\n";
        return false;
    }
    return true;
}

bool createShaderProgram() {
    std::string vertexSrc = readShaderFile("shaders/particle.vert");
    std::string fragmentSrc = readShaderFile("shaders/particle.frag");

    if (vertexSrc.empty() || fragmentSrc.empty()) {
        return false;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!compileShader(vertexShader, vertexSrc)) {
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(fragmentShader, fragmentSrc)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << "\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

// ====== Input Processing ======
void processInput(GLFWwindow* win) {
    static bool spacePrev = false;
    bool spaceNow = glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceNow && !spacePrev) {
        engine.togglePause();
    }
    spacePrev = spaceNow;

    if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS) cam.isPerspective = false;
    if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS) cam.isPerspective = true;

    float rotateDx = 0.0f, rotateDy = 0.0f;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) rotateDx -= 1.0f;
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) rotateDx += 1.0f;
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) rotateDy += 1.0f;
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) rotateDy -= 1.0f;

    if (rotateDx != 0.0f || rotateDy != 0.0f) {
        cam.rotateAroundPoint(rotateDx, rotateDy);
    }
}

// ====== Main ======
int main() {
    int sparkCount = 0;
    std::cout << "Enter number of sparks to simulate per second: ";
    std::cin >> sparkCount;

    if (sparkCount <= 0) {
        std::cerr << "Spark count must be positive.\n";
        return -1;
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 800, "GDPHYSX – Particle Demo", nullptr, nullptr);
    if (!win) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << "\n";
        glfwDestroyWindow(win);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    if (!createShaderProgram()) {
        std::cerr << "Failed to create shader program\n";
        glfwDestroyWindow(win);
        glfwTerminate();
        return -1;
    }

    // Generate sphere mesh
    engine.sphereMesh = melonyx::generateSphereMesh(1.0f, 32, 32);
    melonyx::setupMeshVAO(engine.sphereMesh);

    // Set spawn rate
    engine.setSpawnRate(sparkCount);

    const float dt = 1.0f / 60.0f;

    while (!glfwWindowShouldClose(win)) {
        processInput(win);
        engine.update(dt);

        // Clear buffers
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = cam.getViewMatrix();
        glm::mat4 projection = cam.getProjectionMatrix();

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint colorLoc = glGetUniformLocation(shaderProgram, "particleColor");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Render particles
        glBindVertexArray(engine.sphereMesh.VAO);
        for (const auto& particle : engine.getParticles()) {
            glm::mat4 model = particle.getModelMatrix();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, particle.color.r, particle.color.g, particle.color.b);
            glDrawElements(GL_TRIANGLES, engine.sphereMesh.indices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    if (engine.sphereMesh.VAO) glDeleteVertexArrays(1, &engine.sphereMesh.VAO);
    if (engine.sphereMesh.VBO) glDeleteBuffers(1, &engine.sphereMesh.VBO);
    if (engine.sphereMesh.EBO) glDeleteBuffers(1, &engine.sphereMesh.EBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}