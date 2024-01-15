#pragma	once

#include	"Window.hpp"

#include	<vector>

namespace	Engine
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR>	formats;
		std::vector<VkPresentModeKHR>	presentModes;
	};
	
	struct QueueFamilyIndices
	{
		uint32_t	graphicsFamily;
		bool		graphicsFamilyHasValue = false;
		uint32_t	presentFamily;
		bool		presentFamilyHasValue = false;

		bool	isComplete()
		{
			return (graphicsFamilyHasValue && presentFamilyHasValue);
		}
	};
	
	class Device
	{
	private:
		void	_createInstance(void);
		void	_setupDebugMessenger(void);
		void	_createSurface(void);
		void	_pickPhysicalDevice(void);
		void	_createLogicalDevice(void);
		void	_createCommandPool(void);

		bool						_isDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char *>	_getRequiredExtensions(void);
		bool						_checkValidationLayerSupport(void);
		QueueFamilyIndices			_findQueueFamilies(VkPhysicalDevice device);
		void						_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
		void						_hasGlfwRequiredInstanceExtensions(void);
		bool						_checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails		_querySwapChainSupport(VkPhysicalDevice device);

		VkInstance					_instance;
		VkDebugUtilsMessengerEXT	_debugMessenger;
		VkPhysicalDevice			_physicalDevice = VK_NULL_HANDLE;
		Window						&_window;
		VkCommandPool				_commandPool;

		VkDevice		_device;
		VkSurfaceKHR	_surface;
		VkQueue			_graphicsQueue;
		VkQueue			_presentQueue;

		const std::vector<const char *>	_validationLayers = {"VK_LAYER_KHRONOS_validation"};
		const std::vector<const char *>	_deviceExtensions = {"VK_KHR_SWAPCHAIN_EXTENSION_NAME"};

	public:
	#ifndef	NDEBUG
		const bool	enableValidationLayer = false;
	#else
		const bool	enableValidationLayer = true;
	#endif

		VkPhysicalDeviceProperties	properties;

				Device(Window &window);

				Device(const Device &device) = delete;
				Device(Device &&device) = delete;
				~Device(void);

		void	operator=(const Device &rhs) = delete;
	
		VkCommandPool			&getCommandPool(void) {return (this->_commandPool);}
		VkDevice				&getDevice(void) {return (this->_device);}
		VkSurfaceKHR			&getSurface(void) {return (this->_surface);}
		VkQueue					&getGraphicsQueue(void) {return (this->_graphicsQueue);}
		VkQueue					&getPresentQueue(void) {return (this->_presentQueue);}

		SwapChainSupportDetails	getSwapChainSupport(void) {return (this->_querySwapChainSupport(this->_physicalDevice));}
		QueueFamilyIndices		getPhysicalQueueFamilies(void) {return (this->_findQueueFamilies(this->_physicalDevice));}
		
		uint32_t	findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat	findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		void			createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
		VkCommandBuffer	beginSingleTimeCommands(void);
		void			endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void			copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void			copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void	createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

	};
}