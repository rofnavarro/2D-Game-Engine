#include	"Game.hpp"

namespace	Engine
{
	Game::Game(void)
	{
		return ;
	}

	Game::~Game(void)
	{
		this->_appWin.~Window();
		return ;
	}

	void	Game::run(void)
	{
		while (!this->_appWin.closeWindow())
		{
			glfwPollEvents();
		}
		return ;
	}
}