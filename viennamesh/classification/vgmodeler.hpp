/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_CLASSIFIER_VGMODELER_HPP
#define VIENNAMESH_CLASSIFIER_VGMODELER_HPP

#include <vector>
#include <map>

#include "viennagrid/domain.hpp"

#include "viennamesh/classification/base.hpp"
#include "viennamesh/classification/histogram_latex.hpp"

//#define MESH_CLASSIFIER_DEBUG

namespace vgmodeler {


namespace keys {
   
std::string round = "round";
std::string wedge = "wedge";
std::string slat  = "slat";
std::string needle = "needle";
std::string spindle = "spindle";
std::string spade = "spade";
std::string cap = "cap";
std::string sliver = "sliver";
   
} // end namespace keys

enum TetrahedraType {TTRound, TTWedge, TTSlat, TTNeedle, TTSpindle, TTSpade, TTCap, TTSliver};

template<typename NumericType>
struct SimplexClassification 
{

   
   NumericType Volume;                     //Volume of the Tetrahedron
  NumericType Ratio;                      //Radiusratio of the Tetrahedron (3*Insphereradius/Circumsphereradius)
  NumericType Round,Wedge,Slat,Needle,
    Spindle,Spade,Cap,Sliver;      //Types of Tetrahedron (in %)
  std::vector<NumericType> Length;       //Lengths of the Edges of the Tetrahedron (sorted)
  std::vector<NumericType> DiAngle;      //Dihedral Angles of the Tetrahedron (sorted, in °)
  typedef std::vector<NumericType>  angle_container_type;
  angle_container_type Dagger;       //Daggers of the Faces of the Tetrahedron (in %)
  int    iRatio;                         //1 if Tetrahedron is good, 0 if it is bad (defined by RatioConst)
  int    iDagger;                        //1 if Tetrahedron has Daggers, 0 if it has no Daggers (defined by DaggerConst)
  std::string ResultName;                //Resulttype of the Tetrahedron

  std::vector<NumericType> container;
  TetrahedraType         type;
   
public:
  template<typename InputIterator>
  SimplexClassification(InputIterator first, InputIterator last)
  {
    // if container size -> 12    .. simplex = tetrahedron (4 x 3)
    // if container size -> 9     .. simplex = facet       (3 x 3)
    // if container size -> 6     .. simplex = triangle    (3 x 2)
    while (first != last)   {    container.push_back(*first++);     }
   
    Length.resize(6);
    DiAngle.resize(6);
    Dagger.resize(4);

    classify();  }

