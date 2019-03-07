cmake_minimum_required( VERSION 2.8.8 )

set( TARGET_LINUX_ABI ${LINUX_ABI} )
set( TARGET_TOOLCHAIN ${LINUX_TOOLCHAIN} )
message( STATUS " TARGET_LINUX_ABI=\"${TARGET_LINUX_ABI}\" is specified." )
message( STATUS " TARGET_TOOLCHAIN=\"${TARGET_TOOLCHAIN}\" is specified." )

# config --sysroot
set( LINUX_SDK_ROOT "/disk2/cmc/android_projects/sdk_develop/rk3308_linux" )
set( LINUX_SYSROOT "${LINUX_SDK_ROOT}/buildroot/output/rockchip_rk3308_release/host/aarch64-rockchip-linux-gnu/sysroot" )
set( CMAKE_SYSROOT "${LINUX_SYSROOT}")
message( STATUS " CMAKE_SYSROOT=\"${CMAKE_SYSROOT}\" is specified." )

# operating system on which CMake is targeting
set( CMAKE_SYSTEM_NAME Linux )

# set target LINUX_ABI options
set (TARGET_LINUX_ABI "arm64-v8a")
if( TARGET_LINUX_ABI STREQUAL "x86" )
 set( X86 true )
 set( LINUX_ARCH_NAME "x86" )
 set( CMAKE_SYSTEM_PROCESSOR "i686" )
elseif( TARGET_LINUX_ABI STREQUAL "x86_64" )
 set( X86 true )
 set( X86_64 true )
 set( LINUX_ARCH_NAME "x86_64" )
 set( CMAKE_SYSTEM_PROCESSOR "x86_64" )
elseif( TARGET_LINUX_ABI STREQUAL "arm64-v8a" )
 set( ARM64_V8A true )
 set( LINUX_ARCH_NAME "arm64" )
 set( CMAKE_SYSTEM_PROCESSOR "aarch64" )
elseif( TARGET_LINUX_ABI STREQUAL "armeabi-v7a")
 set( ARMEABI_V7A true )
 set( LINUX_ARCH_NAME "arm" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
else()
 message( SEND_ERROR "Unknown TARGET_LINUX_ABI=\"${TARGET_LINUX_ABI}\" is specified." )
endif()


# config rootfs and linker
set( LINUX_LINKER_FLAGS "-Wl,--allow-multiple-definition" )

# set( LD_LIBRARY_PATH "${LINUX_SYSROOT}/usr/lib" )
# set( LINUX_LINKER_FLAGS "${LINUX_LINKER_FLAGS} -Wl, -Wno-undef, -lstdc++" )
# set( CMAKE_SHARED_LINKER_FLAGS "${LINUX_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}" )
# set( CMAKE_MODULE_LINKER_FLAGS "${LINUX_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}" )
# set( CMAKE_EXE_LINKER_FLAGS    "${LINUX_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}" )
# set ( CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath-link=${LINUX_SYSROOT}/usr/lib -L${LINUX_SYSROOT}/usr/lib" )

# config cross compilation
set( CMAKE_C_COMPILER   ${LINUX_SDK_ROOT}/buildroot/output/rockchip_rk3308_release/host/bin/aarch64-rockchip-linux-gnu-gcc )
set( CMAKE_CXX_COMPILER ${LINUX_SDK_ROOT}/buildroot/output/rockchip_rk3308_release/host/bin/aarch64-rockchip-linux-gnu-g++ )

# config extra build flags
