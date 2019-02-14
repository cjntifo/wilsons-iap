// Std. Includes
#include <string>
#include <stdlib.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include <string>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <soil/SOIL.h>
#include <cmath>

struct vertex
{
	GLfloat x, y, z;
	GLfloat a, b, c;
	GLfloat u, v;
};

struct normal
{
	GLfloat x, y, z;
};

struct index
{
	GLuint a, b, c;
};

struct MeshData
{
	std::vector<vertex> Vertices;
	//std::vector<normal> Normals;
	std::vector<index> Indices;
};

///<summary>
/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
/// at the origin with the specified width and depth.
///</summary>
void CreateGrid(float width, float depth, int m, int n, MeshData& meshData);



// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
GLuint loadTexture(GLchar* path);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void CreateGrid(float width, float depth, int m, int n, MeshData& meshData) //Based off GeometryGenerator class
{
    int vertexCount = m*n;
    int faceCount   = (m-1)*(n-1)*2;
    // Create the vertices.

    float halfWidth = 0.5f*width;
    float halfDepth = 0.5f*depth;

    float dx = width / (n-1);
    float dz = depth / (m-1);

    float du = 1.0f / (n-1);
    float dv = 1.0f / (m-1);


    meshData.Vertices.resize(vertexCount);
    for(GLfloat i = 0.0f; i < m; ++i)
    {
        GLfloat z = halfDepth - i*dz;
        for(GLfloat j = 0.0f; j < n; ++j)
        {
            //code
            GLfloat x = -halfWidth + j*dx;

            meshData.Vertices[i*n+j].x = x;
            meshData.Vertices[i*n+j].y = -3.0f;
            meshData.Vertices[i*n+j].z = z;

			meshData.Vertices[i*n+j].a = 0.0f;
            meshData.Vertices[i*n+j].b = 1.0f;
			meshData.Vertices[i*n+j].c = 0.0f;


            meshData.Vertices[i*n+j].u = j*du;
            meshData.Vertices[i*n+j].v = i*dv;

            //vertex temp = {x, 0.0f, z, j*du, i*dv};
            //meshData.Vertices.push_back(temp);
        }
    }
    
	//indices
    //meshData.Indices.resize(faceCount);
    GLuint offset = 0;
    for(GLuint i = 0; i < m-1; ++i)
    {
        for(GLuint j = 0; j < n-1; ++j)
        {               
            //offset = x * m;
            index temp = 
            {
                i*n+j,
                i*n+j+1,
                (i+1)*n+j
            };

            index temp_2 = 
            {
                (i+1)*n+j,
                i*n+j+1,
                (i+1)*n+j+1
            };

            meshData.Indices.push_back(temp);
            meshData.Indices.push_back(temp_2);
        }
    }
}

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Carl's Magical OpenGL IAP Project", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    //glDepthFunc(GL_ALWAYS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
	
    // Setup and compile our shaders
    Shader shader("C:/Users/Carl J. Ntifo/Documents/Coding/Project/IAP_Waves_Project/Shaders/waves_vs.glsl",
		"C:/Users/Carl J. Ntifo/Documents/Coding/Project/IAP_Waves_Project/Shaders/waves_frag.glsl");
	Shader lampShader("C:/Users/Carl J. Ntifo/Documents/Coding/Project/IAP_Waves_Project/Shaders/lamps_vs.glsl",
		"C:/Users/Carl J. Ntifo/Documents/Coding/Project/IAP_Waves_Project/Shaders/lamps_frag.glsl");

    #pragma region "object_initialization"
    // Set the object data (buffers, vertex attributes)

	MeshData grid;
	CreateGrid(50.0f, 50.0f, 300, 200, grid);
	GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f
    };

	glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,    0.0f,  0.0f),
		glm::vec3(-100.0f,  0.0f,  0.0f),
		glm::vec3( 100.0f,  0.0f,  0.0f)
    };

	// Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3(-100.0f,  0.0f,  0.0f),
        glm::vec3( 100.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    // Setup water VAO
    GLuint waterVAO, waterVBO, waterEBO;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
	glGenBuffers(1, &waterEBO);
    glBindVertexArray(waterVAO);
    
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*grid.Vertices.size(), grid.Vertices.data(), GL_STATIC_DRAW);
    
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index) * grid.Indices.size(), grid.Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindVertexArray(0);


	GLuint lampVAO, lampVBO;
    glGenVertexArrays(1, &lampVAO);
    glGenBuffers(1, &lampVBO);
    glBindVertexArray(lampVAO);
    
	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Load textures
    //GLuint waterTexture = loadTexture("S:/Coding/OpenGL/Resources/Textures/brushwater.png");

	// Load textures
    GLuint diffuseMap, specularMap;
    glGenTextures(1, &diffuseMap);
    glGenTextures(1, &specularMap);
    //glGenTextures(1, &emissionMap);
    int width, height;
    unsigned char* image;
    // Diffuse map
    image = SOIL_load_image("C:/Users/Carl J. Ntifo/Documents/Coding/OpenGL/Resources/Textures/brushwater_diff.png", &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // Specular map
    image = SOIL_load_image("C:/Users/Carl J. Ntifo/Documents/Coding/OpenGL/Resources/Textures/brushwater_spec.png", &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Set texture units
    shader.Use();
    glUniform1i(glGetUniformLocation(shader.Program, "material.diffuseMap"),  0);
    glUniform1i(glGetUniformLocation(shader.Program, "material.specularMap"), 1);
    #pragma endregion

    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw objects
        shader.Use(); 

		//Set Uniforms
		GLint locTime = glGetUniformLocation(shader.Program, "time");
		if (locTime != -1)
			glUniform1f(locTime, (float)glfwGetTime());

		GLint locWLength = glGetUniformLocation(shader.Program, "wavelength");
		if (locWLength != -1)
			glUniform1f(locWLength, 4.0f); //2

		GLint locPeak = glGetUniformLocation(shader.Program, "peak");
		if (locPeak != -1)
			glUniform1f(locPeak, 1.0f);// 1

		float r1 = static_cast <float> (rand()) / (static_cast <float> (255.0f/0.5f));

		GLint locDir = glGetUniformLocation(shader.Program, "waveDir");
		if (locDir != -1)
			glUniform1f(locDir, r1);

		//===============
#pragma region "Lights"

		shader.Use();
		GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
        // Set material properties
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 128.0f);

// Directional light
        glUniform3f(glGetUniformLocation(shader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        //glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), 0.15f, 0.15f, 0.15f);
        glUniform3f(glGetUniformLocation(shader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(shader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		/*for (GLuint i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); i++)
        {
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights["i"].position"), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights["i"].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights["i"].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(shader.Program, "pointLights["i"].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shader.Program, "pointLights["i"].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader.Program, "pointLights["i"].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader.Program, "pointLights["i"].quadratic"), 0.032);
        }*/

        // Point light 1
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.032);
        // Point light 2
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].quadratic"), 0.032);
        // Point light 3
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].quadratic"), 0.032);
        // Point light 4
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].quadratic"), 0.032);

#pragma endregion
		//=======================

		// Set material properties
        //glUniform3f(glGetUniformLocation(shader.Program, "material.ambient"),   0.7f, 0.7f, 0.7f); // I looked up the values for water
        //glUniform3f(glGetUniformLocation(shader.Program, "material.diffuse"),   0.0f, 0.0f, 1.0f);
        //glUniform3f(glGetUniformLocation(shader.Program, "material.specular"),  1.0f, 1.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 2.0f);

		//===============
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);

		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(shader.Program,  "view");
        GLint projLoc  = glGetUniformLocation(shader.Program,  "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // Bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Floor
        glBindVertexArray(waterVAO);
        model = glm::mat4();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_LINES, 3*grid.Indices.size(), GL_UNSIGNED_INT, 0);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glDrawElements(GL_TRIANGLES, 3*grid.Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc  = glGetUniformLocation(lampShader.Program, "view");
        projLoc  = glGetUniformLocation(lampShader.Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Also draw the lamp object, again binding the appropriate shader
		glBindVertexArray(lampVAO);
		
		// Draw 10 containers with the same VAO and VBO information; only their world space coordinates differ
        glm::mat4 lampModel;
        glBindVertexArray(lampVAO);
        for (GLuint i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); i++)
        {
            lampModel = glm::mat4();
            lampModel = glm::translate(lampModel, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(lampModel, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(lampModel, glm::vec3(0.2f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lampModel));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path)
{
    //Generate texture ID and load texture data 
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);	

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;

}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;	
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; 
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}	

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

#pragma endregion