  void classify()
  {
    std::vector<NumericType> A(3),B(3),C(3),D(3);
    std::vector<NumericType> AB(3),AC(3),AD(3),BC(3),BD(3),CD(3);
    NumericType CP1,CP2,CP3,DENOM,FA,FB,FC,FD;
    NumericType LAB,LAC,LAD,LBC,LBD,LCD,PB,PC,PD,T1,T2,VOL,RADRTH;
    std::vector<NumericType> LENGTH(6);
    std::vector<NumericType> NRMABC(3),NRMABD(3),NRMACD(3),NRMBCD(3);
    NumericType LNRMABC,LNRMABD,LNRMACD,LNRMBCD;
    std::vector<NumericType> DANG(6);
    std::vector<NumericType> DaggerLENGTH(3);
    NumericType DaggerConst,DaggerValue;
    NumericType wedge,slat,needle,spindle,spade,cap,sliver;
    NumericType RatioConst,pi;
    int    RatioBOOL;
    int    DaggerBOOL;
    unsigned long kk;

    RatioConst =  0.5;
    DaggerConst = 0.5;

    A[0] = container[0];
    A[1] = container[1];
    A[2] = container[2];
    B[0] = container[3];
    B[1] = container[4];
    B[2] = container[5];
    C[0] = container[6];
    C[1] = container[7];
    C[2] = container[8];
    D[0] = container[9];
    D[1] = container[10];
    D[2] = container[11];

    for (kk = 0; kk < 3; kk++)
      {
   AB[kk] = B[kk] - A[kk];
   AC[kk] = C[kk] - A[kk];
   AD[kk] = D[kk] - A[kk];
   BC[kk] = C[kk] - B[kk];
   BD[kk] = D[kk] - B[kk];
   CD[kk] = D[kk] - C[kk];
      }
    LAB = pow(AB[0],2) + pow(AB[1],2) + pow(AB[2],2);
    LAC = pow(AC[0],2) + pow(AC[1],2) + pow(AC[2],2);
    LAD = pow(AD[0],2) + pow(AD[1],2) + pow(AD[2],2);
    LBC = pow(BC[0],2) + pow(BC[1],2) + pow(BC[2],2);
    LBD = pow(BD[0],2) + pow(BD[1],2) + pow(BD[2],2);
    LCD = pow(CD[0],2) + pow(CD[1],2) + pow(CD[2],2);
    PB = sqrt(LAB*LCD);
    PC = sqrt(LAC*LBD);
    PD = sqrt(LAD*LBC);

    CP1 = AB[1]*AC[2] - AB[2]*AC[1];
    CP2 = AB[2]*AC[0] - AB[0]*AC[2];
    CP3 = AB[0]*AC[1] - AB[1]*AC[0];
    FD = sqrt(pow(CP1,2) + pow(CP2,2) + pow(CP3,2));
    CP1 = AB[1]*AD[2] - AB[2]*AD[1];
    CP2 = AB[2]*AD[0] - AB[0]*AD[2];
    CP3 = AB[0]*AD[1] - AB[1]*AD[0];
    FC = sqrt(pow(CP1,2) + pow(CP2,2) + pow(CP3,2));
    CP1 = BC[1]*BD[2] - BC[2]*BD[1];
    CP2 = BC[2]*BD[0] - BC[0]*BD[2];
    CP3 = BC[0]*BD[1] - BC[1]*BD[0];
    FA = sqrt(pow(CP1,2) + pow(CP2,2) + pow(CP3,2));
    CP1 = AC[1]*AD[2] - AC[2]*AD[1];
    CP2 = AC[2]*AD[0] - AC[0]*AD[2];
    CP3 = AC[0]*AD[1] - AC[1]*AD[0];
    FB = sqrt(pow(CP1,2) + pow(CP2,2) + pow(CP3,2));
    T1 = PB + PC;
    T2 = PB - PC;
    DENOM = (FA+FB+FC+FD)*sqrt(fabs((T1+PD)*(T1-PD)*(PD+T2)*(PD-T2)));
    if (DENOM == 0)
      {
   VOL = 0;
   RADRTH = 0; 
      }
    else
      {
   VOL = AB[0]*CP1 + AB[1]*CP2 + AB[2]*CP3;
   RADRTH = 12*VOL*VOL/DENOM;
      }
      
    Volume = VOL/6;          // simplex 
    Ratio = RADRTH;          // simplex
    Round = RADRTH*100;      // simplex

    LAB = sqrt(LAB);
    LAC = sqrt(LAC);
    LAD = sqrt(LAD);
    LBC = sqrt(LBC);
    LBD = sqrt(LBD);
    LCD = sqrt(LCD);
    LENGTH[0] = LAB;
    LENGTH[1] = LAC;
    LENGTH[2] = LAD;
    LENGTH[3] = LBC;
    LENGTH[4] = LBD;
    LENGTH[5] = LCD;
    NRMABC[0] = AC[1]*AB[2] - AC[2]*AB[1];
    NRMABC[1] = AC[2]*AB[0] - AC[0]*AB[2];
    NRMABC[2] = AC[0]*AB[1] - AC[1]*AB[0];
    NRMABD[0] = AB[1]*AD[2] - AB[2]*AD[1];
    NRMABD[1] = AB[2]*AD[0] - AB[0]*AD[2];
    NRMABD[2] = AB[0]*AD[1] - AB[1]*AD[0];
    NRMACD[0] = AD[1]*AC[2] - AD[2]*AC[1];
    NRMACD[1] = AD[2]*AC[0] - AD[0]*AC[2];
    NRMACD[2] = AD[0]*AC[1] - AD[1]*AC[0];
    NRMBCD[0] = BC[1]*BD[2] - BC[2]*BD[1];
    NRMBCD[1] = BC[2]*BD[0] - BC[0]*BD[2];
    NRMBCD[2] = BC[0]*BD[1] - BC[1]*BD[0];
    LNRMABC = pow(NRMABC[0],2) + pow(NRMABC[1],2) + pow(NRMABC[2],2);
    LNRMABD = pow(NRMABD[0],2) + pow(NRMABD[1],2) + pow(NRMABD[2],2);
    LNRMACD = pow(NRMACD[0],2) + pow(NRMACD[1],2) + pow(NRMACD[2],2);
    LNRMBCD = pow(NRMBCD[0],2) + pow(NRMBCD[1],2) + pow(NRMBCD[2],2);
    pi = 2*acos(0);
    DANG[0] = pi - acos((NRMABC[0]*NRMABD[0]+NRMABC[1]*NRMABD[1]+NRMABC[2]*NRMABD[2])/sqrt(LNRMABC*LNRMABD));
    DANG[1] = pi - acos((NRMABC[0]*NRMACD[0]+NRMABC[1]*NRMACD[1]+NRMABC[2]*NRMACD[2])/sqrt(LNRMABC*LNRMACD));
    DANG[2] = pi - acos((NRMABD[0]*NRMACD[0]+NRMABD[1]*NRMACD[1]+NRMABD[2]*NRMACD[2])/sqrt(LNRMABD*LNRMACD));
    DANG[3] = pi - acos((NRMABC[0]*NRMBCD[0]+NRMABC[1]*NRMBCD[1]+NRMABC[2]*NRMBCD[2])/sqrt(LNRMABC*LNRMBCD));
    DANG[4] = pi - acos((NRMABD[0]*NRMBCD[0]+NRMABD[1]*NRMBCD[1]+NRMABD[2]*NRMBCD[2])/sqrt(LNRMABD*LNRMBCD));
    DANG[5] = pi - acos((NRMACD[0]*NRMBCD[0]+NRMACD[1]*NRMBCD[1]+NRMACD[2]*NRMBCD[2])/sqrt(LNRMACD*LNRMBCD));

    std::sort(LENGTH.begin(), LENGTH.end() );
    std::sort(DANG.begin(), DANG.end() );
    //    qsort (LENGTH,6,sizeof(NumericType),cmp);
    //    qsort (DANG,6,sizeof(NumericType),cmp);
    (LENGTH[1] == 0) ? (wedge = 0) : (wedge = (1-LENGTH[0]/LENGTH[1])*100);
    (LENGTH[2] == 0) ? (slat = 0) : (slat = (1-LENGTH[1]/LENGTH[2])*100);
    (LENGTH[3] == 0) ? (needle = 0) : (needle = (1-LENGTH[2]/LENGTH[3])*100);
    (DANG[1] == 0) ? (spindle = 0) : (spindle = (1-DANG[0]/DANG[1])*100);
    (DANG[2] == 0) ? (spade = 0) : (spade = (1-DANG[1]/DANG[2])*100);
    (DANG[3] == 0) ? (cap = 0) : (cap = (1-DANG[2]/DANG[3])*100);
    (DANG[4] == 0) ? (sliver = 0) : (sliver = (1-DANG[3]/DANG[4])*100);
    Wedge = wedge;       // simplex
    Slat = slat;         // simplex
    Needle = needle;     // simplex
    Spindle = spindle;   // simplex
    Spade = spade;       // simplex
    Cap = cap;           // simplex
    Sliver = sliver;     // simplex


    for (kk = 0; kk < 6; kk++)
      {
   Length[kk] = LENGTH[kk];           // simplex
   DiAngle[kk] = DANG[kk]*180/pi;     // simplex
      }

    DaggerBOOL = 0;

    // Triangle ABC:
    DaggerLENGTH[0] = LAB;
    DaggerLENGTH[1] = LAC;
    DaggerLENGTH[2] = LBC;
    std::sort(DaggerLENGTH.begin(), DaggerLENGTH.end() );

    (DaggerLENGTH[1] == 0) ? (DaggerValue = 0) : (DaggerValue = 1 - DaggerLENGTH[0]/DaggerLENGTH[1]);
    if (DaggerValue >= DaggerConst) DaggerBOOL = 1; 
    Dagger[0] = DaggerValue*100;    // simplex

    // Triangle ABD:
    DaggerLENGTH[0] = LAB;
    DaggerLENGTH[1] = LAD;
    DaggerLENGTH[2] = LBD;
    std::sort(DaggerLENGTH.begin(), DaggerLENGTH.end() );

    (DaggerLENGTH[1] == 0) ? (DaggerValue = 0) : (DaggerValue = 1 - DaggerLENGTH[0]/DaggerLENGTH[1]);
    if (DaggerValue >= DaggerConst) DaggerBOOL = 1;
    Dagger[1] = DaggerValue*100;   // simplex
 
    // Triangle ACD:
    DaggerLENGTH[0] = LAC;
    DaggerLENGTH[1] = LAD;
    DaggerLENGTH[2] = LCD;
    std::sort(DaggerLENGTH.begin(), DaggerLENGTH.end() );

    (DaggerLENGTH[1] == 0) ? (DaggerValue = 0) : (DaggerValue = 1 - DaggerLENGTH[0]/DaggerLENGTH[1]);
    if (DaggerValue >= DaggerConst) DaggerBOOL = 1; 
    Dagger[2] = DaggerValue*100;   // simplex

    // Triangle BCD:
    DaggerLENGTH[0] = LBC;
    DaggerLENGTH[1] = LBD;
    DaggerLENGTH[2] = LCD;
    std::sort(DaggerLENGTH.begin(), DaggerLENGTH.end() );

    (DaggerLENGTH[1] == 0) ? (DaggerValue = 0) : (DaggerValue = 1 - DaggerLENGTH[0]/DaggerLENGTH[1]);
    if (DaggerValue >= DaggerConst) DaggerBOOL = 1; 
    Dagger[3] = DaggerValue*100;  // simplex

    iDagger = DaggerBOOL;
    (RADRTH >= RatioConst) ? (RatioBOOL = 1) : (RatioBOOL = 0);
    iRatio = RatioBOOL;          // simplex




   if (RatioBOOL)
   {
      ResultName = vgmodeler::keys::round;   // simplex
      type = TTRound;
   }
   else 
   {
      if (DaggerBOOL)
      {
         if ((wedge>slat)&&(wedge>needle))
         {
            ResultName = vgmodeler::keys::wedge;    // simplex
            type = TTWedge;
         }
         else if (slat>needle)
         {
            ResultName = vgmodeler::keys::slat;    // simplex
            type = TTSlat;
         }
         else
         {
            ResultName = vgmodeler::keys::needle;  // simplex
            type = TTNeedle;
         }
      }
      else
      {
         if ((spindle>spade)&&(spindle>cap)&&(spindle>sliver))
         {
            ResultName = vgmodeler::keys::spindle;    // simplex
            type = TTSpindle;
         }
         else if ((spade>cap)&&(spade>sliver))
         {
            ResultName = vgmodeler::keys::spade;    // simplex
            type = TTSpade;
         }
         else if (cap>sliver)
         {
            ResultName = vgmodeler::keys::cap;    // simplex
            type = TTCap;
         }
         else
         {
            ResultName = vgmodeler::keys::sliver;    // simplex
            type = TTSliver;
         }
      }
   }
  }

