/* ***********************************************************************
   $Id: numerics_eqs.hh,v 1.32 2004/05/08 16:30:38 wagner Exp $

   Filename: numerics_eqs.hh

   Description:  Equation system building and solving

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

             SW  Stephan Wagner,   Institute for Microelectronics, TU Vienna
				   

   Who When         What
   -----------------------------------------------------------------------
   CF  15 Jan 1997  created
   CF  15 Jan 1997  started defining the class
   SW  2000 - 2001  mmnt integration, scaling
   SW  06 Feb 2002  qqq prefixed function names
   SW  11 Feb 2002  reordering the parameter class
   SW  13 Feb 2002  division of allocation/free methods
   SW  27 Feb 2002  first Newton adjustment step
   SW  04 Mar 2002  second Newton adjustment step
   SW  05 Mar 2002  third Newton adjustment step
   SW  08 Mar 2002  forth Newton adjustment step
   SW  25 Mar 2002  two phases models
   SW  27 Aug 2002  parameter input/output
   SW  28 Aug 2002  system input/output reordered, parameters refined
   SW  10 Mar 2003  external interface added
   SW  17 Apr 2003  qqqEquationInfo class
   SW  25 Apr 2003  parameter and related code moved to numerics_param
   ***********************************************************************   */

#ifndef   __numerics_eqs_hh__included__
#define   __numerics_eqs_hh__included__

#include "numerics_param.hh"

