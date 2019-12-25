cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(SOURCE_FILES
        source/Tools/Log.hpp
        source/Tools/Log.cpp
        source/Common/FileUtils.cpp
        source/Common/FileUtils.hpp
        source/DataBase/DatabaseProxy.cpp
        source/DataBase/DatabaseProxy.h
        source/Command/LocalStorageCmd.cpp
        source/Command/LocalStorageCmd.hpp
        source/ThirdParty/CompatibleFileSystem.hpp
        source/ThirdParty/ghc-filesystem.hpp
        source/LocalStorageService.h
        source/LocalStorageService.cpp
        source/ErrCode.cpp
        source/ErrCode.h)
set(CHATGROUP_LIB_INCLUDE lib/include)
INCLUDE_DIRECTORIES(${CHATGROUP_LIB_INCLUDE})
INCLUDE_DIRECTORIES(source/)
INCLUDE_DIRECTORIES(./lib/include/Elastos.SDK.Contact)
INCLUDE_DIRECTORIES(./lib/include/Elastos.SDK.Keypair.C)
LINK_DIRECTORIES("source/")
add_library(LocalStorageService  SHARED )
target_sources(LocalStorageService PRIVATE
        ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(LocalStorageService pthread sqlite3)

pkg_search_module(pkg-Elastos.SDK.Contact REQUIRED Elastos.SDK.Contact)
message(STATUS "Elastos.SDK.Contact include path: ${pkg-Elastos.SDK.Contact_INCLUDE_DIRS}")
TARGET_INCLUDE_DIRECTORIES(LocalStorageService PRIVATE "${pkg-Elastos.SDK.Contact_INCLUDE_DIRS}" )
TARGET_LINK_LIBRARIES(LocalStorageService PeerNode)

install(TARGETS LocalStorageService
        LIBRARY DESTINATION lib/PeerNodePlugins
        ARCHIVE DESTINATION lib/PeerNodePlugins)