  TetrahedraType getType()
  {
    return type;
  }
   
  friend std::ostream& operator<<(std::ostream& ostr, SimplexClassification& sc)
  {
    ostr << "Simplex Classification .. \nvolume: " << sc.Volume << std::endl
    << "3*Inradius/Circumradius: " << sc.Ratio << (sc.iRatio ? " is good" : " is bad") << std::endl
    << "ratio:   " << sc.Ratio << std::endl;
    for (unsigned int i = 0; i < sc.Length.size(); i++)
      {
   ostr << " length: " << sc.Length[i] << "  diangle: " << sc.DiAngle[i] << std::endl;
      }
    ostr << "Round:   " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Round   << " %" << std::endl 
    << "Wedge:   " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Wedge   << " %" << std::endl 
    << "Slat:    " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Slat    << " %" << std::endl 
    << "Needle:  " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Needle  << " %" << std::endl 
    << "Spindle: " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Spindle << " %" << std::endl 
    << "Spade:   " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Spade   << " %" << std::endl 
    << "Cap:     " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Cap     << " %" << std::endl 
    << "Sliver:  " << std::setw(6)<<std::setfill(' ') <<std::right << sc.Sliver  << " %" << std::endl 
    << " dagger: ";
    for (unsigned int i = 0; i < sc.Dagger.size(); i++)
      {
   ostr <<  sc.Dagger[i] << "  ";
      }
    ostr << std::endl;
    ostr << "   result name: " << sc.ResultName << std::endl;
    return ostr;
  }
};


} // end namespace vgmodeler

