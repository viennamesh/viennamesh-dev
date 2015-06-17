#include <mystdlib.h>

#include <myadt.hpp>
#include <linalg.hpp>
#include <gprim.hpp>

#include <meshing.hpp>

#include "gsse/domain.hpp"
#include "gsse_orientor.hpp"
#include "gsse/math/geometric_barycenter.hpp"


#include "viennagridpp/mesh/mesh.hpp"
#include "viennagridpp/mesh/region.hpp"
#include "viennagridpp/range.hpp"
// #include "viennagrid/config/default_configs.hpp"
#include "viennagridpp/algorithm/cross_prod.hpp"
// #include "viennadata/api.hpp"
// #include "viennagrid/config/simplex.hpp"

#include "stlgeom.hpp"

// #define DEBUGALL
// #define DEBUGFULL

namespace vgmnetgen
{


STLTopology :: STLTopology()
  : pointtol(0), trias(), topedges(), points(), ht_topedges(NULL), ht_topedges2(NULL),
    neighbourtrigs(), trigsperpoint(), searchtree(NULL), pointtree(NULL), orientation_ok(1), topology_ok(1)
{
  ;
}

STLTopology :: ~STLTopology()
{
  ;
}

STLGeometry *  STLTopology :: LoadBinary (istream & ist)
{
  STLGeometry * geom = new STLGeometry();
  ARRAY<STLReadTriangle> readtrigs;

  PrintMessage(1,"Read STL binary file");

  if (sizeof(int) != 4 || sizeof(float) != 4)
    {
      PrintWarning("for stl-binary compatibility only use 32 bit compilation!!!");
    }

  //specific settings for stl-binary format
  const int namelen = 80; //length of name of header in file
  const int nospaces = 2; //number of spaces after a triangle

  //read header: name
  char buf[namelen+1];
  FIOReadStringE(ist,buf,namelen);
  PrintMessage(5,"header = ",buf);

  //Read Number of facets
  int nofacets;
  FIOReadInt(ist,nofacets);
  PrintMessage(5,"NO facets = ",nofacets);

  Point<3> pts[3];
  Vec<3> normal;

  int cntface, j;
  int vertex = 0;
  float f;
  char spaces[nospaces+1];

  for (cntface = 0; cntface < nofacets; cntface++)
    {
      if (cntface % 10000 == 9999) { PrintDot(); }

      FIOReadFloat(ist,f); normal(0) = f;
      FIOReadFloat(ist,f); normal(1) = f;
      FIOReadFloat(ist,f); normal(2) = f;

      for (j = 0; j < 3; j++)
	{
	  FIOReadFloat(ist,f); pts[j](0) = f;
	  FIOReadFloat(ist,f); pts[j](1) = f;
	  FIOReadFloat(ist,f); pts[j](2) = f;
	}

      readtrigs.Append (STLReadTriangle (pts, normal));
      FIOReadString(ist,spaces,nospaces);
    }


  geom->InitSTLGeometry(readtrigs);

  return geom;
}


void STLTopology :: SaveBinary (const char* filename, const char* aname)
{
  ofstream ost(filename);
  PrintFnStart("Write STL binary file '",filename,"'");

  if (sizeof(int) != 4 || sizeof(float) != 4)
    {PrintWarning("for stl-binary compatibility only use 32 bit compilation!!!");}

  //specific settings for stl-binary format
  const int namelen = 80; //length of name of header in file
  const int nospaces = 2; //number of spaces after a triangle

  //write header: aname
  int i, j;
  char buf[namelen+1];
  int strend = 0;
  for(i = 0; i <= namelen; i++)
    {
      if (aname[i] == 0) {strend = 1;}
      if (!strend) {buf[i] = aname[i];}
      else {buf[i] = 0;}
    }

  FIOWriteString(ost,buf,namelen);
  PrintMessage(5,"header = ",buf);

  //RWrite Number of facets
  int nofacets = GetNT();
  FIOWriteInt(ost,nofacets);
  PrintMessage(5,"NO facets = ", nofacets);

  float f;
  char spaces[nospaces+1];
  for (i = 0; i < nospaces; i++) {spaces[i] = ' ';}
  spaces[nospaces] = 0;

  for (i = 1; i <= GetNT(); i++)
    {
      const STLTriangle & t = GetTriangle(i);

      const Vec<3> & n = t.Normal();
      f = n(0); FIOWriteFloat(ost,f);
      f = n(1); FIOWriteFloat(ost,f);
      f = n(2); FIOWriteFloat(ost,f);

      for (j = 1; j <= 3; j++)
	{
	  const Point3d p = GetPoint(t.PNum(j));

	  f = p.X(); FIOWriteFloat(ost,f);
	  f = p.Y(); FIOWriteFloat(ost,f);
	  f = p.Z(); FIOWriteFloat(ost,f);
	}
      FIOWriteString(ost,spaces,nospaces);
    }
  PrintMessage(5,"done");
}


void STLTopology :: SaveSTLE (const char* filename)
{
  ofstream outf (filename);
  int i, j;

  outf << GetNT() << endl;
  for (i = 1; i <= GetNT(); i++)
    {
      const STLTriangle & t = GetTriangle(i);
      for (j = 1; j <= 3; j++)
	{
	  const Point3d p = GetPoint(t.PNum(j));
	  outf << p.X() << " " << p.Y() << " " << p.Z() << endl;
	}
    }


  int ned = 0;
  for (i = 1; i <= GetNTE(); i++)
    {
      if (GetTopEdge (i).GetStatus() == ED_CONFIRMED)
	ned++;
    }

  outf << ned << endl;

  for (i = 1; i <= GetNTE(); i++)
    {
      const STLTopEdge & edge = GetTopEdge (i);
      if (edge.GetStatus() == ED_CONFIRMED)
	for (j = 1; j <= 2; j++)
	  {
	    const Point3d p = GetPoint(edge.PNum(j));
	    outf << p.X() << " " << p.Y() << " " << p.Z() << endl;
	  }
    }
}



STLGeometry *  STLTopology :: LoadNaomi (istream & ist)
{
  int i;
  STLGeometry * geom = new STLGeometry();
  ARRAY<STLReadTriangle> readtrigs;

  PrintFnStart("read NAOMI file format");

  char buf[100];
  Vec<3> normal;

  int cntface = 0;
  int cntvertex = 0;
  double px, py, pz;


  int noface, novertex;
  ARRAY<Point<3> > readpoints;

  ist >> buf;
  if (strcmp (buf, "NODES") == 0)
    {
      ist >> novertex;
      PrintMessage(5,"nuber of vertices = ", novertex);
      for (i = 0; i < novertex; i++)
	{
	  ist >> px;
	  ist >> py;
	  ist >> pz;
	  readpoints.Append(Point<3> (px,py,pz));
	}
    }
  else
    {
      PrintFileError("no node information");
    }


  ist >> buf;
  if (strcmp (buf, "2D_EDGES") == 0)
    {
      ist >> noface;
      PrintMessage(5,"number of faces=",noface);
      int dummy, p1, p2, p3;
      Point<3> pts[3];

      for (i = 0; i < noface; i++)
	{
	  ist >> dummy; //2
	  ist >> dummy; //1
	  ist >> p1;
	  ist >> p2;
	  ist >> p3;
	  ist >> dummy; //0

	  pts[0] = readpoints.Get(p1);
	  pts[1] = readpoints.Get(p2);
	  pts[2] = readpoints.Get(p3);

	  normal = Cross (pts[1]-pts[0], pts[2]-pts[0]) . Normalize();

	  readtrigs.Append (STLReadTriangle (pts, normal));

	}
      PrintMessage(5,"read ", readtrigs.Size(), " triangles");
    }
  else
    {
      PrintMessage(5,"read='",buf,"'\n");
      PrintFileError("ERROR: no Triangle information");
    }

  geom->InitSTLGeometry(readtrigs);

  return geom;
}

void STLTopology :: Save (const char* filename)
{
  PrintFnStart("Write stl-file '",filename, "'");

  ofstream fout(filename);
  fout << "solid\n";

  char buf1[50];
  char buf2[50];
  char buf3[50];

  int i, j;
  for (i = 1; i <= GetNT(); i++)
    {
      const STLTriangle & t = GetTriangle(i);

      fout << "facet normal ";
      const Vec3d& n = GetTriangle(i).Normal();

      sprintf(buf1,"%1.9g",n.X());
      sprintf(buf2,"%1.9g",n.Y());
      sprintf(buf3,"%1.9g",n.Z());

      fout << buf1 << " " << buf2 << " " << buf3 << "\n";
      fout << "outer loop\n";

      for (j = 1; j <= 3; j++)
	{
	  const Point3d p = GetPoint(t.PNum(j));

	  sprintf(buf1,"%1.9g",p.X());
	  sprintf(buf2,"%1.9g",p.Y());
	  sprintf(buf3,"%1.9g",p.Z());

	  fout << "vertex " << buf1 << " " << buf2 << " " << buf3 << "\n";
	}

      fout << "endloop\n";
      fout << "endfacet\n";
    }
  fout << "endsolid\n";


  // write also NETGEN surface mesh:
  ofstream fout2("geom.surf");
  fout2 << "surfacemesh" << endl;
  fout2 << GetNP() << endl;
  for (i = 1; i <= GetNP(); i++)
    {
      for (j = 0; j < 3; j++)
	{
	  fout2.width(8);
	  fout2 << GetPoint(i)(j);
	}

      fout2 << endl;
    }

  fout2 << GetNT() << endl;
  for (i = 1; i <= GetNT(); i++)
    {
      const STLTriangle & t = GetTriangle(i);
      for (j = 1; j <= 3; j++)
	{
	  fout2.width(8);
	  fout2 << t.PNum(j);
	}
      fout2 << endl;
    }
}


STLGeometry *  STLTopology ::Load (istream & ist)
{
  int i;
  STLGeometry * geom = new STLGeometry();

  ARRAY<STLReadTriangle> readtrigs;

  char buf[100];
  Point<3> pts[3];
  Vec<3> normal;

  int cntface = 0;
  int vertex = 0;
  bool badnormals = 0;

  while (ist.good())
    {
      ist >> buf;

      int n = strlen (buf);
      for (i = 0; i < n; i++)
	buf[i] = tolower (buf[i]);

      if (strcmp (buf, "facet") == 0)
	{
	  cntface++;
	}

      if (strcmp (buf, "normal") == 0)
	{
	  ist >> normal(0)
	      >> normal(1)
	      >> normal(2);
	  normal.Normalize();
	}

      if (strcmp (buf, "vertex") == 0)
	{
	  ist >> pts[vertex](0)
	      >> pts[vertex](1)
	      >> pts[vertex](2);

	  vertex++;

	  if (vertex == 3)
	    {
	      if (normal.Length() <= 1e-5)

		{
		  normal = Cross (pts[1]-pts[0], pts[2]-pts[0]);
		  normal.Normalize();
		}

	      else

		{
		  Vec<3> hnormal;
		  hnormal = Cross (pts[1]-pts[0], pts[2]-pts[0]);
		  hnormal.Normalize();

		  if (normal * hnormal < 0.5)
		    {
		      badnormals = 1;
		    }
		}

	      vertex = 0;

	      if ( (Dist2 (pts[0], pts[1]) > 1e-16) &&
		   (Dist2 (pts[0], pts[2]) > 1e-16) &&
		   (Dist2 (pts[1], pts[2]) > 1e-16) )

		readtrigs.Append (STLReadTriangle (pts, normal));
	    }
	}
    }

#ifdef DEBUGALL
  if (badnormals)
    {
      PrintWarning("File has normal vectors which differ extremly from geometry->correct with stldoctor!!!");
    }
#endif

  geom->InitSTLGeometry(readtrigs);
  return geom;
}



STLGeometry *  STLTopology ::Load (const char* filename)
{
  int i;
  STLGeometry * geom = new STLGeometry();

  ifstream ist(filename);

  ARRAY<STLReadTriangle> readtrigs;

  char buf[100];
  Point<3> pts[3];
  Vec<3> normal;

  int cntface = 0;
  int vertex = 0;
  bool badnormals = 0;

  while (ist.good())
    {
      ist >> buf;

      int n = strlen (buf);
      for (i = 0; i < n; i++)
	buf[i] = tolower (buf[i]);

      if (strcmp (buf, "facet") == 0)
	{
	  cntface++;
	}

      if (strcmp (buf, "normal") == 0)
	{
	  ist >> normal(0)
	      >> normal(1)
	      >> normal(2);
	  normal.Normalize();
	}

      if (strcmp (buf, "vertex") == 0)
	{
	  ist >> pts[vertex](0)
	      >> pts[vertex](1)
	      >> pts[vertex](2);

	  vertex++;

	  if (vertex == 3)
	    {
	      if (normal.Length() <= 1e-5)

		{
		  normal = Cross (pts[1]-pts[0], pts[2]-pts[0]);
		  normal.Normalize();
		}

	      else

		{
		  Vec<3> hnormal;
		  hnormal = Cross (pts[1]-pts[0], pts[2]-pts[0]);
		  hnormal.Normalize();

		  if (normal * hnormal < 0.5)
		    {
		      badnormals = 1;
		    }
		}

	      vertex = 0;

	      if ( (Dist2 (pts[0], pts[1]) > 1e-16) &&
		   (Dist2 (pts[0], pts[2]) > 1e-16) &&
		   (Dist2 (pts[1], pts[2]) > 1e-16) )

		readtrigs.Append (STLReadTriangle (pts, normal));
	    }
	}
    }

  geom->InitSTLGeometry(filename);
  return geom;
}







void STLTopology :: InitSTLGeometry( viennagrid::mesh_t const & vgridmesh )
{
//    typedef viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type     DomainType;
   typedef viennagrid::mesh_t MeshType;
   typedef viennagrid::result_of::region<MeshType>::type RegionType;
//    typedef viennagrid::triangular_hull_3d_segmentation SegmentationType;
//    typedef viennagrid::triangular_hull_3d_segment_handle SegmentHandleType;

//    typedef DomainType::config_type                     DomainConfiguration;

//    typedef DomainConfiguration::numeric_type            CoordType;
   typedef double            CoordType;
//    typedef DomainConfiguration::cell_tag                CellTag;
   typedef viennagrid::triangle_tag                CellTag;

//    typedef DomainType::segment_type                                                                  SegmentType;
   typedef viennagrid::result_of::element<MeshType>::type      ElementType;
   typedef viennagrid::result_of::const_element_range<MeshType>::type                               ConstElementRangeType;
   typedef viennagrid::result_of::iterator<ConstElementRangeType>::type                               ConstElementIteratorType;
   typedef viennagrid::result_of::const_element_range<RegionType>::type                               ConstRegionElementRangeType;
   typedef viennagrid::result_of::iterator<ConstRegionElementRangeType>::type                         ConstRegionElementIteratorType;
   typedef viennagrid::result_of::const_element_range<ElementType>::type                              ConstBoundaryRangeType;
   typedef viennagrid::result_of::iterator<ConstBoundaryRangeType>::type                              ConstBoundaryIteratorType;
   typedef viennagrid::result_of::point<MeshType>::type                               PointType;

   static const int DIMT = 3;
   static const int CELLSIZE = DIMT+1;

  int i, j, k;

   // ----------------------------------------------------------------------
   //
   // *** transfer viennagrid domain to gsse01 domain
   //
   // ----------------------------------------------------------------------
   typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_2t;
   typedef gsse::get_domain<unstructured_topology_2t, double, double,3>::type    domain_32t;
   typedef gsse::domain_traits<domain_32t>::cell_iterator		                  cell_iterator;
   typedef gsse::domain_traits<domain_32t>::vertex_handle                        vertex_handle;
   typedef gsse::domain_traits<domain_32t>::cell_2_vertex_mapping                cell_2_vertex_mapping;
   typedef gsse::domain_traits<domain_32t>::segment_iterator	                  segment_iterator;
   typedef gsse::domain_traits<domain_32t>::point_t                              point_t;
   typedef gsse::domain_traits<domain_32t>::vertex_on_cell_iterator              vertex_on_cell_iterator;
   typedef gsse::domain_traits<domain_32t>::edge_on_cell_iterator                edge_on_cell_iterator;
   typedef gsse::domain_traits<domain_32t>::cell_on_edge_iterator                cell_on_edge_iterator;
   typedef gsse::domain_traits<domain_32t>::storage_type                         storage_type;

   domain_32t domain;

//   typedef gsse::detail_topology::unstructured<2>                                   gsse_unstructured_topology_2t;
//   typedef gsse::get_domain<gsse_unstructured_topology_2t, double, double,3>::type  gsse_DomainType;
//   typedef ads::FixedArray<3, CoordType>                                            gsse_point_type;
//   typedef gsse::domain_traits<gsse_DomainType>::segment_type                      gsse_segment_type;
//   typedef gsse::domain_traits<gsse_DomainType>::segment_iterator                  gsse_segment_iterator;
//   typedef gsse::segment_traits<gsse_segment_type>::cell_2_vertex_mapping            cell_2_vertex_mapping;

//   gsse_DomainType domain;

   // transfer geometry
   //
   ConstElementRangeType vertices(vgridmesh, 0);

   for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
   {
      PointType point = viennagrid::get_point( vgridmesh, *vit );
      point_t gssepoint;
      gssepoint[0] = point[0];
      gssepoint[1] = point[1];
      gssepoint[2] = point[2];
      domain.fast_point_insert(gssepoint);
   }

   std::size_t cell_counter = 0;
//    for (std::size_t si = 0; si < vgriddomain.segments().size(); ++si)

//    typedef viennagrid::config::triangular_3d_segmentation::segment_ids_container_type segment_ids_container_type;
//    segment_ids_container_type const & used_segments = vgridsegmentation.segments();
//    viennamesh::segment_id_container_type const & used_segments = viennamesh::segments( vgriddomain );

//     viennagrid::result_of::default_point_accessor<MeshType>::type point_accessor = viennagrid::default_point_accessor( vgridmesh );


    typedef viennagrid::result_of::region_range<MeshType>::type                              RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type                           RegionItertorType;



//    for (std::size_t si = 0; si < num_segments; ++si)
   long gsse_domain_id = 0;

   RegionRangeType regions(vgridmesh);
   for (RegionItertorType rit = regions.begin(); rit != regions.end(); ++rit, ++gsse_domain_id)
   {
      RegionType current_region = *rit;
       this->segment_id_map[gsse_domain_id] = current_region.id();



//        viennagrid::config::triangular_3d_segmentation::segment_id_type current_segment_id = *it;
      // transfer segment
      //
//       SegmentType & seg = vgriddomain.segments()[si];
      segment_iterator gsse_segit = domain.add_segment();
      (*gsse_segit).set_cell_index_offset(cell_counter);
//       cell_counter += viennagrid::elements<CellTag>( vgriddomain ).size();

      // transfer topology
      //
//       CellContainer cells = viennagrid::elements<CellTag>(vgridmesh);

      ConstRegionElementRangeType cells( current_region, viennagrid::cell_dimension(vgridmesh) );
      for (ConstRegionElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
          PointType p0 = viennagrid::get_point(*cit,0);
          PointType p1 = viennagrid::get_point(*cit,1);
          PointType p2 = viennagrid::get_point(*cit,2);

//           point_accessor( viennagrid::elements<viennagrid::vertex_tag>(*cit)[0] );
//           PointType p1 = point_accessor( viennagrid::elements<viennagrid::vertex_tag>(*cit)[1] );
//           PointType p2 = point_accessor( viennagrid::elements<viennagrid::vertex_tag>(*cit)[2] );

//           typedef viennagrid::config::triangular_3d_segmentation::element_segment_info_type element_segment_info_type;
//           element_segment_info_type const & seg_def = vgridsegmentation.segment_info( *cit );
//           viennamesh::face_segment_definition_type const & seg_def = viennamesh::face_segments( *cit );

//           if ((current_segment_id == seg_def.positive_orientation_segment_id) || (current_segment_id == seg_def.negative_orientation_segment_id))
//           if ( viennagrid::is_in_segment( current_segment, *cit ) )
/*

          viennamesh::face_segment_definition_type::const_iterator sdit = seg_def.find( current_segment_id );
          if (sdit != seg_def.end())*/
            {
//                 std::cout << "!!! Adding Triangle" << std::endl;
//                 std::cout << "   " << p0 << std::endl;
//                 std::cout << "   " << p1 << std::endl;
//                 std::cout << "   " << p2 << std::endl;
//                 std::cout << "      " << viennagrid::cross_prod(p1-p0,p2-p0) << std::endl;
//                 std::cout << "      segment? " << sdit->first << std::endl;
//                 std::cout << "      orient good? " << sdit->second << std::endl;

                boost::array<std::size_t,CELLSIZE>     cell;
                std::size_t vi = 0;
//                 VertexOnCellContainer vertices_for_cell = viennagrid::elements<viennagrid::vertex_tag>(*cit);


                ConstBoundaryRangeType vertices_on_cell(*cit, 0);
                for (ConstBoundaryIteratorType bvit = vertices_on_cell.begin(); bvit != vertices_on_cell.end(); ++bvit)
                {
                  cell[vi++] = (*bvit).id();
                }


//                 for (VertexOnCellIterator vocit = vertices_for_cell.begin();
//                     vocit != vertices_for_cell.end();
//                     ++vocit)
//                 {
//         //             std::cout << vocit->id().get() << std::endl;
//                     cell[vi++] = vocit->id().get();
//                 }

                int region0;// = viennagrid::regions(vgridmesh, *cit)[0].id();
                int region1;// = viennagrid::regions(vgridmesh, *cit)[1].id();
                if (region1 < region0)
                  std::swap(region0, region1);


//                 bool const * orientation = viennagrid::segment_element_info( current_segment, *cit );
//                 bool current_orientation = true;
//                 if (orientation)
//                   current_orientation = *orientation;

                if ( current_region.id() == region0 )
                {
                    (*gsse_segit).add_cell_2(cell_2_vertex_mapping(cell[0], cell[1], cell[2]));
                    domain.add_vertex(cell[0], gsse_segit);
                    domain.add_vertex(cell[1], gsse_segit);
                    domain.add_vertex(cell[2], gsse_segit);
                }
                else
                {
                    (*gsse_segit).add_cell_2(cell_2_vertex_mapping(cell[0], cell[2], cell[1]));
                    domain.add_vertex(cell[0], gsse_segit);
                    domain.add_vertex(cell[2], gsse_segit);
                    domain.add_vertex(cell[1], gsse_segit);
                }

                ++cell_counter;
            }
        }

   }

//    std::cout << "BLA" << std::endl;

   //domain.write_file("gsse_hull.gau32");

      int check_nm = 0;
      if(check_nm)
      {

      // [INFO] checking for non-manifold edges
      //
      typedef boost::array<long,2> edge_t;
      std::vector<edge_t> non_manifold_edges;
      std::vector<long> non_oriented_triangles;

      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ;eocit.valid(); ++eocit)
               {
                  int count = 0;
                  edge_t new_edge;
                  cell_on_edge_iterator coeit(*eocit);
                  for (; coeit.valid(); ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  if (count != 2)
                  {
                     new_edge[0] = (*eocit).handle1();
                     new_edge[1] = (*eocit).handle2();

                     non_manifold_edges.push_back(new_edge);

#ifdef DEBUGFULL
                     std::cout << ".. non_manifold_edge found: " << new_edge[0] << " " << new_edge[1]
                               << " .. count: " << count <<  std::endl;
#endif
                  }
                  else
                  {
#ifdef DEBUGFULL
                     std::cout << ".. manifold_edge: " << (*eocit).handle1() << " " << (*eocit).handle2()
                               << " .. count: " << count <<  std::endl;
#endif
                  }
               }
            }
         }
      }

