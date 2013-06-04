
# Copyright (c) 2012 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

APP_ABI:=armeabi-v7a
APP_STL := gnustl_shared
APP_OPTIM := release
APP_GNUSTL_FORCE_CPP_FEATURES := exceptions rtti
APP_CPPFLAGS+=-D_GNU_SOURCE
#APP_CPPFLAGS+=-DDEBUG
APP_CPPFLAGS+=-DNDEBUG
#APP_CPPFLAGS+=-DBOOST_ENABLE_ASSERTS
#APP_CPPFLAGS+=-DBOOST_ENABLE_ASSERT_HANDLER
APP_CPPFLAGS+=-DBOOST_SYSTEM_NO_DEPRECATED
APP_CPPFLAGS+=-Wno-psabi
APP_CPPFLAGS+=-fexceptions
APP_CPPFLAGS+=-frtti

APP_CPPFLAGS+=-Wl,--copy-dt-needed-entries
APP_CPPFLAGS+=-rdynamic
APP_CPPFLAGS+=-std=c++11
APP_CPPFLAGS+=-pthread

