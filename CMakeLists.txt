
cmake_minimum_required (VERSION 3.0.2)

Project(mtca4u-deviceaccess)

set(${PROJECT_NAME}_MAJOR_VERSION 00)
set(${PROJECT_NAME}_MINOR_VERSION 31)
set(${PROJECT_NAME}_PATCH_VERSION 00)

set(${PROJECT_NAME}_VERSION
  ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION})
#This library depends on boost, which does not guarantee a binary stable interface when upgrading the version.
#We add a build version to the version number to be able to install
#the same software version of the library linked against different versions
#of boost.
set(${PROJECT_NAME}_BUILDVERSION $ENV{PROJECT_BUILDVERSION}$ENV{mtca4u_deviceaccess_BUILDVERSION})
if( ${PROJECT_NAME}_BUILDVERSION )
  set(${PROJECT_NAME}_SOVERSION "${${PROJECT_NAME}_SOVERSION}${${PROJECT_NAME}_BUILDVERSION}")
endif( ${PROJECT_NAME}_BUILDVERSION )
message("${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_SOVERSION}")
#We need the build name to be part of the libraries full version name
set(${PROJECT_NAME}_FULL_LIBRARY_VERSION ${${PROJECT_NAME}_SOVERSION}.${${PROJECT_NAME}_PATCH_VERSION})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the build type to Release if none is specified
# Force it into Release if "None" is specified (needed to overrule dkpg_buildpackage)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")

# C++ compiler flags needed to compile this project and against this project
set(${PROJECT_NAME}_CXX_FLAGS "${${PROJECT_NAME}_CXX_FLAGS} -DFUSION_MAX_MAP_SIZE=30 -DFUSION_MAX_VECTOR_SIZE=30")

# add C++ 11 support to compiler flags
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(${PROJECT_NAME}_CXX_FLAGS "${${PROJECT_NAME}_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(${PROJECT_NAME}_CXX_FLAGS "${${PROJECT_NAME}_CXX_FLAGS} -std=c++0x -Doverride")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# set C++ compiler flags for compiling this project
set(CMAKE_CXX_FLAGS "${${PROJECT_NAME}_CXX_FLAGS} -Wall -Wextra -Wshadow -pedantic -Wuninitialized")

# use -DCMAKE_BUILD_TYPE=Debug in your cmake command to turn on the coverage option
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 --coverage")

add_definitions(-DDMAP_FILE_PREFIX="${CMAKE_INSTALL_PREFIX}")

# use -DSUPPRESS_AUTO_DOC_BUILD=true to suppress to create the doc with every
# build. The 'make doc' target will still exist
if(SUPPRESS_AUTO_DOC_BUILD)
    unset(DOC_DEPENDENCY)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc)  # otherwise make install might fail
else(SUPPRESS_AUTO_DOC_BUILD)
    set(DOC_DEPENDENCY ALL)
endif(SUPPRESS_AUTO_DOC_BUILD)

FIND_PACKAGE(Boost COMPONENTS thread system chrono REQUIRED)

FIND_PACKAGE(PkgConfig REQUIRED)
set(LIBXML++_VERSION "libxml++-2.6")
PKG_CHECK_MODULES(LibXML++ REQUIRED ${LIBXML++_VERSION})
PKG_CHECK_MODULES(glib REQUIRED glib-2.0)
include_directories(SYSTEM "${LibXML++_INCLUDEDIR}/${LIBXML++_VERSION}")

# The PCIe backend can only be built on Linux, so we define a variable here that
# we can then use in other places.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(HAVE_PCIE_BACKEND 1)
  add_definitions(-DCHIMERATK_HAVE_PCIE_BACKEND)
else()
  set(HAVE_PCIE_BACKEND 0)
endif()

include_directories(${PROJECT_SOURCE_DIR}/exception/include)
include_directories("${PROJECT_SOURCE_DIR}/util/include")
include_directories("${PROJECT_SOURCE_DIR}/fileparsers/include")
include_directories("${PROJECT_SOURCE_DIR}/device_backends/include")
if(HAVE_PCIE_BACKEND)
include_directories("${PROJECT_SOURCE_DIR}/device_backends/pcie/include")
endif()
include_directories("${PROJECT_SOURCE_DIR}/device/include")

configure_file(cmake/DeviceAccessVersion.h.in ${PROJECT_BINARY_DIR}/generated_include/DeviceAccessVersion.h)
include_directories("${PROJECT_BINARY_DIR}/generated_include")

