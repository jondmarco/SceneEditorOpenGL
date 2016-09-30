#include "OpenGLApp.h"

#include "System\Input\InputManager.h"
#include "Services\Logger.h"


bool OpenGLApp::Initialize(GLuint windowWidth, GLuint windowHeight, string windowTitle, GLboolean isResizeable)
{
	Logger::LogInfo(LOG_CHANNEL, "Initializing OpenGL Application...\n");

	// Initialize GLFW
	glfwInit();

	// Set error callback
	glfwSetErrorCallback(ErrorCallback);

	// Set OpenGL version (4.4 by default to support Intel HD Graphics)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Constrain OpenGL to only run in core-profile mode
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Enable/Disable window resizing
	glfwWindowHint(GLFW_RESIZABLE, isResizeable);

	// Initialize GLFW window
	_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
	if (_window == nullptr)
	{
		Logger::LogError("GLFW", "Failed to create GLFW window");
		glfwTerminate();
		return GL_FALSE;
	}

	// Make the window our current context
	glfwMakeContextCurrent(_window);

	// Set OpenGL viewport (used to determine the size of OpenGL render area and map OpenGL coordinates to screen coordinates)
	// By default equal to window size
	glViewport(0, 0, windowWidth, windowHeight);

	// Initialize GLEW to enable OpenGL functionality
	glewExperimental = GL_TRUE;
	if (!glewInit() == GLEW_OK)
	{
		Logger::LogError("GLEW", "Failed to initialize GLEW");
		return GL_FALSE;
	}

	// Discard OpenGL error caused by a bug in glew
	GLenum err = glGetError();
	if (err == GL_INVALID_ENUM) {} // Ignore the error
	else if(err != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL", "OpenGL error code: " + err);
	}

	// Configure default rendering options
	ConfigureRenderingOptions();

	// Register default input callbacks
	RegisterInputCallbacks();

	// Print current OpenGL version
	string renderer = (char*)glGetString(GL_RENDERER);
	string openGLVersion = (char*)glGetString(GL_VERSION);

	Logger::LogInfo(LOG_CHANNEL, "Renderer: " + renderer);
	Logger::LogInfo(LOG_CHANNEL, "OpenGL version supported: " + openGLVersion + "\n");

	Logger::LogInfo(LOG_CHANNEL, "Initialization completed!\n");

	return GL_TRUE;
}

void OpenGLApp::SetInputManager(InputManager* inputManager)
{
	glfwSetWindowUserPointer(_window, inputManager);
}

void OpenGLApp::CheckOpenGLErrors()
{
	GLenum err = GL_NO_ERROR;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL", "Error code: " + err);
	}
}

void OpenGLApp::ShowCursor()
{
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OpenGLApp::HideCursor()
{
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void OpenGLApp::InitiateShutdown()
{
	glfwSetWindowShouldClose(_window, GL_TRUE);
}

bool OpenGLApp::ShouldTerminate()
{
	return glfwWindowShouldClose(_window) != 0;
}

void OpenGLApp::Terminate()
{
	glfwTerminate();
}

void OpenGLApp::PollEvents()
{
	glfwPollEvents();
}

void OpenGLApp::SwapBuffers()
{
	glfwSwapBuffers(_window);
}

void OpenGLApp::RegisterInputCallbacks()
{
	glfwSetKeyCallback(_window, KeyCallback);
	glfwSetCursorPosCallback(_window, CursorPositionCallback);
	glfwSetScrollCallback(_window, ScrollCallback);
	glfwSetMouseButtonCallback(_window, MouseButtonCallback);
	
	glfwSetFramebufferSizeCallback(_window, FramebufferSizeCallback);
}

void OpenGLApp::ConfigureRenderingOptions()
{
	// Wireframe only render for debugging purposes
	if (_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Enable/Disable vertival synchronization
	if (_disableVSync)
	{
		glfwSwapInterval(0);
	}
	else
	{
		glfwSwapInterval(1);
	}

	// Enable Z buffer depth test
	glEnable(GL_DEPTH_TEST);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLApp::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	InputManager* inputManager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (inputManager != nullptr)
	{
		inputManager->UpdateKeyState(key, action);
	}
}

void OpenGLApp::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	InputManager* inputManager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (inputManager != nullptr)
	{
		inputManager->UpdateCursorPosition(xpos, ypos);
	}
}

void OpenGLApp::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	InputManager* inputManager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (inputManager != nullptr)
	{
		inputManager->UpdateScrollOffset(xoffset, yoffset);
	}
}

void OpenGLApp::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	InputManager* inputManager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (inputManager != nullptr)
	{
		inputManager->UpdateMouseButtonState(button, action);
	}
}

void OpenGLApp::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	InputManager* inputManager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));

	glViewport(0, 0, width, height);
	
	if (inputManager != nullptr)
	{
		inputManager->OnFramebufferSizeChanged(width, height);
	}
}

void OpenGLApp::ErrorCallback(int, const char* err_str)
{
	Logger::LogError("GLFW", err_str);
}
