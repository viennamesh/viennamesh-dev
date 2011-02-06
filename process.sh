#!/bin/bash

INPUT=$1  # inputmesh
OUTPUT=$2 # outputmesh
ORIENT=$3 # orient flag 

if [ $# -ne 3 ]
then
echo "## Error:: usage:"
echo "./process.sh input_hull_mesh.{hin,gau32} output_volume_mesh.gau3 ORIENTATION_FLAG{OFF=0,ON=1}"
echo "shutting down .."
exit
fi

if [ ! -e $INPUT -a ! -r $INPUT -a ! -s $INPUT ] # -r readable, -s file size is not zero
then
echo "## Error:: The input file:\"" $INPUT "\" either not exists, is not readable or has file size zero."
echo "shutting down .."
exit
fi

if [ $ORIENT -ne 0 ]
then
   if [ $ORIENT -ne 1 ]
   then
      echo "## Error:: usage:"
      echo "./process.sh input_hull_mesh.{hin,gau32} output_volume_mesh.gau3 ORIENTATION_FLAG{OFF=0,ON=1}"
      echo "shutting down .."
      exit
   fi
fi

echo "## converting hull .. "
bin/hull_converter $INPUT mesh_converted.gau32
echo ""
if [ $ORIENT -eq 1 ]
then
   echo "## orienting hull .. "
   bin/hull_orienter mesh_converted.gau32 mesh_oriented.gau32
   echo ""
   echo "## adapting hull .. "
   bin/hull_adaptor mesh_oriented.gau32
   mv surface_mesh.gau32 mesh_oriented_adapted.gau32
   echo ""
   echo "## meshing volume .. "
   bin/volume_mesher mesh_oriented_adapted.gau32 mesh_oriented_volume.gau3
   echo ""
   echo "## classifing volume mesh.. "
   bin/mesh_classifier mesh_oriented_volume.gau3
   mv mesh_oriented_volume.gau3 $OUTPUT
else
   echo ""
   echo "## adapting hull .. "
   bin/hull_adaptor mesh_converted.gau32
   mv surface_mesh.gau32 mesh_adapted.gau32
   echo ""
   echo "## meshing volume .. "
   bin/volume_mesher mesh_adapted.gau32 mesh_volume.gau3
   echo ""
   echo "## classifing volume mesh.. "
   bin/mesh_classifier mesh_volume.gau3
   mv mesh_volume.gau3 $OUTPUT   
fi
