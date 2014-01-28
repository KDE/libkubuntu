
set(kubuntu_SRCS
    busyoverlay.cpp
    l10n_language.cpp
    l10n_languagecollection.cpp
    l10n_locale.cpp

# QTC compat
    export.h
    l10n_languagecollection_p.h
)

kde4_add_ui_files(kubuntu_SRCS busyoverlay.ui)
kde4_add_library(Kubuntu SHARED ${kubuntu_SRCS})

set_target_properties(Kubuntu
    PROPERTIES VERSION ${KUBUNTU_VERSION} SOVERSION ${KUBUNTU_VERSION_MAJOR}
)

target_link_libraries(Kubuntu
    ${QAPT_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS} # Needed for KProtocolManager/QApt proxy setup
)

install(TARGETS Kubuntu EXPORT LibKubuntuTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    busyoverlay.h
    export.h
    l10n_language.h
    l10n_languagecollection.h
    l10n_locale.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/Kubuntu COMPONENT Devel
)
