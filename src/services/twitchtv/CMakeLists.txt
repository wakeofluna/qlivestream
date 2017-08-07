include(CMakeLists.files)
include_directories(.)

find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)

QT5_WRAP_CPP(QT_HEADER_FILES_WRAPPED ${QT_HEADER_FILES})

add_library(twitchtv MODULE ${SOURCE_FILES} ${QT_HEADER_FILES_WRAPPED})
set_target_properties(twitchtv PROPERTIES PREFIX "libqlivestream-")
target_link_libraries(twitchtv core Qt5::Core Qt5::Network)