include(StoneCore)

add_custom_target(StoneUnitTests)

set_target_properties(StoneUnitTests PROPERTIES FOLDER "Tests")

function(add_stone_unittest test_dirname)
	add_unittest(StoneUnitTests ${test_dirname} ${ARGN})
endfunction()