#ifdef DEBUGFULL
      std::cout << ".. non manifold edges found: " << non_manifold_edges.size() << std::endl;
#endif


      // [INFO] orient the input domain BUT exclude all triangles adjacent to the non-manifold edges
      //
      {
#ifdef DEBUGALL
         std::cout << ".. before orientation checker .. " << std::endl;
#endif
         check_and_repair_orientation(domain, non_manifold_edges);

#ifdef DEBUGALL
         std::cout << ".. after orientation checker .. " << std::endl;
#endif
      }


      // [INFO] orient triangles adjacent to non-manifold edges according to their neighbour's orientation
      //
      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif

            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               if(domain(*cit, "orientation")(0,0) == -2.0) // -2.0 triangle adjacent to non-manifold edge
               {
#ifdef DEBUGALL
                  std::cout << ".. non oriented triangle: " << (*cit).handle() << std::endl;
#endif

                  int ci = 0;
                  boost::array<long,3>   cell_handle;
                  boost::array<double,3> cell_orient;

                  // [INFO] check if the neighbouring triangles are consistently oriented
                  //
                  for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
                  {
#ifdef DEBUGALL
                     std::cout << ".. current edge: " << (*eocit).handle1() << " " << (*eocit).handle2() << std::endl;
#endif
                     // [INFO] only check manifold edges => count == 2
                     //
                     int count = 0;
                     cell_on_edge_iterator coeit2(*eocit);
                     for( ; coeit2.valid(); ++coeit2, ++count) ;
#ifdef DEBUGALL
                     std::cout << "..count: " << count << std::endl;
#endif

                     if(count == 2)
                     {
                        for(cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
                        {
                           // only use the correct oriented triangle
                           //
                           if ((*coeit).handle() == (*cit).handle())
                              continue;

#ifdef DEBUGALL
                           std::cout << ".. checking triangle: " << (*coeit).handle()  << " .. ci: " << ci << std::endl;
                           std::cout << ".. orientation: " << domain((*coeit), "orientation")(0,0) << std::endl;
#endif

                           cell_handle[ci] = (*coeit).handle();
                           cell_orient[ci] = domain((*coeit), "orientation")(0,0);
                           ci++;
                        }
                     }
                     else
                     {
#ifdef DEBUGALL
                        std::cout << "..non-manifold edge.." << std::endl;
#endif
                     }
                  }

                  if(ci == 2)
                  {
                     bool is_oriented_consistently = 0;

                     if(cell_orient[0] == 1.0 && cell_orient[1] == 1.0)
                     {
                        is_oriented_consistently = 1;
                     }
                     else
                     {
#ifdef DEBUGALL
                        std::cout << ".. neighbour not oriented consistently.." << std::cout;
#endif
                     }

#ifdef DEBUGALL
                     std::cout << "..is_oriented_consistently: " << is_oriented_consistently << std::endl;
#endif

                     if (is_oriented_consistently)
                     {
                        bool correct = gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell(cell_handle[0]),
                                                                    (*segit).retrieve_topology().get_cell((*cit).handle()) );

#ifdef DEBUGALL
                        std::cout << "..correct: " << correct << std::endl;
#endif

                        // try to change this error immediatly
                        //
                        if(!correct)
                        {
                           long temp = (*segit).retrieve_topology().get_cell( (*cit).handle() )[1];
                           (*segit).retrieve_topology().get_cell((*cit).handle())[1] = (*segit).retrieve_topology().get_cell((*cit).handle())[2];
                           (*segit).retrieve_topology().get_cell((*cit).handle())[2] = temp;

#ifdef DEBUGALL
                           std::cout << "..triangle: " << (*cit)
                                     << ".. inverting orientation "
                                     << std::endl;
#endif
                        }
                     }
                  }
                  else
                  {
#ifdef DEBUGALL
                        std::cout << ".. possible error !!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl << std::endl;
#endif
                  }
               }
            }
         }
      }

      long max_points = domain.point_size();

#ifdef DEBUGALL
      std::cout << "..point size: " << max_points << std::endl;
#endif

      std::map<edge_t, int> used_edges;
      std::map<long, int> change_trigs;

      // [INFO] duplicate non-manifold edges and resume normal meshing
      //
      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ; eocit.valid() ; ++eocit)
               {
                  int count = 0;
                  for(cell_on_edge_iterator coeit(*eocit) ; coeit.valid() ; ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  edge_t edge;
                  edge[0] = (*eocit).handle1(); edge[1] = (*eocit).handle2();
                  if(edge[0] < edge[1])
                     std::swap(edge[0], edge[1]);

                  int trig_found = 0;

                  // [INFO] only use non-manifold edges
                  //
                  if (count != 2 && used_edges[edge] != 1)
                  {
#ifdef DEBUGALL
                     std::cout << "..base trig: " << (*cit) << std::endl;
                     std::cout << "..nm-edge: " << (*eocit).handle1() << "-" << (*eocit).handle2() << std::endl;
#endif

                     long change_trig_handle = -1;
                     long first_trig_handle  = -1;
                     long second_trig_handle = -1;

                     for(cell_on_edge_iterator coeit(*eocit); coeit.valid() && !trig_found; ++coeit)
                     {
                        // only use the correctly oriented triangle
                        //
                        if ((*coeit).handle() == (*cit).handle())
                           continue;

#ifdef DEBUGALL
                        std::cout << ".. checking triangle: " << (*coeit).handle() << " ";
                        std::cout << "cell vertices: ";
                        vertex_on_cell_iterator vocit(*coeit);
                        while (vocit.valid())
                        {
                           std::cout << (*vocit).handle() << " ";
                           vocit++;
                        }
                        std::cout << std::endl;
#endif

                        bool is_oriented_consistently =
                           gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell( (*coeit).handle() ),
                                                         (*segit).retrieve_topology().get_cell( (*cit).handle() )  );

                        // [INFO] exclude the triangle on the exact opposite side
                        //
//                        if(is_oriented_consistently)
                        {
#ifdef DEBUGALL
                           std::cout << "..a match found" << std::endl;
#endif

                           point_t base_point;
                           long base_point_handle;
                           int pi;
                           int pc = 0;

                           vertex_on_cell_iterator vocit(*coeit);
#ifdef DEBUGALL
                           std::cout << "cell vertices: ";
#endif

                           while (vocit.valid())
                           {
                              if((*vocit).handle() != (*eocit).handle1() && (*vocit).handle() != (*eocit).handle2())
                              {
                                 base_point = domain.get_point(*vocit);
                                 base_point_handle = (*vocit).handle();
#ifdef DEBUGALL
                                 std::cout << ".. base point: " << (*vocit).handle() ;
#endif
                                 pi = pc;
                              }
//                               std::cout << (*vocit) << " ";
                              vocit++;
                              pc++;
                           }
#ifdef DEBUGALL
                           std::cout << std::endl;
#endif

                           edge_on_cell_iterator eocit2(*cit);
                           for( ; eocit2.valid() && !trig_found ; ++eocit2)
                           {
#ifdef DEBUGALL
                              std::cout << "..edge: " << (*eocit2).handle1() << " " << (*eocit2).handle2() << std::endl;
#endif

                              int count2 = 0;
                              for(cell_on_edge_iterator coeit2(*eocit2); coeit2.valid() ; ++coeit2, ++count2) ;

                              // [INFO] only use triangles not connected on the non-manifold edge
                              //
                              if (count2 == 2)
                              {
                                 for(cell_on_edge_iterator coeit2(*eocit2); coeit2.valid() && !trig_found ; ++coeit2)
                                 {
#ifdef DEBUGALL
                                    std::cout << ".. coeit2: " << (*coeit2) << std::endl;
#endif

                                    vertex_on_cell_iterator vocit2(*coeit2);

                                    int found_edge = 0;
                                    int found_trig = 0;
                                    while (vocit2.valid() && !trig_found)
                                    {
#ifdef DEBUGALL
                                       std::cout << " " << (*vocit2).handle();
#endif
                                       if((*vocit2).handle() == base_point_handle)
                                       {
                                          found_trig = 1;
                                       }
                                       if((*vocit2).handle() == edge[0])
                                       {
                                          found_edge = 1;
                                       }
                                       if(found_edge && found_trig)
                                       {
#ifdef DEBUGALL
                                          std::cout << ".. connection to other trig found " << std::endl ;
#endif
                                          used_edges[edge] = 1;
                                          trig_found = 1;

                                          change_trig_handle = (*coeit2).handle();
                                          first_trig_handle  = (*coeit).handle();
                                          second_trig_handle = (*cit).handle();
                                       }
                                       vocit2++;
                                    }
#ifdef DEBUGALL
                                    std::cout << std::endl;
#endif
                                 }
                              }
#ifdef DEBUGALL
                              std::cout << "..after count==2 " << std::endl;
#endif
                           }
                        }
                     }

                     // apply changes
                     //
                     if(trig_found)
                     {
//                         vertex_on_cell_iterator vocit3(*coeit2);
//                         long pi = 0;
//                         while (vocit3.valid())
                        for(int pi=0; pi < 3; pi++)
                        {
                           std::cout << " " << (*segit).retrieve_topology().get_cell(change_trig_handle)[pi] << std::endl;

                           for(int ei=0; ei < 2; ei++)
                           {
                              if((*segit).retrieve_topology().get_cell(change_trig_handle)[pi] == edge[ei])
                              {
                                 // [INFO] nm-edge handling
                                 //
#ifdef DEBUGALL
                                 std::cout << ".. nm-edge point[" << ei << "] found: " << edge[ei] << std::endl ;
#endif
//                                  point_t new_point;
//                                  new_point = domain.get_point_fromhandle(edge[ei]);

                                 // [INFO] use barycenter calculation for new point
                                 //        => new point is inside the trig but not the original point
                                 //
                                 boost::array<point_t, 3> test_cell;
                                 test_cell[0] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[0]);
                                 test_cell[1] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[1]);
                                 test_cell[2] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[2]);

                                 point_t bary_point = gsse::barycenter(test_cell.begin(), test_cell.end());

                                 domain.fast_point_insert(bary_point);

#ifdef DEBUGALL
                                 std::cout << ".. added new point: " << domain.point_size()-1 << std::endl;
#endif

                                 segment_iterator segit2 = domain.segment_begin();
                                 for( ; segit2 != domain.segment_end(); ++segit2)
                                 {
                                    if(segit != segit2)
                                    {
                                       cell_iterator cit2;
                                       for (cit2 = (*segit2).cell_begin(); cit2 != (*segit2).cell_end(); ++cit2)
                                       {

                                          if(compare_triangles(segit2, (*cit2).handle(), segit, first_trig_handle))
                                          {
#ifdef DEBUGALL
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
#endif
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }

                                          // second trig of nm-edge
                                          //
                                          if(compare_triangles(segit2, (*cit2).handle(), segit, second_trig_handle))
                                          {
#ifdef DEBUGALL
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
#endif
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }

                                          // third trig but not of nm-edge
                                          //
                                          if(compare_triangles(segit2, (*cit2).handle(), segit, change_trig_handle))
                                          {
#ifdef DEBUGALL
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
#endif
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }
                                       }
                                    }
                                 }

                                 (*segit).retrieve_topology().get_cell(change_trig_handle)[pi] = domain.point_size()-1;

                                 for(int ci=0; ci<3; ci++)
                                 {
                                    if(edge[0] == (*segit).retrieve_topology().get_cell(first_trig_handle)[ci])
                                       (*segit).retrieve_topology().get_cell(first_trig_handle)[ci] = domain.point_size()-1;
                                 }

                                 for(int ci=0; ci<3; ci++)
                                 {
                                    if(edge[0] == (*segit).retrieve_topology().get_cell(second_trig_handle)[ci])
                                       (*segit).retrieve_topology().get_cell(second_trig_handle)[ci] = domain.point_size()-1;
                                 }

                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }






                        // ################################################################
                        // target are the points of both triangles under investigation
                        //
//                            if(test_point == 8274)
//                            {
//                               std::vector<long> target;
//                               target.push_back(8272);
//                               target.push_back(3306);
// //                               target.push_back(8274);
// //                               target.push_back(3298);

//                               std::cout << ".. starting nearest match .. " << std::endl;

//                               long max_counter = 10;
//                               long result = find_nearest_match(domain, segit, cit, edge, target, max_counter);
//                               if(result != -1)
//                                  std::cout << ".. found .. " << result << std::endl;
//                               else
//                                  std::cout << ".. NOT found .. " << result << std::endl;

//                               std::cout << ".. finishing .. " << std::endl;
//                            }

                        // ################################################################



//                            point_t new_point1;
//                            point_t new_point2;

//                            // [TODO] quick hack ... redo if this idea works !!!
//                            if(pi == 0)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[1]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[2]);
//                            }
//                            else if(pi == 1)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[0]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[2]);
//                            }
//                            else if(pi == 2)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[0]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[1]);
//                            }

// //                           new_point1[2] += 0.0001;
// //                            new_point2[2] += 0.0001;
//                            std::cout << "..point1: " << new_point1 << std::endl;
//                            std::cout << "..point2: " << new_point2 << std::endl;

// //                            domain.fast_point_insert(new_point1);
//                            domain.fast_point_insert(new_point2);

//                            (*segit).retrieve_topology().get_cell((*cit).handle())[0] = base_point_handle;

//                            if(pi == 0)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[1] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[2] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }
//                            else if(pi == 1)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[0] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[2] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }
//                            else if(pi == 2)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[0] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[1] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }

//                            std::cout << "..new trig: " << (*cit).handle()
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[0]
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[1]
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[2]
//                                      << std::endl;

//                            bool is_oriented_consistently =
//                               gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell((*coeit).handle()),
//                                                             (*segit).retrieve_topology().get_cell((*cit).handle()) );

//                            std::cout << ".. is_oriented_consistently: " << is_oriented_consistently << std::endl;

//                            used_edges[edge] = 0;



      std::cout << ".. before checking orientation the second time .. " << std::endl;

      check_and_repair_orientation_new(domain);

      std::cout << ".. checking again for non-manifold edges .. " << std::endl;

      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ;eocit.valid(); ++eocit)
               {
                  int count = 0;
                  edge_t new_edge;
                  cell_on_edge_iterator coeit(*eocit);
                  for (; coeit.valid(); ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  if (count != 2)
                  {
                     new_edge[0] = (*eocit).handle1();
                     new_edge[1] = (*eocit).handle2();

                     non_manifold_edges.push_back(new_edge);

#ifdef DEBUGALL
                     std::cout << ".. non_manifold_edge found: " << new_edge[0] << " " << new_edge[1]
                               << " .. count: " << count <<  std::endl;
#endif
                  }
                  else
                  {
#ifdef DEBUGFULL
                     std::cout << ".. manifold_edge: " << (*eocit).handle1() << " " << (*eocit).handle2()
                               << " .. count: " << count <<  std::endl;
#endif
                  }
               }
            }
         }
      }
