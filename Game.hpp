#pragma	once

#include	"Window.hpp"

namespace	Engine
{
	class Game
	{
	private:
		Window	_appWin{this->_width, this->_height, "engine_v1"};

	public:
		static constexpr int	_width = 800;
		static constexpr int	_height = 600;

				Game(void);

				~Game(void);

		void	run(void);

	};
}
