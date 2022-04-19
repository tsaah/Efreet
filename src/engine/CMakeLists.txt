include(${SRC_DIR}/renderer/VulkanRenderer.cmake)

set(ADDITIONAL_INCLUDE_DIRECTORIES
    ${SRC_DIR}/common
)

build_core_application(efreet)
