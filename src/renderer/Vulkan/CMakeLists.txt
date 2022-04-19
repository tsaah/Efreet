find_package(Vulkan REQUIRED FATAL_ERROR)

set(ADDITIONAL_INCLUDE_DIRECTORIES
    ${SRC_DIR}/common
    ${Vulkan_INCLUDE_DIRS}
)

set(ADDITIONAL_LINK_LIBRARIES
    ${Vulkan_LIBRARIES}
)

build_shared_library(er_vulkan RENDERER_EXPORT_DEFINE)
