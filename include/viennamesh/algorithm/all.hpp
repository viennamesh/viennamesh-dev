#ifndef VIENNAMESH_ALGORITHM_ALL_HPP
#define VIENNAMESH_ALGORITHM_ALL_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

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
#include "viennamesh/algorithm/map_segments.hpp"
#include "viennamesh/algorithm/merge_meshes.hpp"
#include "viennamesh/algorithm/project_mesh.hpp"
#include "viennamesh/algorithm/make_statistic.hpp"
#include "viennamesh/algorithm/seed_point_segmenting.hpp"
#include "viennamesh/algorithm/remove_duplicate_cells.hpp"
#include "viennamesh/algorithm/make_line_mesh.hpp"

// Triangle algorithms
#ifdef VIENNAMESH_WITH_TRIANGLE
  #include "viennamesh/algorithm/triangle/triangle_make_mesh.hpp"
  #include "viennamesh/algorithm/triangle/triangle_make_hull.hpp"
#endif

// Tetgen algorithms
#ifdef VIENNAMESH_WITH_TETGEN
  #include "viennamesh/algorithm/tetgen/tetgen_make_mesh.hpp"
#endif

// Netgen algorithms
#ifdef VIENNAMESH_WITH_NETGEN
  #include "viennamesh/algorithm/netgen/netgen_make_mesh.hpp"
  #include "viennamesh/algorithm/netgen/netgen_csg_make_mesh.hpp"
  #include "viennamesh/algorithm/netgen/netgen_occ_make_mesh.hpp"
#endif

// VGModeler algorithms
#ifdef VIENNAMESH_WITH_VGMODELER
  #include "viennamesh/algorithm/vgmodeler/vgmodeler_adapt_hull.hpp"
#endif

#endif
