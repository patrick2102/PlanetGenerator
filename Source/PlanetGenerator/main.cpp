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
#include "Sun.h"
#include "Planet.h"
#include <CubeSphere.h>
#include "BiomeGenerator.h"
#include "random"


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
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

// global variables used for rendering
// -----------------------------------
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Shader* skyboxShader;
unsigned int skyboxVAO; // skybox handle
unsigned int cubemapTexture; // skybox texture handle

unsigned int shadowMap, shadowMapFBO;
glm::mat4 lightSpaceMatrix;


// global variables used for control
// ---------------------------------
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float deltaTime;
bool isPaused = false; // stop camera movement when GUI is open

struct Config {

    // ambient light
    glm::vec3 ambientLightColor = {1.0f, 1.0f, 1.0f};
    float ambientLightIntensity = 0.2f;

    // material
    glm::vec3 reflectionColor = {1.0f, 1.0f, 0.0f};
    float ambientReflectance = 0.5f;
    float diffuseReflectance = 0.5f;
    float specularReflectance = 0.7f;
    float specularExponent = 20.0f;

} config;

// function declarations
// ---------------------
void setLightUniforms();
void setUniforms();
void useShader(Shader*);
void drawObjects();
void drawGui();
void drawRTGui();
void drawSkybox();
unsigned int initSkyboxBuffers();
unsigned int loadCubeMap(vector<std::string> faces);


//Shaders:
Shader* shader;
Shader* generate_simplex_shader;
Shader* star_shader;
Shader* atmosphere_shader;

// Variables for solar system
std::vector<Planet> planets;
Sun* sun;

bool loadTextures = false;
bool render_atmosphere = true;

// Functions for solar system
void initializeSun(int);
void initializePlanets(int, int);
PlanetData generatePlanetData(float, float, int, int, glm::vec3, glm::vec3);
void drawSolarSystem();
void drawSun();
void drawPlanets();

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

    // load the shaders
    // ----------------------------------
    generate_simplex_shader = new Shader("shaders/generateSimplex.vert", "shaders/generateSimplex.frag");
    star_shader = new Shader("shaders/starShader.vert", "shaders/starShader.frag");
    atmosphere_shader = new Shader("shaders/atmosphereShader.vert", "shaders/atmosphereShader.frag");

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
    int cubeDivisions = 5;

    //Initialize planets:
    int numOfPlanets = 3;

    initializeSun(cubeDivisions);
    initializePlanets(numOfPlanets, cubeDivisions);

    //glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);
    //lPolygonMode(GL_FRONT, GL_POINT);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        static float lastFrame = 0.0f;
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSkybox();

        useShader(generate_simplex_shader);
        setUniforms();
        drawSolarSystem();

        if (isPaused) {
            drawGui();
        } else {
            drawRTGui();
            }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete shader;
    delete skyboxShader;
    delete sun;

    // Cleanup
    // -------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawRTGui()
{
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

       // ImGui::Begin("Speed");
        //string speed = std::to_string(camera.MovementSpeed);
        //ImGui::Text("%s", speed.data());
        //ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glEnable(GL_FRAMEBUFFER_SRGB);
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
            /*
            if (ImGui::RadioButton("GPU simplex shading", shader == generate_simplex_shader))
            {
                shader = generate_simplex_shader;
            }
            */
        }

        ImGui::Text("Planet settings");
        {
            if (ImGui::RadioButton("Render atmosphere", render_atmosphere))
            {
                render_atmosphere = !render_atmosphere;
            }
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glEnable(GL_FRAMEBUFFER_SRGB);
}

void useShader(Shader *newShader)
{
    shader = newShader;
    shader->use();
}

void setUniforms()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 viewProjection = projection * view;
    shader->setMat4("viewProjection", viewProjection);
    shader->setVec3("camPosition", camera.Position);
    shader->setVec3("camFront", camera.Front);
    setLightUniforms();
}

void setLightUniforms()
{
    // light uniforms
    shader->setVec3("ambientLightColor", glm::vec3(0.0f));
    shader->setVec3("sunPosition", sun->getLight().position);
    shader->setVec3("sunColor", sun->getLight().lightColor);
    shader->setFloat("lightIntensity", sun->getLight().lightIntensity);
    shader->setFloat("lightRadius", sun->getLight().lightRadius);
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

void initializeSun(int divisions)
{
    useShader(star_shader);

    float sunRadius = 1.0f;
    auto sphere = CubeSphere(sunRadius, divisions);
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);

    Atmosphere testAtmosphere = Atmosphere("sun", pos, pos, sunRadius, 1.0f, 8, 80);

    for(int i = 0; i < sphere.vertices.size(); i += 1)
    {
        auto p1 = sphere.vertices[i];

        testAtmosphere.points.insert(testAtmosphere.points.end(), {p1, p1, glm::vec3(0,0,1.0)}); //fix up later
    }
    int scale = 40;
    float amplitude = 20.0f;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
    int diameter = 100;
    int iterations = 10;

    Displacement sunDisplacement = Displacement(scale, amplitude, persistence, lacunarity, diameter, 0);

    StarData starData = StarData(light, sunDisplacement, sunMaterial, testAtmosphere);
    sun = new Sun(pos, starData);
}

