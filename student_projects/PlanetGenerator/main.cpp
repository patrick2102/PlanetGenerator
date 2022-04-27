#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <vector>

// NEW! as our scene gets more complex, we start using more helper classes
//  I recommend that you read through the camera.h and model.h files to see if you can map the the previous
//  lessons to this implementation
#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Sphere.h"

using namespace std;

// glfw and input functions
// ------------------------
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods);
void cursor_input_callback(GLFWwindow* window, double posX, double posY);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// screen settings
// ---------------
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// global variables used for rendering
// -----------------------------------
Shader* shader;
Shader* simple_shader;
Shader* cube_sphere_shader;
Camera camera(glm::vec3(0.0f, 1.6f, 5.0f));

Shader* skyboxShader;
unsigned int skyboxVAO; // skybox handle
unsigned int cubemapTexture; // skybox texture handle

// global variables used for control
// ---------------------------------
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float deltaTime;
bool isPaused = false; // stop camera movement when GUI is open

// function declarations
// ---------------------
void drawSkybox();
void drawObjects();
void drawGui();
unsigned int initSkyboxBuffers();
unsigned int loadCubeMap(vector<std::string> faces);

// Functions for solar system
Sphere initializeSun(int);
std::vector<Sphere> initializePlanets(int);
void drawSun(Sphere sun);
void drawPlanets(std::vector<Sphere>);



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Planet Generator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_input_callback);
    glfwSetKeyCallback(window, key_input_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // load the shaders and the 3D models
    // ----------------------------------
    simple_shader = new Shader("shaders/simple.vert", "shaders/simple.frag");
    cube_sphere_shader = new Shader("shaders/cube_sphere.vert", "shaders/cube_sphere.frag");
    shader = simple_shader;

    // init skybox
    vector<std::string> faces
    {
            "skybox/right.png",
            "skybox/left.png",
            "skybox/top.png",
            "skybox/bottom.png",
            "skybox/front.png",
            "skybox/back.png"
    };

    cubemapTexture = loadCubeMap(faces);
    skyboxVAO = initSkyboxBuffers();
    skyboxShader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");

    // set up the z-buffer
    // -------------------
    glDepthRange(-1,1); // make the NDC a right handed coordinate system, with the camera pointing towards -z
    glEnable(GL_DEPTH_TEST); // turn on z-buffer depth test
    glDepthFunc(GL_LESS); // draws fragments that are closer to the screen in NDC

    // NEW! Enable SRGB framebuffer
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Dear IMGUI init
    // ---------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    //Details of cube
    int cubeDivisions = 32;

    //Initialize planets:
    int numOfPlanets = 1;

    auto sun = initializeSun(cubeDivisions);
    //auto planets = initializePlanets(numOfPlanets);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        static float lastFrame = 0.0f;
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 viewProjection = projection * view;

        shader->setMat4("viewProjection", viewProjection);

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSkybox();

        drawSun(sun);
        //drawPlanets();

        // render the cones
        //glUseProgram(activeShader->ID);

        if (isPaused) {
            drawGui();
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    // -------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete simple_shader;
    delete cube_sphere_shader;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void drawGui(){
    glDisable(GL_FRAMEBUFFER_SRGB);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Camera position");
        string pos = std::to_string(camera.Position.x) + ", " +
                std::to_string(camera.Position.y) + ", " +
                std::to_string(camera.Position.z);

        ImGui::Text("%s", pos.data());
        ImGui::End();

        ImGui::Begin("Settings");

        ImGui::Text("Shading model: ");
        {
            if (ImGui::RadioButton("Simple shader", shader == simple_shader)) { shader = simple_shader; }
            if (ImGui::RadioButton("Cube sphere shader", shader == cube_sphere_shader)) { shader = cube_sphere_shader; }
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glEnable(GL_FRAMEBUFFER_SRGB);
}

// init the VAO of the skybox
// --------------------------
unsigned int initSkyboxBuffers() {
    // triangles forming the six faces of a cube
    // note that the camera is placed inside of the cube, so the winding order
    // is selected to make the triangles visible from the inside
    float skyboxVertices[108]{
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    return skyboxVAO;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubeMap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void drawSkybox()
{
    // render skybox
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    skyboxShader->setMat4("projection", projection);
    skyboxShader->setMat4("view", view);
    skyboxShader->setInt("skybox", 0);

    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

void drawObjects()
{
    // the typical transformation uniforms are already set for you, these are:
    // projection (perspective projection matrix)
    // view (to map world space coordinates to the camera space, so the camera position becomes the origin)
    // model (for each model part we draw)

    // camera parameters
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 viewProjection = projection * view;

    // camera position
    shader->setVec3("camPosition", camera.Position);
    // set viewProjection matrix uniform
    shader->setMat4("viewProjection", viewProjection);

    // set up skybox texture
    shader->setInt("skybox", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (isPaused)
        return;

    // movement commands
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void cursor_input_callback(GLFWwindow* window, double posX, double posY){

    // camera rotation
    static bool firstMouse = true;
    if (firstMouse)
    {
        lastX = (float)posX;
        lastY = (float)posY;
        firstMouse = false;
    }

    float xoffset = (float)posX - lastX;
    float yoffset = lastY - (float)posY; // reversed since y-coordinates go from bottom to top

    lastX = (float)posX;
    lastY = (float)posY;

    if (isPaused)
        return;

    // we use the handy camera class from LearnOpenGL to handle our camera
    camera.ProcessMouseMovement(xoffset, yoffset);
}


void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods){
    // controls pause mode
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        isPaused = !isPaused;
        glfwSetInputMode(window, GLFW_CURSOR, isPaused ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Sphere initializeSun(int n)
{
    auto sun = Sphere(glm::vec3(0.0f), 1.0f, n, shader);
    return sun;
}

std::vector<Sphere> initializePlanets(int n)
{
    std::vector<Sphere> planets;

    for(int i = 0; i < n; i++)
    {
        auto planet =  Sphere(glm::vec3(2.0f, 0.0f, 0.0f), 1.0f, n, shader);
        planets.insert(planets.end(), planet);
    }

    return planets;
}

void drawSun(Sphere sun)
{
    sun.Draw();
}

void drawPlanets(std::vector<Sphere> planets)
{


}
