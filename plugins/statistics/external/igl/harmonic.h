// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_HARMONIC_H
#define IGL_HARMONIC_H
#include "igl_inline.h"
#include <Eigen/Core>
#include <Eigen/Sparse>
namespace igl
{
  // Compute k-harmonic weight functions "coordinates".
  //
  //
  // Inputs:
  //   V  #V by dim vertex positions
  //   F  #F by simplex-size list of element indices
  //   b  #b boundary indices into V
  //   bc #b by #W list of boundary values
  //   k  power of harmonic operation (1: harmonic, 2: biharmonic, etc)
  // Outputs:
  //   W  #V by #W list of weights
  //
  template <
    typename DerivedV,
    typename DerivedF,
    typename Derivedb,
    typename Derivedbc,
    typename DerivedW>
  IGL_INLINE bool harmonic(
    const Eigen::PlainObjectBase<DerivedV> & V,
    const Eigen::PlainObjectBase<DerivedF> & F,
    const Eigen::PlainObjectBase<Derivedb> & b,
    const Eigen::PlainObjectBase<Derivedbc> & bc,
    const int k,
    Eigen::PlainObjectBase<DerivedW> & W);
  // Compute harmonic map using uniform laplacian operator
  //
  // Inputs:
  //   F  #F by simplex-size list of element indices
  //   b  #b boundary indices into V
  //   bc #b by #W list of boundary values
  //   k  power of harmonic operation (1: harmonic, 2: biharmonic, etc)
  // Outputs:
  //   W  #V by #W list of weights
  //
  template <
    typename DerivedF,
    typename Derivedb,
    typename Derivedbc,
    typename DerivedW>
  IGL_INLINE bool harmonic(
    const Eigen::PlainObjectBase<DerivedF> & F,
    const Eigen::PlainObjectBase<Derivedb> & b,
    const Eigen::PlainObjectBase<Derivedbc> & bc,
    const int k,
    Eigen::PlainObjectBase<DerivedW> & W);
  // Compute a harmonic map using a given Laplacian and mass matrix
  //
  // Inputs:
  //   L  #V by #V discrete (integrated) Laplacian  
  //   M  #V by #V mass matrix
  //   b  #b boundary indices into V
  //   bc  #b by #W list of boundary values
  //   k  power of harmonic operation (1: harmonic, 2: biharmonic, etc)
  // Outputs:
  //   W  #V by #V list of weights
  template <
    typename DerivedL,
    typename DerivedM,
    typename Derivedb,
    typename Derivedbc,
    typename DerivedW>
  IGL_INLINE bool harmonic(
    const Eigen::SparseMatrix<DerivedL> & L,
    const Eigen::SparseMatrix<DerivedM> & M,
    const Eigen::PlainObjectBase<Derivedb> & b,
    const Eigen::PlainObjectBase<Derivedbc> & bc,
    const int k,
    Eigen::PlainObjectBase<DerivedW> & W);
};

#ifndef IGL_STATIC_LIBRARY
#include "harmonic.cpp"
#endif
#endif