/* class qqqEQS
   ============
   
   This class provides a data type for a linear equation system.


   *** Principal concept ***

   * Control function
   Although the principal concept has a purely mathematical representation and
   is independent of the box concept, the explanation below is given in terms
   of boxes and fluxes. An abstraction to other applications is fairly easy,
   once the mathematical concept has become familiar.

   In the full linear system, as well as in partial systems, the concept of
   control functions is essential. A control function of a variable in the
   full linear system is a function that is assigned to the variable and
   represents implicitely an equation: the objective of solving is to bring
   the control function to zero. A partial control function is a function
   that is assigned to the variable and will contribute to the full system,
   generally in the full control function for the same variable, although
   for boundary points there may be exceptions.
   
   The full internal equation system is built by linearizing the nonlinear
   equation (1), which is the full nonlinear equation system. Inside this
   class, we do not know about the functional connection between v and
   b, however; we only deal with the linearized parts. It is the purpose of
   the model functions and the nonlinear iterator to fill the components
   of this class properly with information. The vector b is the vector of
   the full control functions for all variables of the equation system.
   (1  )   b(v) = 0

   A box is sort of a geometric representation of the control function;
   in a Finite Boxes scheme, there is a box around each grid point, and
   the control function for a variable on that grid point is the sum of
   fluxes of a well-determined physical quantity out of that box.

   On boundaries, boxes may be submitted to some modifications depending
   on the boundary conditions, e.g. two grid points (with identical
   coordinates) on a material boundary for the Poisson equation may be
   related by an equation, and the dielectric fluxes of both boxes may
   be merged into one big box that extends across the boundary.

   * Main system: As and bs
   The full internal equation system is built from an original system, (main
   system) which is basically the main matrix As and the main right hand side
   vector bs, both of them representing cumulated fluxes and their derivatives
   to the system variables. The fluxes are calculated from space models
   (models for the interior of discretized regions).  The matrix is a linear
   superposition of very small matrices, one for each flux, with a few
   nonzero elements only. The same superposition applies for the vector bs.
   The fluxes are assigned to boxes, a box is in turn assigned to each
   variable. The whole system depends heavily on a sort of diagonal dominance,
   which is not strictly required in a mathematical sense but must nevertheless
   reflect in the matrix structure and numbers.

   As the control function for a box is defined by the user, i.e. being the
   sum of all fluxes leaving the box, consequently the fluxes leaving the
   boxes are entered into the vector bs in the places appropriate for the
   variables that are assigned to the boxes.

   The system of As and bs does not represent an equation system, actually,
   for there are no boundary conditions present.  The matrix As contains the
   negative derivatives of the values in bs to the system variables, so that
   the change dx in the variables leads to a change -As * dv = dbs in the
   right hand side. Considering bs a function of the variable vector v, one
   can write:
   (2  )   bs = bs(v)
   (3  )   As = -dbs/dv

   * Boundary conditions: Ab, bb, and Tb
   Generally, all variables represent meaningful physical quantities,
   so the fluxes that are entered into the main system have some
   physical interpretation. The boundary conditions will perhaps enforce
   some special physical proportions at the boundaries; however, this does
   not make the physical quantities calculated in the main system meaningless.

   The control functions of boxes along the boundary will usually be
   disposed of in a particular way by the boundary conditions; i.e. a
   Dirichlet boundary condition will use the dielectric flux cumulated
   in the boundary box to calculate the surface charge on the surface
   of the adjacent material (i.e. the metal of the contact).

   The equation used to calculate the value of the boundary variable,
   however, will not always make use of the fluxes accumulated in the
   main system.

   The boundary conditions are therefore implemented by a two elements: a
   boundary system (Ab and bb) and a transformation matrix Tb.
   The purpose of the matrix Tb is the forwarding of the fluxes of the
   main system to their final destination; the system of Ab and bb
   represents additional or substitutional parts of the final equation
   for the variables at the boundaries. Again, the entries in the matrix As
   are the negative derivatives of the right hand side vector bb to the
   variable vector v:
   (4  )   bb = bb(v)
   (5  )   Ab = -dbb/dv

   * Full system
   The full system is assembled from the main system and the boundary
   system in the following way:
   (6  )   Tb(v) = const
   (7  )   b = Tb * bs + bb
   (8  )   A = Tb * As + Ab
   (9  )   A = -db/dv
   (10a)   A * x = b
   (10b)   (Tb * As + Ab) * x = Tb * bs + bb
   
   Equation (9) is a consequence of the equations above, and equations
   (10a) and (10b) will be used as synonyms. The vector x represents a
   linear change of the variables vector v; with (11) and the new variables
   vector vn from (12), the new value bn of the vector function b(v) will
   be described by the linear approximation in (13).
   (11 )   x  = dv
   (12 )   vn = v + x
   (13 )   bn(vn) =approx= b + db/dv * dv = b - A * x = 0
   
   * Updating the variables vector
   How the changes described in the solution vector x are applied to the
   vector v, is a matter out of scope of this part of code. Here, we only
   deal with the linearized situation described by the equations above.

   * Default transformation
   If the user chooses not to use boundary conditions as specified above,
   the effect is as if Tb were specified a unit matrix and Ab and bb
   a zero matrix and zero vector, respectively.


   *** Preelimination ***

   * Motivation
   The main matrix As consists of fluxes that will (if the control functions
   are correctly assigned to the variables) satisfy the vaguely formulated
   criterion of diagonal-dominance that is necessary to make the equation 
   system solvable. The transformations and additional terms imposed by the
   boundary conditions may heavily disrupt this feature both in structural
   and numerical aspects. Some of the boundary or interface conditions
   will make the full system matrix so ill-conditioned that this simply
   prevents iterative linear solvers from converging.

   * The concept
   The solution to this problem, which occurs only at the boundary variables
   that are affected in this way by the boundary conditions, is to apply
   Gaussian elimination to these variables/equations before the system is
   passed on to the linear solver. After the iterative solver has converged,
   the eliminated variables are calculated by backsubstitution into the 
   eliminated equations. This process is a partial Gaussian factorization
   of the matrix. Here, it is called pre-elimination.

   * Implementation
   Before they can be eliminated, the equations of this type are sorted to
   the back of the matrix, together with their assigned variables. This is
   done by applying a permutation matrix P to the equation system. The
   permutation matrix is calculated automatically on solving the system;
   the user has to mark the equations of possibly ill condition for
   pre-elimination. The system passed to the linear solver therefore
   changes from (10b) to (14):
   (14 )   E * P * (Tb * As + Ab) * Pt * P * x = E * P * (Tb * bs + bb)

   Here, P is a permutation matrix with its inverse equal to its transposed
   matrix Pt; E is a matrix of elimination coefficients obtained as the
   lower matrix L of a Gaussian elimination of the permuted system matrix.
   E contains nonzero offdiagonals in the outer parts only, the inner matrix
   up to the row/column index that sizes the section passed to the linear
   solver is a strict unity matrix.

   * Default behaviour
   If the user chooses not to use pre-elimination, the effect is as if
   no equations were marked for pre-elimination and P and E were unit
   matrices.

   
   *** Variable transformation ***

   Especially in the case of mixed quantities in the solution vector v/x,
   a variable transformation is sometimes helpful in improving the condition
   of the linear system. The formulation chosen here allows to specify
   arbitrary variable transformations to be applied to the system.

   * Restrictions
   Three restrictions, however, apply to the transformation:
   First, it must be revertible, i.e. the transformation must be expressed
   by a matrix which has an inverse.
   Second, the transformation (as all operations) must not destroy the
   property vaguely expressed as diagonal dominance above. So, the
   transformation matrix itself must be (approximately) diagonally dominant,
   and the new variables created by the transformation must be somehow
   related to the old variables they substitute in the same places in the
   vector. (I.e. this means, substituting the system psi, n, and p, by the
   system psi, phi-n, and phi-p for semiconductors with phi-n in the place
   of n and phi-p in the place of p.)
   Thirdly, the transformation matrix must decompose into small submatrices
   with a limited number of variables involved in a single transformation.
   I.e. it is a good concept to restrict variable transformation to the
   systems of variables on the specific grid points, without considering
   interconnection of these.

   * Representation
   The transformation is expressed by a matrix Tv as described in (15):
   (15 )   E * P * (Tb * As + Ab) * Tv * Pt * P * Tv^-1 *  x = 
                                                   E * P * (Tb * bs + bb)

   Internally, the matrix Tv is inverted by Gaussian elimination/factorization,
   with no pivoting but considering all entries that are created during
   elimination. Therefore, the number of variables forming a partial
   transformation matrix must be limited. The variables that are connected
   in the transformation may be spread across the whole matrix structure,
   however. Still, it would be a bad idea to use the transformation matrix
   Tv to formulate another equation system.

   * Default transformation
   If the user chooses not to apply a variable transformation, the effect
   is as if Tv were specified a unit matrix.


   *** Scaling ***

   * Motivation
   The user can scale the variable vector and the right hand side vector
   independently from the equation system assembly. Two diagonal matrices,
   Sr and Sc, are applied to the rows and columns of the full system matrix,
   respectively. Scaling can improve the condition of the linear equation
   system and therefore help the iterative solver. Scaling also has an effect
   on the norms of the internal vectors in the iterative solver, and on the
   final accuracy of the solution (for the norm of the right hand side
   is affected, which is reduced by a factor given by the accuracy).

   * Representation
   The scaling matrices Sr, Sc are diagonal matrices. Their effect is
   described in (16):
   (16 )   E * P * Sr * (Tb * As + Ab) * Tv * Sc * Pt *
                     P * Sc^-1 * Tv^-1 * x = E * P * Sr * (Tb * bs + bb)

   * Default scaling
   If the user chooses not to apply scaling matrices, the effect is as if
   Sr or Sc were specified unit matrices. The choice of using row and column
   scaling is independent.


   *** Components ***

   Final system:
   The final linear system is described by equations (17) - (20):
   (17 )   Al = E * P * Sr * (Tb * As + Ab) * Tv * Sc * Pt
   (18 )   bl = E * P * Sr * (Tb * bs + bb)
   (19 )   xl = P * Sc^-1 * Tv^-1 * x
   (20 )   Al * xl = bl

   User-visible components:
   The linear system is built from several components. Here is an
   alphabetical list of the components that can be directly accessed
   by the user:
   Ab ... additive matrix of boundary conditions
   As ... main matrix of space (interior of regions)
   bb ... additive right hand side of boundary conditions
   bs ... main right hand side vector of space (interior of regions)
   Sr ... scaling vector for rows (diagonal matrix)
   Sc ... scaling vector for columns (diagonal matrix)
   Tb ... transformation matrix of boundary conditions
   Tv ... transformation matrix for variables
   The accessor functions allow the user to directly specify these
   elements.
   For the matrices Tv, Sr, and Sc, the user has the choice of specifying
   the inverted matrices instead of the original ones. In this case, the
   matrix used internally will be automatically computed.

   Automatically created components:
   The matrices E and P are created after the user has defined the
   equations that should be pre-eliminated. The full matrix Al and bl
   are automatically calculated from their components. The vector xl
   is calculated by the linear solver. The vector x is calculated by
   reverting equation (19). Many auxiliary items are calculated during
   the linear solving in the function qqqSolveLinearSystem().
   
   User-visible auxiliaries:
   e  ... vector with pre-elimination flags, visible through access functions
   p  ... priority vector (deals with priorities of boundary conditions),
          visible through access functions
 
   *** Linear solver ***

   The linear solving is performed by the function qqqSolveLinearSystem().
   For a description of the solving algorithm, auxiliary items created,
   space and time consumption and of the parameter structure see this
   function.

   *** Calling sequence ***
   
   The member functions of this class should be called in the following
   sequence:
   1. allocate()
   2. any functions entering data into the system
   3. solve()
   4. free()
   */

