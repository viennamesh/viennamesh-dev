#ifndef VIENNAMESH_CORE_MESH_QUANTITIES_HPP
#define VIENNAMESH_CORE_MESH_QUANTITIES_HPP

#include "viennamesh/forwards.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"
#include "viennagrid/accessor.hpp"


namespace viennamesh
{

  template<typename KeyT, typename ValueT>
  struct mesh_quantities
  {
    typedef std::map<KeyT, ValueT> QuantitiesType;
    typedef std::map<string, QuantitiesType> NamedQuantitiesType;

    template<typename MeshT, typename SrcFieldT>
    void from_field( MeshT const & mesh, SrcFieldT const & src_field, string const & name )
    {
      typedef typename SrcFieldT::access_type ElementType;
      typedef typename viennagrid::result_of::const_element_range<MeshT, ElementType>::type ConstElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

      ConstElementRangeType elements(mesh);
      for (ConstElementIteratorType eit = elements.begin(); eit != elements.end(); ++eit)
        quantities[name][ (*eit).id().get() ] = src_field(*eit);
    }

    template<typename ElementTagT, typename MeshT>
    typename viennagrid::result_of::field<
        std::map<KeyT, ValueT>,
        typename viennagrid::result_of::element<MeshT, ElementTagT>::type,
        viennagrid::base_id_unpack>::type get_field( string const & name )
    {
      return typename viennagrid::result_of::field<
        std::map<KeyT, ValueT>,
        typename viennagrid::result_of::element<MeshT, ElementTagT>::type,
        viennagrid::base_id_unpack>::type( quantities[name] );
    }

    template<typename ElementTagT, typename MeshT>
    typename viennagrid::result_of::field<
        const std::map<KeyT, ValueT>,
        typename viennagrid::result_of::element<MeshT, ElementTagT>::type,
        viennagrid::base_id_unpack>::type get_field( string const & name ) const
    {
      typedef typename viennagrid::result_of::field<
        const std::map<KeyT, ValueT>,
        typename viennagrid::result_of::element<MeshT, ElementTagT>::type,
        viennagrid::base_id_unpack>::type FieldType;

      typename NamedQuantitiesType::const_iterator qit = quantities.find(name);
      if (qit != quantities.end())
        return FieldType( qit->second );

      return FieldType();
    }

    NamedQuantitiesType quantities;
  };




  template<typename SegmentIDT, typename VertexKeyT, typename CellKeyT, typename ValueT>
  struct segmented_mesh_quantities
  {
    typedef std::map<VertexKeyT, ValueT> VertexValueContainerType;
    typedef std::map<CellKeyT, ValueT> CellValueContainerType;

    template<typename ReaderT, typename MeshT, typename SegmentationT>
    void fromReader(ReaderT const & reader, MeshT const & mesh, SegmentationT const & segmentation)
    {
      typedef typename viennagrid::result_of::vertex<MeshT>::type VertexType;
      typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

      typedef typename viennagrid::result_of::segment_id<SegmentationT>::type SegmentIDType;
      for (typename SegmentationT::const_iterator sit = segmentation.begin(); sit != segmentation.end(); ++sit)
      {
        SegmentIDType segment_id = sit->id();

        std::vector<string> vertex_values_names = reader.scalar_vertex_data_names(segment_id);
        for (unsigned int i = 0; i < vertex_values_names.size(); ++i)
        {
          string const & quantity_name = vertex_values_names[i];
          typedef typename viennagrid::result_of::field<const std::deque<double>, VertexType >::type FieldType;
          FieldType field = reader.vertex_scalar_field( quantity_name, segment_id );

          vertex_segment_quantities[segment_id].from_field( mesh, field, quantity_name );
          info(5) << "Found vertex scalar quantities on segment " << segment_id << ": " << quantity_name << std::endl;
        }

        std::vector<string> cell_values_names = reader.scalar_cell_data_names(segment_id);
        for (unsigned int i = 0; i < cell_values_names.size(); ++i)
        {
          string const & quantity_name = cell_values_names[i];
          typedef typename viennagrid::result_of::field<const std::deque<double>, CellType >::type FieldType;
          FieldType field = reader.cell_scalar_field( quantity_name, segment_id );

          cell_segment_quantities[segment_id].from_field( mesh, field, quantity_name );
          info(5) << "Found cell scalar quantities on segment " << segment_id << ": " << quantity_name << std::endl;
        }
      }
    }



