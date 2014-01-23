project(libkubuntu)
cmake_minimum_required(VERSION 2.8)

include(CMakePackageConfigHelpers)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

find_package(QApt REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${KDE4_INCLUDES})
include_directories(${QAPT_INCLUDE_DIR})
add_definitions(-std=c++11)

set(KUBUNTU_VERSION_MAJOR 0)
set(KUBUNTU_VERSION_MINOR 0)
set(KUBUNTU_VERSION_PATCH 0)
set(KUBUNTU_VERSION ${KUBUNTU_VERSION_MAJOR}.${KUBUNTU_VERSION_MINOR}.${KUBUNTU_VERSION_PATCH})

# --------------------------

add_subdirectory(src)

# --------------------------

set(demo_SRCS
    main.cpp
)

kde4_add_executable(demo ${demo_SRCS})
target_link_libraries(demo
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBRARY}
    Kubuntu
)

# --------------------------

if(KDE4_BUILD_TESTS)
    add_subdirectory(autotests)
endif()

# --------------------------

set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/Kubuntu)

configure_package_config_file(KubuntuConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfig.cmake
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfigVersion.cmake
    VERSION ${KUBUNTU_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT LibKubuntuTargets
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    NAMESPACE Kubuntu::
    FILE KubuntuTargets.cmake
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfig.cmake
#    ${CMAKE_CURRENT_BINARY_DIR}/KubuntuTargets.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfigVersion.cmake
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)