#ifdef DEBUGALL
      domain.write_file("fs_output_test.gau32");
#endif
    }

#ifdef DEBUGALL
      std::cout << "..point size: " << domain.point_size() << std::endl;
#endif

      trias.SetSize(0);
      points.SetSize(0);

//      PrintMessage(3,"number of triangles = ", readtrigs.Size());

      // [FS][MOD] set the size for multiple surfaces
      //
      material_size = domain.segment_size();

      // [FS][TODO] .. maybe this can be removed ?
      //           check if the bounding box really matters
      //
      segment_iterator segit = domain.segment_begin();
      for( ; segit != domain.segment_end(); ++segit)
	{
	  cell_iterator cit = (*segit).cell_begin();
	  for( ; cit != (*segit).cell_end(); ++cit, i++)
	    {
	      vertex_on_cell_iterator vocit(*cit);
	      while(vocit.valid())
		{
		  point_t point = domain.get_point(*vocit);
		  Point<3> p(point[0],point[1],point[2]);
		  boundingbox.Add(p);
		  vocit++;
		}
	    }
	}

#ifdef DEBUGALL
      std::cout << "boundingbox: " << Point3d(boundingbox.PMin()) << " - " << Point3d(boundingbox.PMax()) << std::endl;
#endif

      ARRAY<int> pintersect;
      Box<3> bb = boundingbox;
      bb.Increase (1);

      pointtree = new Point3dTree (bb.PMin(), bb.PMax());

      typedef std::map<point_t, vertex_handle>::iterator point_map_iterator;
      boost::array<vertex_handle, 3>                       element_container;

      std::map<std::vector<int>, int>        triangle_map;

      long element_counter = 1;
      long triangle_counter = 1;
      i=0;
      int material_number = 0;
      segit = domain.segment_begin();


    for( ; segit != domain.segment_end(); ++segit, ++material_number)
    {
        cell_iterator cit = (*segit).cell_begin();

        std::map<point_t, vertex_handle> point_map;

        for( ; cit != (*segit).cell_end(); ++cit, i++)
        {
            vertex_on_cell_iterator vocit(*cit);

            int actual_count = 0;

            for (int element_index = 0; element_index < 3; element_index++, ++vocit)
            {
                if((*vocit).handle() != -1)
                {
                    point_t point = domain.get_point(*vocit);

                    point_map_iterator pmit = point_map.find(point);
    #ifdef DEBUGALL
                    std::cout << "..checking point: " << (*vocit).handle() << " :: " << point << std::endl;
    #endif
                    if(pmit == point_map.end())
                    {
                        vgmnetgen::Point<3> p(point[0],point[1],point[2]);

                        vgmnetgen::Point<3> pmin = p - vgmnetgen::Vec<3> (pointtol, pointtol, pointtol);
                        vgmnetgen::Point<3> pmax = p + vgmnetgen::Vec<3> (pointtol, pointtol, pointtol);

                        pointtree->GetIntersecting (pmin, pmax, pintersect);

    #ifdef DEBUGALL
                        if (pintersect.Size() > 1)
                            std::cout << "too many close points" << std::endl;
    #endif

                        int foundpos = -1;

                        // [INFO] ensure that a point is only added once => topological and geometrical unique
                        //
                        if (pintersect.Size())
                            foundpos = pintersect[0];

    #ifdef DEBUGALL
                        std::cout << "..foundpos: " << foundpos << std::endl;
    #endif

                        if (foundpos == -1)
                        {
    #ifdef DEBUGALL
                            std::cout << "..adding point: " << p << std::endl;
    #endif
                            foundpos = AddPoint(p);
                            pointtree->Insert (p, foundpos);
                        }

                        point_map[point] = foundpos;
                        element_container[element_index] = foundpos;

                        element_counter++;
                    }
                    else
                    {
                        element_container[element_index] = (*pmit).second;
                    }

                    actual_count++;

                    // 	      Point<3> pmin = p - Vec<3> (pointtol, pointtol, pointtol);
                    // 	      Point<3> pmax = p + Vec<3> (pointtol, pointtol, pointtol);

                    // 	      pointtree->GetIntersecting (pmin, pmax, pintersect);

                    // 	      std::cout << ".. after intersection test: " << std::endl;

                    // 	      if (pintersect.Size() > 1)
                    // 		PrintError("too many close points");

                    // 	      int foundpos = -1;

                    // 	      // [FS] .. test: add the interface point a second time
                    // 	      //

                    // 	      if (pintersect.Size())
                    // 		foundpos = pintersect[0];

                    // 	      std::cout << ".. foundpos: " << foundpos << " :: for point: " << p <<  std::endl;

                    // 	      if (foundpos == -1)
                    // 		{
                    // 		  foundpos = AddPoint(p);
                    // 		  pointtree->Insert (p, foundpos);
                    // 		}
                    // 	      st[k] = foundpos;
                }
            }




            if(actual_count == 3)
            {

                // [INFO] .. calculate the normals from the gsse structure
                //
                STLTriangle st;
                Vec<3> normal = Cross (GetPoint(element_container[1])-GetPoint(element_container[0]),
                            GetPoint(element_container[2])-GetPoint(element_container[0]));
                normal.Normalize();
                st.SetNormal (normal);


//                 Vec<3> v0( GetPoint(element_container[0])[0], GetPoint(element_container[0])[1], GetPoint(element_container[0])[2] );
//                 Vec<3> v1( GetPoint(element_container[1])[0], GetPoint(element_container[1])[1], GetPoint(element_container[1])[2] );
//                 Vec<3> v2( GetPoint(element_container[2])[0], GetPoint(element_container[2])[1], GetPoint(element_container[2])[2] );
//
// //                 v0[0] = GetPoint(element_container[0])[0];
// //                 v0[1] = GetPoint(element_container[0])[1];
// //                 v0[2] = GetPoint(element_container[0])[2];
//
// //                 Vec<3> v1 = GetPoint(element_container[1]);
// //                 Vec<3> v2 = GetPoint(element_container[2]);
//
//                 Vec<3> center = ( v0+v1+v2 );
// //                 center[0] /= 3.0;
// //                 center[1] /= 3.0;
// //                 center[2] /= 3.0;
//
//                 int mat_num;
//
//                 if ( (center[2] - 30.0) < -1e-6 )
//                     mat_num = 0;
//                 else if ( (center[2] - 30.0) > 1e-6 )
//                     mat_num = 1;
//                 else
//                 {
//                     if (normal[2] > 0)
//                         mat_num = 0;
//                     else
//                         mat_num = 1;
//                 }
//
//
//
//
//                 st.material[0] = mat_num;
                st.material[0] = material_number;
                st.material[1] = -1;

                st[0] = element_container[0];
                st[1] = element_container[1];
                st[2] = element_container[2];

    #ifdef DEBUGALL

                std::cout << "..triangle_counter: " << triangle_counter << std::endl;
                std::cout << "..st[0]: " << st[0] << std::endl;
                std::cout << "..st[1]: " << st[1] << std::endl;
                std::cout << "..st[2]: " << st[2] << std::endl;

//                 std::cout << "..center: " << center << std::endl;
                std::cout << "..normal: " << normal << std::endl;
                std::cout << "..material: " << st.material[0] << std::endl;
    #endif

                std::vector<int> triangle;
                triangle.push_back(st[0]);
                triangle.push_back(st[1]);
                triangle.push_back(st[2]);

                // [FS][INFO] this is done to for searching in the triangle_map
                //
                sort(triangle.begin(), triangle.end());

                std::map<std::vector<int>, int>::iterator tmit;
                tmit = triangle_map.find(triangle);

                if(tmit == triangle_map.end())
                {
    #ifdef DEBUGALL
                    std::cout << ".. adding new triangle" << std::endl;
    #endif

                    triangle_map[triangle] = triangle_counter;

                    if ( (st[0] == st[1]) || (st[0] == st[2]) || (st[1] == st[2]) )
                    {
                        PrintError("STL Triangle degenerated");
                    }
                    else
                    {
                        AddTriangle(st);
                        triangle_counter++;
                    }
                }
                else
                {
                    // store the second material information onto the triangle
                    //
    #ifdef DEBUGALL
                    std::cout << "..triangle found: triangle_counter: " << (*tmit).second << " :: materialnum: " << material_number << std::endl;
    #endif


                    GetTriangle((*tmit).second).material[1] = material_number;
//                     mat_num = 1 - GetTriangle((*tmit).second).material[0];
//                     GetTriangle((*tmit).second).material[1] = mat_num;
                }


//                 std::cout << "### MATERIAL Number : " << mat_num << std::endl;
//                 std::cout << " ------------------------------------------------------------------------------------- " << std::endl;
            }
        }
    }




  FindNeighbourTrigs();

#ifdef DEBUGALL
  std::cout << "[FS] .. end of NEW InitSTLGeometry: point size: " << GetNP() << std::endl;
#endif
}

