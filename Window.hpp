#pragma	once

#define	GLFW_INCLUDE_VULKAN
# include	<GLFW/glfw3.h>

#include	<string>

namespace	Engine
{
	class Window
	{
	private:
		GLFWwindow	*_window;
		std::string	_name;
		int			_width;
		int			_height;

		void		_initWindow(void);

	public:
					Window(void);
					Window(int width, int height, std::string name);

					Window(const Window &window);
					~Window(void);

		Window		&operator=(const Window &rhs);

		bool		closeWindow(void);

	};
}
