cmake_minimum_required(VERSION 3.10)

project(MyCoolProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Путь к заголовочным файлам
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- 1. СОБИРАЕМ БИБЛИОТЕКУ (БЕЗ MAIN) ---
# Находим все cpp файлы в src, КРОМЕ main.cpp
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
# Фильтруем: исключаем main.cpp из списка исходников библиотеки
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main[.]cpp$")

# Создаем статическую библиотеку (libMyCoolProject.a или .lib) из всех файлов, кроме main
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})

# Указываем, где искать заголовочные файлы для библиотеки
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${INCLUDE_DIR})
# PUBLIC означает, что те, кто будут линковаться с этой библиотекой,
# автоматически получат этот путь к заголовкам.

# --- 2. СОЗДАЕМ ОСНОВНОЙ ИСПОЛНЯЕМЫЙ ФАЙЛ (С MAIN) ---
# Явно добавляем main.cpp отдельно
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(${PROJECT_NAME} "src/main.cpp")

    # Линкуем основную программу с нашей библиотекой
    # Теперь main.cpp увидит все функции из библиотеки
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

    # Наследовать пути к заголовкам от библиотеки не нужно, так как мы прилинковали библиотеку
    # Но если хочешь явно:
    # target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
else()
    message(WARNING "main.cpp not found in src/ directory")
endif()

# --- 3. ТЕСТЫ (Catch2) ---
enable_testing()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

    if(TEST_SOURCES)
        # Создаем исполняемый файл для тестов
        add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

        # Добавляем путь к external с catch.hpp
        target_include_directories(${PROJECT_NAME}_tests PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/external
        )

        # Линкуем тесты с НАШЕЙ БИБЛИОТЕКОЙ (а не с основным исполняемым файлом!)
        # Тесты получат доступ ко всему коду из src/*.cpp, кроме main.cpp
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE
                ${PROJECT_NAME}_lib
        )

        # Важно! Путь к include директории тесты получат автоматически,
        # потому что ${PROJECT_NAME}_lib имеет PUBLIC include директорию

        # Регистрируем тест
        add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_tests)
    endif()
endif()
