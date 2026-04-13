cmake_minimum_required(VERSION 3.15)
project(AcademicSystem VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

file(GLOB_RECURSE CORE_SOURCES 
    "src/*.cpp" 
    "io/*.cpp"
)

list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(${PROJECT_NAME} 
    src/main.cpp 
    UI/UI.cpp
    UI/grade_entry.cpp
    UI/subject_modals.cpp
    UI/dashboard_parts.cpp
    UI/dashboard_top_panel.cpp
    UI/dashboard_subjects_list.cpp
    UI/dashboard_subject_details.cpp
    UI/dashboard_semester_overview.cpp
    ${CORE_SOURCES}
)

find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4 
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

file(GLOB IMGUI_SOURCES
    "${imgui_SOURCE_DIR}/*.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
add_library(ImGuiLib STATIC ${IMGUI_SOURCES})
target_include_directories(ImGuiLib PUBLIC 
    "${imgui_SOURCE_DIR}" 
    "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(ImGuiLib PUBLIC glfw OpenGL::GL)

target_link_libraries(${PROJECT_NAME} PRIVATE ImGuiLib)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/src/core 
    ${CMAKE_SOURCE_DIR}/src/io
    ${CMAKE_SOURCE_DIR}/io
    ${CMAKE_SOURCE_DIR}/UI      
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "tests/*.cpp")

add_executable(SystemTests ${TEST_SOURCES} ${CORE_SOURCES})

target_include_directories(SystemTests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/src/core 
    ${CMAKE_SOURCE_DIR}/src/io
    ${CMAKE_SOURCE_DIR}/io
)

target_link_libraries(SystemTests GTest::gtest_main GTest::gtest)
include(GoogleTest)
gtest_discover_tests(SystemTests)