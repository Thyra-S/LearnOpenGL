#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader and Camera files created as headers for ease and portability
#include <Shader.h>
#include <Camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("materialLighting.vert", "materialLighting.frag");
    Shader lightCubeShader("lightCube.vert", "lightCube.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
	//Positions           // Normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    Material materialValues[] = {
    { glm::vec3(0.0215,0.1745,0.0215) ,glm::vec3(0.07568,0.61424,0.07568),glm::vec3(0.633,0.727811,0.633) ,0.6}, //emerald
    { glm::vec3(0.135,0.2225,0.1575) ,glm::vec3(0.54,0.89,0.63) ,glm::vec3(0.316228,0.316228,0.316228),0.1}, //jade
    { glm::vec3(0.05375,0.05,0.06625) ,glm::vec3(0.18275,0.17,0.22525) ,glm::vec3(0.332741,0.328634,0.346435),0.3}, //obsidian
    { glm::vec3(0.25,0.20725,0.20725) ,glm::vec3(1,0.829,0.829) ,glm::vec3(0.296648,0.296648,0.296648),0.088}, //pearl
    { glm::vec3(0.1745,0.01175,0.01175),glm::vec3(0.61424,0.04136,0.04136),glm::vec3(0.727811,0.626959,0.626959),0.6}, //ruby
    { glm::vec3(0.1,0.18725,0.1745) ,glm::vec3(0.396,0.74151,0.69102) ,glm::vec3(0.297254,0.30829,0.306678) ,0.1}, //turquoise
    { glm::vec3(0.329412,0.223529,0.027451),glm::vec3(0.780392,0.568627,0.113725), glm::vec3(0.992157,0.941176,0.807843),0.21794872}, //brass
    { glm::vec3(0.2125,0.1275,0.054), glm::vec3(0.714,0.4284,0.18144) ,glm::vec3(0.393548,0.271906,0.166721), 0.2}, //bronze
    { glm::vec3(0.25,0.25,0.25) ,glm::vec3(0.4,0.4,0.4) ,glm::vec3(0.774597,0.774597,0.774597), 0.6}, //chrome
    { glm::vec3(0.19125,0.0735,0.0225) ,glm::vec3(0.7038,0.27048,0.0828) ,glm::vec3(0.256777,0.137622,0.086014),0.1}, //copper
    { glm::vec3(0.24725,0.1995,0.0745) ,glm::vec3(0.75164,0.60648,0.22648) ,glm::vec3(0.628281,0.555802,0.366065),0.4}, //gold
    { glm::vec3(0.19225,0.19225,0.19225),glm::vec3(0.50754,0.50754,0.50754) ,glm::vec3(0.508273,0.508273,0.508273),0.4}, //silver
    { glm::vec3(0.0,0.0,0.0) ,glm::vec3(0.01,0.01,0.01) ,glm::vec3(0.50,0.50,0.50), .25 }, //black plastic
    { glm::vec3(0.0,0.1,0.06) ,glm::vec3(0.0,0.50980392,0.50980392),glm::vec3(0.50196078,0.50196078,0.50196078), .25}, //cyan plastic
    { glm::vec3(0.0,0.0,0.0) ,glm::vec3(0.1 ,0.35,0.1) ,glm::vec3(0.45,0.55,0.45),.25 }, //green plastic
    { glm::vec3(0.0,0.0,0.0) ,glm::vec3(0.5 ,0.0,0.0) ,glm::vec3(0.7,0.6 ,0.6),.25 }, //red plastic
    { glm::vec3(0.0,0.0,0.0) ,glm::vec3(0.55 ,0.55,0.55) ,glm::vec3(0.70,0.70,0.70),.25 }, //white plastic
    { glm::vec3(0.0,0.0,0.0) ,glm::vec3(0.5 ,0.5,0.0) ,glm::vec3(0.60,0.60,0.50),.25 }, //yellow plastic
    { glm::vec3(0.02,0.02,0.02) ,glm::vec3(0.01,0.01,0.01) ,glm::vec3(0.4,0.4 ,0.4),.078125}, //black rubber
    { glm::vec3(0.0,0.05,0.05) ,glm::vec3(0.4 ,0.5,0.5) ,glm::vec3(0.04,0.7,0.7) ,.078125}, //cyan rubber
    { glm::vec3(0.0,0.05,0.0) ,glm::vec3(0.4 ,0.5,0.4) ,glm::vec3(0.04,0.7,0.04) ,.078125}, //green rubber
    { glm::vec3(0.05,0.0,0.0) ,glm::vec3(0.5 ,0.4,0.4) ,glm::vec3(0.7,0.04,0.04) ,.078125}, //red rubber
    { glm::vec3(0.05,0.05,0.05) ,glm::vec3(0.5,0.5,0.5) ,glm::vec3(0.7,0.7,0.7) ,.078125}, //white rubber
    { glm::vec3(0.05,0.05,0.0) ,glm::vec3(0.5,0.5,0.4) ,glm::vec3(0.7,0.7,0.04) ,.078125 } //yellow rubber
    };



    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
	// normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
        

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        // set light color
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f) * 0.5f + 0.5f;
        lightColor.y = sin(glfwGetTime() * 0.7f) * 0.5f + 0.5f;
        lightColor.z = sin(glfwGetTime() * 1.3f) * 0.5f + 0.5f;

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor); // darken diffuse light a bit
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // be sure to activate shader when setting uniforms/drawing objects
        for (int i = 0; i < 24; i++) {
            lightingShader.setVec3("material.ambient", materialValues[i].ambient);
            lightingShader.setVec3("material.diffuse", materialValues[i].diffuse);
            lightingShader.setVec3("material.specular", materialValues[i].specular);
            lightingShader.setFloat("material.shininess", materialValues[i].shininess * 128);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            float xPos = (i%6 - (6 / 2.0f)) * 2.0f;
            float yPos = (i/6 - (4 / 2.0f)) * 2.0f;

            model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
            lightingShader.setMat4("model", model);

            // render the cube
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setVec3("lightColor", lightColor);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources once they've outlived their purpose
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