namespace viennamesh {
  
template <>
struct mesh_classifier <viennamesh::tag::vgmodeler>
{
   // -------------------------------------------------------------------------------------
   mesh_classifier() : id("vgmodeler") {}
   // -------------------------------------------------------------------------------------
    
   // ------------------------------------------------------------------------------------- 
   template<typename DatastructureT>
   void operator()(DatastructureT& data) 
   {
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - processing starting up" << std::endl;
//   #endif            
//      static const int DIMG = 3;
//      static const int DIMT = 3;
//      static const int CELLSIZE = DIMT+1; // only for simplices!
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - detected topology dimension: " << DIMT << std::endl;
//   #endif                  
//      
//      typedef typename DatastructureT::geometry_container_type             mesher_geometry_cont_type;
//      typedef typename DatastructureT::segment_container_type              mesher_segment_container_type;
//      typedef typename mesher_segment_container_type::value_type           mesher_cell_container_type;
//      typedef typename DatastructureT::numeric_type                        numeric_type;
//      
//      typedef vgmodeler::SimplexClassification<numeric_type>                        simplex_classification_type;
//      typedef typename simplex_classification_type::angle_container_type::iterator  angle_iterator_type;
//      
//      typedef typename viennamesh::writeLatex<3>::histogram_type histogram_type;
//      histogram_type histogram;      
//      histogram[vgmodeler::keys::round]   = 0;
//      histogram[vgmodeler::keys::wedge]   = 0;
//      histogram[vgmodeler::keys::slat]    = 0;
//      histogram[vgmodeler::keys::needle]  = 0;
//      histogram[vgmodeler::keys::spindle] = 0;
//      histogram[vgmodeler::keys::spade]   = 0;
//      histogram[vgmodeler::keys::cap]     = 0;
//      histogram[vgmodeler::keys::sliver]  = 0;          
//      
//      typedef std::vector< numeric_type >   angles_type;
//      angles_type angles;
//      
//      mesher_geometry_cont_type&     geom_cont = data.geometry();
//      mesher_segment_container_type& seg_cont  = data.topology();      

//      static const int SIMPLEX_POINT_SIZE = DIMG*CELLSIZE;
//      double simplex_point_array[SIMPLEX_POINT_SIZE];
//      
//      for(typename mesher_segment_container_type::iterator si = seg_cont.begin();
//         si != seg_cont.end(); si++)
//      {
//         for(typename mesher_cell_container_type::iterator ci = (*si).begin();
//            ci != (*si).end(); ci++)
//         {
//            for(int i = 0; i < CELLSIZE; i++)
//            {
//               std::size_t vertex = (*ci)[i];
//               
//               for(int dim = 0; dim < DIMG; dim++)
//               {
//                  // load the simplex point array
//                  // take care of the offset, the data ordering is
//                  // [p0_x, p0_y, p0_z, p1_x, p1_y,...]^T
//                  simplex_point_array[(i*DIMG)+dim] = geom_cont[vertex][dim];
//               }
//            }    
////             for(size_t i = 0; i < SIMPLEX_POINT_SIZE; i++)
////             {
////                std::cout << simplex_point_array[i] << std::endl;
////             };

//            simplex_classification_type classification(simplex_point_array, simplex_point_array+SIMPLEX_POINT_SIZE);
////             std::cout << classification << std::endl;
////             exit(0);
//            
//            // record the result within the histogram
//            histogram[classification.ResultName]++;
//            
//            // record all angles within the angles container
//            for(angle_iterator_type iter = classification.DiAngle.begin();
//                  iter != classification.DiAngle.end(); iter++)
//            {
//               angles.push_back(*iter);
//            }            
//         }
//      }    
//      
//     
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - writing latex code" << std::endl;
//   #endif               
//      // transform histogram data into latex code, and dump it to screen
//      // note: use a file stream to print to a file ..
//      std::ofstream  histo_stream("histogram.txt"); 
//      viennamesh::writeLatex<3>::eval(histogram, histo_stream );
//      histo_stream.close();
//      
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - writing latex code" << std::endl;
//   #endif               
//      std::ofstream  angle_stream("angles.txt");       
//      this->dump_angles(angles, angle_stream);
//      angle_stream.close();
//      
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - result:" << std::endl;
//   #endif  
//      // dump the classification results in a human readable form to the screen
//      this->dump_histogram(histogram, "  ", std::cout);
   }
   // -------------------------------------------------------------------------------------    
    
