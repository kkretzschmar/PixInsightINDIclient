#!/usr/bin/env sh
echo "OS:         $TRAVIS_OS_NAME"
echo "BUILD_DIR:  $TAVIS_BUILD_DIR"

if [ "$TRAVIS_OS_NAME" = "linux" ]; 
then
 export OS_PATH="linux"
 echo "Starting: tar --warning=no-unknown-keyword -xzf PCL-02.00.13.0689-20141030.tar.gz ..."
 tar --warning=no-unknown-keyword -xzf PCL-02.00.13.0689-20141030.tar.gz
 echo "done"
 
 # Build gmock and gtest
 export GTEST_DIR=gtest-1.7.0
 export GMOCK_DIR=gmock-1.7.0
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
 echo Building gtest ..."
 mkdir build && cd build && cmake ../${GTEST_DIR} && make && cp libgtest.a ../${GTEST_LIB}/x64 && cp libgtest_main.a ../${GTEST_LIB}/x64 && cd .. 
 echo "done"
 echo Building gmock ..."
 mkdir build && cd build && cmake ../${GMOCK_DIR} && make && cp libgmock.a ../${GMOCK_LIB}/x64 && cd ..
 echo "done"
 
elif [ "$TRAVIS_OS_NAME" = "osx" ];
then
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
pwd
cd module/$OS_PATH/g++/ && mkdir -p x64/Release && mkdir -p x64/Debug  && make -f makefile-x64  && make -f makefile-x64-debug-static && cd ../../../

# build and run PixInsightINDIclient tests
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
 pwd
 cd test/$OS_PATH/g++ && make -f makefile-x64-debug && cd ../../..
 # run tests
 echo "pwd" && pwd
 echo "PCLBINDIR64: ${PCLBINDIR64}"
 ${PCLBINDIR64}/PixInsightINDIclientTest
fi