void STLTopology :: InitSTLGeometry(const char * filename)
{
#ifdef DEBUGALL
  std::cout << "[FS] .. in InitSTLGeometry:  " << filename << std::endl;
#endif

  int i, j, k;

  // const double geometry_tol_fact = 1E6;
  // distances lower than max_box_size/tol are ignored



      // [INFO] .. GSSE STARTS HERE
      //
      typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_2t;
      typedef gsse::get_domain<unstructured_topology_2t, double, double,3>::type    domain_32t;

      typedef gsse::domain_traits<domain_32t>::cell_iterator		  cell_iterator;
      typedef gsse::domain_traits<domain_32t>::vertex_handle              vertex_handle;
      typedef gsse::domain_traits<domain_32t>::cell_2_vertex_mapping    cell_2_vertex_mapping;
      typedef gsse::domain_traits<domain_32t>::segment_iterator	  segment_iterator;
      typedef gsse::domain_traits<domain_32t>::point_t             point_t;
      typedef gsse::domain_traits<domain_32t>::vertex_on_cell_iterator  vertex_on_cell_iterator;

      typedef gsse::domain_traits<domain_32t>::edge_on_cell_iterator    edge_on_cell_iterator;
      typedef gsse::domain_traits<domain_32t>::cell_on_edge_iterator    cell_on_edge_iterator;

      typedef gsse::domain_traits<domain_32t>::storage_type             storage_type;

      typedef gsse::domain_traits<domain_32t>::point_t		    point_t;

      domain_32t domain;

#ifdef DEBUGALL
      std::cout << "filename: " << filename << std::endl;
      std::cout << "pointtol=" << pointtol << std::endl;
#endif

      domain.read_file(filename, false);

//       typedef SpaceGenerator<3, 2, 1>::FullSpace    FullSpace32;
//       FullSpace32 output_space;


//       domain.read_file("simple_face.gau32");

//      domain.read_file("simple_box.gau32");
//       domain.read_file("simple_box_fine.gau32");

      // almost touching 2 segment example
//       domain.read_file("multiple_surfaces_22.gau32");

//       domain.read_file("multiple_surfaces_23.gau32");

      // touching 2 segment example
//      domain.read_file("multiple_surfaces_24.gau32");
//       domain.read_file("multiple_surfaces_24_fine.gau32");

//       domain.read_file("multiple_surfaces_31.gau32");
//       domain.read_file("multiple_surfaces_32.gau32");
//       domain.read_file("multiple_surfaces_33.gau32");

      // 5 boxes - order is changed - cannot be oriented consequentually
//       domain.read_file("multiple_surfaces_34.gau32");
//       domain.read_file("multiple_surfaces_34.gau32_oriented.gau32");

      // 8 boxes arranged as a ring
//      domain.read_file("multiple_surfaces_35_oriented.gau32");

      // 8 segments - each sharing the same single edge => cake
//       domain.read_file("cake_model_1.gau32");

      // 4 segments - sharing the same egde -
//       domain.read_file("cake_model_2.gau32");

//       domain.read_file("multiple_surfaces_24_orient_2.gau32");
//       domain.read_file("multiple_surfaces_25.gau32");
//       domain.read_file("multiple_surfaces_big_small_box_2.gau32");
//      domain.read_file("multiple_surfaces_big_small_box_3.gau32");


      // touching 2 segments - one box is smaller
//       domain.read_file("multiple_surfaces_big_small_box_4.gau32");
//       domain.read_file("multiple_surfaces_big_small_box_5.gau32");

      // touching 3 segments - 2 small boxes on one big box
//       domain.read_file("multiple_surfaces_big_small_box_11.gau32");
//      domain.read_file("multiple_surfaces_big_small_box_12.gau32");

      // touching 2 segments - part is on boundary - part is inside
//         domain.read_file("multiple_surfaces_big_small_box_6.gau32");

      // inner box nearly touching the outer box - 0.99
//       domain.read_file("multiple_surfaces_big_small_box_7.gau32");

      // inner box nearly touching the outer box - 0.999
//       domain.read_file("multiple_surfaces_big_small_box_8.gau32");

      // inner box nearly touching the outer box - 0.9999
//        domain.read_file("multiple_surfaces_big_small_box_9.gau32");

//       domain.read_file("multiple_surfaces_big_box_2.gau32");
//       domain.read_file("multiple_surfaces_big_box_3.gau32");


//       domain.read_file("sample1_cospherical_points.gau32");

//       domain.read_file("trench.gau32");
//        domain.read_file("fs_output_seg0_surface.gau32");
//        domain.read_file("fs_cochlea_oriented.gau32");


//      domain.read_file("sphere_in_sphere_raw.gau32");
//      domain.read_file("cylinder.gau32");


//       domain.read_file("multiple_surfaces_28.gau32");
//      domain.read_file("levelset_orig_seg1.gau32");
//       domain.read_file("levelset_test.gau32");
//       domain.read_file("device_2.gau32");
//       domain.read_file("device_seg5.gau32");
//        domain.read_file("levelset_orig_seg1_oriented.gau32");

//      domain.read_file("lto_depo_over_lines.gau32_oriented_seg0.gau32");
//       domain.read_file("lto_depo_over_lines.gau32_oriented_seg3.gau32");

//      domain.read_file("pecvd_layer_in_trench.gau32_oriented_seg2.gau32");
//      domain.read_file("cyl_mask_etched_flux_oriented_seg0.gau32_oriented.gau32");
//      domain.read_file("cyl_mask_etched_flux_oriented_seg1.gau32");
//      domain.read_file("litho_shaped_gates_sram_cell.gau32_oriented_seg0.gau32");

//      domain.read_file("wordline_bitline_dram_cell.gau32_oriented_seg26.gau32");

      // working - seg 10 temporarily excluded
      //
//      domain.read_file("litho_shaped_gates_sram_cell.gau32_oriented.gau32");
//      domain.read_file("T2_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");

//      domain.read_file("T2_updateDestD5_ascii_seg-1_surface_oriented.gau32");
//      domain.read_file("T2_updateDestD5_ascii_seg-1_surface.gau32_oriented_corrected.gau32");

//      domain.read_file("T3_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_seg-1_surface.gau32_oriented_corrected.gau32");

//      domain.read_file("T3_updateDestD5_ascii_surface_manual.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_surface_manual_seg0.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_surface_manual_seg1.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_surface_manual_seg2.gau32_oriented.gau32");

//      domain.read_file("T4_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");
//last      domain.read_file("T4_updateDestD5_ascii_surface_manual.gau32_oriented.gau32");
//      domain.read_file("T4_updateDestD5_ascii_surface_manual_seg0.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg1.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg2.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg3.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg4.gau32_oriented.gau32");


//      domain.read_file("T5_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");
//      domain.read_file("T5_updateDestD5_ascii_surface_manual.gau32_oriented.gau32");

//      domain.read_file("fs_hase_34_101_1.gau32");
//       domain.read_file("fs_bibi_lunge.gau32");
//      domain.read_file("fs_bibi_lunge_part1.gau32");
//      domain.read_file("fs_hase_34_2_temp_2.gau32");
//      domain.read_file("fs_hase_34_2_temp_3.gau32");

      // [FRAUENHOFER] NON working examples
      // thin layers
//      domain.read_file("copper_and_barriers_in_hole.gau32_oriented.gau32_oriented.gau32");
//      domain.read_file("imp_deposition_into_trench.gau32_oriented.gau32");

      // input orientation
//      domain.read_file("cyl_mask_etched_flux.gau32_oriented.gau32");


      // [DELINK] NON working examples - check topological correctness
//      domain.read_file("vertebra.hin_seg-1_surface.gau32");
//      domain.read_file("beethoven.hin_seg-1_surface.gau32");


//      domain.read_file("fdsoi_3d_25nm_sram_delink_volume_seg-1_surface.gau32");


      // [INTEL] working examples
//       domain.read_file("simplebox.gau32");
//       domain.read_file("diffuseball.gau32_oriented.gau32");
//       domain.read_file("diffusecyl.gau32_oriented.gau32");
//       domain.read_file("report300.gau32");
//       domain.read_file("report325.gau32");
//       domain.read_file("report350.gau32");
//       domain.read_file("device.gau32_oriented.gau32");
//      domain.read_file("levelset_orig.gau32_oriented.gau32");

      // [FRAUENHOFER] working examples
      //
//      domain.read_file("interconnect_via.gau32_oriented.gau32");
//      domain.read_file("lto_depo_over_lines.gau32_oriented.gau32");
//      domain.read_file("litho_shaped_gates_sram_cell.gau32_oriented.gau32");
//      domain.read_file("pecvd_layer_in_trench.gau32_oriented.gau32");
//      domain.read_file("sputter_etched_trench_with_litho_mask.gau32_oriented.gau32");
//      domain.read_file("wordline_bitline_dram_cell.gau32_oriented.gau32");
//      domain.read_file("etched_hole_with_oxide_etch_model.gau32_oriented.gau32");

      // [PF] working examples
//       domain.read_file("cmos.gau32_oriented.gau32");
//      domain.read_file("bunny_fine_seg0_surface.gau32_oriented.gau32");

      int check_nm = 0;
      if(check_nm)
      {

      // [INFO] checking for non-manifold edges
      //
      typedef boost::array<long,2> edge_t;
      std::vector<edge_t> non_manifold_edges;
      std::vector<long> non_oriented_triangles;

      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ;eocit.valid(); ++eocit)
               {
                  int count = 0;
                  edge_t new_edge;
                  cell_on_edge_iterator coeit(*eocit);
                  for (; coeit.valid(); ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  if (count != 2)
                  {
                     new_edge[0] = (*eocit).handle1();
                     new_edge[1] = (*eocit).handle2();

                     non_manifold_edges.push_back(new_edge);

#ifdef DEBUGFULL
                     std::cout << ".. non_manifold_edge found: " << new_edge[0] << " " << new_edge[1]
                               << " .. count: " << count <<  std::endl;
#endif
                  }
                  else
                  {
#ifdef DEBUGFULL
                     std::cout << ".. manifold_edge: " << (*eocit).handle1() << " " << (*eocit).handle2()
                               << " .. count: " << count <<  std::endl;
#endif
                  }
               }
            }
         }
      }

      std::cout << ".. non manifold edges found: " << non_manifold_edges.size() << std::endl;


      // [INFO] orient the input domain BUT exclude all triangles adjacent to the non-manifold edges
      //
      {
#ifdef DEBUGALL
         std::cout << ".. before orientation checker .. " << std::endl;
#endif
         check_and_repair_orientation(domain, non_manifold_edges);

#ifdef DEBUGALL
         std::cout << ".. after orientation checker .. " << std::endl;
#endif
      }


      // [INFO] orient triangles adjacent to non-manifold edges according to their neighbour's orientation
      //
      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
            std::cout << ".. seg: " << (*segit) << std::endl;

            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               if(domain(*cit, "orientation")(0,0) == -2.0) // -2.0 triangle adjacent to non-manifold edge
               {
                  std::cout << ".. non oriented triangle: " << (*cit).handle() << std::endl;

                  int ci = 0;
                  boost::array<long,3>   cell_handle;
                  boost::array<double,3> cell_orient;

                  // [INFO] check if the neighbouring triangles are consistently oriented
                  //
                  for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
                  {
#ifdef DEBUGALL
                     std::cout << ".. current edge: " << (*eocit).handle1() << " " << (*eocit).handle2() << std::endl;
#endif
                     // [INFO] only check manifold edges => count == 2
                     //
                     int count = 0;
                     cell_on_edge_iterator coeit2(*eocit);
                     for( ; coeit2.valid(); ++coeit2, ++count) ;
                     std::cout << "..count: " << count << std::endl;

                     if(count == 2)
                     {
                        for(cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
                        {
                           // only use the correct oriented triangle
                           //
                           if ((*coeit).handle() == (*cit).handle())
                              continue;

#ifdef DEBUGALL
                           std::cout << ".. checking triangle: " << (*coeit).handle()  << " .. ci: " << ci << std::endl;
                           std::cout << ".. orientation: " << domain((*coeit), "orientation")(0,0) << std::endl;
#endif

                           cell_handle[ci] = (*coeit).handle();
                           cell_orient[ci] = domain((*coeit), "orientation")(0,0);
                           ci++;
                        }
                     }
                     else
                     {
#ifdef DEBUGALL
                        std::cout << "..non-manifold edge.." << std::endl;
#endif
                     }
                  }

                  if(ci == 2)
                  {
                     bool is_oriented_consistently = 0;

                     if(cell_orient[0] == 1.0 && cell_orient[1] == 1.0)
                     {
                        is_oriented_consistently = 1;
                     }
                     else
                     {
#ifdef DEBUGALL
                        std::cout << ".. neighbour not oriented consistently.." << std::cout;
#endif
                     }

#ifdef DEBUGALL
                     std::cout << "..is_oriented_consistently: " << is_oriented_consistently << std::endl;
#endif

                     if (is_oriented_consistently)
                     {
                        bool correct = gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell(cell_handle[0]),
                                                                    (*segit).retrieve_topology().get_cell((*cit).handle()) );

#ifdef DEBUGALL
                        std::cout << "..correct: " << correct << std::endl;
#endif

                        // try to change this error immediatly
                        //
                        if(!correct)
                        {
                           long temp = (*segit).retrieve_topology().get_cell( (*cit).handle() )[1];
                           (*segit).retrieve_topology().get_cell((*cit).handle())[1] = (*segit).retrieve_topology().get_cell((*cit).handle())[2];
                           (*segit).retrieve_topology().get_cell((*cit).handle())[2] = temp;

#ifdef DEBUGALL
                           std::cout << "..triangle: " << (*cit)
                                     << ".. inverting orientation "
                                     << std::endl;
#endif
                        }
                     }
                  }
                  else
                  {
#ifdef DEBUGALL
                        std::cout << ".. possible error !!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl << std::endl;
#endif
                  }
               }
            }
         }
      }

      long max_points = domain.point_size();

#ifdef DEBUGALL
      std::cout << "..point size: " << max_points << std::endl;
#endif

      std::map<edge_t, int> used_edges;
      std::map<long, int> change_trigs;

      // [INFO] duplicate non-manifold edges and resume normal meshing
      //
      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ; eocit.valid() ; ++eocit)
               {
                  int count = 0;
                  for(cell_on_edge_iterator coeit(*eocit) ; coeit.valid() ; ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  edge_t edge;
                  edge[0] = (*eocit).handle1(); edge[1] = (*eocit).handle2();
                  if(edge[0] < edge[1])
                     std::swap(edge[0], edge[1]);

                  int trig_found = 0;

                  // [INFO] only use non-manifold edges
                  //
                  if (count != 2 && used_edges[edge] != 1)
                  {
                     std::cout << "..base trig: " << (*cit) << std::endl;
                     std::cout << "..nm-edge: " << (*eocit).handle1() << "-" << (*eocit).handle2() << std::endl;

                     long change_trig_handle = -1;
                     long first_trig_handle  = -1;
                     long second_trig_handle = -1;

                     for(cell_on_edge_iterator coeit(*eocit); coeit.valid() && !trig_found; ++coeit)
                     {
                        // only use the correctly oriented triangle
                        //
                        if ((*coeit).handle() == (*cit).handle())
                           continue;

#ifdef DEBUGALL
                        std::cout << ".. checking triangle: " << (*coeit).handle() << " ";
                        std::cout << "cell vertices: ";
                        vertex_on_cell_iterator vocit(*coeit);
                        while (vocit.valid())
                        {
                           std::cout << (*vocit).handle() << " ";
                           vocit++;
                        }
                        std::cout << std::endl;
#endif

                        bool is_oriented_consistently =
                           gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell( (*coeit).handle() ),
                                                         (*segit).retrieve_topology().get_cell( (*cit).handle() )  );

                        // [INFO] exclude the triangle on the exact opposite side
                        //
//                        if(is_oriented_consistently)
                        {
                           std::cout << "..a match found" << std::endl;

                           point_t base_point;
                           long base_point_handle;
                           int pi;
                           int pc = 0;

                           vertex_on_cell_iterator vocit(*coeit);
                           std::cout << "cell vertices: ";

                           while (vocit.valid())
                           {
                              if((*vocit).handle() != (*eocit).handle1() && (*vocit).handle() != (*eocit).handle2())
                              {
                                 base_point = domain.get_point(*vocit);
                                 base_point_handle = (*vocit).handle();
                                 std::cout << ".. base point: " << (*vocit).handle() ;
                                 pi = pc;
                              }
//                               std::cout << (*vocit) << " ";
                              vocit++;
                              pc++;
                           }
                           std::cout << std::endl;

                           edge_on_cell_iterator eocit2(*cit);
                           for( ; eocit2.valid() && !trig_found ; ++eocit2)
                           {
                              std::cout << "..edge: " << (*eocit2).handle1() << " " << (*eocit2).handle2() << std::endl;

                              int count2 = 0;
                              for(cell_on_edge_iterator coeit2(*eocit2); coeit2.valid() ; ++coeit2, ++count2) ;

                              // [INFO] only use triangles not connected on the non-manifold edge
                              //
                              if (count2 == 2)
                              {
                                 for(cell_on_edge_iterator coeit2(*eocit2); coeit2.valid() && !trig_found ; ++coeit2)
                                 {
                                    std::cout << ".. coeit2: " << (*coeit2) << std::endl;

                                    vertex_on_cell_iterator vocit2(*coeit2);

                                    int found_edge = 0;
                                    int found_trig = 0;
                                    while (vocit2.valid() && !trig_found)
                                    {
                                       std::cout << " " << (*vocit2).handle();
                                       if((*vocit2).handle() == base_point_handle)
                                       {
                                          found_trig = 1;
                                       }
                                       if((*vocit2).handle() == edge[0])
                                       {
                                          found_edge = 1;
                                       }
                                       if(found_edge && found_trig)
                                       {
                                          std::cout << ".. connection to other trig found " << std::endl ;
                                          used_edges[edge] = 1;
                                          trig_found = 1;

                                          change_trig_handle = (*coeit2).handle();
                                          first_trig_handle  = (*coeit).handle();
                                          second_trig_handle = (*cit).handle();
                                       }
                                       vocit2++;
                                    }
                                    std::cout << std::endl;
                                 }
                              }
                              std::cout << "..after count==2 " << std::endl;
                           }
                        }
                     }

                     // apply changes
                     //
                     if(trig_found)
                     {
//                         vertex_on_cell_iterator vocit3(*coeit2);
//                         long pi = 0;
//                         while (vocit3.valid())
                        for(int pi=0; pi < 3; pi++)
                        {
                           std::cout << " " << (*segit).retrieve_topology().get_cell(change_trig_handle)[pi] << std::endl;

                           for(int ei=0; ei < 2; ei++)
                           {
                              if((*segit).retrieve_topology().get_cell(change_trig_handle)[pi] == edge[ei])
                              {
                                 // [INFO] nm-edge handling
                                 //
#ifdef DEBUGALL
                                 std::cout << ".. nm-edge point[" << ei << "] found: " << edge[ei] << std::endl ;
#endif
//                                  point_t new_point;
//                                  new_point = domain.get_point_fromhandle(edge[ei]);

                                 // [INFO] use barycenter calculation for new point
                                 //        => new point is inside the trig but not the original point
                                 //
                                 boost::array<point_t, 3> test_cell;
                                 test_cell[0] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[0]);
                                 test_cell[1] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[1]);
                                 test_cell[2] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[2]);

                                 point_t bary_point = gsse::barycenter(test_cell.begin(), test_cell.end());

                                 domain.fast_point_insert(bary_point);

                                 std::cout << ".. added new point: " << domain.point_size()-1 << std::endl;

                                 segment_iterator segit2 = domain.segment_begin();
                                 for( ; segit2 != domain.segment_end(); ++segit2)
                                 {
                                    if(segit != segit2)
                                    {
                                       cell_iterator cit2;
                                       for (cit2 = (*segit2).cell_begin(); cit2 != (*segit2).cell_end(); ++cit2)
                                       {

                                          if(compare_triangles(segit2, (*cit2).handle(), segit, first_trig_handle))
                                          {
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }

                                          // second trig of nm-edge
                                          //
                                          if(compare_triangles(segit2, (*cit2).handle(), segit, second_trig_handle))
                                          {
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }

                                          // third trig but not of nm-edge
                                          //
                                          if(compare_triangles(segit2, (*cit2).handle(), segit, change_trig_handle))
                                          {
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }
                                       }
                                    }
                                 }

                                 (*segit).retrieve_topology().get_cell(change_trig_handle)[pi] = domain.point_size()-1;

                                 for(int ci=0; ci<3; ci++)
                                 {
                                    if(edge[0] == (*segit).retrieve_topology().get_cell(first_trig_handle)[ci])
                                       (*segit).retrieve_topology().get_cell(first_trig_handle)[ci] = domain.point_size()-1;
                                 }

                                 for(int ci=0; ci<3; ci++)
                                 {
                                    if(edge[0] == (*segit).retrieve_topology().get_cell(second_trig_handle)[ci])
                                       (*segit).retrieve_topology().get_cell(second_trig_handle)[ci] = domain.point_size()-1;
                                 }

                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }






                        // ################################################################
                        // target are the points of both triangles under investigation
                        //
//                            if(test_point == 8274)
//                            {
//                               std::vector<long> target;
//                               target.push_back(8272);
//                               target.push_back(3306);
// //                               target.push_back(8274);
// //                               target.push_back(3298);

//                               std::cout << ".. starting nearest match .. " << std::endl;

//                               long max_counter = 10;
//                               long result = find_nearest_match(domain, segit, cit, edge, target, max_counter);
//                               if(result != -1)
//                                  std::cout << ".. found .. " << result << std::endl;
//                               else
//                                  std::cout << ".. NOT found .. " << result << std::endl;

//                               std::cout << ".. finishing .. " << std::endl;
//                            }

                        // ################################################################



//                            point_t new_point1;
//                            point_t new_point2;

//                            // [TODO] quick hack ... redo if this idea works !!!
//                            if(pi == 0)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[1]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[2]);
//                            }
//                            else if(pi == 1)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[0]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[2]);
//                            }
//                            else if(pi == 2)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[0]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[1]);
//                            }

// //                           new_point1[2] += 0.0001;
// //                            new_point2[2] += 0.0001;
//                            std::cout << "..point1: " << new_point1 << std::endl;
//                            std::cout << "..point2: " << new_point2 << std::endl;

// //                            domain.fast_point_insert(new_point1);
//                            domain.fast_point_insert(new_point2);

//                            (*segit).retrieve_topology().get_cell((*cit).handle())[0] = base_point_handle;

//                            if(pi == 0)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[1] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[2] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }
//                            else if(pi == 1)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[0] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[2] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }
//                            else if(pi == 2)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[0] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[1] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }

//                            std::cout << "..new trig: " << (*cit).handle()
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[0]
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[1]
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[2]
//                                      << std::endl;

//                            bool is_oriented_consistently =
//                               gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell((*coeit).handle()),
//                                                             (*segit).retrieve_topology().get_cell((*cit).handle()) );

//                            std::cout << ".. is_oriented_consistently: " << is_oriented_consistently << std::endl;

//                            used_edges[edge] = 0;



      std::cout << ".. before checking orientation the second time .. " << std::endl;

      check_and_repair_orientation_new(domain);

      std::cout << ".. checking again for non-manifold edges .. " << std::endl;

      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ;eocit.valid(); ++eocit)
               {
                  int count = 0;
                  edge_t new_edge;
                  cell_on_edge_iterator coeit(*eocit);
                  for (; coeit.valid(); ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  if (count != 2)
                  {
                     new_edge[0] = (*eocit).handle1();
                     new_edge[1] = (*eocit).handle2();

                     non_manifold_edges.push_back(new_edge);

#ifdef DEBUGALL
                     std::cout << ".. non_manifold_edge found: " << new_edge[0] << " " << new_edge[1]
                               << " .. count: " << count <<  std::endl;
#endif
                  }
                  else
                  {
#ifdef DEBUGFULL
                     std::cout << ".. manifold_edge: " << (*eocit).handle1() << " " << (*eocit).handle2()
                               << " .. count: " << count <<  std::endl;
#endif
                  }
               }
            }
         }
      }
