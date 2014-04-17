#ifndef VIENNAMESH_ALGORITHM_ALL_HPP
#define VIENNAMESH_ALGORITHM_ALL_HPP

// IO algorithms
#include "viennamesh/algorithm/io/mesh_reader.hpp"
#include "viennamesh/algorithm/io/string_reader.hpp"
#include "viennamesh/algorithm/io/mesh_writer.hpp"

// ViennaMesh/ViennaGrid algorithms
#include "viennamesh/algorithm/affine_transform.hpp"
#include "viennamesh/algorithm/extract_boundary.hpp"
#include "viennamesh/algorithm/extract_plc_geometry.hpp"
#include "viennamesh/algorithm/hyperplane_clip.hpp"
#include "viennamesh/algorithm/laplace_smooth.hpp"
#include "viennamesh/algorithm/line_coarsening.hpp"
#include "viennamesh/algorithm/line_mesh_generator.hpp"
#include "viennamesh/algorithm/map_segments.hpp"
#include "viennamesh/algorithm/merge_meshes.hpp"
#include "viennamesh/algorithm/project_mesh.hpp"

// Triangle algorithms
#include "viennamesh/algorithm/triangle/triangle_mesh_generator.hpp"
#include "viennamesh/algorithm/triangle/triangle_hull_mesh_generator.hpp"

// Tetgen algorithms
#include "viennamesh/algorithm/tetgen/tetgen_mesh_generator.hpp"

// Netgen algorithms
#include "viennamesh/algorithm/netgen/netgen_csg_mesh_generator.hpp"
#include "viennamesh/algorithm/netgen/netgen_occ_mesh_generator.hpp"

#endif
