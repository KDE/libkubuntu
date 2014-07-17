include(ECMAddTests)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

ecm_add_test(languagetest.cpp
    LINK_LIBRARIES
        Qt5::Test
        Kubuntu)

ecm_add_test(localetest.cpp
    LINK_LIBRARIES
        Qt5::Test
        Kubuntu)
