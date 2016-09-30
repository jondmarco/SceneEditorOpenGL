#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"


#include <string>

class InputManager;

using std::string;


class OpenGLApp
{
public:

	friend class RenderSystem;

	bool Initialize(GLuint windowWidth, GLuint windowHeight, string windowTitle, GLboolean isResizeable);
	void SetInputManager(InputManager* inputManager);

	void InitiateShutdown();
	void Terminate();

	bool ShouldTerminate();

	void PollEvents();
	void SwapBuffers();

	void CheckOpenGLErrors();

	void ShowCursor();
	void HideCursor();

private:

	GLFWwindow* _window;

	void ConfigureRenderingOptions();
	void RegisterInputCallbacks();

	static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);
	static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void ErrorCallback(int, const char* err_str);

	const GLboolean _renderWireframe = GL_FALSE;
	const GLboolean _disableVSync = GL_TRUE;

	const string LOG_CHANNEL = "OpenGLApp";
};
