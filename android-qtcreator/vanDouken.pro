#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T11:31:15
#
#-------------------------------------------------

HPX_SRC_DIR = /home/heller/remote/faui36a/programming/hpx
HPX_BUILD_DIR = /home/heller/remote/faui36a-scratch/build/hpx/android
BOOST_DIR = /home/heller/remote/faui36a-scratch/boost/1_54_0
VANDOUKEN_SRC_DIR = /home/heller/remote/faui36a/programming/vanDouken
VANDOUKEN_BUILD_DIR = /home/heller/remote/faui36a-scratch/build/vandouken/android
LIBGEODECOMP_SRC_DIR = /home/heller/remote/faui36a/programming/libgeodecomp/src
LIBGEODECOMP_BUILD_DIR = /home/heller/remote/faui36a-scratch/build/libgeodecomp/android


QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vanDouken
TEMPLATE = app


SOURCES += main.cpp

RESOURCES +=                                                                    \
        $$VANDOUKEN_SRC_DIR/resource.qrc

CONFIG += mobility
MOBILITY =

LIBS += -L$$BOOST_DIR/stage/lib/                                                \
        -lboost_date_time-gcc-mt-1_54-android                                   \
        -lboost_program_options-gcc-mt-1_54-android                             \
        -lboost_regex-gcc-mt-1_54-android                                       \
        -lboost_serialization-gcc-mt-1_54-android                               \
        -lboost_system-gcc-mt-1_54-android                                      \
        -lboost_thread-gcc-mt-1_54-android                                      \
        -lboost_atomic-gcc-mt-1_54-android                                      \
        -lboost_chrono-gcc-mt-1_54-android                                      \
        -lboost_context-gcc-mt-1_54-android                                     \
        -L$$HPX_BUILD_DIR/lib/hpx                                               \
        -lhpx                                                                   \
        -lhpx_init                                                              \
        -lhpx_serialization                                                     \
        -L$$LIBGEODECOMP_BUILD_DIR                                              \
        -lgeodecomp                                                             \
        -L$$VANDOUKEN_BUILD_DIR/lib/hpx/                                        \
        -lvandouken                                                             \

INCLUDEPATH +=                                                                  \
        $$HPX_SRC_DIR                                                           \
        $$HPX_SRC_DIR/external/cache                                            \
        $$HPX_SRC_DIR/external/endian                                           \
        $$HPX_BUILD_DIR                                                         \
        $$BOOST_DIR                                                             \
        $$VANDOUKEN_SRC_DIR                                                     \
        $$LIBGEODECOMP_SRC_DIR                                                  \

DEPENDPATH +=                                                                   \
        $$HPX_SRC_DIR                                                           \
        $$HPX_SRC_DIR/external/cache                                            \
        $$HPX_SRC_DIR/external/endian                                           \
        $$HPX_BUILD_DIR                                                         \
        $$BOOST_DIR                                                             \
        $$VANDOUKEN_SRC_DIR                                                     \
        $$LIBGEODECOMP_SRC_DIR                                                  \

ANDROID_EXTRA_LIBS =                                                            \
        $$HPX_BUILD_DIR/lib/hpx/libhpx_serialization.so                         \
        $$HPX_BUILD_DIR/lib/hpx/libhpx.so                                       \
        $$HPX_BUILD_DIR/lib/hpx/libiostreams.so                                 \
        $$BOOST_DIR/stage/lib/libboost_atomic-gcc-mt-1_54-android.so            \
        $$BOOST_DIR/stage/lib/libboost_chrono-gcc-mt-1_54-android.so            \
        $$BOOST_DIR/stage/lib/libboost_context-gcc-mt-1_54-android.so           \
        $$BOOST_DIR/stage/lib/libboost_date_time-gcc-mt-1_54-android.so         \
        $$BOOST_DIR/stage/lib/libboost_filesystem-gcc-mt-1_54-android.so        \
        $$BOOST_DIR/stage/lib/libboost_graph-gcc-mt-1_54-android.so             \
        $$BOOST_DIR/stage/lib/libboost_math_c99-gcc-mt-1_54-android.so          \
        $$BOOST_DIR/stage/lib/libboost_math_c99f-gcc-mt-1_54-android.so         \
        $$BOOST_DIR/stage/lib/libboost_math_tr1-gcc-mt-1_54-android.so          \
        $$BOOST_DIR/stage/lib/libboost_math_tr1f-gcc-mt-1_54-android.so         \
        $$BOOST_DIR/stage/lib/libboost_prg_exec_monitor-gcc-mt-1_54-android.so  \
        $$BOOST_DIR/stage/lib/libboost_program_options-gcc-mt-1_54-android.so   \
        $$BOOST_DIR/stage/lib/libboost_regex-gcc-mt-1_54-android.so             \
        $$BOOST_DIR/stage/lib/libboost_serialization-gcc-mt-1_54-android.so     \
        $$BOOST_DIR/stage/lib/libboost_signals-gcc-mt-1_54-android.so           \
        $$BOOST_DIR/stage/lib/libboost_system-gcc-mt-1_54-android.so            \
        $$BOOST_DIR/stage/lib/libboost_thread-gcc-mt-1_54-android.so            \
        $$BOOST_DIR/stage/lib/libboost_timer-gcc-mt-1_54-android.so             \
        $$BOOST_DIR/stage/lib/libboost_unit_test_framework-gcc-mt-1_54-android.so\
        $$BOOST_DIR/stage/lib/libboost_wave-gcc-mt-1_54-android.so              \
        $$VANDOUKEN_BUILD_DIR/lib/hpx/libvandouken.so                           \
        $$LIBGEODECOMP_BUILD_DIR/libgeodecomp.so                                \