#ifdef DEBUGALL
      domain.write_file("fs_output_test.gau32");
#endif
    }

#ifdef DEBUGALL
      std::cout << "..point size: " << domain.point_size() << std::endl;
#endif

      trias.SetSize(0);
      points.SetSize(0);

//      PrintMessage(3,"number of triangles = ", readtrigs.Size());

      // [FS][MOD] set the size for multiple surfaces
      //
      material_size = domain.segment_size();

      // [FS][TODO] .. maybe this can be removed ?
      //           check if the bounding box really matters
      //
      segment_iterator segit = domain.segment_begin();
      for( ; segit != domain.segment_end(); ++segit)
	{
	  cell_iterator cit = (*segit).cell_begin();
	  for( ; cit != (*segit).cell_end(); ++cit, i++)
	    {
	      vertex_on_cell_iterator vocit(*cit);
	      while(vocit.valid())
		{
		  point_t point = domain.get_point(*vocit);
		  Point<3> p(point[0],point[1],point[2]);
		  boundingbox.Add(p);
		  vocit++;
		}
	    }
	}

#ifdef DEBUGALL
      std::cout << "boundingbox: " << Point3d(boundingbox.PMin()) << " - " << Point3d(boundingbox.PMax()) << std::endl;
#endif

      ARRAY<int> pintersect;
      Box<3> bb = boundingbox;
      bb.Increase (1);

      pointtree = new Point3dTree (bb.PMin(), bb.PMax());

      typedef std::map<point_t, vertex_handle>::iterator point_map_iterator;
      boost::array<vertex_handle, 3>                       element_container;

      std::map<std::vector<int>, int>        triangle_map;

      long element_counter = 1;
      long triangle_counter = 1;
      i=0;
      int material_number = 0;
      segit = domain.segment_begin();
      for( ; segit != domain.segment_end(); ++segit, ++material_number)
      {
         cell_iterator cit = (*segit).cell_begin();

	  std::map<point_t, vertex_handle> point_map;

	  for( ; cit != (*segit).cell_end(); ++cit, i++)
	    {
	      vertex_on_cell_iterator vocit(*cit);

              int actual_count = 0;

	      for (int element_index = 0; element_index < 3; element_index++, ++vocit)
              {
                 if((*vocit).handle() != -1)
                 {
                    point_t point = domain.get_point(*vocit);

                    point_map_iterator pmit = point_map.find(point);
#ifdef DEBUGALL
                    std::cout << "..checking point: " << (*vocit).handle() << " :: " << point << std::endl;
#endif
                    if(pmit == point_map.end())
                    {
                       vgmnetgen::Point<3> p(point[0],point[1],point[2]);

                       vgmnetgen::Point<3> pmin = p - vgmnetgen::Vec<3> (pointtol, pointtol, pointtol);
                       vgmnetgen::Point<3> pmax = p + vgmnetgen::Vec<3> (pointtol, pointtol, pointtol);

                       pointtree->GetIntersecting (pmin, pmax, pintersect);

#ifdef DEBUGALL
                       if (pintersect.Size() > 1)
                          std::cout << "too many close points" << std::endl;
#endif

                       int foundpos = -1;

                       // [INFO] ensure that a point is only added once => topological and geometrical unique
                       //
                       if (pintersect.Size())
                          foundpos = pintersect[0];

#ifdef DEBUGALL
                       std::cout << "..foundpos: " << foundpos << std::endl;
#endif

                       if (foundpos == -1)
                       {
#ifdef DEBUGALL
                          std::cout << "..adding point: " << p << std::endl;
#endif
                          foundpos = AddPoint(p);
                          pointtree->Insert (p, foundpos);
                       }

                       point_map[point] = foundpos;
                       element_container[element_index] = foundpos;

                       element_counter++;
                    }
                    else
                    {
                       element_container[element_index] = (*pmit).second;
                    }

                    actual_count++;

                    // 	      Point<3> pmin = p - Vec<3> (pointtol, pointtol, pointtol);
                    // 	      Point<3> pmax = p + Vec<3> (pointtol, pointtol, pointtol);

                    // 	      pointtree->GetIntersecting (pmin, pmax, pintersect);

                    // 	      std::cout << ".. after intersection test: " << std::endl;

                    // 	      if (pintersect.Size() > 1)
                    // 		PrintError("too many close points");

                    // 	      int foundpos = -1;

                    // 	      // [FS] .. test: add the interface point a second time
                    // 	      //

                    // 	      if (pintersect.Size())
                    // 		foundpos = pintersect[0];

                    // 	      std::cout << ".. foundpos: " << foundpos << " :: for point: " << p <<  std::endl;

                    // 	      if (foundpos == -1)
                    // 		{
                    // 		  foundpos = AddPoint(p);
                    // 		  pointtree->Insert (p, foundpos);
                    // 		}
                    // 	      st[k] = foundpos;
                 }
              }

              if(actual_count == 3)
              {

	      // [INFO] .. calculate the normals from the gsse structure
	      //
 	      STLTriangle st;
 	      Vec<3> normal = Cross (GetPoint(element_container[1])-GetPoint(element_container[0]),
				     GetPoint(element_container[2])-GetPoint(element_container[0]));
	      normal.Normalize();
	      st.SetNormal (normal);
	      st.material[0] = material_number;
	      st.material[1] = -1;

	      st[0] = element_container[0];
	      st[1] = element_container[1];
	      st[2] = element_container[2];

#ifdef DEBUGALL
	      std::cout << "..triangle_counter: " << triangle_counter << std::endl;
	      std::cout << "..st[0]: " << st[0] << std::endl;
	      std::cout << "..st[1]: " << st[1] << std::endl;
	      std::cout << "..st[2]: " << st[2] << std::endl;

              std::cout << "..normal: " << normal << std::endl;
              std::cout << "..material: " << st.material[0] << std::endl;
#endif

	      std::vector<int> triangle;
	      triangle.push_back(st[0]);
	      triangle.push_back(st[1]);
	      triangle.push_back(st[2]);

	      // [FS][INFO] this is done to for searching in the triangle_map
	      //
	      sort(triangle.begin(), triangle.end());

	      std::map<std::vector<int>, int>::iterator tmit;
	      tmit = triangle_map.find(triangle);

	      if(tmit == triangle_map.end())
		{
#ifdef DEBUGALL
		  std::cout << ".. adding new triangle" << std::endl;
#endif

		  triangle_map[triangle] = triangle_counter;

		  if ( (st[0] == st[1]) ||
		       (st[0] == st[2]) ||
		       (st[1] == st[2]) )
		    {
		      PrintError("STL Triangle degenerated");
		    }
		  else
                  {
                     AddTriangle(st);
                     triangle_counter++;
                  }
		}
	      else
		{
		  // store the second material information onto the triangle
		  //
#ifdef DEBUGALL
		  std::cout << "..triangle found: triangle_counter: " << (*tmit).second << " :: materialnum: " << material_number << std::endl;
#endif

		  GetTriangle((*tmit).second).material[1] = material_number;
		}
              }
	    }

      }




  FindNeighbourTrigs();

#ifdef DEBUGALL
  std::cout << "[FS] .. end of NEW InitSTLGeometry: point size: " << GetNP() << std::endl;
#endif
}



