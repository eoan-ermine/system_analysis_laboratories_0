qt_add_executable(plotter statistics_model.cpp main.cpp)

target_link_libraries(plotter PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick Qt6::Qml Qt6::QuickControls2)
target_link_libraries(plotter PRIVATE common)

qt_add_qml_module(plotter URI plotter_application VERSION 1.0 QML_FILES "main.qml" RESOURCE_PREFIX "qt/qml")

install(TARGETS plotter BUNDLE DESTINATION RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
