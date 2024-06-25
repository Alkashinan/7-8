cmake_minimum_required(VERSION 3.10)

# Название проекта
project(MyProject)

# Версия стандарда C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Пути к заголовочным файлам
include_directories(src/include)

# Пути к библиотекам
link_directories(${CMAKE_SOURCE_DIR}/src/lib)

# Добавляем исполняемый файл
add_executable(main main.cpp)

# Линковка с библиотеками SFML
target_link_libraries(main sfml-graphics sfml-window sfml-system sfml-audio)