void STLTopology :: InitSTLGeometry(const ARRAY<STLReadTriangle> & readtrigs)
{
#ifdef DEBUGALL
   std::cout << "..original InitSTLGeometry " << std::endl;
#endif

  int i, j, k;

  // const double geometry_tol_fact = 1E6;
  // distances lower than max_box_size/tol are ignored

  int netgen = 0;
  if(netgen)
    {
      trias.SetSize(0);
      points.SetSize(0);

//      PrintMessage(3,"number of triangles = ", readtrigs.Size());

      if (!readtrigs.Size())
	return;

      boundingbox.Set (readtrigs[0][0]);
      for (i = 0; i < readtrigs.Size(); i++)
	for (k = 0; k < 3; k++)
	  boundingbox.Add (readtrigs[i][k]);

#ifdef DEBUGALL
      std::cout << "boundingbox: " << Point3d(boundingbox.PMin()) << " - " << Point3d(boundingbox.PMax()) << std::endl;
#endif

      // [INFO] .. NETGEN CODE STARTS HERE
      //
      Box<3> bb = boundingbox;
      bb.Increase (1);

      pointtree = new Point3dTree (bb.PMin(), bb.PMax());

      ARRAY<int> pintersect;

      pointtol = boundingbox.Diam() * stldoctor.geom_tol_fact;

      // [FS] .. here the point tolerance decides which two non identical points are treated as one
      //
#ifdef DEBUGALL
      std::cout << "point tolerance = " << pointtol << std::endl;
#endif

      for(i = 0; i < readtrigs.Size(); i++)
	{
	  const STLReadTriangle & t = readtrigs[i];
	  STLTriangle st;
	  Vec<3> n = t.Normal();

#ifdef DEBUGALL
	  std::cout << ".. normal: " << t.Normal() << std::endl;
#endif

	  st.SetNormal (t.Normal());

	  for (k = 0; k < 3; k++)
	    {
	      Point<3> p = t[k];

	      Point<3> pmin = p - Vec<3> (pointtol, pointtol, pointtol);
	      Point<3> pmax = p + Vec<3> (pointtol, pointtol, pointtol);

	      pointtree->GetIntersecting (pmin, pmax, pintersect);

#ifdef DEBUGALL
	      std::cout << ".. after intersection test: " << std::endl;
#endif

#ifdef DEBUGALL
	      if (pintersect.Size() > 1)
		PrintError("too many close points");
#endif

	      int foundpos = -1;

	      // [FS] .. test: add the interface point a second time
	      //

	      if (pintersect.Size())
		foundpos = pintersect[0];

	      std::cout << ".. foundpos: " << foundpos << " :: for point: " << p <<  std::endl;

	      if (foundpos == -1)
		{
		  foundpos = AddPoint(p);
		  pointtree->Insert (p, foundpos);
		}
	      st[k] = foundpos;
	    }

	  if ( (st[0] == st[1]) ||
	       (st[0] == st[2]) ||
	       (st[1] == st[2]) )
	    {
	      PrintError("STL Triangle degenerated");
	    }
	  else
	    {
	      std::cout << ".. triangle added: " << st[0] << " / " << st[1] << " / " << st[2] << std::endl;

	      AddTriangle(st);
	    }

	}
    }
  else
    {

      // [INFO] .. GSSE STARTS HERE
      //
      typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_2t;
      typedef gsse::get_domain<unstructured_topology_2t, double, double,3>::type    domain_32t;

      typedef gsse::domain_traits<domain_32t>::cell_iterator		  cell_iterator;
      typedef gsse::domain_traits<domain_32t>::vertex_handle              vertex_handle;
      typedef gsse::domain_traits<domain_32t>::cell_2_vertex_mapping    cell_2_vertex_mapping;
      typedef gsse::domain_traits<domain_32t>::segment_iterator	  segment_iterator;
      typedef gsse::domain_traits<domain_32t>::point_t             point_t;
      typedef gsse::domain_traits<domain_32t>::vertex_on_cell_iterator  vertex_on_cell_iterator;

      typedef gsse::domain_traits<domain_32t>::edge_on_cell_iterator    edge_on_cell_iterator;
      typedef gsse::domain_traits<domain_32t>::cell_on_edge_iterator    cell_on_edge_iterator;

      typedef gsse::domain_traits<domain_32t>::storage_type             storage_type;

      typedef gsse::domain_traits<domain_32t>::point_t		    point_t;

      domain_32t domain;


//       typedef SpaceGenerator<3, 2, 1>::FullSpace    FullSpace32;
//       FullSpace32 output_space;




//       domain.read_file("simple_face.gau32");

//      domain.read_file("simple_box.gau32");
//       domain.read_file("simple_box_fine.gau32");

      // almost touching 2 segment example
//       domain.read_file("multiple_surfaces_22.gau32");

//       domain.read_file("multiple_surfaces_23.gau32");

      // touching 2 segment example
//      domain.read_file("multiple_surfaces_24.gau32");
//       domain.read_file("multiple_surfaces_24_fine.gau32");

//       domain.read_file("multiple_surfaces_31.gau32");
//       domain.read_file("multiple_surfaces_32.gau32");
//       domain.read_file("multiple_surfaces_33.gau32");

      // 5 boxes - order is changed - cannot be oriented consequentually
//       domain.read_file("multiple_surfaces_34.gau32");
//       domain.read_file("multiple_surfaces_34.gau32_oriented.gau32");

      // 8 boxes arranged as a ring
//      domain.read_file("multiple_surfaces_35_oriented.gau32");

      // 8 segments - each sharing the same single edge => cake
//       domain.read_file("cake_model_1.gau32");

      // 4 segments - sharing the same egde -
//       domain.read_file("cake_model_2.gau32");

//       domain.read_file("multiple_surfaces_24_orient_2.gau32");
//       domain.read_file("multiple_surfaces_25.gau32");
//       domain.read_file("multiple_surfaces_big_small_box_2.gau32");
//      domain.read_file("multiple_surfaces_big_small_box_3.gau32");


      // touching 2 segments - one box is smaller
//       domain.read_file("multiple_surfaces_big_small_box_4.gau32");
//       domain.read_file("multiple_surfaces_big_small_box_5.gau32");

      // touching 3 segments - 2 small boxes on one big box
//       domain.read_file("multiple_surfaces_big_small_box_11.gau32");
//      domain.read_file("multiple_surfaces_big_small_box_12.gau32");

      // touching 2 segments - part is on boundary - part is inside
//         domain.read_file("multiple_surfaces_big_small_box_6.gau32");

      // inner box nearly touching the outer box - 0.99
//       domain.read_file("multiple_surfaces_big_small_box_7.gau32");

      // inner box nearly touching the outer box - 0.999
//       domain.read_file("multiple_surfaces_big_small_box_8.gau32");

      // inner box nearly touching the outer box - 0.9999
//        domain.read_file("multiple_surfaces_big_small_box_9.gau32");

//       domain.read_file("multiple_surfaces_big_box_2.gau32");
//       domain.read_file("multiple_surfaces_big_box_3.gau32");


//       domain.read_file("sample1_cospherical_points.gau32");

//       domain.read_file("trench.gau32");
//        domain.read_file("fs_output_seg0_surface.gau32");
//        domain.read_file("fs_cochlea_oriented.gau32");


//      domain.read_file("sphere_in_sphere_raw.gau32");
//      domain.read_file("cylinder.gau32");


//       domain.read_file("multiple_surfaces_28.gau32");
//      domain.read_file("levelset_orig_seg1.gau32");
//       domain.read_file("levelset_test.gau32");
//       domain.read_file("device_2.gau32");
//       domain.read_file("device_seg5.gau32");
//        domain.read_file("levelset_orig_seg1_oriented.gau32");

//      domain.read_file("lto_depo_over_lines.gau32_oriented_seg0.gau32");
//       domain.read_file("lto_depo_over_lines.gau32_oriented_seg3.gau32");

//      domain.read_file("pecvd_layer_in_trench.gau32_oriented_seg2.gau32");
//      domain.read_file("cyl_mask_etched_flux_oriented_seg0.gau32_oriented.gau32");
//      domain.read_file("cyl_mask_etched_flux_oriented_seg1.gau32");
//      domain.read_file("litho_shaped_gates_sram_cell.gau32_oriented_seg0.gau32");

//      domain.read_file("wordline_bitline_dram_cell.gau32_oriented_seg26.gau32");

      // working - seg 10 temporarily excluded
      //
//      domain.read_file("litho_shaped_gates_sram_cell.gau32_oriented.gau32");
//      domain.read_file("T2_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");

//      domain.read_file("T2_updateDestD5_ascii_seg-1_surface_oriented.gau32");
//      domain.read_file("T2_updateDestD5_ascii_seg-1_surface.gau32_oriented_corrected.gau32");

//      domain.read_file("T3_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_seg-1_surface.gau32_oriented_corrected.gau32");

//      domain.read_file("T3_updateDestD5_ascii_surface_manual.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_surface_manual_seg0.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_surface_manual_seg1.gau32_oriented.gau32");
//      domain.read_file("T3_updateDestD5_ascii_surface_manual_seg2.gau32_oriented.gau32");

//      domain.read_file("T4_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");
//last      domain.read_file("T4_updateDestD5_ascii_surface_manual.gau32_oriented.gau32");
//      domain.read_file("T4_updateDestD5_ascii_surface_manual_seg0.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg1.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg2.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg3.gau32_oriented.gau32");
//       domain.read_file("T4_updateDestD5_ascii_surface_manual_seg4.gau32_oriented.gau32");


//      domain.read_file("T5_updateDestD5_ascii_seg-1_surface.gau32_oriented.gau32");
//      domain.read_file("T5_updateDestD5_ascii_surface_manual.gau32_oriented.gau32");

//      domain.read_file("fs_hase_34_101_1.gau32");
//       domain.read_file("fs_bibi_lunge.gau32");
//      domain.read_file("fs_bibi_lunge_part1.gau32");
//      domain.read_file("fs_hase_34_2_temp_2.gau32");
//      domain.read_file("fs_hase_34_2_temp_3.gau32");

      // [FRAUENHOFER] NON working examples
      // thin layers
//      domain.read_file("copper_and_barriers_in_hole.gau32_oriented.gau32_oriented.gau32");
//      domain.read_file("imp_deposition_into_trench.gau32_oriented.gau32");

      // input orientation
//      domain.read_file("cyl_mask_etched_flux.gau32_oriented.gau32");


      // [DELINK] NON working examples - check topological correctness
//      domain.read_file("vertebra.hin_seg-1_surface.gau32");
//      domain.read_file("beethoven.hin_seg-1_surface.gau32");


//      domain.read_file("fdsoi_3d_25nm_sram_delink_volume_seg-1_surface.gau32");


      // [INTEL] working examples
       //domain.read_file("simplebox.gau32");
//       domain.read_file("diffuseball.gau32_oriented.gau32");
//       domain.read_file("diffusecyl.gau32_oriented.gau32");
//       domain.read_file("report300.gau32");
//       domain.read_file("report325.gau32");
//       domain.read_file("report350.gau32");
//       domain.read_file("device.gau32_oriented.gau32");
//      domain.read_file("levelset_orig.gau32_oriented.gau32");

      // [FRAUENHOFER] working examples
      //
//      domain.read_file("interconnect_via.gau32_oriented.gau32");
//      domain.read_file("lto_depo_over_lines.gau32_oriented.gau32");
//      domain.read_file("litho_shaped_gates_sram_cell.gau32_oriented.gau32");
//      domain.read_file("pecvd_layer_in_trench.gau32_oriented.gau32");
//      domain.read_file("sputter_etched_trench_with_litho_mask.gau32_oriented.gau32");
//      domain.read_file("wordline_bitline_dram_cell.gau32_oriented.gau32");
//      domain.read_file("etched_hole_with_oxide_etch_model.gau32_oriented.gau32");

      // [PF] working examples
//       domain.read_file("cmos.gau32_oriented.gau32");
//      domain.read_file("bunny_fine_seg0_surface.gau32_oriented.gau32");

      int check_nm = 0;
      if(check_nm)
      {

      // [INFO] checking for non-manifold edges
      //
      typedef boost::array<long,2> edge_t;
      std::vector<edge_t> non_manifold_edges;
      std::vector<long> non_oriented_triangles;

      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ;eocit.valid(); ++eocit)
               {
                  int count = 0;
                  edge_t new_edge;
                  cell_on_edge_iterator coeit(*eocit);
                  for (; coeit.valid(); ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  if (count != 2)
                  {
                     new_edge[0] = (*eocit).handle1();
                     new_edge[1] = (*eocit).handle2();

                     non_manifold_edges.push_back(new_edge);

                     std::cout << ".. non_manifold_edge found: " << new_edge[0] << " " << new_edge[1]
                               << " .. count: " << count <<  std::endl;
                  }
                  else
                  {
#ifdef DEBUGFULL
                     std::cout << ".. manifold_edge: " << (*eocit).handle1() << " " << (*eocit).handle2()
                               << " .. count: " << count <<  std::endl;
#endif
                  }
               }
            }
         }
      }

      std::cout << ".. non manifold edges found: " << non_manifold_edges.size() << std::endl;


      // [INFO] orient the input domain BUT exclude all triangles adjacent to the non-manifold edges
      //
      {
#ifdef DEBUGALL
         std::cout << ".. before orientation checker .. " << std::endl;
#endif
         check_and_repair_orientation(domain, non_manifold_edges);

#ifdef DEBUGALL
         std::cout << ".. after orientation checker .. " << std::endl;
#endif
      }


      // [INFO] orient triangles adjacent to non-manifold edges according to their neighbour's orientation
      //
      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
            std::cout << ".. seg: " << (*segit) << std::endl;

            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               if(domain(*cit, "orientation")(0,0) == -2.0) // -2.0 triangle adjacent to non-manifold edge
               {
                  std::cout << ".. non oriented triangle: " << (*cit).handle() << std::endl;

                  int ci = 0;
                  boost::array<long,3>   cell_handle;
                  boost::array<double,3> cell_orient;

                  // [INFO] check if the neighbouring triangles are consistently oriented
                  //
                  for (edge_on_cell_iterator eocit(*cit); eocit.valid(); ++eocit)
                  {
#ifdef DEBUGALL
                     std::cout << ".. current edge: " << (*eocit).handle1() << " " << (*eocit).handle2() << std::endl;
#endif
                     // [INFO] only check manifold edges => count == 2
                     //
                     int count = 0;
                     cell_on_edge_iterator coeit2(*eocit);
                     for( ; coeit2.valid(); ++coeit2, ++count) ;
                     std::cout << "..count: " << count << std::endl;

                     if(count == 2)
                     {
                        for(cell_on_edge_iterator coeit(*eocit); coeit.valid(); ++coeit)
                        {
                           // only use the correct oriented triangle
                           //
                           if ((*coeit).handle() == (*cit).handle())
                              continue;

                           std::cout << ".. checking triangle: " << (*coeit).handle()  << " .. ci: " << ci << std::endl;
                           std::cout << ".. orientation: " << domain((*coeit), "orientation")(0,0) << std::endl;

                           cell_handle[ci] = (*coeit).handle();
                           cell_orient[ci] = domain((*coeit), "orientation")(0,0);
                           ci++;
                        }
                     }
                     else
                     {
                        std::cout << "..non-manifold edge.." << std::endl;
                     }
                  }

                  if(ci == 2)
                  {
                     bool is_oriented_consistently = 0;

                     if(cell_orient[0] == 1.0 && cell_orient[1] == 1.0)
                     {
                        is_oriented_consistently = 1;
                     }
                     else
                     {
                        std::cout << ".. neighbour not oriented consistently.." << std::cout;
                     }

                     std::cout << "..is_oriented_consistently: " << is_oriented_consistently << std::endl;

                     if (is_oriented_consistently)
                     {
                        bool correct = gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell(cell_handle[0]),
                                                                    (*segit).retrieve_topology().get_cell((*cit).handle()) );

                        std::cout << "..correct: " << correct << std::endl;

                        // try to change this error immediatly
                        //
                        if(!correct)
                        {
                           long temp = (*segit).retrieve_topology().get_cell( (*cit).handle() )[1];
                           (*segit).retrieve_topology().get_cell((*cit).handle())[1] = (*segit).retrieve_topology().get_cell((*cit).handle())[2];
                           (*segit).retrieve_topology().get_cell((*cit).handle())[2] = temp;

                           std::cout << "..triangle: " << (*cit)
                                     << ".. inverting orientation "
                                     << std::endl;
                        }
                     }
                  }
                  else
                  {
                        std::cout << ".. possible error !!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl << std::endl;
                  }
               }
            }
         }
      }

      long max_points = domain.point_size();
      std::cout << "..point size: " << max_points << std::endl;

      std::map<edge_t, int> used_edges;
      std::map<long, int> change_trigs;

      // [INFO] duplicate non-manifold edges and resume normal meshing
      //
      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ; eocit.valid() ; ++eocit)
               {
                  int count = 0;
                  for(cell_on_edge_iterator coeit(*eocit) ; coeit.valid() ; ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  edge_t edge;
                  edge[0] = (*eocit).handle1(); edge[1] = (*eocit).handle2();
                  if(edge[0] < edge[1])
                     std::swap(edge[0], edge[1]);

                  int trig_found = 0;

                  // [INFO] only use non-manifold edges
                  //
                  if (count != 2 && used_edges[edge] != 1)
                  {
                     std::cout << "..base trig: " << (*cit) << std::endl;
                     std::cout << "..nm-edge: " << (*eocit).handle1() << "-" << (*eocit).handle2() << std::endl;

                     long change_trig_handle = -1;
                     long first_trig_handle  = -1;
                     long second_trig_handle = -1;

                     for(cell_on_edge_iterator coeit(*eocit); coeit.valid() && !trig_found; ++coeit)
                     {
                        // only use the correctly oriented triangle
                        //
                        if ((*coeit).handle() == (*cit).handle())
                           continue;

#ifdef DEBUGALL
                        std::cout << ".. checking triangle: " << (*coeit).handle() << " ";
                        std::cout << "cell vertices: ";
                        vertex_on_cell_iterator vocit(*coeit);
                        while (vocit.valid())
                        {
                           std::cout << (*vocit).handle() << " ";
                           vocit++;
                        }
                        std::cout << std::endl;
#endif

                        bool is_oriented_consistently =
                           gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell( (*coeit).handle() ),
                                                         (*segit).retrieve_topology().get_cell( (*cit).handle() )  );

                        // [INFO] exclude the triangle on the exact opposite side
                        //
//                        if(is_oriented_consistently)
                        {
                           std::cout << "..a match found" << std::endl;

                           point_t base_point;
                           long base_point_handle;
                           int pi;
                           int pc = 0;

                           vertex_on_cell_iterator vocit(*coeit);
                           std::cout << "cell vertices: ";

                           while (vocit.valid())
                           {
                              if((*vocit).handle() != (*eocit).handle1() && (*vocit).handle() != (*eocit).handle2())
                              {
                                 base_point = domain.get_point(*vocit);
                                 base_point_handle = (*vocit).handle();
                                 std::cout << ".. base point: " << (*vocit).handle() ;
                                 pi = pc;
                              }
//                               std::cout << (*vocit) << " ";
                              vocit++;
                              pc++;
                           }
                           std::cout << std::endl;

                           edge_on_cell_iterator eocit2(*cit);
                           for( ; eocit2.valid() && !trig_found ; ++eocit2)
                           {
                              std::cout << "..edge: " << (*eocit2).handle1() << " " << (*eocit2).handle2() << std::endl;

                              int count2 = 0;
                              for(cell_on_edge_iterator coeit2(*eocit2); coeit2.valid() ; ++coeit2, ++count2) ;

                              // [INFO] only use triangles not connected on the non-manifold edge
                              //
                              if (count2 == 2)
                              {
                                 for(cell_on_edge_iterator coeit2(*eocit2); coeit2.valid() && !trig_found ; ++coeit2)
                                 {
                                    std::cout << ".. coeit2: " << (*coeit2) << std::endl;

                                    vertex_on_cell_iterator vocit2(*coeit2);

                                    int found_edge = 0;
                                    int found_trig = 0;
                                    while (vocit2.valid() && !trig_found)
                                    {
                                       std::cout << " " << (*vocit2).handle();
                                       if((*vocit2).handle() == base_point_handle)
                                       {
                                          found_trig = 1;
                                       }
                                       if((*vocit2).handle() == edge[0])
                                       {
                                          found_edge = 1;
                                       }
                                       if(found_edge && found_trig)
                                       {
                                          std::cout << ".. connection to other trig found " << std::endl ;
                                          used_edges[edge] = 1;
                                          trig_found = 1;

                                          change_trig_handle = (*coeit2).handle();
                                          first_trig_handle  = (*coeit).handle();
                                          second_trig_handle = (*cit).handle();
                                       }
                                       vocit2++;
                                    }
                                    std::cout << std::endl;
                                 }
                              }
                              std::cout << "..after count==2 " << std::endl;
                           }
                        }
                     }

                     // apply changes
                     //
                     if(trig_found)
                     {
//                         vertex_on_cell_iterator vocit3(*coeit2);
//                         long pi = 0;
//                         while (vocit3.valid())
                        for(int pi=0; pi < 3; pi++)
                        {
                           std::cout << " " << (*segit).retrieve_topology().get_cell(change_trig_handle)[pi] << std::endl;

                           for(int ei=0; ei < 2; ei++)
                           {
                              if((*segit).retrieve_topology().get_cell(change_trig_handle)[pi] == edge[ei])
                              {
                                 // [INFO] nm-edge handling
                                 //
                                 std::cout << ".. nm-edge point[" << ei << "] found: " << edge[ei] << std::endl ;

//                                  point_t new_point;
//                                  new_point = domain.get_point_fromhandle(edge[ei]);

                                 // [INFO] use barycenter calculation for new point
                                 //        => new point is inside the trig but not the original point
                                 //
                                 boost::array<point_t, 3> test_cell;
                                 test_cell[0] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[0]);
                                 test_cell[1] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[1]);
                                 test_cell[2] =
                                    domain.get_point_fromhandle((*segit).retrieve_topology().get_cell(change_trig_handle)[2]);

                                 point_t bary_point = gsse::barycenter(test_cell.begin(), test_cell.end());

                                 domain.fast_point_insert(bary_point);

                                 std::cout << ".. added new point: " << domain.point_size()-1 << std::endl;

                                 segment_iterator segit2 = domain.segment_begin();
                                 for( ; segit2 != domain.segment_end(); ++segit2)
                                 {
                                    if(segit != segit2)
                                    {
                                       cell_iterator cit2;
                                       for (cit2 = (*segit2).cell_begin(); cit2 != (*segit2).cell_end(); ++cit2)
                                       {

                                          if(compare_triangles(segit2, (*cit2).handle(), segit, first_trig_handle))
                                          {
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }

                                          // second trig of nm-edge
                                          //
                                          if(compare_triangles(segit2, (*cit2).handle(), segit, second_trig_handle))
                                          {
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }

                                          // third trig but not of nm-edge
                                          //
                                          if(compare_triangles(segit2, (*cit2).handle(), segit, change_trig_handle))
                                          {
                                             std::cout << ".. cell found .. " << (*cit2) << std::endl;
                                             for(int ci=0; ci<3; ci++)
                                             {
                                                if(edge[0] == (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci])
                                                   (*segit2).retrieve_topology().get_cell((*cit2).handle())[ci] = domain.point_size()-1;
                                             }
                                          }
                                       }
                                    }
                                 }

                                 (*segit).retrieve_topology().get_cell(change_trig_handle)[pi] = domain.point_size()-1;

                                 for(int ci=0; ci<3; ci++)
                                 {
                                    if(edge[0] == (*segit).retrieve_topology().get_cell(first_trig_handle)[ci])
                                       (*segit).retrieve_topology().get_cell(first_trig_handle)[ci] = domain.point_size()-1;
                                 }

                                 for(int ci=0; ci<3; ci++)
                                 {
                                    if(edge[0] == (*segit).retrieve_topology().get_cell(second_trig_handle)[ci])
                                       (*segit).retrieve_topology().get_cell(second_trig_handle)[ci] = domain.point_size()-1;
                                 }

                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }






                        // ################################################################
                        // target are the points of both triangles under investigation
                        //
//                            if(test_point == 8274)
//                            {
//                               std::vector<long> target;
//                               target.push_back(8272);
//                               target.push_back(3306);
// //                               target.push_back(8274);
// //                               target.push_back(3298);

//                               std::cout << ".. starting nearest match .. " << std::endl;

//                               long max_counter = 10;
//                               long result = find_nearest_match(domain, segit, cit, edge, target, max_counter);
//                               if(result != -1)
//                                  std::cout << ".. found .. " << result << std::endl;
//                               else
//                                  std::cout << ".. NOT found .. " << result << std::endl;

//                               std::cout << ".. finishing .. " << std::endl;
//                            }

                        // ################################################################



//                            point_t new_point1;
//                            point_t new_point2;

//                            // [TODO] quick hack ... redo if this idea works !!!
//                            if(pi == 0)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[1]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[2]);
//                            }
//                            else if(pi == 1)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[0]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[2]);
//                            }
//                            else if(pi == 2)
//                            {
//                               new_point1 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[0]);
//                               new_point2 = domain.get_point_fromhandle((*segit).retrieve_topology().get_cell((*cit).handle())[1]);
//                            }

// //                           new_point1[2] += 0.0001;
// //                            new_point2[2] += 0.0001;
//                            std::cout << "..point1: " << new_point1 << std::endl;
//                            std::cout << "..point2: " << new_point2 << std::endl;

// //                            domain.fast_point_insert(new_point1);
//                            domain.fast_point_insert(new_point2);

//                            (*segit).retrieve_topology().get_cell((*cit).handle())[0] = base_point_handle;

//                            if(pi == 0)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[1] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[2] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }
//                            else if(pi == 1)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[0] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[2] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }
//                            else if(pi == 2)
//                            {
// //                               (*segit).retrieve_topology().get_cell((*cit).handle())[0] = domain.point_size()-2;
//                               (*segit).retrieve_topology().get_cell((*cit).handle())[1] = domain.point_size()-1;
//                               (*segit).retrieve_topology().get_cell((*coeit).handle())[1] = domain.point_size()-1;
//                            }

//                            std::cout << "..new trig: " << (*cit).handle()
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[0]
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[1]
//                                      << " " << (*segit).retrieve_topology().get_cell((*cit).handle())[2]
//                                      << std::endl;

//                            bool is_oriented_consistently =
//                               gsse::check_oriented_neighbor((*segit).retrieve_topology().get_cell((*coeit).handle()),
//                                                             (*segit).retrieve_topology().get_cell((*cit).handle()) );

//                            std::cout << ".. is_oriented_consistently: " << is_oriented_consistently << std::endl;

//                            used_edges[edge] = 0;



      std::cout << ".. before checking orientation the second time .. " << std::endl;

      check_and_repair_orientation_new(domain);

      std::cout << ".. checking again for non-manifold edges .. " << std::endl;

      {
         segment_iterator segit = domain.segment_begin();
         for( ; segit != domain.segment_end(); ++segit)
         {
#ifdef DEBUGALL
            std::cout << ".. seg: " << (*segit) << std::endl;
#endif
            cell_iterator cit;
            for (cit = (*segit).cell_begin(); cit != (*segit).cell_end(); ++cit)
            {
               edge_on_cell_iterator eocit(*cit);
               for( ;eocit.valid(); ++eocit)
               {
                  int count = 0;
                  edge_t new_edge;
                  cell_on_edge_iterator coeit(*eocit);
                  for (; coeit.valid(); ++coeit, ++count) ;

                  // [INFO] premises: there are no holes in the input
                  //
                  if (count != 2)
                  {
                     new_edge[0] = (*eocit).handle1();
                     new_edge[1] = (*eocit).handle2();

                     non_manifold_edges.push_back(new_edge);

                     std::cout << ".. non_manifold_edge found: " << new_edge[0] << " " << new_edge[1]
                               << " .. count: " << count <<  std::endl;
                  }
                  else
                  {
#ifdef DEBUGFULL
                     std::cout << ".. manifold_edge: " << (*eocit).handle1() << " " << (*eocit).handle2()
                               << " .. count: " << count <<  std::endl;
#endif
                  }
               }
            }
         }
      }

      domain.write_file("fs_output_test.gau32");

    }

      std::cout << "..point size: " << domain.point_size() << std::endl;

      trias.SetSize(0);
      points.SetSize(0);

//      PrintMessage(3,"number of triangles = ", readtrigs.Size());

      // [FS][MOD] set the size for multiple surfaces
      //
      material_size = domain.segment_size();

      // [FS][TODO] .. maybe this can be removed ?
      //           check if the bounding box really matters
      //
      segment_iterator segit = domain.segment_begin();
      for( ; segit != domain.segment_end(); ++segit)
	{
	  cell_iterator cit = (*segit).cell_begin();
	  for( ; cit != (*segit).cell_end(); ++cit, i++)
	    {
	      vertex_on_cell_iterator vocit(*cit);
	      while(vocit.valid())
		{
		  point_t point = domain.get_point(*vocit);
		  Point<3> p(point[0],point[1],point[2]);
		  boundingbox.Add(p);
		  vocit++;
		}
	    }
	}

#ifdef DEBUGALL
      std::cout << "boundingbox: " << Point3d(boundingbox.PMin()) << " - " << Point3d(boundingbox.PMax()) << std::endl;
#endif

      ARRAY<int> pintersect;
      Box<3> bb = boundingbox;
      bb.Increase (1);

      pointtree = new Point3dTree (bb.PMin(), bb.PMax());

      typedef std::map<point_t, vertex_handle>::iterator point_map_iterator;
      boost::array<vertex_handle, 3>                       element_container;

      std::map<std::vector<int>, int>        triangle_map;

      long element_counter = 1;
      long triangle_counter = 1;
      i=0;
      int material_number = 0;
      segit = domain.segment_begin();
      for( ; segit != domain.segment_end(); ++segit, ++material_number)
      {
         cell_iterator cit = (*segit).cell_begin();

	  std::map<point_t, vertex_handle> point_map;

	  for( ; cit != (*segit).cell_end(); ++cit, i++)
	    {
	      vertex_on_cell_iterator vocit(*cit);

              int actual_count = 0;

	      for (int element_index = 0; element_index < 3; element_index++, ++vocit)
              {
                 if((*vocit).handle() != -1)
                 {
                    point_t point = domain.get_point(*vocit);

                    point_map_iterator pmit = point_map.find(point);
#ifdef DEBUGALL
                    std::cout << "..checking point: " << (*vocit).handle() << " :: " << point << std::endl;
#endif
                    if(pmit == point_map.end())
                    {
                       vgmnetgen::Point<3> p(point[0],point[1],point[2]);

                       vgmnetgen::Point<3> pmin = p - vgmnetgen::Vec<3> (pointtol, pointtol, pointtol);
                       vgmnetgen::Point<3> pmax = p + vgmnetgen::Vec<3> (pointtol, pointtol, pointtol);

                       pointtree->GetIntersecting (pmin, pmax, pintersect);

                       if (pintersect.Size() > 1)
                          std::cout << "too many close points" << std::endl;

                       int foundpos = -1;

                       // [INFO] ensure that a point is only added once => topological and geometrical unique
                       //
                       if (pintersect.Size())
                          foundpos = pintersect[0];

#ifdef DEBUGALL
                       std::cout << "..foundpos: " << foundpos << std::endl;
#endif

                       if (foundpos == -1)
                       {
#ifdef DEBUGALL
                          std::cout << "..adding point: " << p << std::endl;
#endif
                          foundpos = AddPoint(p);
                          pointtree->Insert (p, foundpos);
                       }

                       point_map[point] = foundpos;
                       element_container[element_index] = foundpos;

                       element_counter++;
                    }
                    else
                    {
                       element_container[element_index] = (*pmit).second;
                    }

                    actual_count++;

                    // 	      Point<3> pmin = p - Vec<3> (pointtol, pointtol, pointtol);
                    // 	      Point<3> pmax = p + Vec<3> (pointtol, pointtol, pointtol);

                    // 	      pointtree->GetIntersecting (pmin, pmax, pintersect);

                    // 	      std::cout << ".. after intersection test: " << std::endl;

                    // 	      if (pintersect.Size() > 1)
                    // 		PrintError("too many close points");

                    // 	      int foundpos = -1;

                    // 	      // [FS] .. test: add the interface point a second time
                    // 	      //

                    // 	      if (pintersect.Size())
                    // 		foundpos = pintersect[0];

                    // 	      std::cout << ".. foundpos: " << foundpos << " :: for point: " << p <<  std::endl;

                    // 	      if (foundpos == -1)
                    // 		{
                    // 		  foundpos = AddPoint(p);
                    // 		  pointtree->Insert (p, foundpos);
                    // 		}
                    // 	      st[k] = foundpos;
                 }
              }

              if(actual_count == 3)
              {

	      // [INFO] .. calculate the normals from the gsse structure
	      //
 	      STLTriangle st;
 	      Vec<3> normal = Cross (GetPoint(element_container[1])-GetPoint(element_container[0]),
				     GetPoint(element_container[2])-GetPoint(element_container[0]));
	      normal.Normalize();
	      st.SetNormal (normal);
	      st.material[0] = material_number;
	      st.material[1] = -1;

	      st[0] = element_container[0];
	      st[1] = element_container[1];
	      st[2] = element_container[2];

#ifdef DEBUGALL
	      std::cout << "..triangle_counter: " << triangle_counter << std::endl;
	      std::cout << "..st[0]: " << st[0] << std::endl;
	      std::cout << "..st[1]: " << st[1] << std::endl;
	      std::cout << "..st[2]: " << st[2] << std::endl;

              std::cout << "..normal: " << normal << std::endl;
              std::cout << "..material: " << st.material[0] << std::endl;
#endif

	      std::vector<int> triangle;
	      triangle.push_back(st[0]);
	      triangle.push_back(st[1]);
	      triangle.push_back(st[2]);

	      // [FS][INFO] this is done to for searching in the triangle_map
	      //
	      sort(triangle.begin(), triangle.end());

	      std::map<std::vector<int>, int>::iterator tmit;
	      tmit = triangle_map.find(triangle);

	      if(tmit == triangle_map.end())
		{
#ifdef DEBUGALL
		  std::cout << ".. adding new triangle" << std::endl;
#endif

		  triangle_map[triangle] = triangle_counter;

		  if ( (st[0] == st[1]) ||
		       (st[0] == st[2]) ||
		       (st[1] == st[2]) )
		    {
		      PrintError("STL Triangle degenerated");
		    }
		  else
                  {
                     AddTriangle(st);
                     triangle_counter++;
                  }
		}
	      else
		{
		  // store the second material information onto the triangle
		  //
#ifdef DEBUGALL
		  std::cout << "..triangle found: triangle_counter: " << (*tmit).second << " :: materialnum: " << material_number << std::endl;
#endif

		  GetTriangle((*tmit).second).material[1] = material_number;
		}
              }
	    }

      }

    } // if netgen


  FindNeighbourTrigs();

#ifdef DEBUGALL
  std::cout << "[FS] .. end of InitSTLGeometry: point size: " << GetNP() << std::endl;
#endif
}




