#include	"Game.hpp"

#include	<cstdlib>
#include	<iostream>
#include	<stdexcept>

int	main(void)
{
	Engine::Game	app;

	try
	{
		app.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}