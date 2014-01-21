enable_testing()

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
