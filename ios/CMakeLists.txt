cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 17)

project(clang VERSION 17.0.0 LANGUAGES CXX C)
project(lld VERSION 17.0.0 LANGUAGES CXX C)
project(lldb VERSION 17.0.0 LANGUAGES CXX C)

set(LLVM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Clang built-in
file(GLOB CLANG_AS ${LLVM_LIBS}/libclang*.a)
list(FILTER CLANG_AS EXCLUDE REGEX ".*libclang_rt.*.a$")

# lld & lldb built-in
file(GLOB LLD_LLDB_AS ${LLVM_LIBS}/liblld*.a)

if(CMAKE_OSX_SYSROOT MATCHES iphonesimulator)
    set(LLVM_BUILD_DIR "build-iphonesimulator")
    set(LLVM_MODE "Release-iphonesimulator")
else()
    set(LLVM_BUILD_DIR "build-iphoneos")
    set(LLVM_MODE "Release-iphoneos")
endif()

set(LLVM_LIBS ${LLVM_DIR}/${LLVM_BUILD_DIR}/lib)
set(LLVM_FRAMEWORKS ${LLVM_DIR}/${LLVM_BUILD_DIR}/build/${LLVM_MODE})
set(NOSYSTEM_FRAMEWORK ${LLVM_DIR}/no_system/${LLVM_BUILD_DIR}/Debug-iphoneos)

find_library(LLVM REQUIRED NAMES libLLVM PATHS ${LLVM_FRAMEWORKS} CMAKE_FIND_ROOT_PATH_BOTH)
find_library(NOSYSTEM REQUIRED NAMES nosystem PATHS ${NOSYSTEM_FRAMEWORK} CMAKE_FIND_ROOT_PATH_BOTH)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(COMPRESSION compression)
find_library(XML2 xml2)
find_library(LIBZ z)

# Clang
file(GLOB CLANG_AS ${LLVM_LIBS}/libclang*.a)
list(FILTER CLANG_AS EXCLUDE REGEX ".*libclang_rt.*.a$")

# lld
file(GLOB LLD_AS ${LLVM_LIBS}/liblld*.a)
list(FILTER LLD_AS EXCLUDE REGEX ".*liblldb.*.a$")

# lldb
file(GLOB LLDB_AS ${LLVM_LIBS}/liblldb*.a)

add_library(clang
    SHARED
    ${CLANG_AS}
    clang.cpp
)
include_directories(clang
    ${LLVM_DIR}/llvm/include
    ${LLVM_DIR}/${LLVM_BUILD_DIR}/include
)

add_library(lld
    SHARED
    lld.cpp
)
include_directories(lld
    ${LLVM_DIR}/llvm/include
    ${LLVM_DIR}/${LLVM_BUILD_DIR}/include
)
 
add_library(lldb
    SHARED
    lldb.cpp
)
include_directories(lldb
    ${LLVM_DIR}/llvm/include
    ${LLVM_DIR}/${LLVM_BUILD_DIR}/include
)

target_link_libraries(clang
    PRIVATE
    ${LLVM}
    ${NOSYSTEM}
    ${CLANG_AS}
)

target_link_libraries(lld
    PRIVATE
    ${LLVM}
    ${NOSYSTEM}
    ${FOUNDATION_LIBRARY}
    ${LLD_AS}
    ${CLANG_AS}
    ${LIBZ}
)

target_link_libraries(lldb
    PRIVATE
    ${LLVM}
    ${NOSYSTEM}
    ${LLDB_AS}
    ${CLANG_AS}
    ${FOUNDATION_LIBRARY}
    ${COMPRESSION}
    ${XML2}
    ${LIBZ}
)

set_target_properties(clang PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER me.fredl.clang
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION} 
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        XCODE_ATTRIBUTE_SKIP_INSTALL "Yes"
        LINKER_LANGUAGE CXX
)

set_target_properties(lld PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER me.fredl.wasm-ld
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        XCODE_ATTRIBUTE_SKIP_INSTALL "Yes"
        LINKER_LANGUAGE CXX
)

set_target_properties(lldb PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER me.fredl.lldb
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        XCODE_ATTRIBUTE_SKIP_INSTALL "Yes"
        LINKER_LANGUAGE CXX
)
