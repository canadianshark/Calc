cmake_minimum_required(VERSION 3.10)

project(Calc VERSION 1.0.0 LANGUAGES CXX)

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

enable_testing()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    file(GLOB_RECURSE TEST_FILES "tests/*.cpp")

    foreach(test_file ${TEST_FILES})
        # Получаем имя файла без расширения (например, test_lexer)
        get_filename_component(test_name ${test_file} NAME_WE)

        # Создаем уникальное имя цели (например, Calc_test_lexer)
        set(test_target_name ${PROJECT_NAME}_${test_name})

        # Создаем исполняемый файл конкретно для этого теста
        add_executable(${test_target_name} ${test_file})

        # Настройки инклудов и линковки (как раньше)
        target_include_directories(${test_target_name} PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/external
        )

        # Линкуем с библиотекой проекта
        target_link_libraries(${test_target_name} PRIVATE
                ${PROJECT_NAME}_lib
        )

        # Регистрируем тест в системе запуска (ctest)
        add_test(NAME ${test_name} COMMAND ${test_target_name})

        message(STATUS "Created test: ${test_name}")
    endforeach()
endif()