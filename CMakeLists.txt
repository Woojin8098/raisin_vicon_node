cmake_minimum_required(VERSION 3.15)

## write your package name
project(raisin_vicon)

set(EXEC_NAME ${PROJECT_NAME}_node)
set(LIB_NAME ${PROJECT_NAME}_publisher)

set(ENV{RAISIN_WS} ${CMAKE_CURRENT_SOURCE_DIR}/../..) ## set the path to your raisin workspace
set(CMAKE_PREFIX_PATH $ENV{RAISIN_WS}/install)

## added your dependencies here
raisin_find_package(raisin_network REQUIRED)

add_library(${LIB_NAME} SHARED
        src/raisin_vicon_publisher.cpp
)
set_property(TARGET ${LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

set(VICON_SDK ViconDataStreamSDK_1.11.0.128037h__Linux64)
set(VICON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/raisin_vicon_node/third_party/${VICON_SDK})
set(VICON_LIBRARIES
        ${VICON_DIR}/libViconDataStreamSDK_CPP.so
        ${VICON_DIR}/libboost_chrono.so.1.68.0
        ${VICON_DIR}/libboost_system.so.1.68.0
        ${VICON_DIR}/libboost_thread.so.1.68.0
        ${VICON_DIR}/libboost_timer.so.1.68.0
)

## write your dependencies here
target_link_libraries(${LIB_NAME}
        raisin_network
        ${VICON_LIBRARIES})

target_link_directories(${LIB_NAME}
        PUBLIC
        ${VICON_DIR}
)

target_include_directories(${LIB_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${RAISIN_MASTER_INCLUDE}> ## headers for the message files
        $<BUILD_INTERFACE:${VICON_DIR}>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:${VICON_DIR}>)
target_include_directories(${LIB_NAME} PRIVATE  $ENV{RAISIN_WS}/install)

add_executable(${EXEC_NAME} src/main.cpp)
target_link_libraries(${EXEC_NAME} raisin_network ${LIB_NAME})

set_target_properties(${LIB_NAME} PROPERTIES
  INSTALL_RPATH "$ENV{RAISIN_WS}/install/lib"
  BUILD_RPATH   "$ENV{RAISIN_WS}/install/lib"
)

set_target_properties(${EXEC_NAME} PROPERTIES
  INSTALL_RPATH "$ENV{RAISIN_WS}/install/lib"
  BUILD_RPATH   "$ENV{RAISIN_WS}/install/lib"
)

install(TARGETS ${EXEC_NAME} DESTINATION bin)
install(TARGETS ${LIB_NAME} DESTINATION lib)
install(FILES ${VICON_LIBRARIES} DESTINATION lib)
