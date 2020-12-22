#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkan_loader.h"


PFN_vkCreateInstance vkCreateInstance = NULL;
PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = NULL;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = NULL;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = NULL;

#define VULKAN_DEFINE(func_name)  \
    PFN_##func_name func_name = NULL;

#define VULKAN_FUNCTION VULKAN_DEFINE
VULKAN_FUNCTIONS
#undef VULKAN_FUNCTION

#define VULKAN_LOAD(func_name)  \
    func_name = (PFN_##func_name)glfwGetInstanceProcAddress(instance, #func_name);

#define VULKAN_VALIDATE(func_name)                                 \
    if (func_name == NULL) {                                       \
        fprintf(stderr, "failed to load func: %s\n", #func_name);  \
        return false;                                              \
    }


bool
vulkan_loader_load_initial(void)
{
    vkCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
    vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)glfwGetInstanceProcAddress(NULL, "vkEnumerateInstanceVersion");
    vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)glfwGetInstanceProcAddress(NULL, "vkEnumerateInstanceExtensionProperties");
    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)glfwGetInstanceProcAddress(NULL, "vkEnumerateInstanceLayerProperties");

    if (vkCreateInstance == NULL) {
        fprintf(stderr, "failed to load func: %s\n", "vkCreateInstance");
        return false;
    }
    if (vkEnumerateInstanceVersion == NULL) {
        fprintf(stderr, "failed to load func: %s\n", "vkEnumerateInstanceVersion");
        return false;
    }
    if (vkEnumerateInstanceExtensionProperties == NULL) {
        fprintf(stderr, "failed to load func: %s\n", "vkEnumerateInstanceExtensionProperties");
        return false;
    }
    if (vkEnumerateInstanceLayerProperties == NULL) {
        fprintf(stderr, "failed to load func: %s\n", "vkEnumerateInstanceLayerProperties");
        return false;
    }

    return true;
}

bool
vulkan_loader_load_functions(VkInstance instance)
{
    assert(instance != VK_NULL_HANDLE);

    #define VULKAN_FUNCTION VULKAN_LOAD
    VULKAN_FUNCTIONS
    #undef VULKAN_FUNCTION

    #define VULKAN_FUNCTION VULKAN_VALIDATE
    VULKAN_FUNCTIONS
    #undef VULKAN_FUNCTION

    return true;
}
