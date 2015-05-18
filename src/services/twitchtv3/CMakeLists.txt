include(CMakeLists.files)
include_directories(.)

find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)

add_library(twitchtv3 MODULE ${SOURCE_FILES})
set_target_properties(twitchtv3 PROPERTIES PREFIX "libqlivestream-")
target_link_libraries(twitchtv3 core Qt5::Core Qt5::Network)