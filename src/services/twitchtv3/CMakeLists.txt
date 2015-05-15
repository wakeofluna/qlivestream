include(CMakeLists.files)
include_directories(.)

find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)

add_library(qlivestream-twitchtv3 SHARED ${SOURCE_FILES})
target_link_libraries(qlivestream-twitchtv3 Qt5::Core Qt5::Network)