class qqqEQS
{
private:
   void *hidden;		/* Points to the encapsulated data structure
				   containing all the equation system data */
   bool  flagIsComplex;	        /* The equation system contains
				   complex numbers */
   bool  flagAllErr;		/* Whether an allocation error has occurred */
   bool  flagAccess;		/* Access without previous successful
				   allocation */
   bool  flagComplexAccess;	/* Access to a complex member of a real-valued
				   equation system has been required */

   qqqComplex cdummy;           /* references must reference an valid object 
				   (only pointers may be NULL) */
   double     rdummy;

public:
   /* Constructor and destructor */
   /* ========================== */

   /* qqqEQS()

      constructs the structure and presets it with zeros. Afterwards, the
      function allocate() must be called.
   */
   qqqEQS(qqqSolverParameters &parms);

   /* ~qqqEQS()

      deletes the structure. It frees everything allocated before.  The
      structure may be deleted anytime.
   */
   ~qqqEQS();

public:

   /* Administration functions (slow) */
   /* =============================== */

   /* allocate()

      is the first function to be called after creation of the class object.
      Calling any other function except free() after object creation is an
      error. This function sets the 'hidden' pointer to point to allocated
      data.

      The function allocates all permanent data structures. The function can be
      called anytime; it discards all previous information stored in the
      equation system and resets the error information.

      The function returns true on successful allocation, false on allocation
      errors or bad parameters (dim or rhs too small). If the function returns
      false, the function isValid() can be used to determine the error. 
   */
   bool 
   allocate(qqqSolverParameters &parms,          /* [I/O] solver parameters  */
	    const bool           initE = false,  /* [I] initialization value */
	    const qqqIndex       initP = -1);    /* [I] initialization value */


