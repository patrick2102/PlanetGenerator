#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <vector>

// NEW! as our scene gets more complex, we start using more helper classes
//  I recommend that you read through the camera.h and model.h files to see if you can map the the previous
//  lessons to this implementation
#include "shader.h"
#include "camera.h"
#include "model.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// glfw and input functions
// ------------------------
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods);
void cursor_input_callback(GLFWwindow* window, double posX, double posY);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// screen settings
// ---------------
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

// global variables used for rendering
// -----------------------------------
Shader* shader;
Shader* phong_shading;
Shader* pbr_shading;
Shader* shadowMap_shader;
Model* carBodyModel;
Model* carPaintModel;
Model* carInteriorModel;
Model* carLightModel;
Model* carWindowsModel;
Model* carWheelModel;
Model* floorModel;
GLuint carBodyTexture;
GLuint carPaintTexture;
GLuint carLightTexture;
GLuint carWindowsTexture;
GLuint carWheelTexture;
GLuint floorTexture;
Camera camera(glm::vec3(0.0f, 1.6f, 5.0f));

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

float lightRotationSpeed = 1.0f;

// structure to hold lighting info
// -------------------------------
struct Light
{
    Light(glm::vec3 position, glm::vec3 color, float intensity, float radius)
        : position(position), color(color), intensity(intensity), radius(radius)
    {
    }

    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float radius;
};

// structure to hold config info
// -------------------------------
struct Config
{
    Config() : lights()
    {
        // Adding lights
        //lights.emplace_back(position, color, intensity, radius);

        // light 1
        lights.emplace_back(glm::vec3(-1.0f, 1.0f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f);

        // light 2
        lights.emplace_back(glm::vec3( 1.0f, 1.5f, 0.0f), glm::vec3(0.7f, 0.2f, 1.0f), 1.0f, 10.0f);
    }

    // ambient light
    glm::vec3 ambientLightColor = {1.0f, 1.0f, 1.0f};
    float ambientLightIntensity = 0.25f;

    // material
    glm::vec3 reflectionColor = {0.9f, 0.9f, 0.2f};
    float ambientReflectance = 0.75f;
    float diffuseReflectance = 0.75f;
    float specularReflectance = 0.5f;
    float specularExponent = 10.0f;
    float roughness = 0.5f;
    float metalness = 0.0f;

    std::vector<Light> lights;

} config;


// function declarations
// ---------------------
void setAmbientUniforms(glm::vec3 ambientLightColor);
void setLightUniforms(Light &light);
void setupForwardAdditionalPass();
void resetForwardAdditionalPass();
void drawSkybox();
void drawShadowMap();
void drawObjects();
void drawGui();
unsigned int initSkyboxBuffers();
unsigned int loadCubemap(vector<std::string> faces);
void createShadowMap();
void setShadowUniforms();

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
    phong_shading = new Shader("shaders/common_shading.vert", "shaders/phong_shading.frag");
    pbr_shading = new Shader("shaders/common_shading.vert", "shaders/pbr_shading.frag");
    shader = pbr_shading;

    carBodyModel = new Model("car/Body_LOD0.obj");
    carPaintModel = new Model("car/Paint_LOD0.obj");
    carInteriorModel = new Model("car/Interior_LOD0.obj");
    carLightModel = new Model("car/Light_LOD0.obj");
    carWindowsModel = new Model("car/Windows_LOD0.obj");
    carWheelModel = new Model("car/Wheel_LOD0.obj");
    floorModel = new Model("floor/floor.obj");

    // init skybox
    vector<std::string> faces
    {
            "skybox/right.tga",
            "skybox/left.tga",
            "skybox/top.tga",
            "skybox/bottom.tga",
            "skybox/front.tga",
            "skybox/back.tga"
    };
    cubemapTexture = loadCubemap(faces);
    skyboxVAO = initSkyboxBuffers();
    skyboxShader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");

    createShadowMap();
    shadowMap_shader = new Shader("shaders/shadowmap.vert", "shaders/shadowmap.frag");

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

        processInput(window);

