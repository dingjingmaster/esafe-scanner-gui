#!/bin/bash

buildEnv="/data/build-env"
uosX86="${buildEnv}/uos-x86"
uosARM=""
kylinARM="${buildEnv}/kylin-arm/"

armCompiler="${buildEnv}/gcc-11.2.1-arm"
x86Compiler="${buildEnv}/gcc-12.2.0-x86"

curDir=$(dirname $(realpath -- $0))
uosX86Build="${curDir}/build-uos-x86"
kylinARMBuild="${curDir}/build-kylin-arm"


# uos x86 编译
cmake -S "${curDir}" -B "${uosX86Build}" \
    -D BUILD_DJ=ON \
    -D DJ_IS_UOS=ON -D DJ_IS_KYLIN=OFF \
    -D DJ_IS_X86=ON -D DJ_IS_ARM=OFF \
    -D DJ_IS_ABC=ON -D DJ_IS_UNIVERSAL=OFF \
    -D DJ_OS=Linux -D DJ_OS_ARCH=x86_64 \
    -D DJ_ROOT_FS=${uosX86} \
    -D DJ_COMPILER=${x86Compiler} \
    && \
    make -C "${uosX86Build}" -j6

# kylin arm 编译
#cmake -S "${curDir}" -B "${kylinARMBuild}" \
#    -D BUILD_DJ=ON \
#    -D DJ_IS_UOS=OFF -D DJ_IS_KYLIN=ON \
#    -D DJ_IS_X86=OFF -D DJ_IS_ARM=ON \
#    -D DJ_IS_ABC=ON -D DJ_IS_UNIVERSAL=OFF \
#    -D DJ_OS=Linux -D DJ_OS_ARCH=aarch \
#    -D DJ_ROOT_FS=${kylinARM} \
#    -D DJ_COMPILER=${armCompiler} \
#    && \
#    make -C "${uosX86Build}" -j6

