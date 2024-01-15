#pragma	once

#include	"Window.hpp"
#include	"Pipeline.hpp"
#include	"Device.hpp"


namespace	Engine
{
	class Game
	{
	private:
		static constexpr int	_width = 800;
		static constexpr int	_height = 600;

		Window		_appWin{this->_width, this->_height, "engine_v1"};
		Device		_appDevice{_appWin};
		Pipeline	_appPipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", _appDevice, Pipeline::defaultPipelineConfigInfo(_width, _height)};

	public:
				Game(void);

				Game(const Game &game) = delete;
				~Game(void);

		Game	&operator=(const Game &rhs) = delete;

		void	run(void);

	};
}
