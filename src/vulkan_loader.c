#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkan_loader.h"

// None of the other expansions need this extra param. Is there a
// better way to handle this? I don't love global-ish things...
static VkInstance _instance = VK_NULL_HANDLE;

PFN_vkCreateInstance vkCreateInstance = NULL;

#define VULKAN_DEFINE(func_name)  \
    VKAPI_ATTR VKAPI_CALL PFN_##func_name func_name = NULL;

#define VULKAN_FUNCTION VULKAN_DEFINE
VULKAN_FUNCTIONS
#undef VULKAN_FUNCTION

#define VULKAN_LOAD(func_name)  \
    func_name = (PFN_##func_name)glfwGetInstanceProcAddress(_instance, #func_name);

#define VULKAN_VALIDATE(func_name)                                 \
    if (func_name == NULL) {                                       \
        fprintf(stderr, "failed to load func: %s\n", #func_name);  \
        return false;                                              \
    }


bool
vulkan_loader_load_initial(void)
{
    // handle the vkCreateInstance special case
    vkCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
    if (vkCreateInstance == NULL) {
        fprintf(stderr, "failed to load func: %s\n", "vkCreateInstance");
        return false;
    }

    return true;
}

bool
vulkan_loader_load_functions(VkInstance instance)
{
    assert(instance != VK_NULL_HANDLE);
    _instance = instance;

    #define VULKAN_FUNCTION VULKAN_LOAD
    VULKAN_FUNCTIONS
    #undef VULKAN_FUNCTION

    #define VULKAN_FUNCTION VULKAN_VALIDATE
    VULKAN_FUNCTIONS
    #undef VULKAN_FUNCTION

    return true;
}
