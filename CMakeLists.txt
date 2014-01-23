project(libkubuntu)
cmake_minimum_required(VERSION 2.8)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(QApt REQUIRED)
include_directories(${QAPT_INCLUDE_DIR})

add_definitions(-std=c++11)


set(kubuntu_SRCS
    src/language.cpp
    src/locale.cpp

# QTC compat
    src/export.h
)

kde4_add_library(Kubuntu SHARED ${kubuntu_SRCS})
target_link_libraries(Kubuntu
    ${QAPT_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS} # Needed for KProtocolManager/QApt proxy setup
)

install(TARGETS Kubuntu EXPORT LibKubuntuTargets ${INSTALL_TARGETS_DEFAULT_ARGS})


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

if(KDE4_BUILD_TESTS)
    add_subdirectory(autotests)
endif()
