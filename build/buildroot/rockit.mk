################################################################################
#
# rockit
#
################################################################################
ROCKIT_SITE = $(TOPDIR)/../external/rockit-dev
ROCKIT_SITE_METHOD = local
ROCKIT_INSTALL_STAGING = YES

ROCKIT_CONF_OPTS += -DCMAKE_BUILD_TYPE=DEBUG \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_TOOLCHAIN_FILE=./build/linux/linux.cross.cmake   \
                -DHAVE_PTHREAD=ON                                        \
                -DHAVE_MPI=OFF                                           \
                -DHAVE_GLES=OFF                                          \
                -DHAVE_DRM=OFF                                           \
                -DOS_LINUX=ON                                            \
                -DLINUX_TOOLCHAIN:STRING="aarch64-rockchip-linux-gnu"    \
                -DLINUX_ABI:STRING="arm64-v8a"

$(eval $(cmake-package))
