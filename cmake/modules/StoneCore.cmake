
macro(add_stone_subdirectory name)
  add_llvm_subdirectory(STONE TOOL ${name})
endmacro()

macro(add_stone_library name)

	cmake_parse_arguments(ARG "SHARED" "STATIC" "ADDITIONAL_HEADERS" ${ARGN})
	set(srcs)

  if(MSVC_IDE OR XCODE)
    # Add public headers
    file(RELATIVE_PATH lib_path
      ${STONE_SOURCE_DIR}/lib/
      ${CMAKE_CURRENT_SOURCE_DIR}
    )
    if(NOT lib_path MATCHES "^[.][.]")
      file( GLOB_RECURSE headers
        ${STONE_SOURCE_DIR}/include/stone/${lib_path}/*.h
        ${STONE_SOURCE_DIR}/include/stone/${lib_path}/*.def
      )
      set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

      file( GLOB_RECURSE tds
        ${STONE_SOURCE_DIR}/include/stone/${lib_path}/*.td
      )
      source_group("TableGen descriptions" FILES ${tds})
      set_source_files_properties(${tds}} PROPERTIES HEADER_FILE_ONLY ON)

      if(headers OR tds)
        set(srcs ${headers} ${tds})
      endif()
    endif()
  endif(MSVC_IDE OR XCODE)
  if(srcs OR ARG_ADDITIONAL_HEADERS)
    set(srcs
      ADDITIONAL_HEADERS
      ${srcs}
      ${ARG_ADDITIONAL_HEADERS} # It may contain unparsed unknown args.
      )
  endif()
  if(ARG_SHARED)
    set(ARG_ENABLE_SHARED SHARED)
  endif()
  llvm_add_library(${name} ${ARG_ENABLE_SHARED} ${ARG_UNPARSED_ARGUMENTS} ${srcs})

  if(TARGET ${name})
    target_link_libraries(${name} INTERFACE ${LLVM_COMMON_LIBS})

    if (NOT LLVM_INSTALL_TOOLCHAIN_ONLY OR ${name} STREQUAL "libstone")

      if(${name} IN_LIST LLVM_DISTRIBUTION_COMPONENTS OR
          NOT LLVM_DISTRIBUTION_COMPONENTS)
        set(export_to_stonetargets EXPORT StoneTargets)
        set_property(GLOBAL PROPERTY STONE_HAS_EXPORTS True)
      endif()

      install(TARGETS ${name}
        COMPONENT ${name}
        ${export_to_stonetargets}
        LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
        ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX}
        RUNTIME DESTINATION bin)

      if (${ARG_SHARED} AND NOT CMAKE_CONFIGURATION_TYPES)
        add_custom_target(install-${name}
                          DEPENDS ${name}
                          COMMAND "${CMAKE_COMMAND}"
                                  -DCMAKE_INSTALL_COMPONENT=${name}
                                  -P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
      endif()
    endif()
    set_property(GLOBAL APPEND PROPERTY STONE_EXPORTS ${name})
  else()
    # Add empty "phony" target
    add_custom_target(${name})
  endif()

  set_target_properties(${name} PROPERTIES FOLDER "Stone libraries")
	#set_stone_windows_version_resource_properties(${name})
endmacro(add_stone_library)

macro(add_stone_executable name)
  add_llvm_executable( ${name} ${ARGN} )
  set_target_properties(${name} PROPERTIES FOLDER "Stone executables")
	#set_stone_windows_version_resource_properties(${name})
endmacro(add_stone_executable)

macro(add_stone_tool name)
  if (NOT STONE_BUILD_TOOLS)
    set(EXCLUDE_FROM_ALL ON)
  endif()

  add_stone_executable(${name} ${ARGN})

  if (STONE_BUILD_TOOLS)
    if(${name} IN_LIST LLVM_DISTRIBUTION_COMPONENTS OR
        NOT LLVM_DISTRIBUTION_COMPONENTS)
      set(export_to_stonetargets EXPORT StoneTargets)
      set_property(GLOBAL PROPERTY STONE_HAS_EXPORTS True)
    endif()

    install(TARGETS ${name}
      ${export_to_stonetargets}
      RUNTIME DESTINATION bin
      COMPONENT ${name})

    if(NOT CMAKE_CONFIGURATION_TYPES)
      add_custom_target(install-${name}
        DEPENDS ${name}
        COMMAND "${CMAKE_COMMAND}"
        -DCMAKE_INSTALL_COMPONENT=${name}
        -P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
    endif()
    set_property(GLOBAL APPEND PROPERTY STONE_EXPORTS ${name})
  endif()
endmacro()

macro(add_stone_symlink name dest)
  add_llvm_tool_symlink(${name} ${dest} ALWAYS_GENERATE)
  # Always generate install targets
  llvm_install_symlink(${name} ${dest} ALWAYS_GENERATE)
endmacro()

macro(install_stone)

	if (NOT LLVM_INSTALL_TOOLCHAIN_ONLY)
  install(DIRECTORY include/stone 
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.def"
    PATTERN "*.h"
    PATTERN "config.h" EXCLUDE
	)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/stone
    DESTINATION include
    FILES_MATCHING
    PATTERN "CMakeFiles" EXCLUDE
    PATTERN "*.inc"
    PATTERN "*.h"
    )

  install(PROGRAMS utils/bash-autocomplete.sh
    DESTINATION share/stone
    )
endif()


endmacro()
macro(set_stone_version)
  # If STONE_VERSION_* is specified, use it, if not use LLVM_VERSION_*.
if(NOT DEFINED STONE_VERSION_MAJOR)
  set(STONE_VERSION_MAJOR 0)
endif()
if(NOT DEFINED STONE_VERSION_MINOR)
  set(STONE_VERSION_MINOR 1)
endif()
if(NOT DEFINED STONE_VERSION_PATCHLEVEL)
  set(STONE_VERSION_PATCHLEVEL 0)
endif()

set(STONE_VERSION "${STONE_VERSION_MAJOR}.${STONE_VERSION_MINOR}.${STONE_VERSION_PATCHLEVEL}")

endmacro()
