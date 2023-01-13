/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_MATRIXFUNCTION_H
#define RODIN_VARIATIONAL_MATRIXFUNCTION_H

#include <set>
#include <optional>
#include <mfem.hpp>

#include "Rodin/Alert.h"

#include "ForwardDecls.h"
#include "Function.h"
#include "RangeShape.h"

namespace Rodin::Variational
{
   /**
    * @defgroup MatrixFunctionSpecializations MatrixFunction Template Specializations
    * @brief Template specializations of the MatrixFunction class.
    * @see MatrixFunction
    */

   class MatrixFunctionBase : public FunctionBase
   {
      public:
         MatrixFunctionBase() = default;

         MatrixFunctionBase(const MatrixFunctionBase& other)
            : FunctionBase(other)
         {}

         MatrixFunctionBase(MatrixFunctionBase&& other)
            : FunctionBase(std::move(other))
         {}

         virtual ~MatrixFunctionBase() = default;

         FunctionValue::Matrix operator()(const Geometry::Point& p) const
         {
            return getValue(p).matrix();
         }

         RangeShape getRangeShape() const override
         {
            return {getRows(), getColumns()};
         }

         RangeType getRangeType() const override
         {
            return RangeType::Matrix;
         }

         /**
          * @brief Gets the number of rows in the matrix
          * @returns Number of rows
          */
         virtual int getRows() const = 0;

         /**
          * @brief Gets the number of columns in the matrix
          * @returns Number of columns
          */
         virtual int getColumns() const = 0;

         virtual MatrixFunctionBase* copy() const noexcept override = 0;
   };
}

#endif
