include(CMakeLists.files)
include_directories(.)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Sql REQUIRED)

QT5_WRAP_CPP(QT_HEADER_FILES_WRAPPED ${QT_HEADER_FILES})
QT5_WRAP_UI(QT_FORMS_WRAPPED ${QT_FORMS})
QT5_ADD_RESOURCES(QT_RESOURCES_WRAPPED ${QT_RESOURCES})

add_executable(qtwitch ${SOURCE_FILES} ${QT_HEADER_FILES_WRAPPED} ${QT_FORMS_WRAPPED} ${QT_RESOURCES_WRAPPED})
target_link_libraries(qtwitch Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Sql)
