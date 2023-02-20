#ifndef RODIN_VARIATIONAL_SHAPEFUNCTION_H
#define RODIN_VARIATIONAL_SHAPEFUNCTION_H

#include <mfem.hpp>

#include "Rodin/Alert/Exception.h"
#include "Rodin/FormLanguage/Base.h"

#include "ForwardDecls.h"

#include "H1.h"
#include "RangeShape.h"
#include "BasisOperator.h"
#include "FiniteElementSpace.h"

namespace Rodin::Variational
{

  /**
  * @defgroup ShapeFunctionSpecializations ShapeFunction Template Specializations
  * @brief Template specializations of the ShapeFunction class.
  * @see ShapeFunction
  */

  template <ShapeFunctionSpaceType Space>
  struct DualSpaceType;

  template <>
  struct DualSpaceType<TrialSpace>
  {
    static constexpr ShapeFunctionSpaceType Value = ShapeFunctionSpaceType::Test;
  };

  template <>
  struct DualSpaceType<TestSpace>
  {
    static constexpr ShapeFunctionSpaceType Value = ShapeFunctionSpaceType::Trial;
  };

  class ShapeComputator
  {
    public:
      using Key =
        std::tuple<
        const mfem::FiniteElement*,
              mfem::ElementTransformation*>;

    template <class Data>
    struct Value
    {
      const mfem::IntegrationPoint* pip;
      Data data;
    };

    ShapeComputator() = default;

    const mfem::Vector& getShape(
        const mfem::FiniteElement& el,
        mfem::ElementTransformation& trans,
        const mfem::IntegrationPoint& ip)
    {
      auto it = m_shapeLookup.find({&el, &trans});
      if (it != m_shapeLookup.end())
      {
        if (it->second.pip != &ip)
        {
          it->second.pip = &ip;
          it->second.data.SetSize(el.GetDof());
          el.CalcShape(ip, it->second.data);
        }
        return it->second.data;
      }
      else
      {
        auto itt =  m_shapeLookup.insert(
            it, {{&el, &trans}, {&ip, mfem::Vector(el.GetDof())}});
        el.CalcShape(ip, itt->second.data);
        return itt->second.data;
      }
    }

    const mfem::Vector& getPhysicalShape(
        const mfem::FiniteElement& el,
        mfem::ElementTransformation& trans,
        const mfem::IntegrationPoint& ip)
    {
      auto it = m_physShapeLookup.find({&el, &trans});
      if (it != m_physShapeLookup.end())
      {
        if (it->second.pip != &ip)
        {
          it->second.pip = &ip;
          it->second.data.SetSize(el.GetDof());
          el.CalcPhysShape(trans, it->second.data);
        }
        return it->second.data;
      }
      else
      {
        auto itt =  m_physShapeLookup.insert(
            it, {{&el, &trans}, {&ip, mfem::Vector(el.GetDof())}});
        el.CalcPhysShape(trans, itt->second.data);
        return itt->second.data;
      }
    }

    const mfem::DenseMatrix& getDShape(
        const mfem::FiniteElement& el,
        mfem::ElementTransformation& trans,
        const mfem::IntegrationPoint& ip)
    {
      auto it = m_dShapeLookup.find({&el, &trans});
      if (it != m_dShapeLookup.end())
      {
        it->second.pip = &ip;
        it->second.data.SetSize(el.GetDof(), el.GetDim());
        el.CalcDShape(ip, it->second.data);
        return it->second.data;
      }
      else
      {
        auto itt =  m_dShapeLookup.insert(
            it, {{&el, &trans}, {&ip, mfem::DenseMatrix(el.GetDof(), el.GetDim())}});
        el.CalcDShape(ip, itt->second.data);
        return itt->second.data;
      }
    }

    const mfem::DenseMatrix& getPhysicalDShape(
       const mfem::FiniteElement& el,
       mfem::ElementTransformation& trans,
       const mfem::IntegrationPoint& ip)
    {
      auto it = m_physDShapeLookup.find({&el, &trans});
      if (it != m_physDShapeLookup.end())
      {
        it->second.pip = &ip;
        it->second.data.SetSize(el.GetDof(), trans.GetSpaceDim());
        el.CalcPhysDShape(trans, it->second.data);
        return it->second.data;
      }
      else
      {
        auto itt =  m_physDShapeLookup.insert(
            it, {{&el, &trans}, {&ip, mfem::DenseMatrix(el.GetDof(), trans.GetSpaceDim())}});
        el.CalcPhysDShape(trans, itt->second.data);
        return itt->second.data;
      }
    }

   private:
    std::map<Key, Value<mfem::Vector>> m_shapeLookup;
    std::map<Key, Value<mfem::Vector>> m_physShapeLookup;

    std::map<Key, Value<mfem::DenseMatrix>> m_dShapeLookup;
    std::map<Key, Value<mfem::DenseMatrix>> m_physDShapeLookup;
  };

