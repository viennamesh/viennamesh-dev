// Copyright 2006. Peter Gottschling, Matthias Troyer, Rolf Bonderer

#ifndef  CONCEPT_MACROS_INCLUDE
#define  CONCEPT_MACROS_INCLUDE

// By defining LA_SHOW_WARNINGS a warning is emitted
// when a compiler which does not support concepts is used.

#ifndef __GXX_CONCEPTS__
#  define _GLIBCXX_WHERE(...) //WHERE Macro for concept-disabled compilers.
#  ifdef LA_SHOW_WARNINGS
#    warning "Concepts are NOT used"
#  endif
#endif

#endif //  CONCEPT_MACROS_INCLUDE
