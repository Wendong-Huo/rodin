/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_TRANSPOSE_H
#define RODIN_VARIATIONAL_TRANSPOSE_H

#include "ShapeFunction.h"
#include "MatrixFunction.h"

namespace Rodin::Variational
{
   /**
    * @brief Represents the transpose matrix @f$ A^T @f$ of some matrix @f$ A @f$.
    *
    * For some @f$ n \times m @f$ matrix @f$ A @f$, the transpose matrix @f$
    * A^T @f$ is an @f$ m \times n @f$ matrix defined by
    * @f[
    *    {A^T}_{ij} = A_{ji}
    * @f]
    */
   template <>
   class Transpose<FunctionBase> : public FunctionBase
   {
      public:
         /**
          * @brief Constructs the Transpose matrix of the given matrix.
          */
         Transpose(const FunctionBase& m)
            : m_matrix(m.copy())
         {}

         Transpose(const Transpose& other)
            :  FunctionBase(other),
               m_matrix(other.m_matrix->copy())
         {}

         Transpose(Transpose&& other)
            : FunctionBase(std::move(other)),
              m_matrix(std::move(other.m_matrix))
         {}

         RangeShape getRangeShape() const override
         {
            return m_matrix->getRangeShape().transpose();
         }

         void getValue(
               mfem::DenseMatrix& value,
               mfem::ElementTransformation& trans,
               const mfem::IntegrationPoint& ip) const override
         {
            m_matrix->getValue(value, trans, ip);
            value.Transpose();
         }

         Transpose* copy() const noexcept override
         {
            return new Transpose(*this);
         }

      private:
         std::unique_ptr<FunctionBase> m_matrix;
   };
   Transpose(const FunctionBase&) -> Transpose<MatrixFunctionBase>;

   template <ShapeFunctionSpaceType Space>
   class Transpose<ShapeFunctionBase<Space>> : public ShapeFunctionBase<Space>
   {
      public:
         Transpose(const ShapeFunctionBase<Space>& op)
            : m_op(op.copy())
         {}

         Transpose(const Transpose& other)
            : m_op(other.m_op->copy())
         {}

         Transpose(Transpose&& other)
            : m_op(std::move(other))
         {}

         const ShapeFunctionBase<Space>& getLeaf() const override
         {
            return m_op->getLeaf();
         }

         int getRows() const override
         {
            return m_op->getColumns();
         }

         int getColumns() const override
         {
            return m_op->getRows();
         }

         int getDOFs(
               const mfem::FiniteElement& fe,
               const mfem::ElementTransformation& trans) const override
         {
            return m_op->getDOFs(fe, trans);
         }

         std::unique_ptr<BasisOperator> getOperator(
               const mfem::FiniteElement& fe,
               mfem::ElementTransformation& trans) const override
         {
            return m_op->getOperator(fe, trans)->Transpose();
         }

         FiniteElementSpaceBase& getFiniteElementSpace() override
         {
            return m_op->getFiniteElementSpace();
         }

         const FiniteElementSpaceBase& getFiniteElementSpace() const override
         {
            return m_op->getFiniteElementSpace();
         }

         Transpose* copy() const noexcept override
         {
            return new Transpose(*this);
         }

      private:
         std::unique_ptr<ShapeFunctionBase<Space>> m_op;
   };
}

#endif
