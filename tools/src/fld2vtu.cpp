#include "viennagrid/io/vtk_writer.hpp"
#include "viennamesh/io/fld_reader.hpp"


#include <boost/program_options.hpp>

namespace po = boost::program_options;


int main(int argc, char **argv)
{
    po::options_description desc("Usage: fld2vtu [--output-file/-o] [--input-file/i] <input-file>");
    
    desc.add_options()
        ("help", "Print this message")
        ("output-file,o", po::value<std::string>(), "set VTK output file")
        ("input-file,i", po::value<std::string>(), "the fld input file")
        ;
        
    po::positional_options_description p;
    p.add("input-file", 1);
    
    po::variables_map vm;
    po::store( po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm );
    po::notify(vm);
    
    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }
    
    if (vm.count("input-file") == 0)
    {
        std::cout << "Error: no input file sepcified" << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }
    
    
    std::string input_file = vm[ "input-file" ].as< std::string >();
    std::string output_file;
    
    if (vm.count("output-file"))
        output_file = vm[ "output-file" ].as< std::string >();
    else
    {
        output_file = input_file;
        
        std::string::size_type pos = output_file.rfind('.');
        if (pos != std::string::npos)
            output_file.erase(pos);
        
        output_file += ".vtu";
    }
    
    
    std::cout << "Input-file: " << input_file << std::endl;
    std::cout << "Output-file: " << output_file << std::endl;
    std::cout << "Processing...";

    typedef viennagrid::config::tetrahedral_3d_domain domain_type;
    domain_type domain;
    viennagrid::io::fld_reader fld_reader;
    fld_reader( domain, input_file );
    
    
    
    {        
        viennagrid::io::vtk_writer<viennagrid::config::tetrahedral_3d_domain, viennagrid::config::tetrahedral_3d_cell> vtk_writer;
        viennagrid::io::add_scalar_data_on_vertices<std::string, double>(vtk_writer, fld_reader.nodeattribute_name, fld_reader.nodeattribute_name);
        viennagrid::io::add_scalar_data_on_cells<viennamesh::unique_segment_tag, viennamesh::segment_id_type>(vtk_writer, "segment_id");
        vtk_writer(domain, output_file);
    }

    
    std::cout << " done!" << std::endl;
    
    return 0;
}