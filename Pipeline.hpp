#pragma	once

#include	<string>
#include	<vector>

namespace	Engine
{
	class Pipeline
	{
	private:
		static std::vector<char>	_readFile(const std::string &filePath);

		void	_creatGraphicsPipeline(const std::string &vertFile, const std::string &fragFile);

	public:
			Pipeline(const std::string &vertFile, const std::string &fragFile);

	};
}