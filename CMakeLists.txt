cmake_minimum_required (VERSION 2.8) 
project (JSMySQL) 

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

IF (WIN32)
#check MySQL Server headers
  set(MYSQL_INCLUDE_DIR "C:\\Program Files\\MySQL\\MySQL Server 5.6\\include\\" CACHE STRING "MySQL include directory")
ELSE()
  set(MYSQL_INCLUDE_DIR "/usr/local/mysql/include" CACHE STRING "MySQL include directory")
  set (MYSQL_LIB_DIR "/usr/local/mysql/lib")
ENDIF()


link_directories(${MYSQL_LIB_DIR})
add_library (mysql-js MODULE "udf_javascript.cc" "JSEnv.h" "JSEnv.cc" "Context.h" "Context.cc")

target_include_directories (mysql-js PUBLIC ${MYSQL_INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/deps/v8" "${CMAKE_SOURCE_DIR}/deps/v8/include")

SET_TARGET_PROPERTIES(mysql-js
PROPERTIES LINK_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}")
set_target_properties(mysql-js PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
TARGET_LINK_LIBRARIES(mysql-js mysqld dl "${CMAKE_SOURCE_DIR}/deps/v8/libs/libv8_libbase.a" 
        "${CMAKE_SOURCE_DIR}/deps/v8/libs/libv8_base.a" 
        "${CMAKE_SOURCE_DIR}/deps/v8/libs/libv8_libplatform.a"
        "${CMAKE_SOURCE_DIR}/deps/v8/libs/libicuuc.a"
        "${CMAKE_SOURCE_DIR}/deps/v8/libs/libicui18n.a" 
        "${CMAKE_SOURCE_DIR}/deps/v8/libs/libv8_libsampler.a"
        "${CMAKE_SOURCE_DIR}/deps/v8/libs/libv8_external_snapshot.a")