   // -------------------------------------------------------------------------------------    
   boost::shared_ptr< viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type >
   operator()(viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type & domain ) // ONLY A 3D SIMPLEX VERSION IMPLMEENTED ..
   {   
      return (*this)(boost::make_shared< viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type >(domain));   
   }   
   
   // -------------------------------------------------------------------------------------    
   boost::shared_ptr< viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type >
   operator()(boost::shared_ptr< viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type > domain ) // ONLY A 3D SIMPLEX VERSION IMPLMEENTED ..
   {   
      typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type    domain_type;
      typedef domain_type::config_type                     DomainConfiguration;

      typedef DomainConfiguration::numeric_type            numeric_type;
      typedef DomainConfiguration::cell_tag                CellTag;   
      typedef domain_type::segment_type                                                                  SegmentType;      

      static const int DIMG = DomainConfiguration::coordinate_system_tag::dim;
      static const int DIMT = DomainConfiguration::cell_tag::dim;   
      static const int CELLSIZE = viennagrid::topology::bndcells<CellTag, 0>::num;       

      typedef viennagrid::result_of::ncell<DomainConfiguration, DIMT>::type      CellType;      
      typedef viennagrid::result_of::point<DomainConfiguration>::type                               PointType;      
      typedef viennagrid::result_of::ncell_range<SegmentType, DIMT>::type         CellContainer;            
      typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;      
      typedef viennagrid::result_of::ncell_range<CellType, 0>::type                                  VertexOnCellContainer;
      typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;            
      
      typedef vgmodeler::SimplexClassification<numeric_type>                        simplex_classification_type;
      typedef simplex_classification_type::angle_container_type            angles_type;
      typedef angles_type::iterator                                        angle_iterator_type;   
   
      typedef viennamesh::writeLatex<3>::histogram_type histogram_type;
      histogram_type histogram;      
      histogram[vgmodeler::keys::round]   = 0;
      histogram[vgmodeler::keys::wedge]   = 0;
      histogram[vgmodeler::keys::slat]    = 0;
      histogram[vgmodeler::keys::needle]  = 0;
      histogram[vgmodeler::keys::spindle] = 0;
      histogram[vgmodeler::keys::spade]   = 0;
      histogram[vgmodeler::keys::cap]     = 0;
      histogram[vgmodeler::keys::sliver]  = 0;          
      
      angles_type angles;      
      
      for (std::size_t si = 0; si < domain->segments().size(); ++si)
      {
         SegmentType & seg = domain->segments()[si];
         CellContainer cells = viennagrid::ncells<DIMT>(seg);      
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            static const int SIMPLEX_POINT_SIZE = DIMG*CELLSIZE;
            double simplex_point_array[SIMPLEX_POINT_SIZE];
            std::size_t vi = 0;  
                     
            VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
               for(int dim = 0; dim < DIMG; dim++)
               {
                  //std::cout << "classi debug: " << (vocit->point())[dim] << std::endl;
                  simplex_point_array[(vi*DIMG)+dim] = (vocit->point())[dim];
               }
               vi++;
            }                     
            
            simplex_classification_type classification(simplex_point_array,  simplex_point_array+SIMPLEX_POINT_SIZE);
            //std::cout << "debug: " << classification.ResultName << std::endl;
            histogram[classification.ResultName]++;

            for(angle_iterator_type iter = classification.DiAngle.begin();
                  iter != classification.DiAngle.end(); iter++)
            {
               angles.push_back(*iter);
            }                        
         }
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - writing latex code" << std::endl;
//   #endif               
      // transform histogram data into latex code, and dump it to screen
      // note: use a file stream to print to a file ..
      std::ofstream  histo_stream("histogram.txt"); 
      viennamesh::writeLatex<3>::eval(histogram, histo_stream );
      histo_stream.close();
//      
//   #ifdef MESH_CLASSIFIER_DEBUG
//      std::cout << "## MeshClassifier::"+id+" - writing latex code" << std::endl;
//   #endif               
//      std::ofstream  angle_stream("angles.txt");       
//      this->dump_angles(angles, angle_stream);
//      angle_stream.close();
      
      }      


