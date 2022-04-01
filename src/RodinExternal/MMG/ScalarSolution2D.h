/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_EXTERNAL_MMG_SCALARSOLUTION2D_H
#define RODIN_EXTERNAL_MMG_SCALARSOLUTION2D_H

#include <cstddef>
#include <iterator>
#include <functional>

#include "Rodin/Alert.h"

#include "ForwardDecls.h"
#include "Mesh2D.h"

#include "ScalarSolution.h"

namespace Rodin::External::MMG
{
   /**
    * @brief Scalar solution supported on a 2D mesh.
    *
    * An object of type ScalarSolution2D represents a function
    * @f[
    * f : \Omega \subset \mathbb{R}^2 \rightarrow \mathbb{R}
    * @f]
    * whose known values are given on vertices of some mesh @f$ \Omega @f$.
    */
   class ScalarSolution2D :  public ScalarSolution
   {
      public:
         /**
          * @brief Reads the solution text file.
          *
          * The file is read using MMGv2 format.
          *
          * @param[in] filename Name of file to read.
          */
         static IncompleteScalarSolution2D load(const boost::filesystem::path& filename);

         /**
          * @brief Initializes the object with no data
          *
          * @param[in] mesh Reference to the underlying mesh.
          */
         ScalarSolution2D(Mesh2D& mesh);

         /**
          * @brief Performs a move construction from the `other` solution object.
          *
          * @param[in] other Object to move.
          */
         ScalarSolution2D(ScalarSolution2D&& other);

         /**
          * @brief Performs a copy of the `other` solution object.
          *
          * @param[in] other Object to copy.
          * @note It does not perform a copy the mesh. Instead the new object
          * will have a reference to the original mesh.
          */
         ScalarSolution2D(const ScalarSolution2D& other);

         /**
          * @brief Frees the data.
          */
         virtual ~ScalarSolution2D();

         /**
          * @brief Move assigns the `other` solution object to this object.
          *
          * @param[in] other Object to move.
          */
         ScalarSolution2D& operator=(ScalarSolution2D&& other);

         /**
          * @brief Copy assigns the `other` solution object to this object.
          *
          * @param[in] other Object to copy.
          */
         ScalarSolution2D& operator=(const ScalarSolution2D& other);

         /**
          * @brief Sets the associated mesh.
          *
          * @param[in] mesh Reference to mesh.
          *
          * @returns Reference to self (for method chaining).
          *
          * @note The method does not check to see if the mesh is compatible
          * with the current data in the solution. In general, it is up to the
          * user to ensure that the number of points are the same, keep track
          * of the modifications to the underlying mesh, etc.
          */
         ScalarSolution2D& setMesh(Mesh2D& mesh);

         /**
          * @brief Gets the constant reference to the underlying mesh.
          *
          * @returns Constant reference to the underlying mesh.
          */
         const Mesh2D& getMesh() const;

         /**
          * @brief Gets the reference to the underlying mesh.
          *
          * @returns Reference to the underlying mesh.
          */
         Mesh2D& getMesh();

         MMG5_pSol& getHandle() override;

         const MMG5_pSol& getHandle() const override;

         void save(const boost::filesystem::path& filename) override;

      private:
         std::reference_wrapper<Mesh2D> m_mesh;
         MMG5_pSol m_sol;
   };

   /**
    * @brief A scalar solution which does not have a mesh assigned to it.
    *
    * To unlock the full functionality of the class you must call the
    * @ref setMesh(Mesh2D&) method. For example, when loading it from file:
    *
    * @code{.cpp}
    * auto sol = ScalarSolution2D::load(filename).setMesh(mesh);
    * @endcode
    */
   class IncompleteScalarSolution2D
   {
      public:
         /**
          * @brief Constructs an empty scalar solution object without a mesh.
          */
         IncompleteScalarSolution2D();

         /**
          * @brief Constructs a scalar solution with `n` unitialized entries.
          * @param[in] n Number of entries that the solution has.
          */
         IncompleteScalarSolution2D(int n);

         /**
          * @brief Frees the data if it still owns the data, i.e. the
          * setMesh(Mesh2D&) method has not been called.
          */
         virtual ~IncompleteScalarSolution2D();

         /**
          * @brief Sets the associated mesh and moves ownership to the new
          * object.
          *
          * @param[in] mesh Reference to mesh.
          *
          * @returns An object of type ScalarSolution2D which represents the
          * object with all its functionality.
          *
          * @note The method does not incur any significant performance penalty
          * since no data is copied.
          *
          * @warning The method does not check to see if the mesh is compatible
          * with the current data in the solution. In general, it is up to the
          * user to ensure that the number of points are the same and keep
          * track of the modifications to the underlying mesh.
          */
         ScalarSolution2D setMesh(Mesh2D& mesh);

         MMG5_pSol& getHandle();

         const MMG5_pSol& getHandle() const;

      private:
         MMG5_pSol m_sol;
         bool m_isOwner;
   };
}
#endif
