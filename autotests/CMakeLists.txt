enable_testing()

################################### LANGUAGE ###################################

set(unit_language_SRCS languagetest.cpp)

kde4_add_unit_test(languagetest
    TESTNAME languagetest
    ${unit_language_SRCS}
)

target_link_libraries(languagetest
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
    Kubuntu
)

#################################### Locale ####################################

set(unit_locale_SRCS localetest.cpp)

kde4_add_unit_test(localetest
    TESTNAME localetest
    ${unit_locale_SRCS}
)

target_link_libraries(localetest
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
    Kubuntu
)
