project(AutoSubdirs)

# Получаем список всех элементов в текущей директории
file(GLOB entries LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

# Фильтруем только директории
set(subdirs)
foreach(entry IN LISTS entries)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${entry})
        list(APPEND subdirs ${entry})
    endif()
endforeach()

# Добавляем поддиректории, только если в них есть CMakeLists.txt
foreach(subdir IN LISTS subdirs)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
        message(STATUS "Adding subdirectory: ${subdir}")
        add_subdirectory(${subdir})
    else()
        message(WARNING "Skipping directory '${subdir}': no CMakeLists.txt found")
    endif()
endforeach()