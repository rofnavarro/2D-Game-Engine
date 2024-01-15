#include	"Device.hpp"

#include	<cstring>
#include	<iostream>
#include	<set>
#include	<unordered_set>

namespace	Engine
{
	static VKAPI_ATTR VkBool32	VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, \
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData)
	{
		std::cerr << "validation layer: " << callbackData->pMessage << std::endl;
		return (VK_FALSE);
	}

	VkResult	CreateDebugUtilsMessengerEXT
	(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT *createInfo,
		const VkAllocationCallbacks *allocator,
		VkDebugUtilsMessengerEXT *debugMessenger
	)
	{
		auto	func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
			return func(instance, createInfo, allocator, debugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void	DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *allocator)
	{
		auto	func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
			func(instance, debugMessenger, allocator);
		return ;
	}

	Device::Device(Window &window)
	:_window(window)
	{
		this->_createInstance();
		this->_setupDebugMessenger();
		this->_createSurface();
		this->_pickPhysicalDevice();
		this->_createLogicalDevice();
		this->_createCommandPool();
		return ;
	}

	Device::~Device(void)
	{
		vkDestroyCommandPool(this->_device, this->_commandPool, nullptr);
		vkDestroyDevice(this->_device, nullptr);

		if (this->enableValidationLayer)
			DestroyDebugUtilsMessengerEXT(this->_instance, this->_debugMessenger, nullptr);

		vkDestroySurfaceKHR(this->_instance, this->_surface, nullptr);
		vkDestroyInstance(this->_instance, nullptr);
		return ;
	}

	uint32_t	Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties	memProperties;

		vkGetPhysicalDeviceMemoryProperties(this->_physicalDevice, &memProperties);
		for (uint32_t	i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return (i);
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkFormat	Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat	format : candidates)
		{
			VkFormatProperties	props;

			vkGetPhysicalDeviceFormatProperties(this->_physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}
		throw std::runtime_error("failed to find supported format!");
	}

	void	Device::createBuffer
	(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer &buffer,
		VkDeviceMemory &bufferMemory
	)
	{
		VkBufferCreateInfo	bufferInfo{};

		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("failed to create vertex buffer!");

		VkMemoryRequirements	memRequirements;
		VkMemoryAllocateInfo	allocInfo{};

		vkGetBufferMemoryRequirements(this->_device, buffer, &memRequirements);

		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(this->_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate vertex buffer memory!");

		vkBindBufferMemory(this->_device, buffer, bufferMemory, 0);
		return ;
	}

	VkCommandBuffer	Device::beginSingleTimeCommands(void)
	{
		VkCommandBufferAllocateInfo	allocInfo{};
		VkCommandBuffer				commandBuffer;
		VkCommandBufferBeginInfo	beginInfo{};

		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = this->_commandPool;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(this->_device, &allocInfo, &commandBuffer);

		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return (commandBuffer);
	}

	void	Device::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(this->_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(this->_graphicsQueue);

		vkFreeCommandBuffers(this->_device, this->_commandPool, 1, &commandBuffer);
		return ;
	}

	void	Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer	commandBuffer = this->beginSingleTimeCommands();
		VkBufferCopy	copyRegion{};

		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		this->endSingleTimeCommands(commandBuffer);
		return ;
	}

	void	Device::copyBufferToImage
	(
		VkBuffer buffer, VkImage image,
		uint32_t width, uint32_t height,
		uint32_t layerCount
	)
	{
		VkCommandBuffer		commandBuffer = this->beginSingleTimeCommands();
		VkBufferImageCopy	region{};

		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = {width, height, 1};

		vkCmdCopyBufferToImage
		(
			commandBuffer, buffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &region
		);
		this->endSingleTimeCommands(commandBuffer);
		return ;
	}

	void	Device::createImageWithInfo
	(
		const VkImageCreateInfo &imageInfo,
		VkMemoryPropertyFlags properties,
		VkImage &image,
		VkDeviceMemory &imageMemory
	)
	{
		if (vkCreateImage(this->_device, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image!");

		VkMemoryRequirements	memRequirements;
		VkMemoryAllocateInfo	allocInfo{};

		vkGetImageMemoryRequirements(this->_device, image, &memRequirements);

		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(this->_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate image memory!");

		if (vkBindImageMemory(this->_device, image, imageMemory, 0) != VK_SUCCESS)
			throw std::runtime_error("failed to bind image memory!");
		return ;
	}

	void	Device::_createInstance(void)
	{
		if (enableValidationLayer && !this->_checkValidationLayerSupport())
			throw std::runtime_error("validation layers requested, but not available!");

		VkApplicationInfo		appInfo = {};
		VkInstanceCreateInfo	createInfo = {};

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Engine App";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto	extensions = this->_getRequiredExtensions();
		
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT	debugCreateInfo;

		if (enableValidationLayer)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(this->_validationLayers.size());
			createInfo.ppEnabledLayerNames = this->_validationLayers.data();

			this->_populateDebugMessengerCreateInfo(debugCreateInfo);
			
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &this->_instance) != VK_SUCCESS)
			throw std::runtime_error("failed to create instance!");

		this->_hasGlfwRequiredInstanceExtensions();
		return ;
	}

	void	Device::_setupDebugMessenger(void)
	{
		if (!enableValidationLayer)
			return;
		
		VkDebugUtilsMessengerCreateInfoEXT	createInfo;

		this->_populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(this->_instance, &createInfo, nullptr, &this->_debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug messenger!");
		return ;
	}

	void	Device::_createSurface(void)
	{
		this->_window.createWindowSurface(this->_instance, &this->_surface);
		return ;
	}

	void	Device::_pickPhysicalDevice(void)
	{
		uint32_t	deviceCount = 0;

		vkEnumeratePhysicalDevices(this->_instance, &deviceCount, nullptr);
		
		if (deviceCount == 0)
			throw std::runtime_error("failed to find GPUs with Vulkan support!");

		std::cout << "Device count: " << deviceCount << std::endl;
		std::vector<VkPhysicalDevice>	devices(deviceCount);

		vkEnumeratePhysicalDevices(this->_instance, &deviceCount, devices.data());

		for (const auto	&device : devices)
		{
			if (this->_isDeviceSuitable(device))
			{
				this->_physicalDevice = device;
				break ;
			}
		}

		if (this->_physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find a suitable GPU!");

		vkGetPhysicalDeviceProperties(this->_physicalDevice, &this->properties);
		std::cout << "physical device: " << properties.deviceName << std::endl;
		return ;
	}

	void	Device::_createLogicalDevice(void)
	{
		QueueFamilyIndices	indices = this->_findQueueFamilies(this->_physicalDevice);
		std::set<uint32_t>	uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

		std::vector<VkDeviceQueueCreateInfo>	queueCreateInfos;

		float	queuePriority = 1.0f;

		for (uint32_t	queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo	queueCreateInfo = {};

			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures	deviceFeatures = {};
		VkDeviceCreateInfo			createInfo = {};

		deviceFeatures.samplerAnisotropy = VK_TRUE;

		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(this->_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = this->_deviceExtensions.data();

		if (enableValidationLayer)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(this->_validationLayers.size());
			createInfo.ppEnabledLayerNames = this->_validationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(this->_physicalDevice, &createInfo, nullptr, &this->_device) != VK_SUCCESS)
			throw std::runtime_error("failed to create logical device!");

		vkGetDeviceQueue(this->_device, indices.graphicsFamily, 0, &this->_graphicsQueue);
		vkGetDeviceQueue(this->_device, indices.presentFamily, 0, &this->_presentQueue);
		return ;
	}

	void	Device::_createCommandPool(void)
	{
		QueueFamilyIndices		queueFamilyIndices = this->getPhysicalQueueFamilies();
		VkCommandPoolCreateInfo	poolInfo = {};

		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(this->_device, &poolInfo, nullptr, &this->_commandPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create command pool!");
		return ;
	}

	bool	Device::_isDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices	indices = this->_findQueueFamilies(device);

		bool extensionsSupported = this->_checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;

		if (extensionsSupported)
		{
			SwapChainSupportDetails	swapChainSupport = this->_querySwapChainSupport(device);

			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures	supportedFeatures;

		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
		
		return
		(indices.isComplete() && extensionsSupported &&
		swapChainAdequate && supportedFeatures.samplerAnisotropy
		);
	}

	std::vector<const char *>	Device::_getRequiredExtensions(void)
	{
		uint32_t	glfwExtensionCount = 0;
		const char	**glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *>	extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		
		if (enableValidationLayer)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return (extensions);
	}

	bool	Device::_checkValidationLayerSupport(void)
	{
		uint32_t	layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		
		std::vector<VkLayerProperties>	availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char	*layerName : this->_validationLayers)
		{
			bool	layerFound = false;

			for (const auto	&layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
				return (false);
		}
		return (true);
	}

	QueueFamilyIndices	Device::_findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices	indices;
		uint32_t 			queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int	i = 0;
		for (const auto &queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}

			VkBool32	presentSupport = false;

			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->_surface, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport)
			{
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}
			if (indices.isComplete())
				break;
			i++;
		}
		return (indices);
	}

	void Device::_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
								VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
		return ;
	}

	void	Device::_hasGlfwRequiredInstanceExtensions(void)
	{
		uint32_t	extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties>	extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "available extensions:" << std::endl;

		std::unordered_set<std::string>	available;
		for (const auto &extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		std::cout << "required extensions:" << std::endl;

		auto	requiredExtensions = this->_getRequiredExtensions();
		for (const auto &required : requiredExtensions)
		{
			std::cout << "\t" << required << std::endl;
			if (available.find(required) == available.end())
				throw std::runtime_error("Missing required glfw extension");
		}
		return ;
	}

	bool	Device::_checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t	extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties>	availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string>	requiredExtensions(this->_deviceExtensions.begin(), this->_deviceExtensions.end());

		for (const auto	&extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return (requiredExtensions.empty());
	}

	SwapChainSupportDetails	Device::_querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails	details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->_surface, &details.capabilities);

		uint32_t	formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->_surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->_surface, &formatCount, details.formats.data());
		}

		uint32_t	presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->_surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->_surface, &presentModeCount, details.presentModes.data());
		}
		return (details);
	}
}