  template <class Derived, ShapeFunctionSpaceType Space>
  class ShapeFunctionBase : public FormLanguage::Base
  {
    public:
      using Parent = FormLanguage::Base;

      constexpr
      ShapeFunctionBase()
        : FormLanguage::Base()
      {}

      constexpr
      ShapeFunctionBase(const ShapeFunctionBase& other)
        : FormLanguage::Base(other)
      {}

      constexpr
      ShapeFunctionBase(ShapeFunctionBase&& other)
        : FormLanguage::Base(std::move(other))
      {}

      inline
      constexpr
      ShapeFunctionSpaceType getSpaceType() const
      {
        return Space;
      }

      inline
      constexpr
      RangeShape getRangeShape() const
      {
        return static_cast<const Derived&>(*this).getRangeShape();
      }

      inline
      constexpr
      RangeShape getRangeType() const
      {
        return static_cast<const Derived&>(*this).getRangeType();
      }

      inline
      constexpr
      auto T() const
      {
        return Transpose(*this);
      }

      inline
      constexpr
      const auto& getLeaf() const
      {
        return static_cast<const Derived&>(*this).getLeaf();
      }

      inline
      constexpr
      size_t getDOFs(const Geometry::Simplex& element) const
      {
        return static_cast<const Derived&>(*this).getDOFs(element);
      }

      inline
      constexpr
      auto getOperator(
          ShapeComputator& compute, const Geometry::Point& p) const
      {
        return static_cast<const Derived&>(*this).getOperator(compute, p);
      }

      template <class ... Args>
      inline
      constexpr
      auto operator()(Args&&... args) const
      {
        return getOperator(std::forward<Args>(args)...);
      }

      inline
      constexpr
      auto& getFiniteElementSpace()
      {
        return static_cast<Derived&>(*this).getFiniteElementSpace();
      }

      inline
      constexpr
      const auto& getFiniteElementSpace() const
      {
        return static_cast<const Derived&>(*this).getFiniteElementSpace();
      }

      inline ShapeFunctionBase* copy() const noexcept final override
      {
        return new Derived(static_cast<const Derived&>(*this));
      }
  };

  template <class Derived, class FESType, ShapeFunctionSpaceType Space>
  class FESShapeFunction
    : public ShapeFunctionBase<FESShapeFunction<Derived, FESType, Space>, Space>
  {
    public:
      using FES = FESType;
      using Parent = ShapeFunctionBase<FESShapeFunction<Derived, FESType, Space>, Space>;

      constexpr
      FESShapeFunction(FES& fes)
        : m_fes(fes)
      {}

      constexpr
      FESShapeFunction(const FESShapeFunction& other)
        : Parent(other),
          m_fes(other.m_fes),
          m_gf(other.m_gf)
      {}

      constexpr
      FESShapeFunction(FESShapeFunction&& other)
        : Parent(std::move(other)),
          m_fes(std::move(other.m_fes)),
          m_gf(std::move(other.m_gf))
      {}

      inline
      constexpr
      auto& emplace()
      {
        m_gf.emplace(this->getFiniteElementSpace());
        return *this;
      }

      inline
      constexpr
      GridFunction<FES>& getSolution()
      {
        assert(m_gf);
        return *m_gf;
      }

      inline
      constexpr
      const GridFunction<FES>& getSolution() const
      {
        assert(m_gf);
        return *m_gf;
      }

      inline
      constexpr
      FES& getFiniteElementSpace()
      {
        return m_fes.get();
      }

      inline
      constexpr
      const FES& getFiniteElementSpace() const
      {
        return m_fes.get();
      }

      inline
      constexpr
      const auto& getLeaf() const
      {
        return static_cast<const Derived&>(*this).getLeaf();
      }


      inline
      constexpr
      auto getOperator(
          ShapeComputator& compute, const Geometry::Point& p) const
      {
        return static_cast<const Derived&>(*this).getOperator(compute, p);
      }

    private:
      std::reference_wrapper<FES> m_fes;
      std::optional<GridFunction<FES>> m_gf;
  };

