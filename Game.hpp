#pragma	once

#include	"Window.hpp"
#include	"Pipeline.hpp"


namespace	Engine
{
	class Game
	{
	private:
		static constexpr int	_width = 800;
		static constexpr int	_height = 600;

		Window		_appWin{this->_width, this->_height, "engine_v1"};
		Pipeline	_appPipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv"};

	public:
				Game(void);

				~Game(void);

		void	run(void);

	};
}
