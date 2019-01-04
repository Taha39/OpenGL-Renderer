#ifndef __RENDER_H__
#define __RENDER_H__

#include <Windows.h>
#include <GL/glew.h>
#include "Shader.h"

class render {
public:
	render(HDC hdc, int w, int h);
	~render();
	void present(const uint8_t* data, int w, int h);
	bool init();
	
private:
	HDC context_{ nullptr };
	GLuint texture_;
	GLuint VBO, VAO, EBO;
	Shader* ourShader_{ nullptr };
	int width_;
	int height_;
};

#endif
