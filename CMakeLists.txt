#originaly written by Florian Uhlig
#example of CMake script for compiling and linking root classse.  
#To create an Xcode project:
#   cmake -G Xcode ./ 
#extracted from FairRoot project (fairroot.gsi.de) by M. Al-Turany (m.al-turany@gsi.de)



# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

# Set name of our project to "Event". Has to be done
# after check of cmake version since this is a new feature                  
project(FGParameterConversion)



# find and add boost
include(FindBoost)
find_package(Boost 1.36.0 REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})



# Load some basic macros which are needed later on
include(FindROOT.cmake)

#Check the compiler and set the compile and link flags
set(CMAKE_BUILD_TYPE Debug)

#set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

if (CMAKE_SYSTEM_NAME MATCHES Darwin)
   EXEC_PROGRAM("sw_vers -productVersion | cut -d . -f 1-2" OUTPUT_VARIABLE MAC_OS_VERSION)
   MESSAGE("--- Found a Mac OS X System ${MAC_OS_VERSION}")
   if (CMAKE_COMPILER_IS_GNUCXX)
      MESSAGE("--- Found GNU compiler collection")

      STRING(COMPARE EQUAL "10.5" "${MAC_OS_VERSION}" MAC_OS_10_5)
      IF(MAC_OS_10_5)
        SET(CMAKE_CXX_FLAGS "-m64")
        SET(CMAKE_Fortran_FLAGS "-m64")
      ENDIF(MAC_OS_10_5)

      SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")
      SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")

      # Select flags.
      set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
      set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
      set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
      set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common")
      set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
      set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
      set(CMAKE_C_FLAGS_RELEASE          "-O2")
      set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
      set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
      set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

      
      
      endif (CMAKE_COMPILER_IS_GNUCXX)

endif (CMAKE_SYSTEM_NAME MATCHES Darwin) 





############### build the executable #####################

#get all src files and add them to project
file(GLOB_RECURSE source_files src/*)

add_executable(Convert ${source_files})



#Configuration de l'édition de liens

target_link_libraries(
        Convert
        ${Boost_LIBRARIES}
        ${ROOT_LIBRARIES}
)



# test end



############### build the library #####################
#add_library(Event SHARED ${Event_SRCS})
#target_link_libraries(Event ${ROOT_LIBRARIES})
#set_target_properties(Event PROPERTIES ${Event_LIBRARY_PROPERTIES})

############### install the library ###################
#install(TARGETS Event DESTINATION ${CMAKE_BINARY_DIR}/lib)



