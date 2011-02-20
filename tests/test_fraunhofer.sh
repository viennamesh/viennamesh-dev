
CURRENT=$PWD
VGMODELER=$CURRENT/../external/vgmodeler

echo $CURRENT
echo $VGMODELER

# build viennamesh and the tests
#   note: for testing purposes only the vgmodeler test is being built
#   find the source file here: src/vgmodeler.cpp
#   the source files contains the usage of the new interface
#   the mesher utilizes the vgmodeler volume mesher
mkdir -p build/
cd build/
cmake ..
make -j3


# build old vgmodeler executables
cd $VGMODELER
./build_all.sh 

# change to the tests build directory .. we'll mesh in here ..
cd $CURRENT/build

# remove old output files
rm -f *.vtu *.vol *.pvd *.gau32 *.txt

# use the old vgmodeler tools to prepare the litho mesh
$VGMODELER/bin/hull_converter ../input/frauenhofer/bnd/litho_shaped_gates_sram_cell.bnd litho.gau32

# orient the converted mesh
$VGMODELER/bin/hull_orienter litho.gau32 litho_oriented.gau32

# hull adapt the oriented mesh
$VGMODELER/bin/hull_adaptor litho_oriented.gau32  # outputfile is hardcoded to 'surface_mesh.gau32'

# volume mesh 
#   note: if i use the "old" vgmodeler volume mesher, the tool segfaults O.o
#         but the reimplemented viennamesh::vgmodeler volume mesher works just fine 
./vgmodeler surface_mesh.gau32 litho.vtk

echo 
echo "done .."
echo "  note the quality evaluation of the result mesh at the end of the process chain, "
echo "  look for ## MeshClassifier::vgmodeler - result:  and the following lines.."
echo
echo "  for visualization, there are two options:"
echo "    1. paraview: load the *.pvd mesh - you get the view of the multi-segment domain"
echo "    2. netgen:   load the *.vol files, each segment is stored in a separat vol file"


