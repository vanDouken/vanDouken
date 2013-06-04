
# Copyright (c) 2012 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

THIS_PATH := $(call my-dir)

$(call import-module,hpx_android)

LOCAL_PATH := $(THIS_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE:=hpiif_android
LOCAL_SRC_FILES:=main.cpp
LOCAL_SRC_FILES+=../../../../hpiif/flowingcanvas_collector.cpp
LOCAL_SRC_FILES+=../../../../hpiif/flowingcanvas_component.cpp
LOCAL_SRC_FILES+=../../../../hpiif/flowingcanvas_gui.cpp
LOCAL_SRC_FILES+=../../../../hpiif/flowingcanvas_simulator.cpp
LOCAL_SRC_FILES+=../../../../hpiif/flowingcanvas_updategroup.cpp

LOCAL_C_INCLUDES:=$(HPX_INCLUDES)
LOCAL_C_INCLUDES+=$(realpath ../../../hpiif)
LOCAL_C_INCLUDES+=$(realpath ../../../hpiif/model_test)
LOCAL_C_INCLUDES+=$(LIBGEODECOMP_INCLUDE_DIR)

LOCAL_CPPFLAGS:=$(HPX_CPPFLAGS)
LOCAL_CPPFLAGS+=-DHPX_COMPONENT_NAME=hpiif_android
LOCAL_CPPFLAGS+=-DHPX_COMPONENT_STRING=\"hpiif_android\"
LOCAL_CPPFLAGS+=-DHPX_COMPONENT_EXPORTS

LOCAL_STATIC_LIBRARIES := $(HPX_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(HPX_SHARED_LIBRARIES)

LOCAL_LDLIBS := -llog -lGLESv2 -lGLESv1_CM
LOCAL_LDLIBS += -fuse-ld=gold

NDK_TOOLCHAIN_VERSION:=4.6
LOCAL_ARM_NEON:=true
LOCAL_CPP_FEATURES:=exceptions rtti
include $(BUILD_SHARED_LIBRARY)

$(call import-module,cpufeatures)
