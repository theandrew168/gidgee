#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkan_loader.h"

static void
print_usage(const char* arg0)
{
    printf("usage: %s [options]\n", arg0);
    printf("\n");
    printf("Options:\n");
    printf("  -h --help        print this help\n");
    printf("  -f --fullscreen  fullscreen window\n");
    printf("  -v --vsync       enable vsync\n");
}

int
main(int argc, char* argv[])
{
    bool fullscreen = false;
    bool vsync = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0) {
            fullscreen = true;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--vsync") == 0) {
            vsync = true;
        }
    }

    if (!glfwInit()) {
        const char* error = NULL;
        glfwGetError(&error);
        fprintf(stderr, "failed to init GLFW3: %s\n", error);
        return EXIT_FAILURE;
    }

    if (!glfwVulkanSupported()) {
        fprintf(stderr, "Vulkan is not supported on this device / platform!\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = NULL;
    if (fullscreen) window = glfwCreateWindow(mode->width, mode->height, "Gidgee Physics", monitor, NULL);
    else window = glfwCreateWindow(1280, 720, "Gidgee Physics", NULL, NULL);
    if (window == NULL) {
        const char* error = NULL;
        glfwGetError(&error);
        fprintf(stderr, "failed to create GLFW3 window: %s\n", error);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // load vkCreateInstance function
    vulkan_loader_load_initial();

    uint32_t extension_count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
    assert(extensions != NULL);
    for (uint32_t i = 0; i < extension_count; i++) {
        printf("Desired Extension: %s\n", extensions[i]);
    }

    const char* layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    uint32_t layer_count = sizeof(layers) / sizeof(*layers);

    for (uint32_t i = 0; i < layer_count; i++) {
        printf("Desired Layer: %s\n", layers[i]);
    }

    uint32_t vk_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &vk_extension_count, NULL);
    VkExtensionProperties* vk_extensions = malloc(vk_extension_count * sizeof(VkExtensionProperties));
    assert(vk_extensions != NULL);
    vkEnumerateInstanceExtensionProperties(NULL, &vk_extension_count, vk_extensions);

    for (uint32_t i = 0; i < vk_extension_count; i++) {
        printf("Available Extension: %s\n", vk_extensions[i].extensionName);
    }

    free(vk_extensions);

    uint32_t vk_layer_count = 0;
    vkEnumerateInstanceLayerProperties(&vk_layer_count, NULL);
    VkLayerProperties* vk_layers = malloc(vk_layer_count * sizeof(VkLayerProperties));
    assert(vk_layers != NULL);
    vkEnumerateInstanceLayerProperties(&vk_layer_count, vk_layers);

    for (uint32_t i = 0; i < vk_layer_count; i++) {
        printf("Available Layer: %s\n", vk_layers[i].layerName);
    }

    free(vk_layers);

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &(VkApplicationInfo){
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Gidgee Physics",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_2,
        },
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
    };

    VkInstance instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&instance_create_info, NULL, &instance) != VK_SUCCESS) {
        fprintf(stderr, "failed to create Vulkan instance\n");
        return EXIT_FAILURE;
    }

    // load all other Vulkan functions
    vulkan_loader_load_functions(instance);

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        fprintf(stderr, "failed to create Vulkan surface\n");
        return EXIT_FAILURE;
    }

    double last_second = glfwGetTime();
    double last_frame = last_second;
    long frame_count = 0;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        double now = glfwGetTime();
        double delta = now - last_frame;
        last_frame = now;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        frame_count++;
        if (glfwGetTime() - last_second >= 1.0) {
            printf("FPS: %ld  (%lf ms/frame)\n", frame_count, 1000.0/frame_count);
            frame_count = 0;
            last_second += 1.0;
        }

        glfwPollEvents();
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
