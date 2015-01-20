project(libkubuntu)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.5.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

include(CMakePackageConfigHelpers)

set(REQUIRED_QT_VERSION 5.2.0) # Used in QAptConfig
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets)

find_package(KF5 REQUIRED
    COMPONENTS
        I18n
        KIO)

find_package(QApt REQUIRED)

ecm_setup_version(1.0.0 VARIABLE_PREFIX KUBUNTU
    SOVERSION 1
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion)

# --------------------------

add_subdirectory(autotests)
add_subdirectory(src)

# --------------------------

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/Kubuntu")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KubuntuConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/KubuntuConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KubuntuTargets
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    NAMESPACE Kubuntu::
    FILE KubuntuTargets.cmake)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
