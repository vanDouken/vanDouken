//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MSG
#   if !defined(ANDROID)
#       define MSG(x) std::cout << x;
#   else
#   include <android/log.h>
#   define MSG(x)                                                               \
{                                                                               \
    std::stringstream sstr;                                                     \
    sstr << x;                                                                  \
    __android_log_print(ANDROID_LOG_INFO, "vandouken", "%s", sstr.str().c_str());            \
} 
/**/
#   endif
#endif