# Only these include directories should be copied for examples.
get_property(inc_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

# Now add other includes; headers added after this point will  not be copied as
# part of the installation
include_directories(${Boost_INCLUDE_DIRS} ${glib_INCLUDE_DIRS} ${LibXML++_INCLUDE_DIRS})
# FIXME: The reason this source directory is added to the include path is
# because of fileparsers/parserUtilities.h. This header was intended to be
# localized to the fileparser source directory; But unit tests need access to
# this header. For now including the source directory to the project include
# path. Moving this header to a dedicated location may be considered later.
include_directories("${PROJECT_SOURCE_DIR}/fileparsers/src")


add_subdirectory ("${PROJECT_SOURCE_DIR}/exception")
add_subdirectory ("${PROJECT_SOURCE_DIR}/util")
add_subdirectory ("${PROJECT_SOURCE_DIR}/fileparsers")
add_subdirectory ("${PROJECT_SOURCE_DIR}/device_backends")
if(HAVE_PCIE_BACKEND)
add_subdirectory ("${PROJECT_SOURCE_DIR}/device_backends/pcie")
endif()
add_subdirectory ("${PROJECT_SOURCE_DIR}/device")
add_subdirectory ("${PROJECT_SOURCE_DIR}/tools")

add_library( ${PROJECT_NAME} SHARED ${SOURCE} ${HEADERS} )
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT_NAME}_FULL_LIBRARY_VERSION} SOVERSION ${${PROJECT_NAME}_SOVERSION})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries( ${PROJECT_NAME}  ${LibXML++_LDFLAGS} -Wl,$<$<PLATFORM_ID:Darwin>:-all_load>$<$<NOT:$<PLATFORM_ID:Darwin>>:--whole-archive> ${libexception} ${libfileparser} ${libutil}  -Wl,$<$<PLATFORM_ID:Darwin>:-noall_load>$<$<NOT:$<PLATFORM_ID:Darwin>>:--no-whole-archive> ${Boost_LIBRARIES} ${LibXML++_LIBRARIES} ${glib_LIBRARIES} pthread dl)

#The make coverage command is only available in debug mode
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    configure_file(cmake/Makefile.coverage.in
      ${PROJECT_BINARY_DIR}/Makefile.coverage @ONLY)
    add_custom_target(coverage
      make -f Makefile.coverage
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating test coverage documentation" VERBATIM
      )
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include/mtca4u)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include/ChimeraTK)

foreach(dir ${inc_dirs} )
    file(GLOB headerFiles  "${dir}/*.h")
    foreach(headerFile ${headerFiles} )
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${headerFile} ${PROJECT_BINARY_DIR}/include/ChimeraTK)
    endforeach(headerFile)
endforeach(dir)

include_directories("${PROJECT_BINARY_DIR}/include")

add_subdirectory ("${PROJECT_SOURCE_DIR}/plugin_example")

#change the install prefix to the source directory in case the user has not specified a destination
#i. e. CMAKE_INSTALL_PREFIX is not set manually
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Install directory
                 prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#Install the library and the executables
install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib )

#all include files go into include/PROJECT_NAME
install(DIRECTORY ${PROJECT_BINARY_DIR}/include/ChimeraTK/ DESTINATION include/ChimeraTK)
#The compatibility files
file(COPY ${PROJECT_SOURCE_DIR}/mtca4u_compatibility_headers/ DESTINATION ${PROJECT_BINARY_DIR}/include/mtca4u)
install(DIRECTORY ${PROJECT_BINARY_DIR}/include/mtca4u/ DESTINATION include/mtca4u)

#Create the config files by filling the correct variables into the template (*.cmake.in).
#All variables @VARIABLE@ are replaced with the current values, but they have to be set first....
#Only the reference header is needed. The other variables are for dependencies of this library,
#but there are none.
set(${PROJECT_NAME}_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include ${Boost_INCLUDE_DIRS}")
set(${PROJECT_NAME}_LIBRARIES "${Boost_LIBRARIES}")
set(${PROJECT_NAME}_LIBRARY_DIRS "${CMAKE_INSTALL_PREFIX}/lib ${Boost_LIBRARY_DIRS}")
# already set: ${PROJECT_NAME}_CXX_FLAGS
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
set(${PROJECT_NAME}_LINKER_FLAGS "-Wl,-rpath,${CMAKE_INSTALL_PREFIX}/lib")
else()
set(${PROJECT_NAME}_LINKER_FLAGS "-Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib,--enable-new-dtags")
endif()
set(${PROJECT_NAME}_MEXFLAGS "-I${CMAKE_INSTALL_PREFIX}/include -L${CMAKE_INSTALL_PREFIX}/lib -l${PROJECT_NAME}")
include(${CMAKE_SOURCE_DIR}/cmake/create_cmake_config_files.cmake)

# debian packaging
include(${CMAKE_SOURCE_DIR}/cmake/prepare_debian_package.cmake)
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doc ${DOC_DEPENDENCY}
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  #note the / after ${CMAKE_BINARY_DIR}/doc/. This causes the directory to be renamed to the destination, not copied into
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/ DESTINATION share/doc/${PROJECT_NAME}-${${PROJECT_NAME}_SOVERSION}
          COMPONENT doc)
else(DOXYGEN_FOUND)
  message("Doxygen not found, documentation cannot be build.")
endif(DOXYGEN_FOUND)

# Keep the testing section at the end. It will include boost test, which will modify the Boost_LIBRARIES variable.
# We don't want to link client applications to the boost testing library. This might give trouble.
enable_testing()
add_subdirectory ("${PROJECT_SOURCE_DIR}/tests")
add_subdirectory ("${PROJECT_SOURCE_DIR}/examples")

