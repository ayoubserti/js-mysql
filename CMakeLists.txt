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
  set(MYSQL_INCLUDE_DIR "/usr/local/mysql/include" "/usr/include/mysql" CACHE STRING "MySQL include directory")
  set (MYSQL_LIB_DIR "/usr/local/mysql/lib" "/usr/lib/mysql") 
ENDIF()


link_directories(${MYSQL_LIB_DIR} /usr/local/opt/v8/lib /opt/libv8-5.9/lib )

add_library (mysql-js MODULE "udf_javascript.cc" "JSEnv.h" "JSEnv.cc" "Context.h" "Context.cc")

target_include_directories (mysql-js PUBLIC ${MYSQL_INCLUDE_DIR} "/usr/local/opt/v8/include"  "/opt/libv8-5.9/include")

SET_TARGET_PROPERTIES(mysql-js
PROPERTIES LINK_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}")
set_target_properties(mysql-js PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
IF(APPLE)
  TARGET_LINK_LIBRARIES(mysql-js  dl libv8_base.a libv8_libbase.a libv8_libplatform.a libv8_snapshot.a )
ELSE()
  TARGET_LINK_LIBRARIES(mysql-js  dl v8_base v8_libbase v8_libplatform v8 icui18n icuuc )
ENDIF()
#install target 
if(NOT DOWNLOAD_PRE_BUILD_V8)
  SET (myFiles "${CMAKE_SOURCE_DIR}/v8/out.gn/x64.release/icudtl.dat" 
                  "${CMAKE_SOURCE_DIR}/v8/out.gn/x64.release/natives_blob.bin"
                  "${CMAKE_SOURCE_DIR}/v8/out.gn/x64.release/snapshot_blob.bin"
                  "${CMAKE_SOURCE_DIR}/Loader.js"
                  "${CMAKE_SOURCE_DIR}/script.js")
ELSE()
if (NOT WIN32)
  SET (myFiles   "${CMAKE_SOURCE_DIR}/Loader.js"
                  "${CMAKE_SOURCE_DIR}/script.js")
endif()

ENDIF()
IF (WIN32)
ELSE()
  SET (PLUGIN_DESTINATION "/usr/local/mysql/lib/plugin/")

  
  install(TARGETS mysql-js LIBRARY DESTINATION "${PLUGIN_DESTINATION}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE) 
  install(FILES ${myFiles} DESTINATION "${PLUGIN_DESTINATION}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
ENDIF()