# ###############################################
# Check for a valid version of CMake.
CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )
CMAKE_POLICY (VERSION 3.0)

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    MESSAGE(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
ENDIF()

# ###############################################
# Start the project.

PROJECT( Omega )

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET (PLATFORM_CXX_FLAGS "-Wall -Wextra -m64 -g3")
    IF (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
        SET (GNUCXX_VERSION_SPECIFIC_FLAGS "-std=c++1y")
    ELSE()
        SET (GNUCXX_VERSION_SPECIFIC_FLAGS "-std=c++0x -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD")
    ENDIF()
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET( PLATFORM_CXX_FLAGS "-Wall -Wextra -g3 -stdlib=libc++" )
    SET (GNUCXX_VERSION_SPECIFIC_FLAGS "-std=c++1y")
ELSE()
    MESSAGE (FATAL_ERROR "********************* UNSUPPORTED PLATFORM *********************")
ENDIF()


SET (CMAKE_EXPORT_COMPILE_COMMANDS ON)



# ###############################################
# Perform single-shot actions at the very first configuration only (see http://johnlamp.net/cmake-tutorial-3-gui-tool.html):
# We want CMAKE_xxx_FLAGS to be cached, but either
# 1) we set it _without_ FORCE, and then it won't be cached because CMAKE_xxx_FLAGS is a built-in variable, and hence it's present
#    in the cache even before the first time our project is configured
# 2) we set it _with_ FORCE, but then every time we configure, we'll keep appending
# More generally, many of the CMake variables exist in the cache before CMakeLists.txt is processed so that the only
# solution is to detect when this is the VERY FIRST CONFIGURATION and only in that case FORCEly set the cache variable:

IF (NOT CONFIGURED_ONCE)
    MESSAGE ("\n********** PERFORMING FIRST TIME CONFIGURATION **********")
    SET (CMAKE_CXX_FLAGS_DEBUG "-O0 -UNDEBUG" CACHE STRING "Flags for CXX Debug build" FORCE)

    SET (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags for CXX Release build" FORCE)
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Flags for CXX RelWithDebInfo build" FORCE)
    SET (CMAKE_CXX_VISIBILITY_PRESET "hidden")

    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PLATFORM_CXX_FLAGS} ${GNUCXX_VERSION_SPECIFIC_FLAGS} " CACHE STRING "Flags used by the C++ compiler during all build types" FORCE)
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_EXE_LINKER_FLAGS}" CACHE STRING "Flags used by the linker" FORCE)
    SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PLATFORM_SHARED_LINKER_FLAGS}" CACHE STRING "Flags used by the linker during the creation of DLLs" FORCE)
    SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${PLATFORM_MODULE_LINKER_FLAGS}" CACHE STRING "Flags used by the linker during the creation of modules" FORCE)


    # NOTE: No need to FORCE this, as CONFIGURED_ONCE is almost guaranteed not to be present in the cache...
    SET (CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once")
ENDIF()

MESSAGE ("\n")
MESSAGE ("Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
MESSAGE ("System Name: ${CMAKE_SYSTEM_NAME}")
MESSAGE ("System Version: ${CMAKE_SYSTEM_VERSION}")
MESSAGE ("Install Prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE ("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
MESSAGE ("CMAKE_LINKER: ${CMAKE_LINKER}")
MESSAGE ("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE ("CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE ("CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")


INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/Lib/Include
                     ${CMAKE_SOURCE_DIR}/External/googletest
                     ${CMAKE_SOURCE_DIR}/External/googletest/include)


# target to update git submodules
ADD_CUSTOM_TARGET(
    git_update
    COMMAND git submodule init
    COMMAND git submodule update
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

# google test sources
ADD_LIBRARY(
    gtest
    ${CMAKE_SOURCE_DIR}/External/googletest/src/gtest-all.cc
    ${CMAKE_SOURCE_DIR}/External/googletest/src/gtest_main.cc
    )

# update git submodules before building google test
ADD_DEPENDENCIES(gtest git_update)

# mark these files as generated (they may not be present at configure time)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/External/googletest/src/gtest-all.cc  PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/External/googletest/src/gtest_main.cc PROPERTIES GENERATED 1)


INCLUDE(CTest)
ENABLE_TESTING()

# ADD_GMOCK_TEST(<library_to_test> <sources of test>...)
#
#  Adds a Google Mock based test executable, _Test_<library_to_test>, built from <sources of test> and
#  adds the test so that CTest will run it. Both the executable and the test
#  will be named _Test_<library_to_test>.
#
FUNCTION(ADD_GMOCK_TEST lib_under_test)
    SET (TEST_TARGET_NAME "_Test_${lib_under_test}")

    ADD_EXECUTABLE(${TEST_TARGET_NAME} ${ARGN})

    #Allow tests to access private headers of its dependents
    #TARGET_INCLUDE_DIRECTORIES (${TEST_TARGET_NAME} PRIVATE $<TARGET_PROPERTY:${lib_under_test},INCLUDE_DIRECTORIES>)

    #TARGET_LINK_LIBRARIES(${TEST_TARGET_NAME} ${lib_under_test} gmock)
    TARGET_LINK_LIBRARIES(${TEST_TARGET_NAME} gtest)

    ADD_TEST (NAME
        ${TEST_TARGET_NAME}
        COMMAND
        ${TEST_TARGET_NAME})

    SET_PROPERTY (TARGET ${TEST_TARGET_NAME} PROPERTY FOLDER Tests)
ENDFUNCTION()

ADD_GMOCK_TEST ("Omega" "${CMAKE_SOURCE_DIR}/Test/main.cpp")
