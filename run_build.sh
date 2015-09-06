#!/usr/bin/env sh
echo "OS:         $TRAVIS_OS_NAME"
echo "BUILD_DIR:  $TRAVIS_BUILD_DIR"
echo "CXX:        $CXX"
if [ "$TRAVIS_OS_NAME" = "linux" ]; 
then
 #define OS_PATH
 export OS_PATH="linux"
 echo "Starting: tar --warning=no-unknown-keyword -xzf PCL-02.00.13.0689-20141030.tar.gz ..."
 tar --warning=no-unknown-keyword -xzf PCL-02.00.13.0689-20141030.tar.gz
 echo "done"
 
 # Build gmock and gtest
 export GTEST_DIR=$TRAVIS_BUILD_DIR/gtest-1.7.0
 export GMOCK_DIR=$TRAVIS_BUILD_DIR/gmock-1.7.0
 export GTEST_INC=${GTEST_DIR}/include
 export GMOCK_INC=${GMOCK_DIR}/include
 export GTEST_LIB=${GTEST_DIR}/lib
 export GMOCK_LIB=${GMOCK_DIR}/lib
 mkdir -p ${GTEST_LIB}/x64
 mkdir -p ${GMOCK_LIB}/x64
 echo "Starting:  unzip gtest and gmock ..."
 unzip gtest-1.7.0.zip
 unzip gmock-1.7.0.zip
 echo "done"
 echo "Building gtest ..."
 mkdir build_gtest && cd build_gtest && cmake ${GTEST_DIR} && make && cp libgtest.a ${GTEST_LIB}/x64 && cp libgtest_main.a ${GTEST_LIB}/x64 && cd .. 
 echo "done"
 echo "Building gmock ..."
 mkdir build_gmock && cd build_gmock && cmake ${GMOCK_DIR} && make && cp libgmock.a ${GMOCK_LIB}/x64 && cd ..
 echo "done"
 
elif [ "$TRAVIS_OS_NAME" = "osx" ];
then
 #define OS_PATH
 export OS_PATH="macosx"
 echo "Starting: gunzip PCL-02.00.13.0689-20141030.tar.gz..."
 echo "Starting: tar -xf PCL-02.00.13.0689-20141030.tar.gz..."
 gunzip PCL-02.00.13.0689-20141030.tar.gz
 tar -xf PCL-02.00.13.0689-20141030.tar
 echo "done"
fi
echo $OS_PATH
export PCLLIBDIR64=../../../PCL/lib/$OS_PATH/x64
echo $PCLLIBDIR64

# create build directory for clang++ builds
if [ "${CXX}" = "clang++" ]; then
 mkdir -p module/$OS_PATH/${CXX}/
 cp module/$OS_PATH/g++/makefile* module/$OS_PATH/${CXX}/
 # change explicit references to g++ directory
 sed -i 's/\/g++\//\/clang++\//g' module/$OS_PATH/${CXX}/makefile-x64
 sed -i 's/\/g++\//\/clang++\//g' module/$OS_PATH/${CXX}/makefile-x64-debug-static
 
 mkdir -p test/$OS_PATH/${CXX}/
 cp test/$OS_PATH/g++/makefile* test/$OS_PATH/${CXX}/
 # change explicit references to g++ directory
 sed -i 's/\/g++\//\/clang++\//g' test/$OS_PATH/${CXX}/makefile-x64-debug
fi 

pwd
cd module/$OS_PATH/${CXX}/ && mkdir -p x64/Release  && make -f makefile-x64  && cd ${TRAVIS_BUILD_DIR}

# build and run PixInsightINDIclient tests
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
 cd module/$OS_PATH/${CXX}/   && mkdir -p x64/Debug   && make -f makefile-x64-debug-static && cd ${TRAVIS_BUILD_DIR}
 pwd
 cd test/$OS_PATH/${CXX} && mkdir -p x64/Debug && mkdir -p x64/Debug/fakes && make -f makefile-x64-debug && cd ${TRAVIS_BUILD_DIR}
 # run tests
 echo "pwd" && pwd
 echo "PCLBINDIR64: ${PCLBINDIR64}"
 # run tests
 ${TRAVIS_BUILD_DIR}/PCL/bin/PixInsightINDIclientTest
fi

# package build results
ARCHIVE_NAME=PixInsightINDI_linux_osx_x64.tar

# check if archive exits and set tar commands accordingly
if [ -e "${ARCHIVE_NAME}" ]; then
 TAR_CMD="-uvf"
else
 TAR_CMD="-cvf"
fi

if [ "$TRAVIS_OS_NAME" = "linux" ]; 
then
	tar ${TAR_CMD}  ${ARCHIVE_NAME} module/$OS_PATH/${CXX}/x64/Release/PixInsightINDIclient-pxm.so module/$OS_PATH/${CXX}/x64/Debug/PixInsightINDIclient-pxm.so 
elif [ "$TRAVIS_OS_NAME" = "osx" ];
then
    tar ${TAR_CMD} ${ARCHIVE_NAME}  module/$OS_PATH/${CXX}/x64/Release/PixInsightINDIclient-pxm.dylib
fi
