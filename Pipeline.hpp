#pragma	once

#include	"Device.hpp"

#include	<string>
#include	<vector>

namespace	Engine
{
	struct PipelineConfigInfo
	{
	};
	

	class Pipeline
	{
	private:
		Device			&_device;
		VkPipeline		_graphicsPipeline;
		VkShaderModule	_vertShaderModule;
		VkShaderModule	_fragShaderModule;

		static std::vector<char>	_readFile(const std::string &filePath);

		void	_creatGraphicsPipeline(const std::string &vertFile, const std::string &fragFile, const PipelineConfigInfo &configInfo);
		void	_createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);
	public:
				Pipeline(const std::string &vertFile, const std::string &fragFile, Device &device, const PipelineConfigInfo &configInfo);

				Pipeline(const Pipeline &pipeline) = delete;
				~Pipeline(void){};

		Pipeline	&operator=(const Pipeline &rhs) = delete;

		static PipelineConfigInfo	defaultPipelineConfigInfo(uint32_t width, uint32_t height);
		
	};
}