   #ifdef MESH_CLASSIFIER_DEBUG
      std::cout << "## MeshClassifier::"+id+" - result:" << std::endl;
   #endif  
      // dump the classification results in a human readable form to the screen
      this->dump_histogram(histogram, "  ", std::cout);         

      return domain;
   }
   // -------------------------------------------------------------------------------------       
   
private:
   
   template<typename KeyT, typename ValueT>
   void dump_histogram(std::map<KeyT, ValueT>& histogram, std::string prefix = "", std::ostream& ostr = std::cout)
   {
      size_t sum = 0;
      for(typename std::map<KeyT, ValueT>::iterator iter = histogram.begin();
          iter != histogram.end(); iter++)
      {
         sum += iter->second;
      }      
      ostr << prefix << "----------------------------------------" << std::endl;
      for(typename std::map<KeyT, ValueT>::iterator iter = histogram.begin();
          iter != histogram.end(); iter++)
      {
         double percentage = ( double(iter->second) / double(sum))*100.0;
         ostr.precision(2);
         ostr << prefix << iter->first << ":  " << percentage << "%" << std::endl;
      }      
      ostr << prefix << "----------------------------------------" << std::endl;      
   }
   
   template<typename ValueT>
   void dump_angles(std::vector<ValueT>& histogram, std::ostream& ostr = std::cout)
   {
      for(typename std::vector<ValueT>::iterator iter = histogram.begin();
          iter != histogram.end(); iter++)
      {
         ostr.precision(2);
         ostr << *iter << std::endl;
      }      
   }   
   
   // ------------------------------------------------------------------------------------- 
   std::string id;
   // -------------------------------------------------------------------------------------    
};
  

} // end namespace viennamesh
  




#endif
