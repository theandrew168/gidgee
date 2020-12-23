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
        fprintf(stderr, "failed to init GLFW3!\n");
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
        fprintf(stderr, "failed to create GLFW3 window!\n");
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

    // TODO: ensure available extensions subset of required extensions
    // TODO: ensure available layers subset of required layers

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

    // query number of Vulkan devices
    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);

    // abort if no devices are available
    if (physical_device_count == 0) {
        fprintf(stderr, "no Vulkan physical devices available\n");
        return EXIT_FAILURE;
    }

    // allocate buffer to hold device info
    VkPhysicalDevice* physical_devices = malloc(physical_device_count * sizeof(VkPhysicalDevice));
    assert(physical_devices != NULL);

    // query Vulkan device info
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);

    // declare which device extensions we want
    const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    uint32_t device_extension_count = sizeof(device_extensions) / sizeof(*device_extensions);

    // keep track of which physical device to use and its queue families
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    long device_graphics_queue = -1;
    long device_presentation_queue = -1;

    // check devices for first one that:
    // 1. has at least one graphics queue
    // 2. has at least one presentation queue
    for (uint32_t i = 0; i < physical_device_count; i++) {
        long graphics_queue = -1;
        long presentation_queue = -1;
        bool swapchain_support = false;
        bool swapchain_viable = true;

        // query device properties
        VkPhysicalDeviceProperties properties = { 0 };
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        // query device features
        VkPhysicalDeviceFeatures features = { 0 };
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        // summarize device properties
        printf("Device %d: %s\n", i + 1, properties.deviceName);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            printf("  + discrete GPU\n");
        } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            printf("  + integrated GPU\n");
        } else {
            printf("  - other GPU\n");
        }

        // summarize device features
        if (features.geometryShader) {
            printf("  + supports geometry shaders\n");
        } else {
            printf("  - does not support geometry shaders\n");
        }

        // query device queue families
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, NULL);
        VkQueueFamilyProperties* queue_families = malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
        assert(queue_families != NULL);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_families);

        // check queue family capabilities
        for (uint32_t queue = 0; queue < queue_family_count; queue++) {
            // check queue family for graphics capabilities
            if (queue_families[queue].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                if (graphics_queue == -1) graphics_queue = queue;
            }

            // check queue family for presentation capabilities
            VkBool32 supports_presentation = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_devices[i], queue, surface, &supports_presentation);
            if (supports_presentation) {
                if (presentation_queue == -1) presentation_queue = queue;
            }
        }

        free(queue_families);

        // summarize graphics queue families
        if (graphics_queue != -1) {
            printf("  + supports graphics queues\n");
        } else {
            printf("  - does not support graphics queues\n");
        }

        // summarize presentation queue families
        if (presentation_queue != -1) {
            printf("  + supports presentation queues\n");
        } else {
            printf("  - does not support presentation queues\n");
        }

        // query available device extensions
        uint32_t ext_count = 0;
        vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &ext_count, NULL);
        VkExtensionProperties* exts = malloc(ext_count * sizeof(VkExtensionProperties));
        assert(exts != NULL);
        vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &ext_count, exts);

        // debug print the available device extensions
        // for (long ext = 0; ext < ext_count; ext++) {
        //     printf("Available Device Extension: %s\n", exts[ext].extensionName);
        // }

        // ensure availability of VK_KHR_swapchain device extension
        // TODO: check for a proper subset of device_extensions
        for (uint32_t ext = 0; ext < ext_count; ext++) {
            if (strcmp(exts[ext].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                swapchain_support = true;
            }
        }

        free(exts);

        // summarize swapchain support
        if (swapchain_support) {
            printf("  + supports swapchain\n");
        } else {
            printf("  - does not support swapchain\n");
        }

        // ensure at least one image format is supported
        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_devices[i], surface, &format_count, NULL);
        if (format_count > 0) {
            printf("  + supports image formats\n");
        } else {
            printf("  - does not support image formats\n");
            swapchain_viable = false;
        }

        // ensure at least one present mode is supported
        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_devices[i], surface, &present_mode_count, NULL);
        if (present_mode_count > 0) {
            printf("  + supports present modes\n");
        } else {
            printf("  - does not support present modes\n");
            swapchain_viable = false;
        }

        // if this device won't work, skip it
        if (graphics_queue == -1) continue;
        if (presentation_queue == -1) continue;
        if (swapchain_support == false) continue;
        if (swapchain_viable == false) continue;

        // choose a device if this one is viable and the choice hasn't been made yet
        if (physical_device == VK_NULL_HANDLE) {
            physical_device = physical_devices[i];
            device_graphics_queue = graphics_queue;
            device_presentation_queue = presentation_queue;
        }
    }

    free(physical_devices);

    // summarize physical device selection
    if (physical_device == VK_NULL_HANDLE
        || device_graphics_queue == -1
        || device_presentation_queue == -1) {
        fprintf(stderr, "no suitable Vulkan devices are available\n");
        return EXIT_FAILURE;
    } else {
        VkPhysicalDeviceProperties properties = { 0 };
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        printf("Moving forward with device: %s\n", properties.deviceName);
        printf("Device graphics queue: %ld\n", device_graphics_queue);
        printf("Device presentation queue: %ld\n", device_presentation_queue);
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