   /* allocateNewton()

      is called before the assembly of one Newton iteration step starts.

      The function allocates all Newton dependent data structures. The function
      can be called anytime; it discards all previous Newton information stored
      in the equation system and resets the error information.

      The function returns true on successful allocation, false on allocation
      errors.  If the function returns false, the function isValid() can be
      used to determine the error.
   */
   bool
   allocateNewton(qqqSolverParameters &parms); /* [I/O] solver parameters */

   /* freeNewton()

      can be called anytime between creating and deleting the object. It frees
      all memory allocated in connection with the Newton equation system. It
      also resets all error information and discards all other data. 
   */
   void
   freeNewton();


   /* free()

      can be called anytime between creating and deleting the object. It frees
      all memory allocated in connection with the equation system.  It also
      resets all error information and discards all other data. 
   */
   void
   free();

   /* readLinearSystem()

      reads a linear system from the specified file. It discards all previous
      information stored in the equation system and resets the error
      information. Reading of additional information can be switched on (see
      parameter readAuxiliary).  

      Remark: use parameter linearSystem (together with writeAuxiliary) to
      write a linear system.
   */
   bool                                   
   readLinearSystem(	
        qqqSolverParameters &parms     /* [I/O] solver parameters              */
	);

   /* writeLinearSystem()

      writes a linear system to the specified file. 

   */
   bool                                   
   writeLinearSystem(	
        qqqMCSR<double>     &A,        /* [I/O] matrix to be written (sorted)  */
	const char*          filename, /* [I]   name of the matrix output file */
	      bool           preelims, /* [I]   write preelimination flags?    */
	      bool           qnames,   /* [I]   write quantity information?    */
	      double       **rhs,      /* [I]   write rhs vectors?             */
        qqqSolverParameters &parms     /* [I/O] solver parameters              */
	);

   bool                                   
   writeComplexLinearSystem(	
        qqqMCSR<qqqComplex> &A,        /* [I/O] matrix to be written (sorted)  */
	const char*          filename, /* [I]   name of the matrix output file */
	      bool           preelims, /* [I]   write preelimination flags?    */
	      bool           qnames,   /* [I]   write quantity information?    */
	      qqqComplex   **rhs,      /* [I]   write rhs vectors?             */
        qqqSolverParameters &parms     /* [I/O] solver parameters              */
	);

