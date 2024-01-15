#include	"Pipeline.hpp"

#include	<fstream>
#include	<stdexcept>
#include	<iostream>

namespace	Engine
{
	Pipeline::Pipeline(const std::string &vertFile, const std::string &fragFile, Device &device, const PipelineConfigInfo &configInfo)
	:_device{device}
	{
		this->_creatGraphicsPipeline(vertFile, fragFile, configInfo);
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

	PipelineConfigInfo	Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo	configInfo{};

		return (configInfo);
	}


	void	Pipeline::_creatGraphicsPipeline(const std::string &vertFile, const std::string &fragFile, const PipelineConfigInfo &configInfo)
	{
		auto vertCode = Pipeline::_readFile(vertFile);
		auto fragCode = Pipeline::_readFile(fragFile);

		std::cout << "Vertex Shader Code Size " << vertCode.size() << std::endl;
		std::cout << "Fragment Shader Code Size " << fragCode.size() << std::endl;
		return ;
	}

	void	Pipeline::_createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule)
	{
		VkShaderModuleCreateInfo	createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

		if (vkCreateShaderModule(this->_device.getDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module!");
		return ;
	}
}