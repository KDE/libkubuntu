set(kubuntu_SRCS
    busyoverlay.cpp
    l10n_language.cpp
    l10n_languagecollection.cpp
    l10n_locale.cpp

# QTC compat
    export.h
    l10n_languagecollection_p.h
)

qt5_wrap_ui(kubuntu_SRCS busyoverlay.ui)

add_library(Kubuntu SHARED ${kubuntu_SRCS})

target_include_directories(Kubuntu PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>")
target_include_directories(Kubuntu INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/Kubuntu>" )

set_target_properties(Kubuntu
    PROPERTIES 
        VERSION ${KUBUNTU_VERSION_STRING}
        SOVERSION ${KUBUNTU_SOVERSION}
        EXPORT_NAME Main)

target_link_libraries(Kubuntu
    KF5::I18n
    KF5::KIOCore # Needed for KProtocolManager/QApt proxy setup
    QApt::Main)

install(TARGETS Kubuntu EXPORT KubuntuTargets LIBRARY DESTINATION  ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    busyoverlay.h
    export.h
    l10n_language.h
    l10n_languagecollection.h
    l10n_locale.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/Kubuntu
    COMPONENT Devel)