        // Rotate light 2
        if (lightRotationSpeed > 0.0f)
        {
            glm::vec4 rotatedLight = glm::rotate(glm::mat4(1.0f), lightRotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(config.lights[1].position, 1.0f);
            config.lights[1].position = glm::vec3(rotatedLight.x, rotatedLight.y, rotatedLight.z);
        }

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        drawSkybox();

        drawShadowMap();

        shader->use();

        // First light + ambient
        setAmbientUniforms(config.ambientLightColor * config.ambientLightIntensity);
        setLightUniforms(config.lights[0]);
        setShadowUniforms();
        drawObjects();

        // Additional additive lights
        setupForwardAdditionalPass();
        for (int i = 1; i < config.lights.size(); ++i)
        {
            setLightUniforms(config.lights[i]);
            drawObjects();
        }
        resetForwardAdditionalPass();

        if (isPaused) {
            drawGui();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    // -------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete carBodyModel;
    delete carPaintModel;
    delete carInteriorModel;
    delete carLightModel;
    delete carWindowsModel;
    delete carWheelModel;
    delete floorModel;
    delete phong_shading;
    delete pbr_shading;
    delete shadowMap_shader;

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
        ImGui::Begin("Settings");

        ImGui::Text("Ambient light: ");
        ImGui::ColorEdit3("ambient light color", (float*)&config.ambientLightColor);
        ImGui::SliderFloat("ambient light intensity", &config.ambientLightIntensity, 0.0f, 1.0f);
        ImGui::Separator();

        ImGui::Text("Light 1: ");
        ImGui::DragFloat3("light 1 direction", (float*)&config.lights[0].position, .1f, -20, 20);
        ImGui::ColorEdit3("light 1 color", (float*)&config.lights[0].color);
        ImGui::SliderFloat("light 1 intensity", &config.lights[0].intensity, 0.0f, 2.0f);
        ImGui::Separator();

        ImGui::Text("Light 2: ");
        ImGui::DragFloat3("light 2 position", (float*)&config.lights[1].position, .1f, -20, 20);
        ImGui::ColorEdit3("light 2 color", (float*)&config.lights[1].color);
        ImGui::SliderFloat("light 2 intensity", &config.lights[1].intensity, 0.0f, 2.0f);
        ImGui::SliderFloat("light 2 radius", &config.lights[1].radius, 0.01f, 50.0f);
        ImGui::SliderFloat("light 2 speed", &lightRotationSpeed, 0.0f, 2.0f);
        ImGui::Separator();

        ImGui::Text("Car paint material: ");
        ImGui::ColorEdit3("color", (float*)&config.reflectionColor);
        ImGui::Separator();
        ImGui::SliderFloat("ambient reflectance", &config.ambientReflectance, 0.0f, 1.0f);
        ImGui::SliderFloat("diffuse reflectance", &config.diffuseReflectance, 0.0f, 1.0f);
        ImGui::SliderFloat("specular reflectance", &config.specularReflectance, 0.0f, 1.0f);
        ImGui::SliderFloat("specular exponent", &config.specularExponent, 0.0f, 100.0f);
        ImGui::Separator();
        ImGui::SliderFloat("roughness", &config.roughness, 0.01f, 1.0f);
        ImGui::SliderFloat("metalness", &config.metalness, 0.0f, 1.0f);
        ImGui::Separator();

        ImGui::Text("Shading model: ");
        {
            if (ImGui::RadioButton("Blinn-Phong Shading", shader == phong_shading)) { shader = phong_shading; }
            if (ImGui::RadioButton("PBR Shading", shader == pbr_shading)) { shader = pbr_shading; }
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glEnable(GL_FRAMEBUFFER_SRGB);
}

void setAmbientUniforms(glm::vec3 ambientLightColor)
{
    // ambient uniforms
    shader->setVec4("ambientLightColor", glm::vec4(ambientLightColor, glm::length(ambientLightColor) > 0.0f ? 1.0f : 0.0f));
}

void setLightUniforms(Light& light)
{
    glm::vec3 lightEnergy = light.color * light.intensity;

    // TODO 8.3 : if we are using the PBR shader, multiply the lightEnergy by PI to match the color of the previous setup


    // light uniforms
    shader->setVec3("lightPosition", light.position);
    shader->setVec3("lightColor", lightEnergy);
    shader->setFloat("lightRadius", light.radius);
}

void setupForwardAdditionalPass()
{
    // Remove ambient from additional passes
    setAmbientUniforms(glm::vec3(0.0f));

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Set depth test to GL_EQUAL (only the fragments that match the depth buffer are rendered)
    glDepthFunc(GL_EQUAL);

    // Disable shadowmap
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void resetForwardAdditionalPass()
{
    // Restore ambient
    setAmbientUniforms(config.ambientLightColor * config.ambientLightIntensity);

    //Disable blend and restore default blend function
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    // Restore default depth test
    glDepthFunc(GL_LESS);
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
unsigned int loadCubemap(vector<std::string> faces)
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
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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


void createShadowMap()
{
    // create depth texture
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // if you replace GL_LINEAR with GL_NEAREST you will see pixelation in the borders of the shadow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // if you replace GL_LINEAR with GL_NEAREST you will see pixelation in the borders of the shadow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void drawShadowMap()
{
    Shader* currShader = shader;
    shader = shadowMap_shader;

    // setup depth shader
    shader->use();

    // We use an ortographic projection since it is a directional light.
    // left, right, bottom, top, near and far values define the 3D volume relative to
    // the light position and direction that will be rendered to produce the depth texture.
    // Geometry outside of this range will not be considered when computing shadows.
    float near_plane = 1.0f;
    float shadowMapSize = 6.0f;
    float shadowMapDepthRange = 10.0f;
    float half = shadowMapSize / 2.0f;
    glm::mat4 lightProjection = glm::ortho(-half, half, -half, half, near_plane, near_plane + shadowMapDepthRange);
    glm::mat4 lightView = glm::lookAt(glm::normalize(config.lights[0].position) * shadowMapDepthRange * 0.5f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    // setup framebuffer size
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    // bind our depth texture to the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    // clear the depth texture/depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw scene from the light's perspective into the depth texture
    drawObjects();

    // unbind the depth texture from the frame buffer, now we can render to the screen (frame buffer) again
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    shader = currShader;
}


void setShadowUniforms()
{
    // shadow uniforms
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader->setInt("shadowMap", 6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    //shader->setFloat("shadowBias", config.shadowBias * 0.01f);
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

    // material uniforms for car paint
    shader->setVec3("reflectionColor", config.reflectionColor);
    shader->setFloat("ambientReflectance", config.ambientReflectance);
    shader->setFloat("diffuseReflectance", config.diffuseReflectance);
    shader->setFloat("specularReflectance", config.specularReflectance);
    shader->setFloat("specularExponent", config.specularExponent);
    shader->setFloat("roughness", config.roughness);
    shader->setFloat("metalness", config.metalness);

    glm::mat4 model = glm::mat4(1.0f);
    shader->setMat4("model", model);
    carPaintModel->Draw(*shader);

    // material uniforms for other car parts (hardcoded)
    shader->setVec3("reflectionColor", 1.0f, 1.0f, 1.0f);
    shader->setFloat("ambientReflectance", 0.75f);
    shader->setFloat("diffuseReflectance", 0.75f);
    shader->setFloat("specularReflectance", 0.5f);
    shader->setFloat("specularExponent", 10.0f);
    shader->setFloat("roughness", 0.5f);
    shader->setFloat("metalness", 0.0f);

    carBodyModel->Draw(*shader);

    // draw car
    shader->setMat4("model", model);
    carLightModel->Draw(*shader);
    carInteriorModel->Draw(*shader);

    // draw wheel
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-.7432f, .328f, 1.39f));
    shader->setMat4("model", model);
    carWheelModel->Draw(*shader);

    // draw wheel
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-.7432f, .328f, -1.39f));
    shader->setMat4("model", model);
    carWheelModel->Draw(*shader);

    // draw wheel
    model = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0, 1.0, 0.0));
    model = glm::translate(model, glm::vec3(-.7432f, .328f, 1.39f));
    shader->setMat4("model", model);
    carWheelModel->Draw(*shader);

    // draw wheel
    model = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0, 1.0, 0.0));
    model = glm::translate(model, glm::vec3(-.7432f, .328f, -1.39f));
    shader->setMat4("model", model);
    carWheelModel->Draw(*shader);

    // draw floor
    model = glm::scale(glm::mat4(1.0), glm::vec3(5.f, 5.f, 5.f));
    shader->setMat4("model", model);
    shader->setFloat("specularReflectance", 0.2f);
    shader->setFloat("roughness", 0.95f);
    floorModel->Draw(*shader);

    shader->setFloat("specularReflectance", 1.0f);
    shader->setFloat("specularExponent", 20.0f);
    shader->setFloat("roughness", 0.25f);
    model = glm::mat4(1.0f);
    shader->setMat4("model", model);

    carWindowsModel->Draw(*shader);
}


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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