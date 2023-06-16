cmake_minimum_required(VERSION 3.25)
project(WebScan)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_PREFIX_PATH "C:/Qt/6.4.3/msvc2019_64/bin")
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.4.3/msvc2019_64/bin")
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        WebEngineWidgets
        Network
        )
find_package(unofficial-curlpp CONFIG REQUIRED)

add_subdirectory(src)
add_executable(WebScan main.cpp mainwindow.cpp mainwindow.ui)
add_executable(test test.cpp)
target_link_libraries(WebScan PRIVATE utils Qt::Gui Qt::Core Qt::Widgets Qt::WebEngineWidgets)
target_link_libraries(test PRIVATE unofficial::curlpp::curlpp utils Qt::Gui Qt::Core Qt::Widgets Qt::WebEngineWidgets Qt::Network)