
add_subdirectory(components)

set(
    VANDOUKEN_COMPONENT_SOURCES
    component.cpp
    simulator.cpp
    simulationcontrollerserver.cpp
)
set(
    VANDOUKEN_COMPONENT_HEADERS
    simulator.hpp
    simulationcontrollerserver.hpp
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

set(
    VANDOUKEN_LIBRARY_SOURCES
    initializer.cpp
    parameters.cpp
    simulation.cpp
    simulationcontroller.cpp
)
set(
    VANDOUKEN_LIBRARY_HEADERS
    initializer.hpp
    parameters.hpp
    simulation.hpp
    simulationcontroller.hpp
)

add_hpx_library(
    vandouken
    ESSENTIAL
    SOURCES ${VANDOUKEN_LIBRARY_SOURCES}
    HEADERS ${VANDOUKEN_LIBRARY_HEADERS}
    DEPENDENCIES
        ${LOCAL_LIBGEODECOMP_LINK_LIB}
    COMPONENT_DEPENDENCIES
        vandouken_component
    FOLDER "vanDouken Library"
)

set(
    VANDOUKEN_SERVER_SOURCES
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
