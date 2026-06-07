function(SETUP_UNIT_TEST name)
    add_executable(${name})
    SETUP_COMMON(${name})

      file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.h)

      # If explicit sources were passed to the macro, use them only.
      if (ARGN)
          set(EXPLICIT_SOURCES ${ARGN})
      else()
          file(GLOB_RECURSE EXPLICIT_SOURCES CONFIGURE_DEPENDS *.c)
      endif()

      target_sources(${name}
        PRIVATE
          ${EXPLICIT_SOURCES}
        PRIVATE
          FILE_SET HEADERS
          BASE_DIRS .
          FILES ${HEADERS}
        )

      # Link common libraries required by tests that reference core helpers and DT parsing.
      target_link_libraries(${name} PRIVATE unity libcore libboot_dt)

	add_test(NAME ${name} COMMAND ${name})

	set_tests_properties(${name} PROPERTIES
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
	)
endfunction()
