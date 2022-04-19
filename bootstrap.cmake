set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "${default_build_type}")
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}") # CMake 3.9+
endif()



if(UNIX)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        message(STATUS "Debug symbols         : Included")
        set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -g")
    else()
        message(STATUS "Debug symbols         : Disabled")
    endif()

#   if(GCC_SANITIZE)
#     set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
#     set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=address")
#     #set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=thread")
#     set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=return")
#     set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=shift")
#     if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=bounds")
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=vptr")
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fno-sanitize-recover")
#     endif()
#     #set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fsanitize=unsigned-integer-overflow")
#   endif()

#   set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} --no-warnings -fexceptions -fnon-call-exceptions -march=native -pipe ")
#   set(CMAKE_CXX_STANDARD 14)
#   if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     if(PROFILE_GENERATE)
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fprofile-generate")
#     elseif(PROFILE_USE)
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fprofile-use -fprofile-correction -Wno-error=coverage-mismatch")
#     endif()
#   elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#     if(PROFILE_GENERATE)
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fprofile-instr-generate")
#     elseif(PROFILE_USE)
#       set(BUILD_ADDITIONAL_FLAGS "${BUILD_ADDITIONAL_FLAGS} -fprofile-instr-use")
#     endif()
#   endif()

#   set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${BUILD_ADDITIONAL_FLAGS}")
#   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BUILD_ADDITIONAL_FLAGS}")
#   set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${BUILD_ADDITIONAL_FLAGS}")
#   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${BUILD_ADDITIONAL_FLAGS}")
elseif(WIN32)
    # Disable warnings in Visual Studio 8 and above and add /MP
    if(MSVC AND NOT CMAKE_GENERATOR MATCHES "Visual Studio 7")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /wd4996 /wd4355 /wd4244 /wd4985 /wd4267 /MP")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4996 /wd4355 /wd4244 /wd4267 /MP")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /wd4996 /wd4355 /wd4244 /wd4985 /wd4267 /MP")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4996 /wd4355 /wd4244 /wd4985 /wd4267 /MP")
    endif()
endif()

set(DEFINITIONS_RELEASE ${DEFINITIONS_RELEASE} NDEBUG)
set(DEFINITIONS_DEBUG ${DEFINITIONS_DEBUG} _DEBUG EFREET_DEBUG)

# Some small tweaks for Visual Studio 7 and above.
if(MSVC)
  # Mark 32 bit executables large address aware so they can use > 2GB address space
  #if(PLATFORM MATCHES X86)
  #  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  #endif()

#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")


    if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
        string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Gr /GR- /fp:fast /MP /sdl- /GS- /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4189") # warning C4189: local variable is initialized but not referenced
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100") # warning C4100: unreferenced formal parameter
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201") # warning C4201: nonstandard extension used: nameless struct/union
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4530") # warning C4530: C++ exception handler used, but unwind semantics are not enabled.

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O3 /Gy /Oi /Opar")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od")
    endif()
endif()

set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")
set(SRC_DIR "${PROJECT_DIR}/src")



find_package(Vulkan REQUIRED FATAL_ERROR)
