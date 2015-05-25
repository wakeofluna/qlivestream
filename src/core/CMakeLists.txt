include(CMakeLists.files)

find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Sql REQUIRED)

QT5_WRAP_CPP(QT_HEADER_FILES_WRAPPED ${QT_HEADER_FILES})

add_library(core SHARED ${SOURCE_FILES} ${QT_HEADER_FILES_WRAPPED})
set_target_properties(core PROPERTIES PREFIX "libqlivestream-")
target_link_libraries(core Qt5::Core Qt5::Network Qt5::Sql)