   /* solve()

      compiles and solves the linear system.

      can be called anytime the components of the equation system are properly
      preset. The function returns true if solving has succeeded, false in case
      of an error. In this case, the function isValid() can be used to get the
      error code.
   */

   bool
   solve(qqqSolverParameters &parms);   /* [I/O] solver parameters */


   /* isValid()

      can be called anytime between creating and deleting the object.  It
      returns information about the validity of the information stored in the
      class.

      The function returns true if since creation of the object, or since the
      last call to free(), no error has occurred. In this case, the error
      parameter is not changed.  Otherwise, the error parameter will be set to
      an error-enum-number (see type_error.hh for further details.
   */   
   bool
   isValid(qqqError &error) const;  /* [O] error information */


   /* isValidBool()
       
   is similar to isValid() except that it does not return the
   error information.
   */
   bool isValidBool() const;


   /* Access functions (quick) */
   /* ======================== */

   /* dimension()

      returns the dimension of the equation system, a nonnegative integer
      number. The return value is only valid if the system has been
      successfully allocated; if this is not known, it can be checked by
      isValid().
   */
   qqqIndex dimension () const;


   /* getNumberRhs()

      returns the number of different right hand side vectors of the equation
      system, a positive integer number. The return value is only valid if the
      system has been successfully allocated; if this is not known, it can be
      checked by isValid().
   */
   qqqIndex getNumberRhs () const;


   /* As()

      returns a reference to the entry in As() at position row/col. It returns
      a reference to a dummy value if the position specification is out of
      range. In this case, if the define QQQ_EQS_CHECK_INDEX_BOUNDS has been
      enabled at compilation time, the internal error parameter is set and the
      data is marked invalid; else the indexing error is silently ignored.  If
      the position specification indicates a valid position, the matrix entry
      at this position can be modified in an arbitrary way.
      
      If the equation system is complex-valued, this function returns a
      reference to the real part of the value at the specified position.  In
      this case, the function complexAs() returns a reference to the full
      complex value. Else, complexAs() returns a reference to a dummy value.
   */
   double&      As(qqqIndex const row, qqqIndex const col);
   qqqComplex&  complexAs(qqqIndex const row, qqqIndex const col);

   /* Ab() 

      is similar to As(). 
   */
   double&      Ab(qqqIndex const row, qqqIndex const col);
   qqqComplex&  complexAb(qqqIndex const row, qqqIndex const col);

   /* Ab_clearrow() 
      sets the specified row in Ab to all zeros. All entries of the row
      will be removed from the structure, if remove is true. If the row
      specification is out of range, no operation is performed, and if the
      function has been compiled with the define QQQ_EQS_CHECK_INDEX_BOUNDS, the
      internal error parameter is set and the data is marked invalid.
   */
   void AbClearRow(qqqIndex const row, bool const remove = true);
   void AsClearRow(qqqIndex const row, bool const remove = true);

   /* Tb() 

      is similar to As(). 
   */
   double&      Tb(qqqIndex const row, qqqIndex const col);
   qqqComplex&  complexTb(qqqIndex const row, qqqIndex const col);

   /* Tb_clearCol()

      sets the specified column in Tb to all zeros. All entries of the column
      will be removed from the structure, if remove is true.  If the column
      specification is out of range, no operation is performed, and if the
      function has been compiled with the define QQQ_EQS_CHECK_INDEX_BOUNDS, the
      internal error parameter is set and the data is marked invalid.
   */
   void TbClearCol(qqqIndex const col, bool const remove = true);

   /* Tv() 

      is similar to As(). 
   */
   double&      Tv(qqqIndex const row, qqqIndex const col);
   qqqComplex&  complexTv(qqqIndex const row, qqqIndex const col);

