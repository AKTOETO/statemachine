cmake_minimum_required(VERSION 3.10)
project(libstate VERSION 1.0 LANGUAGES CXX)

# Установим стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найдем все исходники .cpp в папке src (относительно текущего CMakeLists.txt)
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Найдем все заголовочные файлы .h и .hpp в папке src для копирования при install
file(GLOB_RECURSE HEADER_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

# Создадим статическую библиотеку libstate из исходников
add_library(libstate STATIC ${SRC_FILES})

# Путь к заголовочным файлам для использования при компиляции
target_include_directories(libstate PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# # Установка библиотеки
# install(TARGETS libstate
#     EXPORT libstateTargets
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION include
# )

# # Установка заголовочных файлов в libstate/include
# install(FILES ${HEADER_FILES} DESTINATION libstate/include)

# # Создадим файл для экспорта библиотеки (опционально для find_package)
# install(EXPORT libstateTargets
#     FILE libstateTargets.cmake
#     NAMESPACE libstate::
#     DESTINATION libstate/cmake
# )
