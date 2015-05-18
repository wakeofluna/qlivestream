include(CMakeLists.files)

find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Sql REQUIRED)

add_library(core SHARED ${SOURCE_FILES})
set_target_properties(core PROPERTIES PREFIX "libqlivestream-")
target_link_libraries(core Qt5::Core Qt5::Network Qt5::Sql)