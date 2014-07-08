kde4_add_executable(libkubuntu-force-setlocale-rewrite libkubuntu-force-setlocale-rewrite.cpp)
target_link_libraries(libkubuntu-force-setlocale-rewrite
    ${KDE4_KDECORE_LIBS}
    Kubuntu)
install(TARGETS libkubuntu-force-setlocale-rewrite DESTINATION ${LIB_INSTALL_DIR}/kconf_update_bin/)

install(FILES libkubuntu-force-setlocale-rewrite-14-04.upd DESTINATION ${KCONF_UPDATE_INSTALL_DIR})
