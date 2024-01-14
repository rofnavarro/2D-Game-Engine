#include	"Window.hpp"

namespace	Engine
{
	Window::Window(void)
	:_name("Default"), _width(800), _height(600)
	{
		this->_initWindow();
		return;
	}

	Window::Window(int width, int height, std::string name)
	: _name(name), _width(width), _height(height)
	{
		this->_initWindow();
		return ;
	}

	Window::Window(const Window &window)
	{
		*this = window;
		return ;
	}

	Window::~Window(void)
	{
		if (this->_window)
		{
			glfwDestroyWindow(this->_window);

			glfwTerminate();
		}
		return ;
	}

	Window	&Window::operator=(const Window &rhs)
	{
		if (this != &rhs)
		{
			this->_width = rhs._width;
			this->_height = rhs._height;
			this->_name = rhs._name;
		}
		return (*this);
	}

	bool	Window::closeWindow(void)
	{
		return (glfwWindowShouldClose(this->_window));
	}

	void	Window::_initWindow(void)
	{
		if (!glfwInit())
		{
			glfwDestroyWindow(this->_window);
			glfwTerminate();
		}
		else
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			this->_window = glfwCreateWindow(this->_width, this->_height, \
											this->_name.c_str(), nullptr, nullptr);
			
			if (!this->_window)
				glfwTerminate();
		}
		return ;
	}
}
