#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

	BuildColoredTable();

	BuildColoredPlane();

	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	
	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth/2;
	double midY = screenHeight/2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);	
}

void Demo::Update(double deltaTime) {
	angle += (float)((deltaTime * 1.5f) / 1000);
}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	DrawColoredTable();

	DrawColoredPlane();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildColoredTable() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("Plywood.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		//Table
		
		// front
		-4.0,  0.2, 2.0, 0, 0,  // 0
		 4.0,  0.2, 2.0, 1, 0,  // 1
		 4.0,  0.4, 2.0, 1, 1,  // 2
		-4.0,  0.4, 2.0, 0, 1,  // 3

		// right
		 4.0,  0.4, -2.0, 0, 0, // 4
		 4.0,  0.4,  2.0, 1, 0, // 5
		 4.0,  0.2,  2.0, 1, 1, // 6
		 4.0,  0.2, -2.0, 0, 1, // 7

		// back
		-4.0,  0.2, -2.0, 0, 0, // 8 
		-4.0,  0.4, -2.0, 1, 0, // 9
		 4.0,  0.4, -2.0, 1, 1, // 10
		 4.0,  0.2, -2.0, 0, 1, // 11

		// left
		-4.0,  0.2, -2.0, 0, 0, // 12
		-4.0,  0.2,  2.0, 1, 0, // 13
		-4.0,  0.4,  2.0, 1, 1, // 14
		-4.0,  0.4, -2.0, 0, 1, // 15
	
		// upper
		 4.0,  0.4,  2.0, 0, 0,  // 16
		-4.0,  0.4,  2.0, 1, 0,  // 17
		-4.0,  0.4, -2.0, 1, 1,  // 18
		 4.0,  0.4, -2.0, 0, 1,  // 19

		// bottom
		-4.0,  0.2, -2.0, 0, 0,  // 20
		 4.0,  0.2, -2.0, 1, 0,  // 21
		 4.0,  0.2,  2.0, 1, 1,  // 22
	    -4.0,  0.2,  2.0, 0, 1,  // 23

		// right leg
		// front
		 2.5,  0.2,  1.6, 0, 0,  // 24
		 3.0,  0.2,  1.6, 0, 0,  // 25
		 3.0, -0.6,  1.6, 0, 0,  // 26
		 2.5, -0.6,  1.6, 0, 0,  // 27
		
		// right
		 3.0,  0.2,  1.6, 0, 0,  // 28
		 3.0,  0.2, -1.6, 0, 0,  // 29
		 3.0, -0.6, -1.6, 0, 0,  // 30
		 3.0, -0.6,  1.6, 0, 0,  // 31

		 // back
		 3.0,  0.2, -1.6, 0, 0,  // 32
		 2.5,  0.2, -1.6, 0, 0,  // 33
		 2.5, -0.6, -1.6, 0, 0,  // 34
		 3.0, -0.6, -1.6, 0, 0,  // 35

		 // left
		 2.5,  0.2,  1.6, 0, 0,  // 36
		 2.5,  0.2, -1.6, 0, 0,  // 37
		 2.5, -0.6, -1.6, 0, 0,  // 38
		 2.5, -0.6,  1.6, 0, 0,  // 39

		 // upper
		 2.5,  0.2, -1.6, 0, 0,	 // 40
		 3.0,  0.2, -1.6, 0, 0,  // 41
		 3.0,  0.2,  1.6, 0, 0,  // 42
		 2.5,  0.2,  1.6, 0, 0,  // 43

		 // bottom
		 2.5, -0.6, -1.6, 0, 0,	 // 44
		 3.0, -0.6, -1.6, 0, 0,  // 45
		 3.0, -0.6,  1.6, 0, 0,  // 46
		 2.5, -0.6,  1.6, 0, 0,  // 47

		// left leg
		// front
		-2.5,  0.2,  1.6, 0, 0,  // 48
		-3.0,  0.2,  1.6, 0, 0,  // 49
		-3.0, -0.6,  1.6, 0, 0,  // 50
		-2.5, -0.6,  1.6, 0, 0,  // 51

		// right
		-3.0,  0.2,  1.6, 0, 0,  // 52
		-3.0,  0.2, -1.6, 0, 0,  // 53
		-3.0, -0.6, -1.6, 0, 0,  // 54
		-3.0, -0.6,  1.6, 0, 0,  // 55

		 // back
		-3.0,  0.2, -1.6, 0, 0,  // 56
		-2.5,  0.2, -1.6, 0, 0,  // 57
		-2.5, -0.6, -1.6, 0, 0,  // 58
		-3.0, -0.6, -1.6, 0, 0,  // 59

		 // left
		-2.5,  0.2,  1.6, 0, 0,  // 60
		-2.5,  0.2, -1.6, 0, 0,  // 61
		-2.5, -0.6, -1.6, 0, 0,  // 62
		-2.5, -0.6,  1.6, 0, 0,  // 63

		 // upper
		-3.0,  0.2, -1.6, 0, 0,  // 64
		-2.5,  0.2, -1.6, 0, 0,  // 65
		-2.5,  0.2,  1.6, 0, 0,  // 66
		-3.0,  0.2,  1.6, 0, 0,  // 67

		 // bottom
		-3.0, -0.6, -1.6, 0, 0,  // 68
		-2.5, -0.6, -1.6, 0, 0,  // 69
		-2.5, -0.6,  1.6, 0, 0,  // 70
		-3.0, -0.6,  1.6, 0, 0,  // 71
			
		 
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front	
		24, 25, 26, 24, 26, 27,	 // right leg
		48, 49, 50, 48, 50, 51,	 // left leg
		4,  5,  6,  4,  6,  7,   // right
		28, 29, 30, 28, 30, 31,  // right leg
		52, 53, 54, 52, 54, 55,	 // left leg
		8,  9,  10, 8,  10, 11,  // back
		32, 33, 34, 32, 34, 35,  // right leg
		56, 57, 58, 56, 58, 59,  // left leg
		12, 14, 13, 12, 15, 14,  // left
		36, 38, 37, 36, 39, 38,  // right leg
		60, 62, 61, 60, 63, 62,  // left leg
		16, 18, 17, 16, 19, 18,  // upper
		40, 42, 41, 40, 43, 42,  // right leg
		64, 66, 65, 64, 67, 66,  // left leg
		20, 22, 21, 20, 23, 22,  // bottom
		44, 46, 45, 44, 47, 46,  // right leg
		68, 70, 69, 68, 71, 70   // left leg

	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawColoredTable()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0, 3, 0));
	
	model = glm::rotate(model, angle, glm::vec3(1, 0, 0));
	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
    model = glm::rotate(model, angle, glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(3, 3, 3));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 112, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildColoredPlane()
{
	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("grass.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, -10.5, -50.0,  0,  0,
		 50.0, -10.5, -50.0, 50,  0,
		 50.0, -10.5,  50.0, 50, 50,
		-50.0, -10.5,  50.0,  0, 50,
	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}



void Demo::DrawColoredPlane()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 0.1f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.001f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}


int main(int argc, char** argv) {
	RenderEngine &app = Demo();
	app.Start("Camera: Free Camera and Object Rotation Implementation", 800, 600, false, false);
}