int STLTopology :: GetPointNum (const Point<3> & p)
{
  Point<3> pmin = p - Vec<3> (pointtol, pointtol, pointtol);
  Point<3> pmax = p + Vec<3> (pointtol, pointtol, pointtol);

  ARRAY<int> pintersect;

  pointtree->GetIntersecting (pmin, pmax, pintersect);
  if (pintersect.Size() == 1)
    return pintersect[0];
  else
    return 0;
}



void STLTopology :: FindNeighbourTrigs()
{
  //  if (topedges.Size()) return;
#ifdef DEBUGALL
   std::cout << "[FS] .. ENTER Find Neighbour Triangles" << std::endl;
#endif

  int i, j, k, l;

  // build up topology tables

  int np = GetNP();
  int nt = GetNT();

#ifdef DEBUGALL
  std::cout << " .. np: " << np << std::endl;
  std::cout << " .. nt: " << nt << std::endl;
#endif

//   INDEX_2_HASHTABLE<int> * oldedges = ht_topedges;
//   ht_topedges = new INDEX_2_HASHTABLE<int> (GetNP()+1);

  INDEX_3_HASHTABLE<int> ht_topedges(GetNP()+1);

  topedges.SetSize(0);

  for(int m=0; m<material_size; m++)
  {
     for (i = 1; i <= nt; i++)
     {
        STLTriangle & trig = GetTriangle(i);

#ifdef DEBUGALL
        std::cout << "..trig: " << i << " " << trig << " :: mat1: " << trig.material[0] << " :: " << " :: mat2: " << trig.material[1] << " :: ";
#endif

        // [INFO] only use the first material, because the orientation in the first mat is the correct one
        //
        if(trig.material[0] == m)
        {
#ifdef DEBUGALL
           std::cout << ".. mat: " << m << std::endl;
#endif

           // [INFO] go through the edges of the triangle
           //
           for (j = 1; j <= 3; j++)
           {
              int pi1 = trig.PNumMod (j+1);
              int pi2 = trig.PNumMod (j+2);

              INDEX_2 temp(pi1, pi2);
              temp.Sort();
              INDEX_3 i2(temp.I1(), temp.I2(), m);

#ifdef DEBUGALL
              std::cout << "..temp.I1: " << temp.I1() << " .. temp.I2: " << temp.I2() << std::endl;
              std::cout << "..p1: " << pi1 << " :: p2: "<< pi2 << " ";
#endif

              int enr;
              int othertn;

              if (ht_topedges.Used(i2))
              {
#ifdef DEBUGALL
//                  std::cout << "..here 4 " << std::endl;
#endif

                 enr = ht_topedges.Get(i2);

#ifdef DEBUGALL
                 std::cout << ".. found enr: " << enr << std::endl;
#endif

                 if(trig.material[1] != -1)
                 {
                    INDEX_3 m2(temp.I1(), temp.I2(), trig.material[1]);

#ifdef DEBUGALL
                    std::cout << ".. in material[1]: " << trig.material[1]
                              << " ..temp.I1: " << temp.I1()
                              << " .. temp.I2: " << temp.I2()
                              << std::endl;
#endif

                    ht_topedges.Set(m2, enr);
                 }

                 // [FS] .. not used anymore
                 // 	      topedges.Elem(enr).TrigNum(2) = i;

//                  if(topedges.Elem(enr).segment_info[trig.material[0]].trigs[0] == -1)
//                  {
//                     meta_info_t meta_info;
//                     meta_info.material = trig.material[0];
// // 			  meta_info.status = -1;

//                     std::cout << ".. before m == 1 .. " << std::endl;

//                     meta_info.trigs[0] =  i;
//                     meta_info.trigs[1] = -1;

//                     std::cout << ".. after m == 1 .. " << std::endl;

//                     topedges.Elem(enr).segment_info[trig.material[0]] = meta_info;

//                     std::cout << " start1 mat: " << trig.material[0]
//                               << " trig1: " << i
//                               << " end1";
//                  }
//                  else
                 if(topedges.Elem(enr).segment_info[trig.material[0]].trigs[0] != -1)
                 {
                    topedges.Elem(enr).segment_info[trig.material[0]].material = trig.material[0];
                    topedges.Elem(enr).segment_info[trig.material[0]].trigs[1] = i;

#ifdef DEBUGALL
                    std::cout << " start2 mat: " << trig.material[0]
                              << " trig1: " << topedges.Elem(enr).segment_info[trig.material[0]].trigs[0]
                              << " trig2: " << topedges.Elem(enr).segment_info[trig.material[0]].trigs[1]
                              << " end2";
#endif

                    if(trig.material[1] != -1)
                    {
                       topedges.Elem(enr).segment_info[trig.material[1]].material = trig.material[1];
                       topedges.Elem(enr).segment_info[trig.material[1]].trigs[0] = i;

#ifdef DEBUGALL
                       std::cout << " start3 mat: " << trig.material[1]
                                 << " trig1: " << topedges.Elem(enr).segment_info[trig.material[1]].trigs[0]
                                 << " trig2: " << topedges.Elem(enr).segment_info[trig.material[1]].trigs[1]
                                 << " end2";
#endif
                    }
                 }
                 else if(topedges.Elem(enr).segment_info[trig.material[0]].trigs[0] == -1)
                 {
                    topedges.Elem(enr).segment_info[trig.material[0]].material = trig.material[0];
                    topedges.Elem(enr).segment_info[trig.material[0]].trigs[0] = i;

#ifdef DEBUGALL
                    std::cout << " start4 mat: " << trig.material[0]
                              << " trig1: " << topedges.Elem(enr).segment_info[trig.material[0]].trigs[0]
                              << " trig2: " << topedges.Elem(enr).segment_info[trig.material[0]].trigs[1]
                              << " end2";
#endif

                    if(trig.material[1] != -1)
                    {
                       topedges.Elem(enr).segment_info[trig.material[1]].material = trig.material[1];
                       topedges.Elem(enr).segment_info[trig.material[1]].trigs[1] = i;

#ifdef DEBUGALL
                       std::cout << " start5 mat: " << trig.material[1]
                                 << " trig1: " << topedges.Elem(enr).segment_info[trig.material[1]].trigs[0]
                                 << " trig2: " << topedges.Elem(enr).segment_info[trig.material[1]].trigs[1]
                                 << " end2";
#endif
                    }
                 }
                 else
                 {
                    std::cout << ".. topology does not match - maybe a hole ???? " << std::endl;
                 }

                 // [FS][INFO] set the neighbouthood information
                 //
                 // [FS] not used anymore
                 //   	      othertn = topedges.Get(enr).TrigNum(1);
                 othertn = topedges.Get(enr).segment_info[trig.material[0]].trigs[0];
                 STLTriangle & othertrig = GetTriangle(othertn);

#ifdef DEBUGALL
                 std::cout << " othertrig: " << othertrig << " :: ";
#endif

                 // [FS] not used anymore
                 trig.NBTrigNum(j) = othertn;
                 trig.segment_info[trig.material[0]].nbtrigs[0][j-1] = othertn;

                 trig.EdgeNum(j) = enr;
                 for (k = 1; k <= 3; k++)
                    if (othertrig.EdgeNum(k) == enr)
                    {
                       // [FS] not used anymore
                       othertrig.NBTrigNum(k) = i;
                       othertrig.segment_info[trig.material[0]].nbtrigs[0][k-1] = i;
                    }
              }
              else
              {
// 	      enr = topedges.Append (STLTopEdge (pi1, pi2, i, 0));

#ifdef DEBUGALL
//                  std::cout << "..here 5 " << std::endl;
#endif
                 STLTopEdge etp = STLTopEdge (pi1, pi2, i, 0);

#ifdef DEBUGALL
//                  std::cout << "..here 6 " << std::endl;
#endif

                 meta_info_t meta_info = meta_info_t();

                 // [FS][MOD] set the metainformation for the adjacent triangles
                 //
#ifdef DEBUGALL
                 std::cout << ".. correct triangle" << std::endl;
#endif

                 meta_info.material = trig.material[0];
// 		    meta_info.status = -1;
                 meta_info.trigs[0] =  i;
                 meta_info.trigs[1] = -1;


                 etp.segment_info[trig.material[0]] = meta_info;

                 if(trig.material[1] != -1)
                 {
                    meta_info.material = trig.material[1];
//                  meta_info.status = -1;
                    meta_info.trigs[0] = -1;
                    meta_info.trigs[1] =  i;

                    etp.segment_info[trig.material[1]] = meta_info;

#ifdef DEBUGALL
                    std::cout << ".. material[1]: " << trig.material[1] << std::endl;
#endif
                 }

//                  std::cout << " mat: " << trig.material[0];

                 enr = topedges.Append(etp);

#ifdef DEBUGALL
//                  std::cout << "..here 1 " << std::endl;
#endif

                 ht_topedges.Set(i2, enr);

                 if(trig.material[1] != -1)
                 {
                    INDEX_3 m2(temp.I1(), temp.I2(), trig.material[1]);
#ifdef DEBUGALL
                    std::cout << ".. in material[1]: " << trig.material[1] << " ..temp.I1: " << temp.I1() << " .. temp.I2: " << temp.I2() << std::endl;
#endif
                    ht_topedges.Set(m2, enr);
                 }

#ifdef DEBUGALL
//                  std::cout << "..here 2 .. enr: " << enr << std::endl;
#endif

                 trig.EdgeNum(j) = enr;

#ifdef DEBUGALL
//                  std::cout << "..here 3 " << std::endl;
#endif
              }

#ifdef DEBUGALL
              std::cout << " / ";
#endif
           }

#ifdef DEBUGALL
           std::cout << std::endl;
#endif
        }
     }
  }

#ifdef DEBUGALL
  std::cout << ".. number of top edges: " << topedges.Size() << std::endl;
#endif

//  PrintMessage(5,"topology built, checking");

#ifdef DEBUGALL
  for(int p=0; p < GetNP(); p++)
    {
      std::cout << "..point: " << p << " :: " << points[p] << std::endl;
    }
#endif


  // [FS][MOD] modify these checks
  //
  topology_ok = 1;
  int ne = GetNTE();

  for (i = 1; i <= nt; i++)
    GetTriangle(i).flags.toperror = 0;


  // [FS][INFO] included material information to the check for consistent neighbours
  //
  for (i = 1; i <= nt; i++)
    {
      int mat = GetTriangle(i).material[0];

      for (j = 1; j <= 3; j++)
	{
	  const STLTopEdge & edge = GetTopEdge (GetTriangle(i).EdgeNum(j));

	  if (edge.segment_info[mat].trigs[0] != i && edge.segment_info[mat].trigs[1] != i)
	    {
	      topology_ok = 0;
	      GetTriangle(i).flags.toperror = 1;

#ifdef DEBUGALL
              std::cout << ".. ERROR trig: " << i
                        << ".. trig[0]: " << edge.segment_info[mat].trigs[0]
                        << ".. trig[1]: " << edge.segment_info[mat].trigs[1]
                        << ".. mat: " << mat
                        << std::endl;
#endif
	    }
	}
    }

#ifdef DEBUGALL
  std::cout << ".. topology_ok: " << topology_ok << std::endl;
#endif

  // [FS][INFO] also added the material information: if the material is set but the second trig is set to -1
  //            => error . topology not consistent
  //
  for (i = 1; i <= ne; i++)
    {
       STLTopEdge & edge = GetTopEdge (i);

       // [FS][TODO] set the maximum number of segments here
       //
       for(int m=0; m < material_size; m++)
	 {
#ifdef DEBUGALL
	   std::cout << "..edge segment info test: "
		     << " p1: " << edge.PNum(1) << " :: p2: " << edge.PNum(2)
		     << " mat: "   << edge.segment_info[m].material << " :: "
		     << " trig1: " << edge.segment_info[m].trigs[0] << " :: "
		     << " trig2: " << edge.segment_info[m].trigs[1] << " :: "  << std::endl;
#endif

	   if (edge.segment_info[m].material != -1 && edge.segment_info[m].trigs[1] == -1)
	     {
	       topology_ok = 0;
	       GetTriangle(edge.TrigNum(1)).flags.toperror = 1;

#ifdef DEBUGALL
	       std::cout << "ERROR..edge segment info test: "
			 << " p1: " << edge.PNum(1) << " :: p2: " << edge.PNum(2)
			 << " mat: "   << edge.segment_info[m].material << " :: "
			 << " trig1: " << edge.segment_info[m].trigs[0] << " :: "
			 << " trig2: " << edge.segment_info[m].trigs[1] << " :: "  << std::endl;
#endif
	     }

	 }
    }

#ifdef DEBUGALL
  std::cout << ".. topology_ok: " << topology_ok << std::endl;
#endif

//   if (topology_ok)
//     {
//       orientation_ok = 1;
//       for (i = 1; i <= nt; i++)
// 	{
// 	  const STLTriangle & t = GetTriangle (i);

// 	  std::cout << ".. t: " << t;

// 	  // [FS][MOD] changed to go through the adjacent materials of each triangle
// 	  //
// 	  for(int m=0; m<2; m++)
// 	    {
// 	      if(t.material[m] != -1)
// 		{
// 		  for (j = 1; j <= 3; j++)
// 		    {
// 		      // [FS] not used anymore
// // 		      const STLTriangle & nbt = GetTriangle(t.NBTrigNum(j));
// 		      const STLTriangle & nbt = GetTriangle(t.segment_info[t.material[m]].nbtrigs[0][j-1]);
// 		      std::cout << " :: nbt: " << nbt;
// 		      if (!t.IsNeighbourFrom (nbt))
// 			{
// 			  std::cout << std::endl << "..NOT NB: t: " << t << " :: " << " nbt: " << nbt << std::endl;
// 			  std::cout << "t   p1: " << points[t[0]-1] << " :: p2: " << points[t[1]-1] << " :: p3: " << points[t[2]-1] << std::endl;
// 			  std::cout << "nbt p1: " << points[nbt[0]-1] << " :: p2: " << points[nbt[1]-1] << " :: p3: " << points[nbt[2]-1] << std::endl;

// 			  // [FS][TODO] change this back - check orientation
// 			  //
// //   			  orientation_ok = 0;
// 			}
// 		    }
// 		}
// 	    }
// 	  std::cout << std::endl;
// 	}
//     }
//   else
//     orientation_ok = 0;



  status = STL_GOOD;
  statustext = "";
//   std::cout << topology_ok << " " << orientation_ok << std::endl;
  if (!topology_ok || !orientation_ok)
    {
      status = STL_ERROR;

#ifdef DEBUGALL
      if (!topology_ok)
	statustext = "Topology not ok";
      else
	statustext = "Orientation not ok";
#endif
    }

#ifdef DEBUGALL
  PrintMessage(3,"topology_ok = ",topology_ok);
  PrintMessage(3,"orientation_ok = ",orientation_ok);
  PrintMessage(3,"topology found");
#endif

  // [FS][INFO] generate the point to trig table
  //
  trigsperpoint.SetSize(GetNP());
  for (i = 1; i <= GetNT(); i++)
    for (j = 1; j <= 3; j++)
      trigsperpoint.Add1(GetTriangle(i).PNum(j),i);

#ifdef DEBUGALL
  // [FS] debug info
  for(i = 1; i<=GetNP(); i++)
    {
      int trig;
//       trigsperpoint.Get(i, trig);
//       std::cout << ".. trigs at point: " << i << " :: " << GetTriangle(trig) << std::endl;
      std::cout << ".. no trigs at point: " << i << " :: "  << NOTrigsPerPoint(i) << std::endl;
    }
#endif

#ifdef DEBUGALL
  // check trigs per point:
  for (i = 1; i <= GetNP(); i++)
    {
      if (trigsperpoint.EntrySize(i) < 3 )
	{
	  std::cout << "ERROR: Point " << i << " has " << trigsperpoint.EntrySize(i) << " triangles!!!" << std::endl;
	}
    }
#endif

  // [FS][INFO] generate the topedges per point datastructure
  //
  topedgesperpoint.SetSize (GetNP());
  for (i = 1; i <= ne; i++)
    for (j = 1; j <= 2; j++)
      topedgesperpoint.Add1 (GetTopEdge (i).PNum(j), i);


#ifdef DEBUGALL
  // [FS] debug info
  for(i = 1; i<=GetNP(); i++)
    {
      int trig;
//       trigsperpoint.Get(i, trig);
//       std::cout << ".. trigs at point: " << i << " :: " << GetTriangle(trig) << std::endl;
      std::cout << ".. no top edges at: " << i << " :: "  << NTopEdgesPerPoint(i) << std::endl;
    }
#endif

//  PrintMessage(5,"point -> trig table generated");

#ifdef DEBUGALL
  for(i = 1; i<GetNT(); i++)
    {
      std::cout << "..trig: " << i << " :: " << GetTriangle(i) << " :: ";

      for(int m=0;m<2; m++)
	{
	  std::cout << " mat: " << m << " ::"
		    << " " << GetTriangle(i).segment_info[m].nbtrigs[0][0]
		    << " " << GetTriangle(i).segment_info[m].nbtrigs[0][1]
		    << " " << GetTriangle(i).segment_info[m].nbtrigs[0][2];
	}
      std::cout << std::endl;
    }
#endif


  // transfer edge data:
  // .. to be done
//  delete oldedges;


  // [FS][INFO]
  //
  for (STLTrigIndex ti = 0; ti < GetNT(); ti++)
    {
      STLTriangle & trig = trias[ti];
      for (k = 0; k < 3; k++)
	{
	  // [INFO] STLBASE = 1
	  //
	  STLPointIndex pi = trig[k] - STLBASE;
	  STLPointIndex pi2 = trig[(k+1)%3] - STLBASE;
	  STLPointIndex pi3 = trig[(k+2)%3] - STLBASE;

	  // vector along edge
	  Vec<3> ve = points[pi2] - points[pi];
	  ve.Normalize();

	  // vector along third point
	  Vec<3> vt = points[pi3] - points[pi];
	  vt -= (vt * ve) * ve;
	  vt.Normalize();

	  Vec<3> vn = trig.GeomNormal (points);
	  vn.Normalize();

	  double phimin = 10, phimax = -1; // out of (0, 2 pi)

	  for (j = 0; j < trigsperpoint[pi].Size(); j++)
	    {
	      STLTrigIndex ti2 = trigsperpoint[pi][j] - STLBASE;
	      const STLTriangle & trig2 = trias[ti2];

	      if (ti == ti2) continue;

	      bool hasboth = 0;
	      for (l = 0; l < 3; l++)
		if (trig2[l] - STLBASE == pi2)
		  {
		    hasboth = 1;
		    break;
		  }
	      if (!hasboth) continue;


	      // [FS][TODO] check what is done here ???
	      //
	      STLPointIndex pi4;
	      for (l = 0; l < 3; l++)
		if (trig2[l] - STLBASE != pi && trig2[l] - STLBASE != pi2)
		  pi4 = trig2[l] - STLBASE;

	      Vec<3> vt2 = points[pi4] - points[pi];

	      double phi = atan2 (vt2 * vn, vt2 * vt);
	      if (phi < 0) phi += 2 * M_PI;

	      if (phi < phimin)
		{
		  phimin = phi;
		  trig.NBTrig (0, (k+2)%3) = ti2 + STLBASE;
		}
	      if (phi > phimax)
		{
		  phimax = phi;
		  trig.NBTrig (1, (k+2)%3) = ti2 + STLBASE;
		}
	      //
	      // [FS] check end

	    }
	}
    }

#ifdef DEBUGALL
  std::cout << " .. status: " << status << " :: STL_GOOD: " << STL_GOOD << std::endl;
#endif

  if (status == STL_GOOD)
    {
      // for compatibility:
      neighbourtrigs.SetSize(GetNT());
      for (i = 1; i <= GetNT(); i++)
// 	for(int m=0; m<2; m++)
	  {
// 	    if(GetTriangle(i).material[m] != -1)
	      {
		for (k = 1; k <= 3; k++)
		  {
 		    AddNeighbourTrig (i, GetTriangle(i).NBTrigNum(k));
// 		    AddNeighbourTrig (i, GetTriangle(i).segment_info[GetTriangle(i).material[m]].nbtrigs[0][k-1]);
// 		    std::cout << ".. adding nb trig to trig: " << i << " :: " << GetTriangle(i).segment_info[GetTriangle(i).material[m]].nbtrigs[0][k-1] << " :: " << GetTriangle(GetTriangle(i).segment_info[GetTriangle(i).material[m]].nbtrigs[0][k-1]) << std::endl;
		  }
	      }
	  }

    }
  else
    {
      // assemble neighbourtrigs (should be done only for illegal topology):

      neighbourtrigs.SetSize(GetNT());

      int tr, found;
      int wrongneighbourfound = 0;
      for (i = 1; i <= GetNT(); i++)
	{
	  SetThreadPercent((double)i/(double)GetNT()*100.);
	  if (multithread.terminate)
	    {
	      PopStatus();
	      return;
	    }

	  for (k = 1; k <= 3; k++)
	    {
	      for (j = 1; j <= trigsperpoint.EntrySize(GetTriangle(i).PNum(k)); j++)
		{
		  tr = trigsperpoint.Get(GetTriangle(i).PNum(k),j);
		  if (i != tr && (GetTriangle(i).IsNeighbourFrom(GetTriangle(tr))
				  || GetTriangle(i).IsWrongNeighbourFrom(GetTriangle(tr))))
		    {
		      if (GetTriangle(i).IsWrongNeighbourFrom(GetTriangle(tr)))
			{
// 			  (*testout) << "ERROR: triangle " << i << " has a wrong neighbour triangle!!!" << endl;
wrongneighbourfound ++;
			}

		      found = 0;
		      for (int ii = 1; ii <= NONeighbourTrigs(i); ii++)
			{if (NeighbourTrig(i,ii) == tr) {found = 1;break;};}
		      if (! found) {AddNeighbourTrig(i,tr);}
		    }
		}
	    }

#ifdef DEBUGALL
	  if (NONeighbourTrigs(i) != 3)
	    {
	      PrintError("TRIG ",i," has ",NONeighbourTrigs(i)," neighbours!!!!");
	      for (int kk=1; kk <= NONeighbourTrigs(i); kk++)
		{
		  PrintMessage(5,"neighbour-trig",kk," = ",NeighbourTrig(i,kk));
		}
	    };
#endif

	}
      if (wrongneighbourfound)
	{
#ifdef DEBUGALL
	  PrintError("++++++++++++++++++++\n");
	  PrintError(wrongneighbourfound, " wrong oriented neighbourtriangles found!");
	  PrintError("try to correct it (with stldoctor)!");
	  PrintError("++++++++++++++++++++\n");
#endif

	  status = STL_ERROR;
	  statustext = "STL Mesh not consistent";

	  multithread.terminate = 1;
#ifdef STAT_STREAM
	  (*statout) << "non-conform stl geometry \\hline" << endl;
#endif
	}
    }


  // [INFO] set calcedgedataanglesnew = 1
  //
  TopologyChanged();

  PopStatus();

#ifdef DEBUGALL
  std::cout << "[FS] .. EXIT Find Neighbour Triangles" << std::endl;
#endif
}







