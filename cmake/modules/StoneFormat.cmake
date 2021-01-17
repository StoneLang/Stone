set(stone_include_files
	${STONE_SOURCE_DIR}/include/stone/Compile/*.h
	${STONE_SOURCE_DIR}/include/stone/Compile/*.def
	${STONE_SOURCE_DIR}/include/stone/Core/*.h
	${STONE_SOURCE_DIR}/include/stone/Driver/*.h
	${STONE_SOURCE_DIR}/include/stone/Session/*.h
	${STONE_SOURCE_DIR}/include/stone/*.h

)
set(stone_lib_files
	${STONE_SOURCE_DIR}/lib/Compile/*.cpp
	${STONE_SOURCE_DIR}/lib/Core/*.cpp
	${STONE_SOURCE_DIR}/lib/Driver/*.cpp
	${STONE_SOURCE_DIR}/lib/Session/*.cpp
	#${STONE_SOURCE_DIR}/tests/units/Compile/*.cpp
	#${STONE_SOURCE_DIR}/tests/units/Core/*.cpp
	#${STONE_SOURCE_DIR}/tests/units/Driver/*.cpp

)
set(stone_tools_files
	${STONE_SOURCE_DIR}/tools/stone/*.cpp
	${STONE_SOURCE_DIR}/tools/driver/*.cpp

)

set(stone_def_files
	${STONE_SOURCE_DIR}/include/stone/*.def
)
	find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
	add_custom_target(
		stone-fmt
		clang-format
		-i
		-style=google
		${stone_lib_files}
		${stone_include_files}
		${stone_def_files}
		${stone_tools_files}


	)
endif()
