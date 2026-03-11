cmake_minimum_required(VERSION 3.15)
project(AcademicSystem VERSION 1.0)

# Встановлюємо стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === ДОЗВОЛЯЄМО ТЕСТИ (ОБОВ'ЯЗКОВО ДЛЯ CTEST) ===
enable_testing()

# 1. Збираємо всі вихідні файли ядра та введення/виведення (БЕЗ main.cpp)
# Це потрібно, щоб ми могли підключити логіку і до основної програми, і до тестів
file(GLOB_RECURSE CORE_SOURCES 
    "src/core/*.cpp" 
    "src/io/*.cpp"
)

# 2. ГОЛОВНА ПРОГРАМА (AcademicSystem.exe)
add_executable(${PROJECT_NAME} src/main.cpp ${CORE_SOURCES})

# Вказуємо шляхи до хедерів для головної програми
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/core 
    ${CMAKE_SOURCE_DIR}/src/io
)

# ==========================================
#             GOOGLE TEST БЛОК
# ==========================================
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Вимикаємо встановлення GTest у систему, щоб не смітити
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ==========================================
#             ЮНІТ-ТЕСТИ (SystemTests.exe)
# ==========================================

# 3. Збираємо всі файли тестів з папки tests/
file(GLOB TEST_SOURCES "tests/*.cpp")

# Створюємо виконуваний файл для тестів
add_executable(SystemTests ${TEST_SOURCES} ${CORE_SOURCES})

# Вказуємо шляхи до хедерів, щоб тести бачили твої класи
target_include_directories(SystemTests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/core 
    ${CMAKE_SOURCE_DIR}/src/io
)

# Лікуємо GTest до тестової програми
target_link_libraries(SystemTests GTest::gtest_main GTest::gtest)

# Реєструємо тести в CTest
include(GoogleTest)
gtest_discover_tests(SystemTests)