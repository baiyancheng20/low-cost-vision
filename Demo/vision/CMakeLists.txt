cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
include(FindPkgConfig)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
rosbuild_genmsg()
#uncomment if you have defined services
rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})

rosbuild_add_executable(vision src/main.cpp src/visionNode.cpp src/CrateTracker.cpp)

pkg_check_modules(PKG_LIBS REQUIRED opencv zbar libunicap)

include_directories($ENV{LCV_PROJECT_PATH}/Vision/Fiducial/include)
target_link_libraries(vision $ENV{LCV_PROJECT_PATH}/Vision/Fiducial/lib/libFiducial.a)

include_directories($ENV{LCV_PROJECT_PATH}/Vision/unicap_cv_bridge/include)
target_link_libraries(vision $ENV{LCV_PROJECT_PATH}/Vision/unicap_cv_bridge/lib/libunicap_cv_bridge.a)

include_directories($ENV{LCV_PROJECT_PATH}/Vision/PixelCord_RealifeCord_Transformation/include)
target_link_libraries(vision $ENV{LCV_PROJECT_PATH}/Vision/PixelCord_RealifeCord_Transformation/lib/libcoordinates.a)

include_directories($ENV{LCV_PROJECT_PATH}/Vision/CameraCalibration/include)
target_link_libraries(vision $ENV{LCV_PROJECT_PATH}/Vision/CameraCalibration/lib/libCameraCalibration.a)

include_directories(/usr/include/unicap)
include_directories(/usr/include/local/unicap)

target_link_libraries(vision boost_thread)
target_link_libraries(vision boost_filesystem)
target_link_libraries(vision boost_system)

include_directories(${PKG_LIBS_INCLUDE_DIRS})
target_link_libraries(vision ${PKG_LIBS_LIBRARIES})
