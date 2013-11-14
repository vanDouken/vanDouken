
add_subdirectory(components)

set(VANDOUKEN_COMPONENT_SOURCES
    forceprimitives.cpp
    initializer.cpp
    component.cpp
    particlesteerer.cpp
    particlesteererserver.cpp
    singletracer.cpp
    simulator.cpp
    simulationcontrollerserver.cpp
    gridcollectorserver.cpp
)
set(VANDOUKEN_COMPONENT_HEADERS
    forceprimitives.hpp
    initializer.hpp
    particlesteerer.hpp
    particlesteererserver.hpp
    singletracer.hpp
    simulator.hpp
    simulationcontrollerserver.hpp
    gridcollectorserver.hpp
)

#add_hpx_component(
#    vandouken_component
#    ESSENTIAL
#    SOURCES ${VANDOUKEN_COMPONENT_SOURCES}
#    HEADERS ${VANDOUKEN_COMPONENT_HEADERS}
#    DEPENDENCIES
#        ${LOCAL_LIBGEODECOMP_LINK_LIB}
#    FOLDER "vanDouken Components"
#)
#
#qt5_use_modules(vandouken_component_component Gui)


set(VANDOUKEN_LIBRARY_MOC_HEADERS
    camerapreview.hpp
    forcecontrol.hpp
    forceview.hpp
    mainwindow.hpp
    maincontrol.hpp
    imagewidget.hpp
    particlewidget.hpp
)

#QT4_WRAP_CPP(
#    VANDOUKEN_LIBRARY_MOC_SOURCES
#    ${VANDOUKEN_LIBRARY_MOC_HEADERS}
#    OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED
#)
if(NOT HPX_NATIVE_MIC)
  qt5_wrap_cpp(
      VANDOUKEN_LIBRARY_MOC_SOURCES
      ${VANDOUKEN_LIBRARY_MOC_HEADERS}
      OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED -p.
  )
endif()

set(VANDOUKEN_UI_SOURCES
    ui/startdialog.ui
    ui/main.ui)

#QT4_WRAP_UI(
#    VANDOUKEN_UI_HEADERS
#    ${VANDOUKEN_UI_SOURCES})

if(NOT HPX_NATIVE_MIC)
    qt5_wrap_ui(
        VANDOUKEN_UI_HEADERS
        ${VANDOUKEN_UI_SOURCES})

    qt5_add_resources(
        VANDOUKEN_RESOURCES
        ../resource.qrc)
endif()

set(VANDOUKEN_LIBRARY_SOURCES
    camerapreview.cpp
    forcecontrol.cpp
    forceview.cpp
    framegrabber.cpp
    gridprovider.cpp
    gridcollector.cpp
    imagesteerer.cpp
    forcesteerer.cpp
    resetsimulation.cpp
    imagewidget.cpp
    parameters.cpp
    particlewidget.cpp
    maincontrol.cpp
    mainwindow.cpp
    mainwindowserver.cpp
    startgui.cpp
    steeringprovider.cpp
    simulation.cpp
    simulationcontroller.cpp
    controlgui.cpp
    widgetbase.cpp
    ${VANDOUKEN_RESOURCES}
    ${VANDOUKEN_UI_SOURCES}
    ${VANDOUKEN_LIBRARY_MOC_SOURCES}
)
set(VANDOUKEN_LIBRARY_HEADERS
    camerapreview.hpp
    forcecontrol.hpp
    forceview.hpp
    framegrabber.hpp
    gridprovider.hpp
    gridcollector.hpp
    maincontrol.hpp
    mainwindowserver.hpp
    parameters.hpp
    steeringprovider.hpp
    simulation.hpp
    simulationcontroller.hpp
    startgui.hpp
    controlgui.hpp
    widgetbase.hpp
    ${VANDOUKEN_LIBRARY_MOC_HEADERS}
    ${VANDOUKEN_UI_HEADERS}
)

set(ADDITIONAL_LIBS "")

if("${HPX_PLATFORM}" STREQUAL "Android")
    set(ADDITIONAL_LIBS
        log
        z
        EGL
        GLESv1_CM
        GLESv2
    )
else()
    if(NOT HPX_NATIVE_MIC)
        set(ADDITIONAL_LIBS
            opencv_core
            opencv_highgui)
    else()
        set(ADDITIONAL_LIBS
            z
            png)
    endif()
endif()

add_hpx_component(
    vandouken
    ESSENTIAL
    SOURCES ${VANDOUKEN_LIBRARY_SOURCES}
            ${VANDOUKEN_COMPONENT_SOURCES}
    HEADERS ${VANDOUKEN_LIBRARY_HEADERS}
        ${VANDOUKEN_COMPONENT_HEADERS}
    DEPENDENCIES
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        #GLU
        ${ADDITIONAL_LIBS}
    FOLDER "vanDouken Library"
)

if(NOT HPX_NATIVE_MIC)
    qt5_use_modules(vandouken_component Core Gui OpenGL Svg Widgets)
endif()

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
    COMPONENT_DEPENDENCIES
        vandouken
    DEPENDENCIES
        ${ADDITIONAL_LIBS}
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        ${QT_LIBRARIES}
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
    COMPONENT_DEPENDENCIES
        vandouken
    DEPENDENCIES
        ${ADDITIONAL_LIBS}
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        ${QT_LIBRARIES}
    FOLDER "vanDouken GUI Application"
)

add_hpx_executable(
    vandouken_controlgui
    ESSENTIAL
    SOURCES controlgui_main.cpp
    COMPONENT_DEPENDENCIES
        vandouken
    DEPENDENCIES
        ${ADDITIONAL_LIBS}
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
        ${QT_LIBRARIES}
    FOLDER "vanDouken GUI Application"
)
