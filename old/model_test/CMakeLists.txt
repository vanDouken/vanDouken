cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
find_package(Qt4 REQUIRED QtOpenGL QtGui QtCore)
find_package(Boost REQUIRED date_time filesystem thread system)
find_package(MPI)

add_definitions(${MPI_COMPILE_FLAGS})

include(${QT_USE_FILE})
set(MY_HEADERS
  particlewidget.h
  fakesimulator.h
  forcesteerer.h
  debugoutput.h
  storage.h)

QT4_WRAP_CPP(MY_MOC_SOURCES ${MY_HEADERS} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

include_directories(./)
include_directories(../)
include_directories($ENV{LIBGEODECOMP_INCLUDE_DIR})
include_directories(${MPI_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIR})
link_directories($ENV{LIBGEODECOMP_LIB_DIR})

add_executable(model_test main.cpp ${MY_MOC_SOURCES})
# add_executable(model_test main.cpp)
target_link_libraries(model_test ${QT_LIBRARIES})
target_link_libraries(model_test ${Boost_LIBRARIES})
target_link_libraries(model_test ${MPI_LIBRARIES})
target_link_libraries(model_test GL)
target_link_libraries(model_test GLU)
target_link_libraries(model_test geodecomp)
set_property(TARGET model_test PROPERTY LINK_FLAGS ${MPI_LINK_FLAGS})
