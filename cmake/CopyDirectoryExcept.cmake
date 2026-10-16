if(NOT DEFINED SOURCE_DIR OR NOT DEFINED DESTINATION_DIR OR
   NOT DEFINED EXCLUDED_RELATIVE_DIR)
    message(FATAL_ERROR "SOURCE_DIR, DESTINATION_DIR, and EXCLUDED_RELATIVE_DIR are required")
endif()

file(GLOB_RECURSE asset_files LIST_DIRECTORIES false "${SOURCE_DIR}/*")

foreach(asset_file IN LISTS asset_files)
    file(RELATIVE_PATH relative_path "${SOURCE_DIR}" "${asset_file}")
    string(FIND "${relative_path}" "${EXCLUDED_RELATIVE_DIR}/" excluded_prefix)
    if(excluded_prefix EQUAL 0)
        continue()
    endif()

    get_filename_component(relative_directory "${relative_path}" DIRECTORY)
    file(MAKE_DIRECTORY "${DESTINATION_DIR}/${relative_directory}")
    file(COPY_FILE "${asset_file}"
        "${DESTINATION_DIR}/${relative_path}"
        ONLY_IF_DIFFERENT
    )

    if(DEFINED SOURCE_MAP_FILE)
        file(TO_CMAKE_PATH "${relative_path}" normalized_relative_path)
        file(TO_CMAKE_PATH "${asset_file}" normalized_source_path)
        file(APPEND "${SOURCE_MAP_FILE}"
            "${normalized_relative_path}|${normalized_source_path}\n")
    endif()
endforeach()
