/* ***********************************************************************
   $Id: mattools.hh,v 1.2 2003/03/11 13:01:54 wagner Exp $
   Filename: mattest8.cc

   Description: Several tools used in mattest programs

   Author: CS   Christoph Schwarz, Institute for Microelectronics, TU Vienna
           GM   Georg Mach,        Institute for Microelectronics, TU Vienna

   Who When         What
   -------------------------------------------------------------------------
   CS  06 Mar 2003  argument parsing class
   GM  06 Mar 2003  matrix generating functions

   *************************************************************************

*/

#ifndef __mattools_hh__included__
#define __mattools_hh__included__

#include "numerics_all.hh"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>


/* Return value constants required by the test script */
/* ================================================== */

typedef enum retVal_e
{
   rvOK            = 0, // everything allright
   rvNotSolved     = 1, // qqqSolve... returned false
   rvSolutionError = 2, // solution was compared and is not equal
   rvAllocErr      = 3, // error allocating memory
   rvCopyError     = 4, // copy/conversion failed
   rvArgumentError = 5,
   rvIOError       = 6, // input/output, eg file not found, header mismatch...
   rvFormatError   = 7, // dimension, ...
   rvGeneralError  = 8, // due to unsufficient error checking, no more detailed mapping
   rvUnknown
} retVal_t;


/* Argument parser class */
/* ===================== */

typedef struct apFlag_s
{
  const char * name;
  bool   set;
  struct apFlag_s * next;
} apFlag_t;

typedef struct apOption_s
{
  const char * name;
  const char * text;
  struct apOption_s * next;
} apOption_t;

typedef struct apFile_s
{
  const char * name;
  const char * filename;
  bool readable;
  struct apFile_s * next;
} apFile_t;

typedef enum apType_e
{
  apUNKNOWN = 0,
  apFLAG    = 1,
  apOPTION  = 2,
  apFILE    = 3
} apType_t;

class ArgParser
{
private:
  apFlag_t    *flags;
  apOption_t  *options;
  apFile_t    *files;

  apType_t     Type        (const char * name) const;
  bool        &Flag        (const char * name) const;
  const char **Option      (const char * name) const;
  const char **Filename    (const char * name) const;
  bool         ReadableFile(const char * name) const;

  void         dump() const;

public:
  /* constructor / destructor */
   ArgParser();
  ~ArgParser();

  /* init arguments */
  bool AddFlag  (const char * name);
  bool AddOption(const char * name);
  bool AddFile  (const char * name, bool readable = true);

  /* test/access arguments */
  bool         FlagSet  (const char * name) const;
  bool         OptionSet(const char * name) const;
  bool         FileSet  (const char * name) const;
  const char * OptionStr(const char * name) const;
  const char * FileStr  (const char * name) const;

  /* parse the arguments */
  bool Parse(int argc, char *argv[], bool showerr = true);

}; /* class ArgParser */

/* Matrix generating functions */
/* =========================== */

bool makeComplexMatrix  (qqqMCSR<qqqComplex> &C, const long dim, bool print);
bool makeRealMatrix     (qqqMCSR<double>     &C, const long dim, bool print);
bool makeHermitianMatrix(qqqMCSR<qqqComplex> &C, const long dim, bool print);
bool makeSymmetricMatrix(qqqMCSR<double>     &C, const long dim, bool print);

#endif // __mattools_hh__included__