Displacement generateDisplacement()
{
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> distrAmp(0.5, 3.0);
    std::uniform_real_distribution<> distrPers(0.45, 0.75);
    std::uniform_real_distribution<> distrLac(1.0, 2.0);
    std::uniform_real_distribution<> distrDia(0.5, 2.0);

    int scale = 400 * distrDia(eng);
    float amplitude = 10.0 * distrAmp(eng);
    //float persistence = 0.5f;
    float persistence = distrPers(eng);
    float lacunarity = 1.8f * distrLac(eng);
    int diameter = 1000;
    int iterations = 10;

    return Displacement(scale, amplitude, persistence, lacunarity, diameter, iterations);
}


PlanetData generatePlanetData(float seed, float radius, int divisions, int nCells, glm::vec3 center, glm::vec3 sunPosition)
{
    PlanetType pt = earthLike;
    Ocean ocean = Ocean(waterMaterial);
    Displacement displacement = generateDisplacement();
    Atmosphere testAtmosphere = Atmosphere("earthlike", center, sunPosition, radius*1.f, 1.5f, 8, 8);
    auto sphere = CubeSphere(radius, divisions);

    for(int i = 0; i < sphere.vertices.size(); i += 1)
    {
        auto p1 = sphere.vertices[i];

        ocean.material.points.insert(ocean.material.points.end(), {p1, p1, glm::vec3(0,0,1.0)});
        testAtmosphere.points.insert(testAtmosphere.points.end(), {p1, p1, glm::vec3(0,0,1.0)}); //fix up later
    }

    auto tg = BiomeGenerator();
    std::vector<Vertex> coords;
    std::vector<Material> materials = tg.SetupMaterials(nCells, pt);

    auto voronoiPoints = tg.DistributePointsRandom(seed, nCells, radius);

    std::vector<std::vector<Vertex>> vertexPairs(nCells);
    int n = 1;

    for(int i = 0; i < sphere.vertices.size(); i += 3)
    {
        auto p1 = sphere.vertices[i];
        auto p2 = sphere.vertices[i+1];
        auto p3 = sphere.vertices[i+2];

        std::vector<Vertex> vertices;
        int pIndex;

        std::tie(vertices, pIndex) = tg.AssignMaterialsMix(p1, p2, p3, materials, voronoiPoints, n);
        vertexPairs[pIndex].insert(vertexPairs[pIndex].end(), vertices.begin(), vertices.end());
    }

    for(int i = 0; i < vertexPairs.size(); i++)
    {
        materials[i].points.insert(materials[i].points.end(), vertexPairs[i].begin(), vertexPairs[i].end());
    }

    return PlanetData(materials, displacement, ocean, testAtmosphere, true, glm::vec3(0,0,0));
}

void initializePlanets(int n, int divisions)
{
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> distr(0, 99999);

    useShader(generate_simplex_shader);
    for(int i = 0; i < n; i++)
    {
        //float seed = 1.0f;
        float seed = distr(eng);

        std::string planetName = "planet";
        planetName.append(to_string(i)).append(".bmp");

        glm::vec3 pos = glm::vec3(6.0f, 0.0f, 6.0f * float(i) - 6.0f);

        auto planetData = generatePlanetData(seed, 1.0f, divisions, 1, pos, sun->GetPosition());

        HeightMapGenerator hmg = HeightMapGenerator(seed);
        hmg.CopyToShader(shader);
        auto planet = Planet(pos, planetData, planetName.c_str());

        planets.insert(planets.end(), planet);
    }
}

void drawSolarSystem()
{
    drawSun();
    drawPlanets();
}

void drawSun()
{
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    useShader(star_shader);
    sun->Draw(shader);
    setUniforms();
    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void drawPlanets()
{
    useShader(generate_simplex_shader);
    setUniforms();
    for(auto p : planets)
    {
        p.Draw(shader);
    }

    if(render_atmosphere)
    {
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        useShader(atmosphere_shader);
        setUniforms();
        for(auto p : planets)
        {
            p.DrawAtmosphere(shader);
        }
        //glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

}
