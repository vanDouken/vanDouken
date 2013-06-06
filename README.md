PIMPF - Preview
=====

Parallel Image Particle Flow

This is a preview of the source code formerly known as HPPIF.
We will release a refactored and cleaned up version under the PIMPF name soon.

Android App Build Instructions
=====

Prerequesites:
  * hpx: https://github.com/STEllAR-GROUP/hpx
  * HPXAndroid: https://github.com/STEllAR-GROUP/HPXAndroid
  * Boost 1.53.0: https://sourceforge.net/projects/boost/files/boost/1.53.0/
  * Android SDK: http://developer.android.com/sdk/index.html
  * Android NDK: http://developer.android.com/tools/sdk/ndk/index.html
  * LibGeoDecomp: https://bitbucket.org/sithhell/libgeodecomp

Instructions:
First, patch boost with this patch: https://github.com/PIMPF/PIMPF/blob/master/boost_android_1_53_0.patch

    cd /path/to/PIMPF/android/sc_image_demo_app
    export PATH=$PATH:/path/to/android-ndk-r8e:/path/to/android-sdk-linux/tools:/path/to/android-sdk-linux/platform-tools
    android update project --target <target-id> --path . --name PIMPF
    export NDK_MODULE_PATH=/path/to/HPXAndroid/modules
    export HPX_SRC_ROOT=/path/to/hpx
    export BOOST_SRC_ROOT=/path/to/boost/1_53_0
    export LIBGEODECOMP_INCLUDE_DIR=/path/to/libgeodecomp/src/
    ndk-build 
    ant debug

You can now use bin/PIMPF-debug.apk at your pleasure
