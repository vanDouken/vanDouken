
add_subdirectory(components)

set(VANDOUKEN_COMPONENT_SOURCES
    forceprimitives.cpp
    initializer.cpp
    component.cpp
    singletracer.cpp
    simulator.cpp
    simulationcontrollerserver.cpp
    gridcollectorserver.cpp
)
set(VANDOUKEN_COMPONENT_HEADERS
    forceprimitives.hpp
    initializer.hpp
    singletracer.hpp
    simulator.hpp
    simulationcontrollerserver.hpp
    gridcollectorserver.hpp
)

add_hpx_component(
    vandouken_component
    ESSENTIAL
    SOURCES ${VANDOUKEN_COMPONENT_SOURCES}
    HEADERS ${VANDOUKEN_COMPONENT_HEADERS}
    DEPENDENCIES
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
    FOLDER "vanDouken Components"
)

set(VANDOUKEN_LIBRARY_MOC_HEADERS
    particlewidget.hpp
    mainwindow.hpp
)

QT4_WRAP_CPP(
    VANDOUKEN_LIBRARY_MOC_SOURCES
    ${VANDOUKEN_LIBRARY_MOC_HEADERS}
    OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED
)

set(VANDOUKEN_UI_SOURCES
    ui/startdialog.ui
    ui/main.ui)

QT4_WRAP_UI(
    VANDOUKEN_UI_HEADERS
    ${VANDOUKEN_UI_SOURCES})


set(VANDOUKEN_LIBRARY_SOURCES
    gridprovider.cpp
    gridcollector.cpp
    parameters.cpp
    particlewidget.cpp
    mainwindow.cpp
    startgui.cpp
    simulation.cpp
    simulationcontroller.cpp
    ${VANDOUKEN_LIBRARY_MOC_SOURCES}
)
set(VANDOUKEN_LIBRARY_HEADERS
    gridprovider.hpp
    gridcollector.hpp
    parameters.hpp
    simulation.hpp
    simulationcontroller.hpp
    startgui.hpp
    ${VANDOUKEN_LIBRARY_MOC_HEADERS}
    ${VANDOUKEN_UI_HEADERS}
)

add_hpx_library(
    vandouken
    ESSENTIAL
    SOURCES ${VANDOUKEN_LIBRARY_SOURCES}
    HEADERS ${VANDOUKEN_LIBRARY_HEADERS}
    DEPENDENCIES
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        GLU
    COMPONENT_DEPENDENCIES
        vandouken_component
    FOLDER "vanDouken Library"
)

set(VANDOUKEN_SERVER_SOURCES
    server.cpp
)
#set(
#    VANDOUKEN_LIBRARY_HEADERS
#    parameters.hpp
#)

add_hpx_executable(
    vandouken_server
    ESSENTIAL
    SOURCES ${VANDOUKEN_SERVER_SOURCES}
    #HEADERS ${VANDOUKEN_SERVER_HEADERS}
    DEPENDENCIES
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        ${QT_LIBRARIES}
        vandouken_lib
    FOLDER "vanDouken Server Application"
)

set(VANDOUKEN_GUI_SOURCES
    gui.cpp
)
set(
    VANDOUKEN_GUI_HEADERS
)

add_hpx_executable(
    vandouken_gui
    ESSENTIAL
    SOURCES ${VANDOUKEN_GUI_SOURCES}
    HEADERS ${VANDOUKEN_GUI_HEADERS}
    DEPENDENCIES
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        ${QT_LIBRARIES}
        vandouken_lib
    FOLDER "vanDouken GUI Application"
)