void STLTopology :: GetTrianglesInBox (/*
					  const Point<3> & pmin,
					  const Point<3> & pmax,
				       */
				       const Box<3> & box,
				       ARRAY<int> & trias) const
{
  if (searchtree)

    searchtree -> GetIntersecting (box.PMin(), box.PMax(), trias);

  else
    {
      int i;
      Box<3> box1 = box;
      box1.Increase (1e-4);

      trias.SetSize(0);

      int nt = GetNT();
      for (i = 1; i <= nt; i++)
	{
	  if (box1.Intersect (GetTriangle(i).box))
	    {
	      trias.Append (i);
	    }
	}
    }
}



void STLTopology :: AddTriangle(const STLTriangle& t)
{
  trias.Append(t);

  const Point<3> & p1 = GetPoint (t.PNum(1));
  const Point<3> & p2 = GetPoint (t.PNum(2));
  const Point<3> & p3 = GetPoint (t.PNum(3));

  Box<3> box;
  box.Set (p1);
  box.Add (p2);
  box.Add (p3);
  /*
  //  Point<3> pmin(p1), pmax(p1);
  pmin.SetToMin (p2);
  pmin.SetToMin (p3);
  pmax.SetToMax (p2);
  pmax.SetToMax (p3);
  */

  trias.Last().box = box;
  trias.Last().center = Center (p1, p2, p3);
  double r1 = Dist (p1, trias.Last().center);
  double r2 = Dist (p2, trias.Last().center);
  double r3 = Dist (p3, trias.Last().center);
  trias.Last().rad = max2 (max2 (r1, r2), r3);

  if (geomsearchtreeon)
    {searchtree->Insert (box.PMin(), box.PMax(), trias.Size());}
}




int STLTopology :: GetLeftTrig(int p1, int p2) const
{
  int i;
  for (i = 1; i <= trigsperpoint.EntrySize(p1); i++)
    {
      if (GetTriangle(trigsperpoint.Get(p1,i)).HasEdge(p1,p2)) {return trigsperpoint.Get(p1,i);}
    }
  PrintSysError("ERROR in GetLeftTrig !!!");

  return 0;
}

int STLTopology :: GetRightTrig(int p1, int p2) const
{
  return GetLeftTrig(p2,p1);
}


int STLTopology :: NeighbourTrigSorted(int trig, int edgenum) const
{
  int i, p1, p2;
  int psearch = GetTriangle(trig).PNum(edgenum);

  for (i = 1; i <= 3; i++)
    {
      GetTriangle(trig).GetNeighbourPoints(GetTriangle(NeighbourTrig(trig,i)),p1,p2);
      if (p1 == psearch) {return NeighbourTrig(trig,i);}
    }

  PrintSysError("ERROR in NeighbourTrigSorted");
  return 0;
}






int STLTopology :: GetTopEdgeNum (int pi1, int pi2) const
{
  if (!ht_topedges) return 0;

  INDEX_2 i2(pi1, pi2);
  i2.Sort();

  if (!ht_topedges->Used(i2)) return 0;
  return ht_topedges->Get(i2);
}




void STLTopology :: InvertTrig (int trig)
{
  if (trig >= 1 && trig <= GetNT())
    {
      GetTriangle(trig).ChangeOrientation();
      FindNeighbourTrigs();
    }
  else
    {
      PrintUserError("no triangle selected!");
    }
}




void STLTopology :: DeleteTrig (int trig)
{
  if (trig >= 1 && trig <= GetNT())
    {
      trias.DeleteElement(trig);
      FindNeighbourTrigs();
    }
  else
    {
      PrintUserError("no triangle selected!");
    }
}



void STLTopology :: OrientAfterTrig (int trig)
{
  int starttrig = trig;

  if (starttrig >= 1 && starttrig <= GetNT())
    {

      ARRAY <int> oriented;
      oriented.SetSize(GetNT());
      int i;
      for (i = 1; i <= oriented.Size(); i++)
	{
	  oriented.Elem(i) = 0;
	}

      oriented.Elem(starttrig) = 1;

      int j = 0,k;

      ARRAY <int> list1;
      list1.SetSize(0);
      ARRAY <int> list2;
      list2.SetSize(0);
      list1.Append(starttrig);

      int cnt = 1;
      int end = 0;
      int nt;
      while (!end)
	{
	  end = 1;
	  for (i = 1; i <= list1.Size(); i++)
	    {
	      const STLTriangle& tt = GetTriangle(list1.Get(i));
	      for (k = 1; k <= 3; k++)
		{
		  nt = tt.NBTrigNum (k); // NeighbourTrig(list1.Get(i),k);
		  if (oriented.Get(nt) == 0)
		    {
		      if (tt.IsWrongNeighbourFrom(GetTriangle(nt)))
			{
			  GetTriangle(nt).ChangeOrientation();
			}
		      oriented.Elem(nt) = 1;
		      list2.Append(nt);
		      cnt++;
		      end = 0;
		    }
		}
	    }
	  list1.SetSize(0);
	  for (i = 1; i <= list2.Size(); i++)
	    {
	      list1.Append(list2.Get(i));
	    }
	  list2.SetSize(0);
	}

      PrintMessage(5,"NO corrected triangles = ",cnt);
      if (cnt == GetNT())
	{
	  PrintMessage(5,"ALL triangles oriented in same way!");
	}
      else
	{
	  PrintWarning("NOT ALL triangles oriented in same way!");
	}

      //      topedges.SetSize(0);
      FindNeighbourTrigs();
    }
  else
    {
      PrintUserError("no triangle selected!");
    }
}


}
