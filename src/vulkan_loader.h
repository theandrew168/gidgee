#ifndef VULKAN_LOADER_H_INCLUDED
#define VULKAN_LOADER_H_INCLUDED

#include <stdbool.h>

#include <vulkan/vulkan.h>

#define VULKAN_FUNCTIONS                  \
    VULKAN_FUNCTION(vkDestroyInstance)    \
    VULKAN_FUNCTION(vkDestroySurfaceKHR)

// vkCreateInstance is a special case because it gets loaded
// before a VkInstance gets created (this func will create one)
extern PFN_vkCreateInstance vkCreateInstance;

#define VULKAN_DECLARE(func_name)  \
    extern VKAPI_ATTR VKAPI_CALL PFN_##func_name func_name;

#define VULKAN_FUNCTION VULKAN_DECLARE
VULKAN_FUNCTIONS
#undef VULKAN_FUNCTION


bool vulkan_loader_load_initial(void);
bool vulkan_loader_load_functions(VkInstance instance);

#endif
