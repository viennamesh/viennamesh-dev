/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at     
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at          
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at                     

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_ALGORITHMS_LATEX_HISTOGRAM
#define GSSE_ALGORITHMS_LATEX_HISTOGRAM

// *** system includes
#include <iomanip>
#include <map>
// *** BOOST includes
#include <boost/lexical_cast.hpp>
// *** GSSE includes


//#define DEBUG

namespace gsse {

// #### search for special characters within a given string 
//      and prefix them with a '\'
//
std::string check_for_special_characters(std::string key)
{
#ifdef DEBUG   
   std::string old_key = key;   
#endif   

   size_t found = key.find("_");
   
   if(found != std::string::npos) {
      key.replace(found, 1, "\\_");
   #ifdef DEBUG      
      std::cout << "old key containing a '_': " 
         << old_key << " - new key: " << key << std::endl;
   #endif
   }
   return key;
}

// #### function for dumping a latex conform histogram to a stream. 
//
// AssociativeContainer needs to support:
//    -) nested iterator
//    -) begin, end functions
//    -) first, second iterator access
//    -) size
//
template< typename AssociativeContainer >
void writeLatex(  AssociativeContainer & p, 
                  bool draw_box = true, 
                  bool dumpcout = false,
                  std::ostream & ostr = std::cout)
{   
   typedef typename AssociativeContainer::iterator Iterator;
   Iterator it;

   ostr << std::fixed << std::setprecision(8);

   int sum = 0;
   for(it = p.begin(); it != p.end(); it++)
   {
      sum += it->second;
   }

   // ##########
   // create statistics plot

   
   // a priori computation of the vertical position of the histogram bar names
   //
   long size_entries = p.size();
   size_t key_size_max = 0;
   double text_vertical_position = 0.0;
   double box_vertical_size = 0.0;
   double box_horizontal_size = 0.0;
   
   for(it = p.begin(); it != p.end(); it++)
   {
      // determine the maximal key size of all entries
      if( (it->first).size() > key_size_max )
         key_size_max = (it->first).size();

      // the reference is: for a key of 8 characters, the vspace is 40.0
      // so conclude about the actual size of the key by using this ratio.
      text_vertical_position = key_size_max * 40.0 / 8.0;
      
      // determine the size of the pspicture box. 
      // use some reference ratios to derive a suitable size
      box_vertical_size = text_vertical_position;
      box_horizontal_size = size_entries * 4.0 / 3.0;
   }     

   ostr << std::endl;
   ostr << "%-----------" << std::endl;
   ostr << "%%% PLOT %%%" << std::endl;
   ostr << "%-----------" << std::endl;
   ostr << "\\begin{minipage}[ht]{0.20\\textwidth}" << std::endl;
   if(draw_box)
      ostr << "\\fbox{" << std::endl;
   ostr << "\\setlength{\\unitlength}{1cm}" << std::endl;
   ostr << "\\psset{xunit=0.7cm, yunit=0.03017058cm}" << std::endl;
   ostr << "\\begin{pspicture}(-1.0,-" << box_vertical_size << ")("<< box_horizontal_size <<",130)" << std::endl;
   ostr << "\\psaxes[labels=y,Dx=1,Dy=50.](0,0)(" << size_entries << ",100)" << std::endl;
   ostr << "\\pspolygon[fillcolor=black, fillstyle=solid](-0.08,104.55414013)(0,109.53290870)(0.08,104.55414013)" << std::endl;
   
   int i=0;
   for(it = p.begin(); it != p.end(); it++)
   {
      // detect underscores in the key and prefix them with a '\' 
      // so its latex conform
      std::string key = gsse::check_for_special_characters(it->first);      

      double percentage = ( boost::lexical_cast<double>(it->second) /  boost::lexical_cast<double>(sum))*100.0;

   #ifdef DEBUG
      std::cout << "  key_size" << key_size << std::endl;
      std::cout << "  text_vertical_position: " << text_vertical_position << std::endl;
   #endif         
      ostr.precision(2);
      ostr << "\\psframe[linewidth=1pt, fillcolor=gray, fillstyle=solid] (" << i 
         << ".30000000,0)(" << i << ".70000000," << percentage << ")" << std::endl;
      ostr << "\\rput[bl](" << i++ << ".35000000,-" << text_vertical_position << "){\\rotateleft{\\footnotesize " 
         << key << "}}" << std::endl;
   }

   ostr << "\\end{pspicture}" << std::endl;
   if(draw_box)
      ostr << "}" << std::endl;      
   ostr << "\\end{minipage}" << std::endl;

   ostr << std::endl;

   ostr << "%------------" << std::endl;
   ostr << "%%% TABLE %%%" << std::endl;
   ostr << "%------------" << std::endl;

   // ##########
   // create statistics table
   ostr << "\\begin{minipage}[ht]{0.20\\textwidth}" << std::endl;
   ostr << "\\begin{tabular}{crr}" << std::endl;

   if(dumpcout)
   {
      std::cout << "## mesh classification: " << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
   }

   for(it = p.begin(); it != p.end(); it++)
   {
      double percentage = ( boost::lexical_cast<double>(it->second) /  boost::lexical_cast<double>(sum))*100.0;      
      if(dumpcout)
      {
         std::cout << "  " << gsse::check_for_special_characters(it->first) << "    \t" << percentage << " %" << std::endl;
         std::cout << "---------------------------------------------" << std::endl;
      }
      ostr << gsse::check_for_special_characters(it->first) << " & " << it->second << " & " << percentage << "\\% \\\\" << std::endl;
   }

   ostr << "\\end{tabular}" << std::endl;
   ostr << "\\end{minipage}" << std::endl;
   ostr << "%------------" << std::endl;
   ostr << "%------------" << std::endl;
   ostr << std::endl;   
}


}  // end namespace: gsse

#endif
