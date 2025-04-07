file(
    GLOB
    EXAMPLE_SOURCE_FILES
    *.cpp
)

foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE_FILE})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})
endforeach()
