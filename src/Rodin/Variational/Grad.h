/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_GRADIENT_H
#define RODIN_VARIATIONAL_GRADIENT_H

#include "ForwardDecls.h"

#include "H1.h"
#include "Utility.h"
#include "Jacobian.h"
#include "GridFunction.h"
#include "TestFunction.h"
#include "TrialFunction.h"
#include "VectorFunction.h"

namespace Rodin::Variational
{
   /**
    * @defgroup GradSpecializations Grad Template Specializations
    * @brief Template specializations of the Grad class.
    * @see Grad
    */

   /**
    * @ingroup GradSpecializations
    */
   template <class Trait>
   class Grad<GridFunction<H1<Trait>>> : public VectorFunctionBase
   {
      void GetGradient(
            mfem::Vector& grad, mfem::ElementTransformation& trans,
            const mfem::IntegrationPoint& ip) const
      {
         mfem::Mesh* gf_mesh = m_u.getHandle().FESpace()->GetMesh();
         if (trans.mesh == gf_mesh)
         {
            m_u.getHandle().GetGradient(trans, grad);
         }
         else
         {
            mfem::IntegrationPoint coarse_ip;
            mfem::ElementTransformation *coarse_T = refinedToCoarse(*gf_mesh, trans, ip, coarse_ip);
            m_u.getHandle().GetGradient(*coarse_T, grad);
         }
      }

      public:
         /**
          * @brief Constructs the gradient of an @f$ H^1 @f$ function
          * @f$ u @f$.
          * @param[in] u Grid function to be differentiated
          */
         Grad(const GridFunction<H1<Trait>>& u)
            : m_u(u)
         {}

         Grad(const Grad& other)
            :  VectorFunctionBase(other),
               m_u(other.m_u)
         {}

         Grad(Grad&& other)
            :  VectorFunctionBase(std::move(other)),
               m_u(other.m_u)
         {}

         int getDimension() const override
         {
            return m_u.getFiniteElementSpace().getMesh().getSpaceDimension();
         }

         void getValue(
               mfem::Vector& value,
               mfem::ElementTransformation& trans, const mfem::IntegrationPoint& ip) const override
         {
            GetGradient(value,
                  FunctionBase::getTraceElementTrans(
                     FunctionBase::getSubMeshElementTrans(
                        m_u.getFiniteElementSpace().getMesh(), trans, ip), ip), ip);
         }

         VectorFunctionBase* copy() const noexcept override
         {
            return new Grad(*this);
         }

      private:
         const GridFunction<H1<Trait>>& m_u;
   };
   template <class Trait>
   Grad(const GridFunction<H1<Trait>>&) -> Grad<GridFunction<H1<Trait>>>;

   /**
    * @ingroup GradSpecializations
    */
   template <ShapeFunctionSpaceType Space, class Trait>
   class Grad<ShapeFunction<H1<Trait>, Space>> : public ShapeFunctionBase<Space>
   {
      public:
         constexpr
         Grad(ShapeFunction<H1<Trait>, Space>& u)
            : m_u(u)
         {
            if (u.getRangeType() != RangeType::Scalar)
               UnexpectedRangeTypeException(RangeType::Scalar, u.getRangeType()).raise();
         }

         constexpr
         Grad(const Grad& other)
            : ShapeFunctionBase<Space>(other),
              m_u(other.m_u)
         {}

         constexpr
         Grad(Grad&& other)
            : ShapeFunctionBase<Space>(std::move(other)),
              m_u(other.m_u)
         {}

         const ShapeFunction<H1<Trait>, Space>& getLeaf() const override
         {
            return m_u.getLeaf();
         }

         int getRows() const override
         {
            return m_u.getFiniteElementSpace().getMesh().getSpaceDimension();
         }

         int getColumns() const override
         {
            return 1;
         }

         int getDOFs(
               const mfem::FiniteElement& fe,
               const mfem::ElementTransformation& trans) const override
         {
            return m_u.getDOFs(fe, trans);
         }

         void getOperator(
               DenseBasisOperator& op,
               const mfem::FiniteElement& fe,
               mfem::ElementTransformation& trans,
               const mfem::IntegrationPoint& ip,
               ShapeComputator& compute) const override
         {
            const auto& dshape = compute.getPhysicalDShape(fe, trans, ip);
            const int n = dshape.NumRows();
            const int sdim = trans.GetSpaceDim();
            op.setSize(sdim, 1, n);
            for (int j = 0; j < n; j++)
               for (int k = 0; k < sdim; k++)
                  op(k, 0, j) = dshape(j, k);
         }

         H1<Trait>& getFiniteElementSpace() override
         {
            return m_u.getFiniteElementSpace();
         }

         const H1<Trait>& getFiniteElementSpace() const override
         {
            return m_u.getFiniteElementSpace();
         }

         Grad* copy() const noexcept override
         {
            return new Grad(*this);
         }
      private:
         ShapeFunction<H1<Trait>, Space>& m_u;
   };
   template <ShapeFunctionSpaceType Space, class Trait>
   Grad(ShapeFunction<H1<Trait>, Space>&) -> Grad<ShapeFunction<H1<Trait>, Space>>;
}

#endif
