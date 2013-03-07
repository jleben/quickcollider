
if(NOT SNDFILE_LIBRARY OR NOT SNDFILE_INCLUDE_DIR)
    find_library(SNDFILE_LIBRARY NAMES sndfile sndfile-1)
    find_path(SNDFILE_INCLUDE_DIR sndfile.h)
    message( STATUS "sndfile library: ${SNDFILE_LIBRARY}" )
    message( STATUS "sndfile include dir: ${SNDFILE_INCLUDE_DIR}" )
endif()

if(NOT LO_LIBRARY OR NOT LO_INCLUDE_DIR)
    find_library(LO_LIBRARY NAMES lo)
    find_path(LO_INCLUDE_DIR lo/lo.h)
    message( STATUS "lo library: ${LO_LIBRARY}" )
    message( STATUS "lo include dir: ${LO_INCLUDE_DIR}" )
endif()

if(
    SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR
    AND LO_LIBRARY AND LO_INCLUDE_DIR
)
    set(Dependencies_FOUND TRUE)
else()
    set(Dependencies_FOUND FALSE)
endif()
