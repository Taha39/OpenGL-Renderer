#include "render.h"

render::render(HDC hdc, int w, int h) :context_{ hdc },
width_{ w }, height_{h} {
}

render::~render() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);	
	delete ourShader_;
}

bool render::init() {
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	//// Build and compile our shader program
	ourShader_ = new Shader("res/shaders/core.vs", "res/shaders/core.frag");
	
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions        // Texture Coords
		1.0f, 1.0f, 0.0f,	 1.0f, 1.0f, // Top Right
		1.f, -1.0f, 0.0f,	 1.0f, 0.0f, // Bottom Right
		-1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, // Bottom Left
		-1.0f,  1.0f, 0.0f,	 0.0f, 1.0f  // Top Left
	};
	GLuint indices[] =
	{  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};
	//EBO: Element Buffer Object.
	//VBO: Vertex Buffer Object.
	//VAO: Vertex Array Object.

	glGenVertexArrays(1, &VAO);

	//It stores the format of the vertex data as well as the Buffer Objects (see below) providing the vertex data arrays.
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);// this is for the vertices that have created above.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);// This is for the indices that have created above.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	
	// Texture Coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO


	// ===================
	// Texture loading
	// ===================

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void render::present(const uint8_t* data, int w, int h) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Render
	// Clear the colorbuffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the triangle
	ourShader_->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);	
	// Draw container
	glBindVertexArray(VAO);

	//When glDrawElements is called, it uses count sequential elements from an enabled array,
	//starting at indices to construct a sequence of geometric primitives. mode specifies what
	// kind of primitives are constructed and how the array elements construct these primitives. 
	//If more than one array is enabled, each is used.
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //glDrawElements — render primitives from array data
	glBindVertexArray(0);

	// Swap the screen buffers
	::SwapBuffers(context_);	
}

