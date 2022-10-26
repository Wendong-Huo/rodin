/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_LINEARFORMINTEGRATOR_H
#define RODIN_VARIATIONAL_LINEARFORMINTEGRATOR_H

#include <set>
#include <memory>
#include <mfem.hpp>

#include "Rodin/FormLanguage/Base.h"

#include "ForwardDecls.h"
#include "Assembly.h"
#include "TestFunction.h"

namespace Rodin::Variational
{
   class LinearFormIntegratorBase : public FormLanguage::Base
   {
      public:
         LinearFormIntegratorBase(const ShapeFunctionBase<ShapeFunctionSpaceType::Test>& v)
            : m_v(v.copy())
         {}

         LinearFormIntegratorBase(const LinearFormIntegratorBase& other)
            : FormLanguage::Base(other),
              m_v(other.m_v->copy()),
              m_attrs(other.m_attrs)
         {}

         LinearFormIntegratorBase(LinearFormIntegratorBase&& other)
            : FormLanguage::Base(std::move(other)),
              m_v(std::move(other.m_v)),
              m_attrs(std::move(other.m_attrs))
         {}

         virtual ~LinearFormIntegratorBase() = default;

         const ShapeFunctionBase<ShapeFunctionSpaceType::Test>& getTestFunction() const
         {
            assert(m_v);
            return *m_v;
         }

         /**
          * @brief Gets the attributes of the elements being integrated.
          */
         const std::set<int>& getAttributes() const
         {
            return m_attrs;
         }

         /**
          * @brief Gets the integration region.
          */
         virtual IntegratorRegion getIntegratorRegion() const = 0;

         virtual void getElementVector(const Linear::Assembly::Common& as) const = 0;

         virtual void getElementVector(const Linear::Assembly::Device&) const
         {
            assert(false); // Unimplemented
         }

         virtual bool isSupported(Linear::Assembly::Type t) const
         {
            switch (t)
            {
               case Linear::Assembly::Type::Common:
                  return true;
               default:
                  return false;
            }
            return false;
         }

         std::unique_ptr<mfem::LinearFormIntegrator> build() const;

         virtual LinearFormIntegratorBase* copy() const noexcept override = 0;


         /**
          * @brief Specifies the material reference over which to integrate.
          * @returns Reference to self (for method chaining)
          *
          * Specifies the material reference over which the integration should
          * take place.
          */
         LinearFormIntegratorBase& over(int attr)
         {
            return over(std::set<int>{attr});
         }

         /**
          * @brief Specifies the material references over which to integrate.
          * @returns Reference to self (for method chaining)
          *
          * Specifies the material references over which the integration should
          * take place.
          */
         LinearFormIntegratorBase& over(const std::set<int>& attrs)
         {
            assert(attrs.size() > 0);
            m_attrs = attrs;
            return *this;
         }

      private:
         std::unique_ptr<ShapeFunctionBase<ShapeFunctionSpaceType::Test>> m_v;
         std::set<int> m_attrs;
   };
}

namespace Rodin::Variational::Internal
{
   class ProxyLinearFormIntegrator : public mfem::LinearFormIntegrator
   {
      public:
         ProxyLinearFormIntegrator(const LinearFormIntegratorBase& lfi)
            : m_lfi(lfi)
         {}

         ProxyLinearFormIntegrator(const ProxyLinearFormIntegrator& other)
            : mfem::LinearFormIntegrator(other),
              m_lfi(other.m_lfi)
         {}

         ProxyLinearFormIntegrator(ProxyLinearFormIntegrator&& other)
            : mfem::LinearFormIntegrator(std::move(other)),
              m_lfi(other.m_lfi)
         {}

         void AssembleRHSElementVect(
               const mfem::FiniteElement& fe,
               mfem::ElementTransformation& trans, mfem::Vector& vec) override
         {
            m_lfi.getElementVector(Linear::Assembly::Common{fe, trans, vec});
         }

         void AssembleDevice(const mfem::FiniteElementSpace& fes,
               const mfem::Array<int>& markers, mfem::Vector& b) override
         {
            m_lfi.getElementVector(Linear::Assembly::Device{fes, markers, b});
         }

         bool SupportsDevice() override
         {
            return m_lfi.isSupported(Linear::Assembly::Type::Device);
         }

      private:
         const LinearFormIntegratorBase& m_lfi;
   };
}

#endif
