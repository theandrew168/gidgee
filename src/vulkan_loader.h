#ifndef VULKAN_LOADER_H_INCLUDED
#define VULKAN_LOADER_H_INCLUDED

#include <stdbool.h>

#include <vulkan/vulkan.h>

#define VULKAN_FUNCTIONS                  \
    VULKAN_FUNCTION(vkDestroyInstance)    \
    VULKAN_FUNCTION(vkDestroySurfaceKHR)  \

// some functions can be loaded prior to obtaining a vulkan instance:
// https://stackoverflow.com/questions/8648428/an-error-occurred-while-validating-hresult-8000000a
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;

#define VULKAN_DECLARE(func_name)  \
    extern PFN_##func_name func_name;

#define VULKAN_FUNCTION VULKAN_DECLARE
VULKAN_FUNCTIONS
#undef VULKAN_FUNCTION


bool vulkan_loader_load_initial(void);
bool vulkan_loader_load_functions(VkInstance instance);

#endif