   /* Sr()

      returns a reference to the matrix diagonal at position row/row. It
      returns a reference to a dummy value if the position specification is out
      of range. In this case, if the define QQQ_EQS_CHECK_INDEX_BOUNDS has been
      enabled at compilation time, the internal error parameter is set and the
      data is marked invalid; else the indexing error is silently ignored.  If
      the position specification indicates a valid position, the matrix entry
      at this position can be modified in an arbitrary way.

      If the equation system is complex-valued, this function returns a
      reference to the real part of the value at the specified position.  In
      this case, the function complexSr() returns a reference to the full
      complex value. Else, complexSr() returns a reference to a dummy value.
   */
   double&      Sr(qqqIndex const row);
   qqqComplex&  complexSr(qqqIndex const row);

   /* Sc() is similar to Sr(). */
   double&      Sc(qqqIndex const row);
   qqqComplex&  complexSc(qqqIndex const row);

   /* bs()
    
      returns a reference to the right hand side (bs) entry at position row.
      It returns a reference to a dummy value if the position specification is
      out of range. In this case, if the define QQQ_EQS_CHECK_INDEX_BOUNDS has
      been enabled at compilation time, the internal error parameter is set and
      the data is marked invalid; else the indexing error is silently
      ignored. If the position specification indicates a valid position, the
      rhs entry at this position can be modified in an arbitrary way.

      The col index is used to select one of more right-hand side vectors.  The
      first vector (which is always present) has the number 0. The number of
      vectors present has been specified on allocating the system.

      If the equation system is complex-valued, this function returns a
      reference to the real part of the value at the specified position.  In
      this case, the function complexbs() returns a reference to the full
      complex value. Else, complexbs() returns a reference to a dummy value.
   */
   double&      bs(qqqIndex const row, qqqIndex const col = 0);
   qqqComplex&  complexBs(qqqIndex const row, qqqIndex const col = 0);

   /* bb() 
   
      is similar to bs(). 
   */
   double&      bb(qqqIndex const row, qqqIndex const col = 0);
   qqqComplex&  complexBb(qqqIndex const row, qqqIndex const col = 0);

   /* x() 

      is similar to bs(). 
   */
   double&      x (qqqIndex const row, qqqIndex const col = 0);
   qqqComplex&  complexX (qqqIndex const row, qqqIndex const col = 0);


   /* setEliminateFlag()

      sets the eliminate flag for the equation/variable pair with index row.
      If the index is out of range, no flag is set, and if the define
      QQQ_EQS_CHECK_INDEX_BOUNDS has been enabled at compilation time, the
      internal error parameter is set and the data is marked invalid; else the
      indexing error is silently ignored.
   */
   void     setEliminateFlag(qqqIndex const row, bool eliminate);
   bool     getEliminateFlag(qqqIndex const row);
   qqqIndex getUnsortedIndex(qqqIndex const row);
   qqqIndex getSortedIndex  (qqqIndex const row);

   /* getPriority()

      gets the priority number for the equation/variable pair with index row.
      If the index is out of range, -1 is returned, and if the define
      QQQ_EQS_CHECK_INDEX_BOUNDS has been enabled at compilation time, the
      internal error parameter is set and the data is marked invalid; else the
      indexing error is silently ignored.
   */
   qqqIndex getPriority(qqqIndex const row) const;

   /* setPriority()
    
      sets the priority number for the equation/variable pair with index row.
      If the index is out of range, no value is set, and if the define
      QQQ_EQS_CHECK_INDEX_BOUNDS has been enabled at compilation time, the
      internal error parameter is set and the data is marked invalid; else the
      indexing error is silently ignored.
   */
   void setPriority(qqqIndex const row, qqqIndex const priority);

   /* Direct Access Methods
      =====================
 
      for arrays: x, bs, sc, and sr (real- and complex-valued)
      
      These methods return the address of the respective array to enable
      fast and direct access
   */
   double      *getXArray(qqqIndex const col = 0);
   qqqComplex  *getComplexXArray(qqqIndex const col = 0);
   double      *getBsArray(qqqIndex const col = 0);
   qqqComplex  *getComplexBsArray(qqqIndex const col = 0);
   double      *getScArray();
   qqqComplex  *getComplexScArray();
   double      *getSrArray();
   qqqComplex  *getComplexSrArray();

   bool         existAsEntry(qqqIndex const row, qqqIndex const col);


   /* First Newton Adjustment Step Methods */
   /* ==================================== */

   /* clearNewton()

      is called instead of allocateNewton() and clears all values in the
      Newton-related structures. So the structures and indices may remain and
      only the values are changed.
   */

   bool clearNewton(qqqSolverParameters &parms);

