/**************************************************************************/
/* File:   spbita2d.cpp                                                   */
/* Author: Joachim Schoeberl                                              */
/* Date:   01. Jun. 95                                                    */
/**************************************************************************/

/* 
   Implementation of sparse 2 dimensional bitarray
*/


#include <mystdlib.h>
#include <myadt.hpp>

namespace vgmodeler
{
  using namespace vgmodeler;

  // [RH] correct data types
  //
  SPARSE_BIT_ARRAY_2D :: SPARSE_BIT_ARRAY_2D (INDEX ah, INDEX aw)
  {
    lines = NULL;
    SetSize (ah, aw);
  }

  SPARSE_BIT_ARRAY_2D :: ~SPARSE_BIT_ARRAY_2D ()
  {
    DeleteElements ();
    delete lines;
  }


  // [RH] correct data types
  //
  void SPARSE_BIT_ARRAY_2D :: SetSize (INDEX ah, INDEX aw)
  {
    DeleteElements();
    if (lines)
      {
	delete lines;
	lines = NULL;
      }

    if (!aw) aw = ah;

    height = ah;
    width = aw;

    if (!ah) return;
    lines = new linestruct[ah];

    if (lines)
      {
	for (int i = 0; i < ah; i++)
	  {
	    lines[i].size = 0;
	    lines[i].maxsize = 0;
	    lines[i].col = NULL;
	  }
      }
    else
      {
	height = width = 0;
	std::cout << "SPARSE_ARRAY::SetSize: Out of memory" << std::endl;
      }
  }



  void SPARSE_BIT_ARRAY_2D :: DeleteElements ()
  {
    if (lines)
      {
	for (int i = 0; i < height; i++)
	  {
	    if (lines[i].col)
	      {
		delete [] lines[i].col;
		lines[i].col = NULL;
		lines[i].size = 0;
		lines[i].maxsize = 0;
	      }
	  }
      }
  }


  // [RH] correct data type
  //
  int SPARSE_BIT_ARRAY_2D :: Test (INDEX i, INDEX j) const
  {
    INDEX k, max, *col;

    if (!lines) return 0;
    if (i < 1 || i > height) return 0;

    col = lines[i-1].col;
    max = lines[i-1].size;

    for (k = 0; k < max; k++, col++)
      if (*col == j) return 1;

    return 0;
  }



  // [RH] correct data type
  //
  void SPARSE_BIT_ARRAY_2D :: Set(INDEX i, INDEX j)
  {
    INDEX k, max, *col;

    i--;
    col = lines[i].col;
    max = lines[i].size;

    for (k = 0; k < max; k++, col++)
      if (*col == j)
	return;

    if (lines[i].size)
      {
	if (lines[i].size == lines[i].maxsize)
	  {
	    col = new INDEX[lines[i].maxsize+2];
	    if (col)
	      {
		lines[i].maxsize += 2;
		memcpy (col, lines[i].col, sizeof (int) * lines[i].size);
		delete [] lines[i].col;
		lines[i].col = col;
	      }
	    else
	      {
		std::cout << "SPARSE_BIT_ARRAY::Set: Out of mem 1" << std::endl;
		return;
	      }
	  }
	else
	  col = lines[i].col;

	if (col)
	  {
	    k = lines[i].size-1;
	    while (k >= 0 && col[k] > j)
	      {
		col[k+1] = col[k];
		k--;
	      }

	    k++;
	    lines[i].size++;
	    col[k] = j;
	    return;
	  }
	else
	  {
	    std::cout << "SPARSE_ARRAY::Set: Out of memory 2" << std::endl;;
	  }
      }
    else
      {
	lines[i].col = new INDEX[4];
	if (lines[i].col)
	  {
	    lines[i].maxsize = 4;
	    lines[i].size = 1;
	    lines[i].col[0] = j;
	    return;
	  }
	else
	  {
	    std::cout << "SparseMatrix::Elem: Out of memory 3" << std::endl;
	  }
      }
  }

}