  /**
  * @ingroup ShapeFunctionSpecializations
  * @brief L2 ShapeFunction
  */
  template <class Derived, class ... Ts, ShapeFunctionSpaceType Space>
  class ShapeFunction<Derived, L2<Ts...>, Space>
    : public FESShapeFunction<ShapeFunction<Derived, L2<Ts...>, Space>, L2<Ts...>, Space>
  {
    public:
      using FES = L2<Ts...>;
      using Parent = FESShapeFunction<ShapeFunction<Derived, L2<Ts...>, Space>, L2<Ts...>, Space>;

      constexpr
      ShapeFunction(FES& fes)
        : Parent(fes)
      {}

      constexpr
      ShapeFunction(const ShapeFunction& other)
        : Parent(other)
      {}

      constexpr
      ShapeFunction(ShapeFunction&& other)
        : Parent(std::move(other))
      {}

      inline
      constexpr
      auto x() const
      {
        assert(getFiniteElementSpace().getVectorDimension() >= 1);
        return static_cast<const Derived&>(*this).x();
      }

      inline
      constexpr
      auto y() const
      {
        assert(getFiniteElementSpace().getVectorDimension() >= 2);
        return static_cast<const Derived&>(*this).y();
      }

      inline
      constexpr
      auto z() const
      {
        assert(getFiniteElementSpace().getVectorDimension() >= 3);
        return static_cast<const Derived&>(*this).z();
      }

      inline
      constexpr
      RangeShape getRangeShape()
      const
      {
        return { this->getFiniteElementSpace().getVectorDimension(), 1 };
      }

      inline
      constexpr
      size_t getDOFs(const Geometry::Simplex& element) const
      {
        const auto& fe = this->getFiniteElementSpace().getFiniteElement(element);
        return fe.GetDof() * this->getFiniteElementSpace().getVectorDimension();
      }

      inline
      constexpr
      const auto& getLeaf() const
      {
        return static_cast<const Derived&>(*this).getLeaf();
      }

      inline
      constexpr
      auto getOperator(
          ShapeComputator& compute, const Geometry::Point& p) const
      {
        return static_cast<const Derived&>(*this).getOperator(compute, p);
      }

      // void getOperator(
      //    DenseBasisOperator& op,
      //    ShapeComputator& compute,
      //    const Geometry::Point& point,
      //    const Geometry::Element& element) const override
      // {
      //   const auto& shape =
      //    compute.getPhysicalShape(
      //       getFiniteElementSpace().getFiniteElement(element),
      //       element.getTransformation(),
      //       element.getTransformation().GetIntPoint());
      //   const int n = shape.Size();
      //   const int vdim = getFiniteElementSpace().getVectorDimension();
      //   op.setSize(vdim, 1, vdim * n);
      //   op = 0.0;
      //   for (int i = 0; i < vdim; i++)
      //    for (int j = 0; j < n; j++)
      //     op(i, 0, j + i * n) = shape(j);
      // }
  };

  /**
  * @ingroup ShapeFunctionSpecializations
  * @brief H1 ShapeFunction
  */
  template <class Derived, class ... Ts, ShapeFunctionSpaceType Space>
  class ShapeFunction<Derived, H1<Ts...>, Space>
    : public FESShapeFunction<ShapeFunction<Derived, H1<Ts...>, Space>, H1<Ts...>, Space>
  {
    public:
      using FES = H1<Ts...>;
      using Parent = FESShapeFunction<ShapeFunction<Derived, H1<Ts...>, Space>, H1<Ts...>, Space>;

      constexpr
      ShapeFunction(FES& fes)
        : Parent(fes)
      {}

      constexpr
      ShapeFunction(const ShapeFunction& other)
        : Parent(other)
      {}

      constexpr
      ShapeFunction(ShapeFunction&& other)
        : Parent(std::move(other))
      {}

      inline
      constexpr
      auto x() const
      {
        assert(getFiniteElementSpace().getVectorDimension() >= 1);
        return static_cast<const Derived&>(*this).x();
      }

      inline
      constexpr
      auto y() const
      {
        assert(getFiniteElementSpace().getVectorDimension() >= 2);
        return static_cast<const Derived&>(*this).y();
      }

      inline
      constexpr
      auto z() const
      {
        assert(getFiniteElementSpace().getVectorDimension() >= 3);
        return static_cast<const Derived&>(*this).z();
      }

      inline
      constexpr
      RangeShape getRangeShape()
      const
      {
        return { this->getFiniteElementSpace().getVectorDimension(), 1 };
      }

      inline
      constexpr
      size_t getDOFs(const Geometry::Simplex& element) const
      {
        const auto& fe = this->getFiniteElementSpace().getFiniteElement(element);
        return fe.GetDof() * this->getFiniteElementSpace().getVectorDimension();
      }

      inline
      constexpr
      const auto& getLeaf() const
      {
        return static_cast<const Derived&>(*this).getLeaf();
      }

      inline
      constexpr
      auto getOperator(ShapeComputator& compute, const Geometry::Point& p) const
      {
        const auto& element = p.getSimplex();
        const auto& shape =
          compute.getPhysicalShape(
              this->getFiniteElementSpace().getFiniteElement(element),
              element.getTransformation(),
              element.getTransformation().GetIntPoint());
        const size_t n = shape.Size();
        const size_t vdim = this->getFiniteElementSpace().getVectorDimension();
        if constexpr (std::is_same_v<typename FES::Range, Scalar>)
        {
          assert(vdim == 1);
          return TensorBasis(n, [&](size_t i) { return shape(i); });
        }
        else if constexpr (std::is_same_v<typename FES::Range, Math::Vector>)
        {
          assert(false);
          return TensorBasis(vdim * n, [&](size_t) { return Math::Vector::Zero(vdim); });
        }
        else
        {
          assert(false);
          return void();
        }
      }
  };
}

#endif
