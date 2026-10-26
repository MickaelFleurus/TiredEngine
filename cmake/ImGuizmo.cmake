function(setup_imguizmo)
    FetchContent_Declare(
        ImGuizmo
        GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo
        GIT_TAG 1.10
        SOURCE_DIR external/ImGuizmo
    )

    add_library(imguizmo STATIC
        external/ImGuizmo/src/ImGuizmo.cpp
        external/ImGuizmo/src/ImVectorEditor.cpp
        external/ImGuizmo/src/ImSequencer.cpp
        external/ImGuizmo/src/ImCurveEdit.cpp
        external/ImGuizmo/src/GraphEditor.cpp   
        external/ImGuizmo/src/ImGradient.cpp   
    )

    target_include_directories(imguizmo PUBLIC
        ${CMAKE_SOURCE_DIR}/external/imgui   # wherever imgui.h lives
    )

    target_link_libraries(imguizmo PUBLIC imgui)
endfunction()