    template<typename MeshT, typename WriterT>
    void toWriter(WriterT & writer) const
    {
      typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;

      for (typename std::map<SegmentIDT, VertexQuantitesType>::const_iterator vsqit = vertex_segment_quantities.begin(); vsqit != vertex_segment_quantities.end(); ++vsqit)
      {
        for (typename VertexQuantitesType::NamedQuantitiesType::const_iterator vqit = vsqit->second.quantities.begin(); vqit != vsqit->second.quantities.end(); ++vqit)
        {
          info(5) << "Found vertex scalar quantities on segment " << vsqit->first << ": " << vqit->first << std::endl;
          writer.add_scalar_data_on_vertices( vsqit->first, vsqit->second.template get_field<viennagrid::vertex_tag, MeshT>(vqit->first), vqit->first );
        }
      }

      for (typename std::map<SegmentIDT, CellQuantitesType>::const_iterator vsqit = cell_segment_quantities.begin(); vsqit != cell_segment_quantities.end(); ++vsqit)
      {
        for (typename CellQuantitesType::NamedQuantitiesType::const_iterator vqit = vsqit->second.quantities.begin(); vqit != vsqit->second.quantities.end(); ++vqit)
        {
          info(5) << "Found cell scalar quantities on segment " << vsqit->first << ": " << vqit->first << std::endl;
          writer.add_scalar_data_on_cells( vsqit->first, vsqit->second.template get_field<CellTag, MeshT>(vqit->first), vqit->first );
        }
      }
    }


    template<typename MeshT>
    typename viennagrid::result_of::field<
        std::map<VertexKeyT, ValueT>,
        typename viennagrid::result_of::vertex<MeshT>::type,
        viennagrid::base_id_unpack>::type get_vertex_field( MeshT const & mesh, SegmentIDT segment_id, string const & name )
    {
      return vertex_segment_quantities[segment_id].template get_field<viennagrid::vertex_tag, MeshT>( name);
    }

    template<typename MeshT>
    typename viennagrid::result_of::field<
        const std::map<VertexKeyT, ValueT>,
        typename viennagrid::result_of::vertex<MeshT>::type,
        viennagrid::base_id_unpack>::type get_vertex_field( MeshT const & mesh, SegmentIDT segment_id, string const & name ) const
    {
      typename std::map<SegmentIDT, VertexQuantitesType>::iterator qit = vertex_segment_quantities.find( segment_id );
      if (qit != vertex_segment_quantities.end())
        return qit->second.template get_field<viennagrid::vertex_tag, MeshT>( name);

      return typename viennagrid::result_of::field<
        const std::map<VertexKeyT, ValueT>,
        typename viennagrid::result_of::vertex<MeshT>::type,
        viennagrid::base_id_unpack>::type();
    }



    template<typename MeshT>
    typename viennagrid::result_of::field<
        std::map<CellKeyT, ValueT>,
        typename viennagrid::result_of::cell<MeshT>::type,
        viennagrid::base_id_unpack>::type get_cell_field( MeshT const & mesh, SegmentIDT segment_id, string const & name )
    {
      return cell_segment_quantities[segment_id].template get_field<viennagrid::vertex_tag, MeshT>( name);
    }

    template<typename MeshT>
    typename viennagrid::result_of::field<
        const std::map<CellKeyT, ValueT>,
        typename viennagrid::result_of::cell<MeshT>::type,
        viennagrid::base_id_unpack>::type get_cell_field( MeshT const & mesh, SegmentIDT segment_id, string const & name ) const
    {
      typename std::map<SegmentIDT, CellQuantitesType>::iterator qit = cell_segment_quantities.find( segment_id );
      if (qit != cell_segment_quantities.end())
        return qit->second.template get_field<viennagrid::vertex_tag, MeshT>( name);

      return typename viennagrid::result_of::field<
        const std::map<CellKeyT, ValueT>,
        typename viennagrid::result_of::cell<MeshT>::type,
        viennagrid::base_id_unpack>::type();
    }




    typedef mesh_quantities<VertexKeyT, ValueT> VertexQuantitesType;
    typedef mesh_quantities<CellKeyT, ValueT> CellQuantitesType;

    VertexQuantitesType vertex_quantities;
    CellQuantitesType cell_quantities;

    std::map<SegmentIDT, VertexQuantitesType> vertex_segment_quantities;
    std::map<SegmentIDT, CellQuantitesType> cell_segment_quantities;
  };


  namespace result_of
  {
    template<typename MeshT, typename SegmentationT>
    struct segmented_mesh_quantities
    {
      typedef typename viennagrid::result_of::segment_id<SegmentationT>::type SegmentIDType;
      typedef typename viennagrid::result_of::vertex_id<MeshT>::type::base_id_type VertexKeyType;
      typedef typename viennagrid::result_of::cell_id<MeshT>::type::base_id_type CellKeyType;
      typedef typename viennagrid::result_of::coord<MeshT>::type NumericType;

      typedef viennamesh::segmented_mesh_quantities<SegmentIDType, VertexKeyType, CellKeyType, NumericType> type;
    };
  }
}



#endif
