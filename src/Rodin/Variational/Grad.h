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
  template <class ... Ts>
  class Grad<GridFunction<H1<Ts...>>> final
    : public VectorFunctionBase<Grad<GridFunction<H1<Ts...>>>>
  {
    public:
      using Operand = GridFunction<H1<Ts...>>;
      using Parent = VectorFunctionBase<Grad<GridFunction<H1<Ts...>>>>;

      /**
       * @brief Constructs the gradient of an @f$ H^1 @f$ function
       * @f$ u @f$.
       * @param[in] u Grid function to be differentiated
       */
      constexpr
      Grad(const Operand& u)
        : m_u(u)
      {
        assert(u.getFiniteElementSpace().getVectorDimension() == 1);
      }

      constexpr
      Grad(const Grad& other)
        : Parent(other),
          m_u(other.m_u)
      {}

      constexpr
      Grad(Grad&& other)
        : Parent(std::move(other)),
          m_u(other.m_u)
      {}

      inline
      constexpr
      size_t getDimension() const
      {
        return m_u.getFiniteElementSpace().getMesh().getSpaceDimension();
      }

      auto getValue(const Geometry::Point& p) const
      {
        assert(false);
        // mfem::Vector grad;
        // const auto& simplex = p.getSimplex();
        // const auto& simplexMesh = simplex.getMesh();
        // const auto& fesMesh = m_u.getFiniteElementSpace().getMesh();
        // if (simplex.getDimension() == fesMesh.getDimension())
        // {
        //   assert(dynamic_cast<const Geometry::Element*>(&p.getSimplex()));
        //   const auto& element = p.getSimplex();
        //   auto& trans = element.getTransformation();
        //   m_u.getHandle().GetGradient(trans, grad);
        //   Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //   return res;
        // }
        // else if (simplex.getDimension() == fesMesh.getDimension() - 1)
        // {
        //   assert(dynamic_cast<const Geometry::Face*>(&p.getSimplex()));
        //   const auto& face = static_cast<const Geometry::Face&>(p.getSimplex());
        //   mfem::FaceElementTransformations* ft =
        //     const_cast<Geometry::MeshBase&>(simplexMesh).getHandle()
        //     .GetFaceElementTransformations(face.getIndex());
        //   if (simplexMesh.isSubMesh())
        //   {
        //     const auto& submesh = static_cast<const Geometry::SubMesh<Context::Serial>&>(simplexMesh);
        //     assert(submesh.getParent() == fesMesh);
        //     if (ft->Elem1 && this->getTraceDomain() == ft->Elem1->Attribute)
        //     {
        //       Geometry::Index parentIdx = submesh.getElementMap().left.at(ft->Elem1No);
        //       ft->Elem1->ElementNo = parentIdx;
        //       ft->Elem1No = parentIdx;
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem1, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else if (ft->Elem2 && this->getTraceDomain() == ft->Elem2->Attribute)
        //     {
        //       Geometry::Index parentIdx = submesh.getElementMap().left.at(ft->Elem2No);
        //       ft->Elem2->ElementNo = parentIdx;
        //       ft->Elem2No = parentIdx;
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem2, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else if (face.isBoundary())
        //     {
        //       assert(ft->Elem1);
        //       Geometry::Index parentIdx = submesh.getElementMap().left.at(ft->Elem1No);
        //       ft->Elem1->ElementNo = parentIdx;
        //       ft->Elem1No = parentIdx;
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem1, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else
        //     {
        //       assert(false);
        //       return Math::Vector(0);
        //     }
        //   }
        //   else if (fesMesh.isSubMesh())
        //   {
        //     const auto& submesh = static_cast<const Geometry::SubMesh<Context::Serial>&>(fesMesh);
        //     assert(submesh.getParent() == simplexMesh);
        //     const auto& s2pe = submesh.getElementMap();
        //     if (ft->Elem1 && s2pe.right.count(ft->Elem1No) && this->getTraceDomain() == ft->Elem1->Attribute)
        //     {
        //       Geometry::Index idx = s2pe.right.at(ft->Elem1No);
        //       ft->Elem1->ElementNo = idx;
        //       ft->Elem1No = idx;
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem1, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else if (ft->Elem2 && s2pe.right.count(ft->Elem2No) && this->getTraceDomain() == ft->Elem2->Attribute)
        //     {
        //       Geometry::Index idx = s2pe.right.at(ft->Elem2No);
        //       ft->Elem2->ElementNo = idx;
        //       ft->Elem2No = idx;
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem2, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else if (face.isBoundary())
        //     {
        //       assert(ft->Elem1);
        //       Geometry::Index idx = s2pe.right.at(ft->Elem1No);
        //       ft->Elem1->ElementNo = idx;
        //       ft->Elem1No = idx;
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem1, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else
        //     {
        //       assert(false);
        //       return Math::Vector(0);
        //     }
        //   }
        //   else
        //   {
        //     if (ft->Elem1 && this->getTraceDomain() == ft->Elem1->Attribute)
        //     {
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem1, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else if (ft->Elem2 && this->getTraceDomain() == ft->Elem2->Attribute)
        //     {
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       m_u.getHandle().GetGradient(*ft->Elem2, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else if (face.isBoundary())
        //     {
        //       ft->SetAllIntPoints(&p.getIntegrationPoint());
        //       assert(ft->Elem1);
        //       m_u.getHandle().GetGradient(*ft->Elem1, grad);
        //       Math::Vector res = Eigen::Map<Math::Vector>(grad.GetData(), grad.Size());
        //       return res;
        //     }
        //     else
        //     {
        //       assert(false);
        //       return Math::Vector(0);
        //     }
        //   }
        // }
        // else
        // {
        //   assert(false);
        //   return Math::Vector(0);
        // }
      }

    private:
      std::reference_wrapper<const Operand> m_u;
  };

  template <class ... Ts>
  Grad(const GridFunction<H1<Ts...>>&) -> Grad<GridFunction<H1<Ts...>>>;

  /**
   * @ingroup GradSpecializations
   */
  template <class NestedDerived, ShapeFunctionSpaceType Space, class ... Ts>
  class Grad<ShapeFunction<NestedDerived, H1<Ts...>, Space>> final
    : public ShapeFunctionBase<Grad<ShapeFunction<NestedDerived, H1<Ts...>, Space>>, Space>
  {
    public:
      using FES = H1<Ts...>;
      using Operand = ShapeFunction<NestedDerived, H1<Ts...>, Space>;
      using Parent = ShapeFunctionBase<Grad<Operand>, Space>;

      constexpr
      Grad(Operand& u)
        : m_u(u)
      {}

      constexpr
      Grad(const Grad& other)
        : Parent(other),
          m_u(other.m_u)
      {}

      constexpr
      Grad(Grad&& other)
        : Parent(std::move(other)),
          m_u(std::move(other.m_u))
      {}

      inline
      constexpr
      const auto& getLeaf() const
      {
        return m_u.get().getLeaf();
      }

      inline
      constexpr
      RangeShape getRangeShape() const
      {
        return { m_u.get().getFiniteElementSpace().getMesh().getSpaceDimension(), 1 };
      }

      inline
      constexpr
      size_t getDOFs(const Geometry::Simplex& element) const
      {
        return m_u.get().getDOFs(element);
      }

      auto getTensorBasis(const FiniteElement& fe, const Geometry::Point& p) const
      {
        assert(false);
        // auto& trans = p.getSimplex().getTransformation();
        // const auto& fe = getFiniteElementSpace().getFiniteElement(p.getSimplex());
        // const auto& dshape = compute.getPhysicalDShape(fe, trans, trans.GetIntPoint());
        // const size_t n = dshape.NumRows();
        // const size_t sdim = trans.GetSpaceDim();
        // return TensorBasis(n,
        //     [sdim](size_t) -> Math::Vector { return Math::Vector::Zero(sdim); });

        // TensorBasis res(static_cast<int>(n), static_cast<int>(sdim), 1);
        // for (size_t j = 0; j < n; j++)
        // {
        //   for (size_t k = 0; k < sdim; k++)
        //   {
        //     res(static_cast<int>(j), static_cast<int>(k), 0) = dshape(j, k);
        //   }
        // }
      }

      inline
      constexpr
      auto& getFiniteElementSpace()
      {
        return m_u.getFiniteElementSpace();
      }

      inline
      constexpr
      const auto& getFiniteElementSpace() const
      {
        return m_u.get().getFiniteElementSpace();
      }

      inline
      Grad* copy() const noexcept final override
      {
        return new Grad(*this);
      }

    private:
      std::reference_wrapper<Operand> m_u;
  };

  template <class NestedDerived, ShapeFunctionSpaceType Space, class ... Ts>
  Grad(ShapeFunction<NestedDerived, H1<Ts...>, Space>&)
    -> Grad<ShapeFunction<NestedDerived, H1<Ts...>, Space>>;

  // /**
  //  * @ingroup GradSpecializations
  //  *
  //  * @brief Represents the broken gradient.
  //  */
  // template <ShapeFunctionSpaceType Space, class ... Ts>
  // class Grad<ShapeFunction<L2<Ts...>, Space>> : public ShapeFunctionBase<Space>
  // {
  //   public:
  //     using FES = L2<Ts...>;

  //     constexpr
  //     Grad(ShapeFunction<FES, Space>& u)
  //       : m_u(u)
  //     {
  //       if (u.getRangeType() != RangeType::Scalar)
  //         UnexpectedRangeTypeException(RangeType::Scalar, u.getRangeType()).raise();
  //     }

  //     constexpr
  //     Grad(const Grad& other)
  //       : ShapeFunctionBase<Space>(other),
  //         m_u(other.m_u)
  //     {}

  //     constexpr
  //     Grad(Grad&& other)
  //       : ShapeFunctionBase<Space>(std::move(other)),
  //         m_u(other.m_u)
  //     {}

  //     const ShapeFunction<FES, Space>& getLeaf() const override
  //     {
  //       return m_u.getLeaf();
  //     }

  //     int getRows() const override
  //     {
  //       return m_u.getFiniteElementSpace().getMesh().getSpaceDimension();
  //     }

  //     int getColumns() const override
  //     {
  //       return 1;
  //     }

  //     int getDOFs(const Geometry::Simplex& element) const override
  //     {
  //       return m_u.getDOFs(element);
  //     }

  //     TensorBasis getOperator(
  //         ShapeComputator& compute, const Geometry::Simplex& element) const override
  //     {
  //       auto& trans = element.getTransformation();
  //       const auto& fe = getFiniteElementSpace().getFiniteElement(element);
  //       const auto& dshape = compute.getPhysicalDShape(fe, trans, trans.GetIntPoint());
  //       const size_t n = dshape.NumRows();
  //       const size_t sdim = trans.GetSpaceDim();
  //       TensorBasis res(n, sdim, 1);
  //       for (size_t j = 0; j < n; j++)
  //       {
  //         for (size_t k = 0; k < sdim; k++)
  //         {
  //           res(j, k, 0) = dshape(j, k);
  //         }
  //       }
  //       return res;
  //     }

  //     FES& getFiniteElementSpace() override
  //     {
  //       return m_u.getFiniteElementSpace();
  //     }

  //     const FES& getFiniteElementSpace() const override
  //     {
  //       return m_u.getFiniteElementSpace();
  //     }

  //     Grad* copy() const noexcept override
  //     {
  //       return new Grad(*this);
  //     }
  //   private:
  //     ShapeFunction<FES, Space>& m_u;
  // };
  // template <ShapeFunctionSpaceType Space, class ... Ts>
  // Grad(ShapeFunction<L2<Ts...>, Space>&) -> Grad<ShapeFunction<L2<Ts...>, Space>>;
}

#endif
