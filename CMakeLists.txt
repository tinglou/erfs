

cmake_minimum_required(VERSION 3.5.1)

project(resource-fs)
set(CMAKE_CXX_STANDARD "17")

enable_testing()
find_package(GTest REQUIRED)


include_directories(
    src
    ${CMAKE_CURRENT_BINARY_DIR}
)

#
# Resource Filesystem Readonly
#
set(RFS_ROM "resource_fs")
set(RFS_ROM_FILES "src/resource_fs.c")
add_library(${RFS_ROM} STATIC "${RFS_ROM_FILES}")

#
# generator 
#
set(RFS_APP "rfs_generator")
set(RFS_APP_FILES 
    src/main.cpp
    src/rfs_generator.cpp
    )
add_executable(${RFS_APP}  "${RFS_APP_FILES}")

#
# generate resourc_fs .c source file
#
function(gen_rfs_source sourcedir id target)
    add_custom_command(
        OUTPUT ${target}/rfs_${id}.c
        COMMAND ${RFS_APP} ${sourcedir} ${id} ${target}
        COMMENT "Generating resourc_fs .c source file from: ${sourcedir}"
    )
endfunction()

gen_rfs_source("${CMAKE_CURRENT_SOURCE_DIR}/src" "rfsrc" "${CMAKE_CURRENT_BINARY_DIR}")


#
# Unit test
#
set(RFS_UT "rfs_ut")
set(RFS_UT_FILES tests/rfs_test.cpp ${CMAKE_CURRENT_BINARY_DIR}/rfs_rfsrc.c)
add_executable(${RFS_UT} ${RFS_UT_FILES} ${RFS_ROM_FILES})
#set_property(TARGET ${RFS_UT}  PROPERTY CXX_STANDARD 11)
#target_link_libraries(${RFS_UT}  GTest::GTest GTest::Main -lgcov)
target_link_libraries(${RFS_UT}  GTest::GTest GTest::Main)
#set_target_properties(${RFS_UT} PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
add_test(${RFS_UT} ${RFS_UT})