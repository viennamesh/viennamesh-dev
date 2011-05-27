/* ***********************************************************************
   $Id: mattools.cc,v 1.3 2004/03/03 14:36:38 wagner Exp $
   Filename: mattest8.cc

   Description: Several tools used in mattest programs

   Author: CS   Christoph Schwarz, Institute for Microelectronics, TU Vienna
           GM   Georg Mach,        Institute for Microelectronics, TU Vienna

   Who When         What
   -------------------------------------------------------------------------
   CS  06 Mar 2003  argument parsing class
   GM  06 Mar 2003  matrix generating functions

   *************************************************************************   */

#include "mattools.hh"


/* Argument parser class */
/* ===================== */

static char const * emptystr = "";

ArgParser::ArgParser()
{
   flags   = NULL;
   options = NULL;
   files   = NULL;
}

ArgParser::~ArgParser()
{
   apFlag_t * rmflag;
   while ((rmflag = flags))
   {
      flags = rmflag->next;
      delete rmflag;
   }
   apOption_t * rmoption;
   while ((rmoption = options))
   {
      options = rmoption->next;
      delete rmoption;
   }
   apFile_t * rmfile;
   while ((rmfile = files))
   {
      files = rmfile->next;
      delete rmfile;
   }
   return;
}

bool ArgParser::AddFlag(const char * name)
{
   apFlag_t * add;

   if ((add = new apFlag_t) == NULL)
      return false;

   add->next = flags;
   add->name = name;
   add->set  = false;
   flags = add;
   return true;
}

bool ArgParser::AddOption(const char * name)
{
   apOption_t * add;

   if ((add = new apOption_t) == NULL)
      return false;

   add->next = options;
   add->name = name;
   add->text = emptystr;
   options = add;
   return true;
}

bool ArgParser::AddFile(char const * name, bool readable)
{
   apFile_t * add;

   if ((add = new apFile_t) == NULL)
      return false;

   add->next = files;
   add->name = name;
   add->filename = emptystr;
   add->readable = readable;
   files = add;

   return true;
}


void ArgParser::dump() const
{
   void * pointer;
   pointer = flags;
   while (pointer)
   {
      printf("FLAG '%s'\n", ((apFlag_t *)pointer)->name);
      pointer = ((apFlag_t *)pointer)->next;
   }
   pointer = options;
   while (pointer)
   {
      printf("OPTION '%s'\n", ((apOption_t *)pointer)->name);
      pointer = ((apOption_t *)pointer)->next;
   }
   pointer = files;
   while (pointer)
   {
      printf("FILE '%s'\n", ((apFile_t *)pointer)->name);
      pointer = ((apFile_t *)pointer)->next;
   }
}


bool ArgParser::Parse(int argc, char * argv[], bool showerr)
{
   //  Dump();
   bool retval = true;
   FILE * readtest = NULL;
   for (int i = 1; i < argc; ++i)
   {
      switch (Type(argv[i]))
      {
      case apFLAG:
	 if (FlagSet(argv[i]))
	 {
	    if (showerr) 
	       printf("%s: flag %s specified more than once\n", argv[0], argv[i]);
	    retval = false;
	 }
	 Flag(argv[i]) = true;
	 break;
      case apOPTION:
	 if ((i+1) == argc)
	 {
	    if (showerr) 	    
	       printf("%s: option %s needs an argument\n", argv[0], argv[i]);
	    retval = false;
	 }
	 if ((strlen(argv[i+1]) > 1) && (argv[i+1][0] == '-'))
	 {
	    if (showerr) 	    
	       printf("%s: option %s needs an argument; \"%s\" is not believed to be one\n", argv[0], argv[i], argv[i+1]);
	    retval = false;
	 }
	 if (OptionSet(argv[i]))
	 {
	    if (showerr)
	       printf("%s: option %s specified more than once\n", argv[0], argv[i]);
	    retval = false;
	 }
	 *Option(argv[i]) = argv[i+1];
	 i++;
	 break;
      case apFILE:
	 if (ReadableFile(argv[i]) && ((readtest = fopen(argv[i], "r")) == NULL))
	 {
	    if (showerr) 
	       printf("%s: file '%s' cannot be read\n", argv[0], argv[i]);
	    retval = false;
	 }
	 if (ReadableFile(argv[i]) && (readtest != NULL)) fclose(readtest);
	 if (FileSet(argv[i]))
	 {
	    if (showerr) 
	       printf("%s: file with type of '%s' specified more than once\n", argv[0], argv[i]);
	    retval = false;
	 }
	 *Filename(argv[i]) = argv[i];
	 fflush(stdout);
	 break;
      case apUNKNOWN:
      default:
	 if (showerr) 
	    printf("%s: unrecognized option '%s'\n", argv[0], argv[i]);
	 retval = false;
      }
   }
   // dump();
   return retval;
}

