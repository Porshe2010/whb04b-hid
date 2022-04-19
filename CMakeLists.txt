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
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/repo/CMakeLists.txt")
    message(FATAL_ERROR "The submodules (hidapi,libWRP-key) were not downloaded!\nGIT_SUBMODULE was turned off or failed.\nPlease update submodules and try again.")
endif()


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

include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the submodules 
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/hidapi)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/libWRP-key)

include_directories(${PROJECT_SOURCE_DIR}/extern/hidapi/hidapi)
include_directories(${PROJECT_SOURCE_DIR}/extern/libWRP-key/include)

