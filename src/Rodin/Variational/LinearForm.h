/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_LINEARFORM_H
#define RODIN_VARIATIONAL_LINEARFORM_H

#include <mfem.hpp>

#include "Rodin/FormLanguage/List.h"

#include "ForwardDecls.h"
#include "TestFunction.h"
#include "LinearFormIntegrator.h"

namespace Rodin::Variational
{
   template <class VectorType>
   class LinearFormBase : public FormLanguage::Base
   {
      public:
         constexpr
         LinearFormBase() = default;

         constexpr
         LinearFormBase(const LinearFormBase& other)
            :  FormLanguage::Base(other),
               m_lfis(other.m_lfis)
         {}

         constexpr
         LinearFormBase(LinearFormBase&& other)
            :  FormLanguage::Base(std::move(other)),
               m_lfis(std::move(other.m_lfis))
         {}

         constexpr
         const FormLanguage::List<LinearFormIntegratorBase>& getIntegrators() const
         {
            return m_lfis;
         }

         /**
          * @brief Assembles the linear form.
          *
          * This method will assemble the underlying vector associated
          * the linear form.
          *
          * @see getVector()
          */
         virtual void assemble() = 0;

         /**
          * @brief Updates the state after a refinement in the mesh.
          *
          * This method will update the linear form after a call to the
          * @ref MeshBase::refine() "refine()" method.
          */
         virtual LinearFormBase& update() = 0;

         /**
          * @brief Gets the reference to the (local) associated vector
          * to the LinearForm.
          */
         virtual VectorType& getVector() = 0;

         /**
          * @brief Gets the reference to the (local) associated vector
          * to the LinearForm.
          */
         virtual const VectorType& getVector() const = 0;

         /**
          * @brief Builds the linear form the given LinearFormIntegratorBase
          * instance
          * @param[in] lfi Integrator which will be used to build the linear form.
          * @returns Reference to this (for method chaining)
          */
         virtual LinearFormBase& from(const LinearFormIntegratorBase& lfi)
         {
            m_lfis.clear();
            add(lfi).assemble();
            return *this;
         }

         virtual LinearFormBase& from(
               const FormLanguage::List<LinearFormIntegratorBase>& lfi)
         {
            m_lfis.clear();
            add(lfi).assemble();
            return *this;
         }

         /**
          * @brief Builds the linear form the given LinearFormIntegratorBase
          * instance
          * @param[in] lfi Integrator which will be used to build the linear form.
          * @returns Reference to this (for method chaining)
          */
         virtual LinearFormBase& add(const LinearFormIntegratorBase& lfi)
         {
            m_lfis.add(lfi);
            return *this;
         }

         virtual LinearFormBase& add(
               const FormLanguage::List<LinearFormIntegratorBase>& lfis)
         {
            m_lfis.add(lfis);
            return *this;
         }

         virtual LinearFormBase& operator=(const LinearFormIntegratorBase& lfi)
         {
            from(lfi).assemble();
            return *this;
         }

         virtual LinearFormBase& operator=(
               const FormLanguage::List<LinearFormIntegratorBase>& lfis)
         {
            from(lfis).assemble();
            return *this;
         }

         /**
          * @brief Gets the test function argument associated to this bilinear
          * form.
          */
         virtual const ShapeFunctionBase<ShapeFunctionSpaceType::Test>& getTestFunction() const = 0;

         virtual LinearFormBase* copy() const noexcept override = 0;

      private:
         FormLanguage::List<LinearFormIntegratorBase> m_lfis;
   };

   /**
    * @brief Represents a linear form defined over some finite element space
    *
    * An object of type LinearForm represents a linear map
    * @f[
    * \begin{aligned}
    *    L : V &\rightarrow \mathbb{R}\\
    *        v &\mapsto L(v)
    * \end{aligned}
    * @f]
    * where @f$ V @f$ is a finite element space.
    *
    * A linear form can be specified by from one or more
    * LinearFormIntegratorBase instances.
    */
   template <class FES>
   class LinearForm<FES, Context::Serial, mfem::Vector> : public LinearFormBase<mfem::Vector>
   {
      static_assert(std::is_same_v<typename FES::Context, Context::Serial>);

      public:
         using Context = typename FES::Context;
         using VectorType = mfem::Vector;

         /**
          * @brief Constructs a linear form defined on some finite element
          * space
          * @param[in] fes Reference to the finite element space
          */
         constexpr
         LinearForm(TestFunction<FES>& v);

         constexpr
         LinearForm(const LinearForm& other)
            :  LinearFormBase(other),
               m_v(other.m_v)
         {}

         constexpr
         LinearForm(LinearForm&& other)
            :  LinearFormBase(std::move(other)),
               m_v(other.m_v)
         {}

         /**
          * @brief Evaluates the linear form at the function @f$ u @f$.
          *
          * Given a grid function @f$ u @f$, this function will compute the
          * action of the linear mapping @f$ L(u) @f$.
          *
          * @returns The value which the linear form takes at @f$ u @f$.
          */
         constexpr
         double operator()(const GridFunction<FES>& u) const;

         void assemble() override;

         LinearForm& update() override;

         /**
          * @brief Gets the reference to the (local) associated vector
          * to the LinearForm.
          */
         VectorType& getVector() override
         {
            return static_cast<mfem::Vector&>(*m_lf);
         }

         /**
          * @brief Gets the reference to the (local) associated vector
          * to the LinearForm.
          */
         const VectorType& getVector() const override
         {
            return static_cast<const mfem::Vector&>(*m_lf);
         }

         const TestFunction<FES>& getTestFunction() const override
         {
            return m_v;
         }

         LinearForm& operator=(
            const LinearFormIntegratorBase& lfi) override
         {
            from(lfi).assemble();
            return *this;
         }

         LinearForm& operator=(
            const FormLanguage::List<LinearFormIntegratorBase>& lfis) override
         {
            from(lfis).assemble();
            return *this;
         }

         LinearForm* copy() const noexcept override
         {
            return new LinearForm(*this);
         }

      private:
         TestFunction<FES>& m_v;

         std::unique_ptr<mfem::LinearForm> m_lf;
   };
   template <class FES>
   LinearForm(TestFunction<FES>&) -> LinearForm<FES, typename FES::Context, mfem::Vector>;
}

#include "LinearForm.hpp"

#endif