static bool strendswith(const char * str, const char * end)
{
   if (strlen(end) > strlen(str))
      return false;
   for (unsigned int i = 1; i <= strlen(end); ++i)
      if (str[strlen(str)-i] != end[strlen(end)-i])
	 return false;
   return true;
}

apType_t ArgParser::Type(const char * name) const
{
   apFlag_t   *pflag = flags;
   apOption_t *popt  = options;
   apFile_t   *pfile = files;

   while (pflag)
   {
      if (strcmp(pflag->name, name) == 0)
	 return apFLAG;
      pflag = pflag->next;
   }
   while (popt)
   {
      if (strcmp(popt->name, name) == 0)
	 return apOPTION;
      popt = popt->next;
   }
   while (pfile)
   {
      if (strendswith(name, pfile->name))
	 return apFILE;
      pfile = pfile->next;
   }
   
   return apUNKNOWN;
}

bool & ArgParser::Flag(const char * name) const
{
   static bool retValFalse = false;	
   apFlag_t * curr = flags;
   retValFalse = false;

   while (curr)
   {
      if (strcmp(name, curr->name) == 0)
	 return curr->set;
      curr = curr->next;
   }
   return retValFalse;      
}

const char **ArgParser::Option(const char *name) const
{
   apOption_t * curr = options;

   while (curr)
   {
      if (strcmp(name, curr->name) == 0)
	 return &curr->text;
      curr = curr->next;
   }
   return &emptystr;	
}

const char **ArgParser::Filename(const char *name) const
{
   apFile_t * curr = files;

   while (curr)
   {
      if (strendswith(name, curr->name))
	 return &curr->filename;
      curr = curr->next;
   }
   return &emptystr;
}

bool ArgParser::ReadableFile(const char *name) const
{
   apFile_t * curr = files;

   while (curr)
   {
      if (strendswith(name, curr->name))
	 return curr->readable;
      curr = curr->next;
   }
   return false;
}

bool ArgParser::FlagSet(const char * name) const
{
   if (Type(name) == apFLAG)
      return Flag(name);
   fprintf(stderr, "!! please debug your program: flag '%s' was not created\n", name);
   return false;
}

bool ArgParser::OptionSet(const char * name) const
{
   if (Type(name) == apOPTION)
      return (strlen(*Option(name)) > 0);
   fprintf(stderr, "!! please debug your program: option '%s' was not created\n", name);
   return emptystr;
}

bool ArgParser::FileSet(const char * name) const
{
   if (Type(name) == apFILE)
      return (strlen(*Filename(name)) > 0);
   fprintf(stderr, "!! please debug your program: file-extension '%s' was not created\n", name);
   return emptystr;
}

const char * ArgParser::OptionStr(const char * name) const
{
   if (Type(name) == apOPTION)
      return *Option(name);
   fprintf(stderr, "!! please debug your program: option '%s' was not created\n", name);
   return emptystr;
}

const char * ArgParser::FileStr(const char * name) const
{
   if (Type(name) == apFILE)
      return *Filename(name);
   fprintf(stderr, "!! please debug your prograM; file-extention '%s' was not created\n", name);
   return emptystr;
}


/* Matrix generating functions */
/* =========================== */

