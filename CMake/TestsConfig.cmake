function(SETUP_UNIT_TEST name)
    add_executable(${name})
    SETUP_COMMON(${name})

	file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.c)
    file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.h)

    target_sources(${name}
      PRIVATE
        ${SOURCES}
		${ARGN}
      PRIVATE
        FILE_SET HEADERS
        BASE_DIRS .
        FILES ${HEADERS}
      )

	target_link_libraries(${name} PRIVATE unity)

	add_test(NAME ${name} COMMAND ${name})

	set_tests_properties(${name} PROPERTIES
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
	)
endfunction()
