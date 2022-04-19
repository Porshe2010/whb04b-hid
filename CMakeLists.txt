cmake_minimum_required(VERSION 3.16)
project(whb04bhid CXX)


find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule (hidapi,libWRP-key) update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
else()
    message(FATAL_ERROR "The submodules (hidapi,libWRP-key) were not downloaded!\nGIT_SUBMODULE was turned off or failed.\nPlease update submodules and try again.")
endif()

# Clean build directories
if(EXISTS "${CMAKE_SOURCE_DIR}/extern/hidapi/build")
    message(STATUS "** Removing ${CMAKE_SOURCE_DIR}/extern/hidapi/build")
    file(REMOVE ${CMAKE_SOURCE_DIR}/extern/libWRP-key/build)
endif()

# Recreate build directories
message(STATUS "** Creating build directories")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/libWRP-key/build)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/libWRP-key/build)


# BUilding hidapi
message(STATUS "BUILDING HIDAPI")
execute_process(COMMAND ${CMAKE_COMMAND} -DBUILD_SHARED_LIBS=OFF ..
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/extern/hidapi/build"
    RESULT_VARIABLE RESULT_VAR)
if(NOT RESULT_VAR EQUAL "0")
    message(FATAL_ERROR "Configure failed")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/extern/hidapi/build"
    RESULT_VARIABLE RESULT_VAR)
if(NOT RESULT_VAR EQUAL "0")
    message(FATAL_ERROR "Build failed")
endif()


# Add include search paths
include_directories(${PROJECT_SOURCE_DIR}/extern/hidapi/hidapi)
include_directories(${PROJECT_SOURCE_DIR}/extern/hidapi/libusb)
include_directories(${PROJECT_SOURCE_DIR}/extern/libWRP-key/include)

# Add lib search paths
link_directories(${PROJECT_SOURCE_DIR}/extern/hidapi/build/src/linux)
link_directories(${PROJECT_SOURCE_DIR}/extern/hidapi/build/src/libusb)

# Build the executable
add_executable(whb04bhid src/whb04bhid.cpp)
target_link_libraries(whb04bhid hidapi-hidraw)

#[[
if(NOT WIN32 AND NOT APPLE AND CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(TARGET hidapi::hidraw)
        add_executable(hidtest_hidraw test.c)
        target_link_libraries(hidtest_hidraw hidapi::hidraw)
        list(APPEND HIDAPI_HIDTEST_TARGETS hidtest_hidraw)
    endif()
    if(TARGET hidapi::libusb)
        add_executable(hidtest_libusb test.c)
        target_link_libraries(hidtest_libusb hidapi::libusb)
        list(APPEND HIDAPI_HIDTEST_TARGETS hidtest_libusb)
    endif()
else()
    add_executable(hidtest test.c)
    target_link_libraries(hidtest hidapi::hidapi)
    list(APPEND HIDAPI_HIDTEST_TARGETS hidtest)
endif()

install(TARGETS ${HIDAPI_HIDTEST_TARGETS}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

include(ExternalProject)
ExternalProject_Add(hidapi
        GIT_REPOSITORY https://github.com/libusb/hidapi
        GIT_TAG "75aafc3ac972947a5ded5e5e85a9d62b9ed06b59"
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/hidapi-src"
        BINARY_DIR        "${CMAKE_BINARY_DIR}/hidapi-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
)

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/hidapi-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/hidapi-download"
)



include(FetchContent)

FetchContent_Declare(
        hidapi
        GIT_REPOSITORY "https://github.com/libusb/hidapi"
        GIT_TAG "75aafc3ac972947a5ded5e5e85a9d62b9ed06b59"
)

FetchContent_MakeAvailable(hidapi)




find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule (hidapi,libWRP-key) update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# verify that git submodule update worked
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/hidapi/CMakeLists.txt")
    message(FATAL_ERROR "The submodules (hidapi,libWRP-key) were not downloaded!\nGIT_SUBMODULE was turned off or failed.\nPlease update submodules and try again.")
else()
        execute_process(COMMAND "mkdir build"
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extern/hidapi
                        RESULT_VARIABLE CMD_RESULT)
        execute_process(COMMAND "cmake .."
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extern/hidapi/build
                        RESULT_VARIABLE CMD_RESULT)
endif()



include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the submodules 
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/hidapi)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/libWRP-key)

include_directories(${PROJECT_SOURCE_DIR}/extern/hidapi/hidapi)
include_directories(${PROJECT_SOURCE_DIR}/extern/libWRP-key/include)
]]


