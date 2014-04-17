add_executable(convert_mesh src/convert_mesh.cpp)
target_link_libraries(convert_mesh viennamesh)

add_executable(generate_triangle src/generate_triangle.cpp)
target_link_libraries(generate_triangle viennamesh)

add_executable(generate_tetgen src/generate_tetgen.cpp)
target_link_libraries(generate_tetgen viennamesh)

add_executable(extract_boundary src/extract_boundary.cpp)
target_link_libraries(extract_boundary viennamesh)

add_executable(transform_mesh src/transform_mesh.cpp)
target_link_libraries(transform_mesh viennamesh)

add_executable(project_mesh src/project_mesh.cpp)
target_link_libraries(project_mesh viennamesh)

add_executable(coarsen_linemesh src/coarsen_linemesh.cpp)
target_link_libraries(coarsen_linemesh viennamesh)

add_executable(hyperplane_clip src/hyperplane_clip.cpp)
target_link_libraries(hyperplane_clip viennamesh)

add_executable(merge_meshes src/merge_meshes.cpp)
target_link_libraries(merge_meshes viennamesh)

add_executable(map_segments src/map_segments.cpp)
target_link_libraries(map_segments viennamesh)

add_executable(vmesh src/vmesh.cpp)
target_link_libraries(vmesh viennamesh)