/* makeComplexMatrix(...)

   sets up and prints a complex valued Test-Matrix

*/
bool makeComplexMatrix(qqqMCSR<qqqComplex> &C, const long dim, bool print = false)
{
   long i, j;
   qqqSMatrix<qqqComplex> sC;
  
   sC.allocate(dim, true); 
  
   /* Set main diagonal */
   for(i = 0; i < dim; i++)
      sC.entry(i, i) = (double)i; // (double)i);
  
   /* Set codiagonals */
   for(i = 0; i < dim - 1; i++)
      sC.entry(i, i+1) = qqqComplex(i, 1);
   for(i = 0; i < dim - 1; i++)
      sC.entry(i+1, i) = qqqComplex(i, 2);
  
   sC.convertToMCSR(C, true);
  
   if (print)
   {
      /* Print matrix */
      printf("\nComplex Test-Matrix\n");
      for (i = 0; i < dim; i++)
      {
	 printf("[%ld] ", i);
	 for (j = 0; j < dim; j++)
	    printf("% e j% e    ", qqqReal(sC.entry(i, j)), qqqImag(sC.entry(i, j)));
	 printf("\n");
      }
   }

   sC.free();
   return true;
}

/* makeRealMatrix(...)

   sets up and prints a complex valued Test-Matrix
*/
bool makeRealMatrix(qqqMCSR<double> &R, const long dim, bool print = false)
{
   long i, j;
   qqqSMatrix<double> sR;
  
   sR.allocate(dim, true); 
  
   /* Set main diagonal */
   for(i = 0; i < dim; i++)
      sR.entry(i, i) = (double)i;
  
   /* Set codiagonals */
   for(i = 0; i < dim - 1; i++)
      sR.entry(i, i+1) = 1.5;
   for(i = 0; i < dim - 1; i++)
      sR.entry(i+1, i) = 2.0;
  
   /* Convert SMatrix */ 
   sR.convertToMCSR(R, true);
  
   if (print)
   {
      /* Print matrix */
      printf("\nReal Test-Matrix\n");
      for (i = 0; i < dim; i++)
      {
	 printf("[%ld] ", i);
	 for (j = 0; j < dim; j++)
	    printf("% e    ", sR.entry(i, j));
	 printf("\n");
      }
   }

   sR.free();
   return true;
}

/* makeHermitianMatrix(...)

   sets up and prints a complex valued hermitian Test-Matrix
*/
bool makeHermitianMatrix(qqqMCSR<qqqComplex> &H, const long dim, bool print = false)
{
   long i, j;
   qqqSMatrix<qqqComplex> sH;
  
   sH.allocate(dim, true); 
  
   /* Set main diagonal */
   for(qqqIndex ccirow = 0; ccirow < dim; ccirow++)
      sH.entry(ccirow, ccirow) = qqqComplex(ccirow, 0); // (double)i);
  
   /* Set codiagonals */
   for(qqqIndex ccirow = 0; ccirow < dim - 1; ccirow++)
   {
      qqqComplex const value(ccirow, 1.0);
      sH.entry(ccirow, ccirow+1) = value;
   }
   for(qqqIndex ccirow = 0; ccirow < dim - 1; ccirow++)
   {
      qqqComplex const value(ccirow, -1.0);
      sH.entry(ccirow+1, ccirow) = value;
   }

   sH.convertToMCSR(H, true);
  
   if (print)
   {
      /* Print matrix */
      printf("\nHermitian Test-Matrix\n");
      for (i = 0; i < dim; i++)
      {
	 printf("[%ld] ", i);
	 for (j = 0; j < dim; j++)
	    printf("% e j% e    ", qqqReal(sH.entry(i, j)), qqqImag(sH.entry(i, j)));
	 printf("\n");
      }
   }

   sH.free();
   return true;
}

/* makeSymmetricMatrix(...)

   sets up and prints a double valued symmetric Test-Matrix
*/
bool makeSymmetricMatrix(qqqMCSR<double> &S, const long dim, bool print = false)
{
   long i, j;
   qqqSMatrix<double> sS;
  
   sS.allocate(dim, true);
  
   /* Set main diagonal */
   for(i = 0; i < dim; i++)
      sS.entry(i, i) = (double)i; // (double)i);
  
   /* Set codiagonals */
   for(i = 0; i < dim - 1; i++)
      sS.entry(i, i+1) = 1.0;
   for(i = 0; i < dim - 1; i++)
      sS.entry(i+1, i) = 1.0;
  
   sS.convertToMCSR(S, true);
  
   if (print)
   {
      /* Print matrix */
      printf("\nSymmetric Test-Matrix\n");
      for (i = 0; i < dim; i++)
      {
	 printf("[%ld] ", i);
	 for (j = 0; j < dim; j++)
	    printf("% e    ", sS.entry(i, j));
	 printf("\n");
      }
   }

   sS.free();
   return true;
}
