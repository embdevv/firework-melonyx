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
// Reads shader source code from files with multiple fallback path options
std::string readShaderFile(const std::string& filename) {
    // Try multiple paths relative to execution location (for development and release)
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

    // Read entire file into memory
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::string content(size, ' ');
    fread(&content[0], 1, size, file);
    fclose(file);
    return content;
}

// Compiles a single shader (vertex or fragment) from source code
bool compileShader(GLuint shader, const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
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

// Creates the complete shader program from vertex and fragment shaders
bool createShaderProgram() {
    // Load shader source code from files
    std::string vertexSrc = readShaderFile("shaders/particle.vert");
    std::string fragmentSrc = readShaderFile("shaders/particle.frag");

    if (vertexSrc.empty() || fragmentSrc.empty()) {
        return false;
    }

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!compileShader(vertexShader, vertexSrc)) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(fragmentShader, fragmentSrc)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Link shaders into complete program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
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

    // Clean up individual shaders (program has copies)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

// ====== Input Processing ======
// Handles all keyboard input and state management
void processInput(GLFWwindow* win) {
    // Pause/Resume: Detect single press (not continuous) using state tracking
    static bool spacePrev = false;
    bool spaceNow = glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceNow && !spacePrev) {
        engine.togglePause();
    }
    spacePrev = spaceNow;

    // Camera projection switching
    if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS) cam.isPerspective = false;  // Orthographic
    if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS) cam.isPerspective = true;   // Perspective

    // Camera rotation around fountain (continuous input)
    float rotateDx = 0.0f, rotateDy = 0.0f;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) rotateDx -= 1.0f;  // Rotate left
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) rotateDx += 1.0f;  // Rotate right
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) rotateDy += 1.0f;  // Zoom out / tilt up
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) rotateDy -= 1.0f;  // Zoom in / tilt down

    if (rotateDx != 0.0f || rotateDy != 0.0f) {
        cam.rotateAroundPoint(rotateDx, rotateDy);
    }
}

// ====== Main ======
// Application entry point and main loop
int main() {
    // Get user input: how many particles to spawn per second
    int sparkCount = 0;
    std::cout << "Enter number of sparks to simulate per second: ";
    std::cin >> sparkCount;

    if (sparkCount <= 0) {
    std::cerr << "Spark count must be positive.\n";
    return -1;
    }

    // Initialize GLFW window system
    if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
    }

    // Request OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create 800x800 window with engine name in title
    GLFWwindow* win = glfwCreateWindow(800, 800, "GDPHYSX – Particle Demo", nullptr, nullptr);
    if (!win) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return -1;
    }

    // Make window context current and enable vsync
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    // Load OpenGL function pointers with Glad
    if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize Glad\n";
    glfwDestroyWindow(win);
    glfwTerminate();
    return -1;
    }

    // Enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Compile and link shader program
    if (!createShaderProgram()) {
    std::cerr << "Failed to create shader program\n";
    glfwDestroyWindow(win);
    glfwTerminate();
    return -1;
    }

    // Generate and upload sphere mesh to GPU
    engine.sphereMesh = melonyx::generateSphereMesh(1.0f, 32, 32);
    melonyx::setupMeshVAO(engine.sphereMesh);

    // Set particle spawn rate (converts per-second count to internal spawn interval)
    engine.setSpawnRate(sparkCount);

    // Fixed timestep: 60 FPS target
    const float dt = 1.0f / 60.0f;

    // Main rendering loop
    while (!glfwWindowShouldClose(win)) {
    // Handle input and update camera
    processInput(win);
        
    // Update physics simulation
    engine.update(dt);

    // Clear color and depth buffers
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);  // Dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use compiled shader program
    glUseProgram(shaderProgram);

    // Get view and projection matrices from camera
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    // Get uniform locations in shader
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "particleColor");

    // Set view and projection uniforms (same for all particles)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    /**
     * @brief Render particles to screen.
     * Each particle is rendered as a sphere with position, radius, and color.
     */
    // Render particles
    glBindVertexArray(engine.sphereMesh.VAO);
    for (const auto& particle : engine.getParticles()) {
        // Calculate model matrix: translate to particle position, scale by radius
        glm::mat4 model = particle.getModelMatrix();
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
        // Set particle color uniform
        glUniform3f(colorLoc, particle.color.r, particle.color.g, particle.color.b);
            
        // Draw sphere for this particle
        glDrawElements(GL_TRIANGLES, engine.sphereMesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // Swap front/back buffers and poll events
    glfwSwapBuffers(win);
    glfwPollEvents();
    }

    // Cleanup GPU resources
    if (engine.sphereMesh.VAO) glDeleteVertexArrays(1, &engine.sphereMesh.VAO);
    if (engine.sphereMesh.VBO) glDeleteBuffers(1, &engine.sphereMesh.VBO);
    if (engine.sphereMesh.EBO) glDeleteBuffers(1, &engine.sphereMesh.EBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);

    // Cleanup GLFW
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}