   /* MCSR access methods 
      for matrices As, Ab, Tb, Tv (real- and complex-valued)

      These methods add a value to the given position within the respective
      matrix and return true if this was successful.
   */
   bool	addMcsrAs(qqqIndex const row, qqqIndex const col, double value);
   bool	addMcsrComplexAs(qqqIndex const row, qqqIndex const col, qqqComplex value);
   bool	addMcsrAb(qqqIndex const row, qqqIndex const col, double value);
   bool	addMcsrComplexAb(qqqIndex const row, qqqIndex const col, qqqComplex value);
   bool	addMcsrTb(qqqIndex const row, qqqIndex const col, double value);
   bool	addMcsrComplexTb(qqqIndex const row, qqqIndex const col, qqqComplex value);
   bool	addMcsrTv(qqqIndex const row, qqqIndex const col, double value);
   bool	addMcsrComplexTv(qqqIndex const row, qqqIndex const col, qqqComplex value);

   /* MCSR row or col clearing methods
      for matrices Ab, As, and Tb

   */
   void mcsrAbClearRow(qqqIndex const row);
   void mcsrAsClearRow(qqqIndex const row);
   void mcsrTbClearCol(qqqIndex const col);

   /* Second Newton Adjustment Step Methods */
   /* ===================================== */

   /* Direct Access Methods for the 
      transformation matrix arrays
   */
   double      *getMcsrTbArray();
   qqqComplex  *getMcsrComplexTbArray();
   qqqIndex    *getMcsrTbIndexArray();

   /* Two Phases Models */
   /* ================= */

   /* prepareStepOneMatrices()

      is called after the first assembly step has been completed. It converts
      the linked-list structure to the final MCSC-structure needed by the
      direct row transformation algorithm.

      This function is also called by the solve method. In that case the flag
      "easLikeCompiling" is not set and the linked-list is converted to the
      final MCSR-structure needed by the conventional matrix multiplication
      step.

      If the parameter flag "fixTbProblem" is set, the
      transferred-transformation problem will be fixed (see TPP-documents).
   */
   bool prepareStepOneMatrices(qqqSolverParameters &parms);


   /* right hand side vector assembly methods 

      for double and qqqComplex values

   */
   bool addRHSLineEntry  (qqqIndex const row, double const value, qqqIndex const col = 0);
   bool addRHSEntry      (qqqIndex const row, double const value, qqqIndex const col = 0);
   bool addRHSDoubleEntry(qqqIndex const row1, qqqIndex const row2, 
			  double const value1, double const value2, qqqIndex const col = 0);
    
   bool addComplexRHSLineEntry  (qqqIndex const row, qqqComplex const value, qqqIndex const col = 0);
   bool addComplexRHSEntry      (qqqIndex const row, qqqComplex const value, qqqIndex const col = 0);
   bool addComplexRHSDoubleEntry(qqqIndex const row1, qqqIndex const row2, 
				 qqqComplex const value1, qqqComplex const value2, qqqIndex const col = 0);


   /* system matrix assembly methods 

      for double and qqqComplex values

   */
   
   bool addMatrixLineEntry(qqqIndex const row, qqqIndex const col, double const value, bool const adjustment);
   bool addMatrixEntry    (qqqIndex const row, qqqIndex const col, double const value, bool const adjustment);

   bool addMatrixDoubleEntry(qqqIndex const row1, qqqIndex const row2, qqqIndex const col,
			     double const value1, double const value2, bool const adjustment);

   bool addComplexMatrixLineEntry(qqqIndex const row, qqqIndex const col, qqqComplex const value, bool const adjustment);
   bool addComplexMatrixEntry    (qqqIndex const row, qqqIndex const col, qqqComplex const value, bool const adjustment);

   bool addComplexMatrixDoubleEntry(qqqIndex const row1, qqqIndex const row2, qqqIndex const col,
				    qqqComplex const value1, qqqComplex const value2, bool const adjustment);

   /* Input-Output auxiliary methods */
   /* ============================== */

   bool getReadMatrix       (qqqMCSR<double>     &matrix);
   bool getComplexReadMatrix(qqqMCSR<qqqComplex> &matrix);

   bool callNumericalServer(const char *numServer, const char *numServerHost, qqqSolverParameters &parms);
};

#endif // __numerics_eqs_hh__included__
