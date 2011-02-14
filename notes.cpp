

template<typename DatastructureTag, typename Datastructure>
struct datastructure_wrapper
{
};

template<typename Datastructure>
struct datastructure_wrapper <tag::viennagrid_domain, Datastructure>
{
   static const int DIMT = cell_tag_type::topology_level;
   static const int DIMG = domain_config_type::dimension_tag::value;   
   
   
};

for (vertex_container_iterator_type it = vertices.begin();
      it != vertices.end(); ++it)
{
   add(viennamesh::object<viennamesh::key::point>(it->getPoint()));
}   


application()
{
   
   val::advancing_front
   val::incremental_delaunay
   val::constrained_delaunay
   val::conforming_delaunay

   
   key::algorithm
   key::criteria
   key::DIMT
   key::DIMG

   typedef typename result_of::make_map<
      key::algorithm,            key::criteria,            key::DIMT, key::DIMG, 
      val::incremental_delaunay, val::conforming_delaunay, 2,         2          >::type                    mesher_properties; 

   typedef typename result_of::generate_mesh_kernel<mesher_properties>::type                                mesh_kernel_type; // == tag::triangle

   typedef typename result_of::datastructure_wrapper<tag::viennagrid_domain, viennagrid::domain_type>::type datastructure_wrapper_type;

   typedef typename result_of::generate_mesh_generator<mesh_kernel_type, datastructure_wrapper_type>::type  mesh_generator_type; // == Triangle Interface
   mesh_generator_type mesh_generator(paras);
}



