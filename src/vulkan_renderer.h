#ifndef VULKAN_RENDERER_H_INCLUDED
#define VULKAN_RENDERER_H_INCLUDED

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct vulkan_renderer {
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;
    VkSwapchainKHR swapchain;
};

int vulkan_renderer_init(struct vulkan_renderer* renderer, GLFWwindow* window);
int vulkan_renderer_term(struct vulkan_renderer* renderer);

#endif
