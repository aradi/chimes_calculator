cmake_minimum_required(VERSION 3.10)

############################################################
# set the project name
############################################################

project(ChIMES_Calculator
	VERSION 1.0
	DESCRIPTION "Utilities to compute ChIMES interactions"
	LANGUAGES CXX C Fortran)
    

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_definitions(-DDEBUG=${DEBUG})


############################################################
# Executables/libraries for C++ via the serial wrapper
############################################################

# Define an executable target

add_executable(CPP-interface
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/cpp/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/serial_chimes_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp
)

# install(TARGETS myapp DESTINATION bin)    


# Define properties for the executable target

target_include_directories(CPP-interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/cpp)
target_include_directories(CPP-interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/)
target_include_directories(CPP-interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
#target_compile_definitions(CPP-interface PRIVATE DEBUG=0)
target_compile_features   (CPP-interface PRIVATE cxx_std_11)
target_compile_options    (CPP-interface PRIVATE -O3)

install(TARGETS CPP-interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/cpp/ OPTIONAL)



############################################################
# Executables/libraries for C (direct)
############################################################

# Define an executable target

add_executable(C_wrapper-direct_interface
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/c/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/wrapper-C.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp
)

# Define properties for the executable target

target_include_directories(C_wrapper-direct_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/c/)
target_include_directories(C_wrapper-direct_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/)
target_include_directories(C_wrapper-direct_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
target_compile_features   (C_wrapper-direct_interface PRIVATE cxx_std_11)
target_compile_options    (C_wrapper-direct_interface PRIVATE -O3)

install(TARGETS C_wrapper-direct_interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/c/ OPTIONAL)



############################################################
# Executables/libraries for C via the serial wrapper
############################################################

# Define an executable target

add_executable(C_wrapper-serial_interface
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/c/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/wrapper-C.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/serial_chimes_interface.cpp
)

# Define properties for the executable target

target_include_directories(C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/c/)
target_include_directories(C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/)
target_include_directories(C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
target_include_directories(C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/)
#target_compile_definitions(C_wrapper-serial_interface PRIVATE DEBUG=0)
target_compile_features   (C_wrapper-serial_interface PRIVATE cxx_std_11)
target_compile_options    (C_wrapper-serial_interface PRIVATE -O3)

install(TARGETS C_wrapper-serial_interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/c/ OPTIONAL)



############################################################
# Executables/libraries for Python (direct)
############################################################

# Shared: .dylib
# Static: .a
# Module: .so

ADD_LIBRARY(lib-C_wrapper-direct_interface MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/wrapper-C.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp
)

set_target_properties(lib-C_wrapper-direct_interface PROPERTIES PREFIX "") # Prevent CMake from prepending "lib" to libwrapper-C.so

# Define properties for the executable target

target_include_directories(lib-C_wrapper-direct_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
target_include_directories(lib-C_wrapper-direct_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/)
target_compile_features   (lib-C_wrapper-direct_interface PUBLIC cxx_std_11)
target_compile_options    (lib-C_wrapper-direct_interface PUBLIC -O3)
target_compile_options    (lib-C_wrapper-direct_interface PUBLIC -fPIC)

install(TARGETS lib-C_wrapper-direct_interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/python/)



############################################################
# Executables/libraries for Python via the serial wrapper
############################################################

ADD_LIBRARY(lib-C_wrapper-serial_interface MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/wrapper-C.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/serial_chimes_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp    
)

set_target_properties(lib-C_wrapper-serial_interface PROPERTIES PREFIX "") # Prevent CMake from prepending "lib" to libwrapper-C.so

# Define properties for the executable target

target_include_directories(lib-C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/)
target_include_directories(lib-C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/)
target_include_directories(lib-C_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
#target_compile_definitions(lib-C_wrapper-serial_interface PRIVATE DEBUG=0)
target_compile_features   (lib-C_wrapper-serial_interface PUBLIC cxx_std_11)
target_compile_options    (lib-C_wrapper-serial_interface PUBLIC -O3)
target_compile_options    (lib-C_wrapper-serial_interface PUBLIC -fPIC)

install(TARGETS lib-C_wrapper-serial_interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/python/)


############################################################
# Executables/libraries for Fortran (direct)
############################################################

# Define an executable target

add_executable(fortran_wrapper-direct_interface
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/fortran/main.F90
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/wrapper-F.F90
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/wrapper-C.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp
)

# Define properties for the executable target

target_include_directories(fortran_wrapper-direct_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/fortran/)
target_include_directories(fortran_wrapper-direct_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/api/)
target_include_directories(fortran_wrapper-direct_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
target_compile_features   (fortran_wrapper-direct_interface PRIVATE cxx_std_11)
target_compile_options    (fortran_wrapper-direct_interface PRIVATE -O3)
target_compile_options    (fortran_wrapper-direct_interface PRIVATE -fPIC)
target_compile_options    (fortran_wrapper-direct_interface PRIVATE -std=f2003)

install(TARGETS fortran_wrapper-direct_interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/examples/fortran/ OPTIONAL)

############################################################
# Executables/libraries for Fortran via the serial wrapper
############################################################

# Define an executable target

add_executable(fortran_wrapper-serial_interface
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/fortran/main.F90
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/wrapper-F.F90
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/wrapper-C.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/serial_chimes_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/chimesFF.cpp
)

# Define properties for the executable target

target_include_directories(fortran_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/fortran/)
target_include_directories(fortran_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/api/)
target_include_directories(fortran_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/src/)
target_include_directories(fortran_wrapper-serial_interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chimesFF/src/)
#target_compile_definitions(fortran_wrapper-serial_interface PRIVATE DEBUG=0)
target_compile_features   (fortran_wrapper-serial_interface PRIVATE cxx_std_11)
target_compile_options    (fortran_wrapper-serial_interface PRIVATE -O3)
target_compile_options    (fortran_wrapper-serial_interface PRIVATE -fPIC)
target_compile_options    (fortran_wrapper-serial_interface PRIVATE -std=f2003)

#target_compile_options    (fortran_wrapper-serial_interface PRIVATE -cpp)
#if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#    target_compile_options(fortran_wrapper-serial_interface PRIVATE -lc++)
#else()
#    target_compile_options(fortran_wrapper-serial_interface PRIVATE -stdc++)    
#endif()

install(TARGETS fortran_wrapper-serial_interface DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface/examples/fortran/ OPTIONAL)



