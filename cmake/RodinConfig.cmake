include(CheckLinkerFlag)
include (CheckCCompilerFlag)
include (CheckCXXCompilerFlag)

# ---- Set colors for displaying messages ------------------------------------
string(ASCII 27 Esc )

set(reset   "${Esc}[m"  )
set(red     "${Esc}[31m")
set(blue    "${Esc}[34m")
set(green   "${Esc}[32m")
set(yellow  "${Esc}[33m")
set(gray    "${Esc}[0;37m")

# ---- Overwrite message function --------------------------------------------
function(message)
  if (NOT MESSAGE_QUIET)
    _message(${ARGN})
  endif()
endfunction()
set (MESSAGE_QUIET OFF)

#! @brief Add options to the compilation of source files.
#
# Adds options to the COMPILE_OPTIONS directory property. These options are
# used when compiling targets from the current directory and below. Each option
# is added only if the compiler supports it.
#
# @param LANG List of languages for which the options will be added
# @param OPTIONS Compiler options
function(rodin_add_compile_options)
  cmake_parse_arguments(RODIN_ADD_COMPILE_OPTIONS "" "" "LANG;OPTIONS" ${ARGN})
  if (NOT RODIN_ADD_COMPILE_OPTIONS_LANG)
    message (FATAL_ERROR "You must specify one or more languages.")
  endif()
  foreach (LANG IN LISTS RODIN_ADD_COMPILE_OPTIONS_LANG)
    if (${LANG} MATCHES CXX)
      foreach (FLAG IN LISTS RODIN_ADD_COMPILE_OPTIONS_OPTIONS)
        string(REGEX REPLACE "[-=+]" "" FLAG_NO_SIGNS ${FLAG})
        check_cxx_compiler_flag(${FLAG} ${LANG}_COMPILER_SUPPORTS_${FLAG_NO_SIGNS})
        if(${LANG}_COMPILER_SUPPORTS_${FLAG_NO_SIGNS})
          add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
        else()
          message(WARNING "${CMAKE_CXX_COMPILER_ID} does not support flag \"${FLAG}\".")
        endif()
      endforeach()
    elseif (${LANG} MATCHES C)
      foreach (FLAG IN LISTS RODIN_ADD_COMPILE_OPTIONS_OPTIONS)
        string(REGEX REPLACE "[-=+]" "" FLAG_NO_SIGNS ${FLAG})
        check_c_compiler_flag(${FLAG} ${LANG}_COMPILER_SUPPORTS_${FLAG_NO_SIGNS})
        if(${LANG}_COMPILER_SUPPORTS_${FLAG_NO_SIGNS})
          add_compile_options($<$<COMPILE_LANGUAGE:C>:${FLAG}>)
        else()
          message(WARNING "${CMAKE_C_COMPILER_ID} does not support flag \"${FLAG}\".")
        endif()
      endforeach()
    elseif (${LANG} MATCHES "")
      # Do nothing
    else ()
      message(FATAL_ERROR "Language ${language} not supported.")
    endif()
  endforeach()
endfunction()

function(rodin_add_link_options)
  cmake_parse_arguments(RODIN_ADD_LINK_OPTIONS "" "" "LANG;OPTIONS" ${ARGN})
  if (NOT RODIN_ADD_LINK_OPTIONS_LANG)
    message (FATAL_ERROR "You must specify one or more languages.")
  endif()
  foreach (LANG IN LISTS RODIN_ADD_LINK_OPTIONS_LANG)
    if (${LANG} MATCHES CXX)
      foreach (FLAG IN LISTS RODIN_ADD_LINK_OPTIONS_OPTIONS)
        string(REGEX REPLACE "[-=+]" "" FLAG_NO_SIGNS ${FLAG})
        check_linker_flag(CXX ${FLAG} ${LANG}_LINKER_SUPPORTS_${FLAG_NO_SIGNS})
        if(${LANG}_LINKER_SUPPORTS_${FLAG_NO_SIGNS})
          add_link_options(${FLAG})
        else()
          message(WARNING "${CMAKE_CXX_COMPILER_ID} does not support link flag \"${FLAG}\".")
        endif()
      endforeach()
    elseif (${LANG} MATCHES C)
      foreach (FLAG IN LISTS RODIN_ADD_LINK_OPTIONS_OPTIONS)
        string(REGEX REPLACE "[-=+]" "" FLAG_NO_SIGNS ${FLAG})
        check_linker_flag(C ${FLAG} ${LANG}_LINKER_SUPPORTS_${FLAG_NO_SIGNS})
        if(${LANG}_LINKER_SUPPORTS_${FLAG_NO_SIGNS})
          add_link_options(${FLAG})
        else()
          message(WARNING "${CMAKE_C_COMPILER_ID} does not support link flag \"${FLAG}\".")
        endif()
      endforeach()
    elseif (${LANG} MATCHES "")
      # Do nothing
    else ()
      message(FATAL_ERROR "Language ${language} not supported.")
    endif()
  endforeach()
endfunction()

function(rodin_add_test)
  cmake_parse_arguments(RODIN_ADD_TEST_ARG "" "NAME" "MODULES;SOURCES;LIBRARIES" ${ARGN})
  list(SORT RODIN_ADD_TEST_ARG_MODULES)
  string(REPLACE ";" "-" RODIN_ADD_TEST_MODULES "${RODIN_ADD_TEST_ARG_MODULES}")
  string(REPLACE ";" " " RODIN_ADD_TEST_LIBRARIES "${RODIN_ADD_TEST_ARG_LIBRARIES}")
  string(REPLACE ";" " " RODIN_ADD_TEST_SOURCES "${RODIN_ADD_TEST_ARG_SOURCES}")
  corrade_add_test(
    RodinTest_${RODIN_ADD_TEST_MODULES}_${RODIN_ADD_TEST_ARG_NAME}
    ${RODIN_ADD_TEST_ARG_SOURCES}
    LIBRARIES ${RODIN_ADD_TEST_ARG_LIBRARIES})
endfunction()
