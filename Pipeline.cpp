#include	"Pipeline.hpp"

#include	<fstream>
#include	<stdexcept>
#include	<iostream>

namespace	Engine
{
	Pipeline::Pipeline(const std::string &vertFile, const std::string &fragFile)
	{
		Pipeline::_creatGraphicsPipeline(vertFile, fragFile);
		return ;
	}

	std::vector<char>	Pipeline::_readFile(const std::string &filePath)
	{
		std::ifstream		file(filePath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("failed to open file: " + filePath);

		size_t				fileSize = static_cast<size_t>(file.tellg());
		std::vector<char>	buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return (buffer);
	}

	void	Pipeline::_creatGraphicsPipeline(const std::string &vertFile, const std::string &fragFile)
	{
		auto vertCode = Pipeline::_readFile(vertFile);
		auto fragCode = Pipeline::_readFile(fragFile);

		std::cout << "Vertex Shader Code Size " << vertCode.size() << std::endl;
		std::cout << "Fragment Shader Code Size " << fragCode.size() << std::endl;
	}
}