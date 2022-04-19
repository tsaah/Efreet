macro(add_file_dependancy file)
    set(${file}_deps ${${file}_deps} ${AGRN})
    set_source_files_properties(${files} PROPERTIES OBJECT_DEPENDS "${${file}_deps}")
endmacro()

macro(build_shared_library targetName exportDefinition)
    file(GLOB_RECURSE SOURCES *.h *.cpp ${ADDITIONAL_FILE_PATTERNS})
    set(ADDITIONAL_DEFINES ${ADDITIONAL_DEFINES} "${exportDefinition}")
    build_library(${targetName} SHARED "${CMAKE_BINARY_DIR}")
endmacro()

macro(build_library targetName libType destinationPath)
    set(TARGET ${targetName})
    make_included_lists_unique()
    make_additional_lists_unique()
    set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${INCLUDED_INCLUDE_DIRECTORIES}
        ${ADDITIONAL_INCLUDE_DIRECTORIES}
        #${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        ${INCLUDED_LINK_LIBRARIES}
        ${ADDITIONAL_LINK_LIBRARIES}
    )
    if("${libType}" STREQUAL "SHARED")
        generate_target_version_file("${TARGET}.dll")
    endif()
    include_directories(${INCLUDE_DIRECTORIES})

    add_library(${TARGET} ${libType}
        ${INCLUDED_HEADERS}
        ${SOURCES}
        ${ADDITIONAL_FILES}
    )
    set_target_properties(${TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${destinationPath}"
        LIBRARY_OUTPUT_DIRECTORY "${destinationPath}"
        RUNTIME_OUTPUT_DIRECTORY "${destinationPath}"
    )

    target_compile_definitions(${TARGET} PRIVATE ${INCLUDED_DEFINES} ${ADDITIONAL_DEFINES})
    target_link_libraries(${TARGET} ${LINK_LIBRARIES})
    if(INCLUDED_DEPENDENCIES)
	    add_dependencies(${TARGET} ${INCLUDED_DEPENDENCIES})
    endif()
    if(ADDITIONAL_DEPENDENCIES)
	    add_dependencies(${TARGET} ${ADDITIONAL_DEPENDENCIES})
    endif()
    install(TARGETS ${TARGET} DESTINATION ${ARTIFACTS_DIR})
endmacro()

macro(build_gui_application targetName)
    set(TARGET ${targetName})
    make_included_lists_unique()
    make_additional_lists_unique()
    file(GLOB_RECURSE SOURCES *.h *.cpp ${ADDITIONAL_FILE_PATTERNS})
    set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${INCLUDED_INCLUDE_DIRECTORIES}
        ${ADDITIONAL_INCLUDE_DIRECTORIES}
        #${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        ${INCLUDED_LINK_LIBRARIES}
        ${ADDITIONAL_LINK_LIBRARIES}
    )
    generate_target_version_file("${TARGET}.exe")
    include_directories(${INCLUDE_DIRECTORIES})
    add_executable(${TARGET}
        ${INCLUDED_HEADERS}
        ${SOURCES}
        ${ADDITIONAL_FILES}
    )
    set_target_properties(${TARGET} PROPERTIES
        WIN32_EXECUTABLE TRUE
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    target_compile_definitions(${TARGET} PRIVATE ${INCLUDED_DEFINES} ${ADDITIONAL_DEFINES})
    target_link_libraries(${TARGET} ${LINK_LIBRARIES})
    if(INCLUDED_DEPENDENCIES)
        add_dependencies(${TARGET} ${INCLUDED_DEPENDENCIES})
    endif()
    if(ADDITIONAL_DEPENDENCIES)
        add_dependencies(${TARGET} ${ADDITIONAL_DEPENDENCIES})
    endif()
    install(TARGETS ${TARGET} DESTINATION ${ARTIFACTS_DIR})
endmacro()

macro(build_unit_test targetName)
    set(TARGET ${targetName})
    make_included_lists_unique()
    make_additional_lists_unique()
    file(GLOB_RECURSE SOURCES *.h *.cpp ${ADDITIONAL_FILE_PATTERNS})
    set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${INCLUDED_INCLUDE_DIRECTORIES}
        ${ADDITIONAL_INCLUDE_DIRECTORIES}
        #${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        ${INCLUDED_LINK_LIBRARIES}
        ${ADDITIONAL_LINK_LIBRARIES}
    )
    include_directories(${INCLUDE_DIRECTORIES})
    add_executable(${TARGET}
        ${INCLUDED_HEADERS}
        ${SOURCES}
        ${ADDITIONAL_FILES}
    )
    set_target_properties(${TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    target_compile_definitions(${TARGET} PRIVATE ${INCLUDED_DEFINES} ${ADDITIONAL_DEFINES})
    target_link_libraries(${TARGET} ${LINK_LIBRARIES})
    install(TARGETS ${TARGET} DESTINATION ${ARTIFACTS_DIR})
endmacro()

macro(build_core_application targetName)
    set(TARGET ${targetName})
    make_included_lists_unique()
    make_additional_lists_unique()
    file(GLOB_RECURSE SOURCES *.h *.cpp ${ADDITIONAL_FILE_PATTERNS})
    set(INCLUDE_DIRECTORIES
        ${INCLUDE_DIRECTORIES}
        ${INCLUDED_INCLUDE_DIRECTORIES}
        ${ADDITIONAL_INCLUDE_DIRECTORIES}
        #${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set(LINK_LIBRARIES
        ${LINK_LIBRARIES}
        ${INCLUDED_LINK_LIBRARIES}
        ${ADDITIONAL_LINK_LIBRARIES}
    )
    generate_target_version_file("${TARGET}.exe")
    include_directories(${INCLUDE_DIRECTORIES})
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        if(WIN32)
            add_executable(${TARGET} WIN32
                ${INCLUDED_HEADERS}
                ${SOURCES}
                ${ADDITIONAL_FILES}
            )
        else()
            add_executable(${TARGET}
                ${INCLUDED_HEADERS}
                ${SOURCES}
                ${ADDITIONAL_FILES}
            )
        endif()
    else()
        add_executable(${TARGET}
            ${INCLUDED_HEADERS}
            ${SOURCES}
            ${ADDITIONAL_FILES}
        )
    endif()
    set_target_properties(${TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    target_compile_definitions(${TARGET} PRIVATE ${INCLUDED_DEFINES} ${ADDITIONAL_DEFINES})
    target_link_libraries(${TARGET} ${LINK_LIBRARIES})
    if(INCLUDED_DEPENDENCIES)
        add_dependencies(${TARGET} ${INCLUDED_DEPENDENCIES})
    endif()
    if(ADDITIONAL_DEPENDENCIES)
        add_dependencies(${TARGET} ${ADDITIONAL_DEPENDENCIES})
    endif()
    install(TARGETS ${TARGET} DESTINATION ${ARTIFACTS_DIR})
endmacro()

macro(make_included_lists_unique)
    if(INCLUDED_DEFINES)
        list(REMOVE_DUPLICATES INCLUDED_DEFINES)
    endif()
    if(INCLUDED_DEPENDENCIES)
        list(REMOVE_DUPLICATES INCLUDED_DEPENDENCIES)
    endif()
    if(INCLUDED_HEADERS)
        list(REMOVE_DUPLICATES INCLUDED_HEADERS)
    endif()
    if(INCLUDED_INCLUDE_DIRECTORIES)
        list(REMOVE_DUPLICATES INCLUDED_INCLUDE_DIRECTORIES)
    endif()
    if(INCLUDED_LINK_LIBRARIES)
        list(REMOVE_DUPLICATES INCLUDED_LINK_LIBRARIES)
    endif()
endmacro()

macro(make_additional_lists_unique)
    if(ADDITIONAL_FILES)
        list(REMOVE_DUPLICATES INCLUDED_DEFINES)
    endif()
    if(ADDITIONAL_QM_FILES)
        list(REMOVE_DUPLICATES ADDITIONAL_QM_FILES)
    endif()
    if(ADDITIONAL_INCLUDE_DIRECTORIES)
        list(REMOVE_DUPLICATES ADDITIONAL_INCLUDE_DIRECTORIES)
    endif()
    if(ADDITIONAL_DEPENDENCIES)
        list(REMOVE_DUPLICATES ADDITIONAL_DEPENDENCIES)
    endif()
    if(ADDITIONAL_LINK_LIBRARIES)
        list(REMOVE_DUPLICATES ADDITIONAL_LINK_LIBRARIES)
    endif()
endmacro()

macro(generate_target_version_file targetFilename)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(TARGET_FILENAME ${targetFilename})
        configure_file(
            ${PROJECT_DIR}/version.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        )
        set(ADDITIONAL_FILES ${ADDITIONAL_FILES} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
endmacro()