project(Glasso)

####################################################################################################
# Build executable
####################################################################################################

enable_language(Fortran)

add_library(lglasso STATIC glasso.f90)
set_target_properties(lglasso PROPERTIES LINKER_LANGUAGE Fortran)

add_executable(glasso main.cpp glasso.h)
target_link_libraries(glasso gtcore lglasso)

# We need to link libgfortran when using the
# gfortran compiler.
if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(glasso gfortran)
endif()

set_target_properties(glasso PROPERTIES
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
)

GT2_COMPILE_FLAGS(glasso)

####################################################################################################
# Build executable
####################################################################################################

install(TARGETS glasso
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
