FILE(GLOB SRC_FILES *.cpp)
add_library(utils STATIC ${SRC_FILES})
target_link_libraries(utils PRIVATE Urlmon)
find_package(unofficial-gumbo CONFIG REQUIRED)
find_package(unofficial-curlpp CONFIG REQUIRED)
target_link_libraries(utils PRIVATE unofficial::gumbo::gumbo unofficial::curlpp::curlpp)

