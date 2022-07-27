/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_COMPOSITION_H
#define RODIN_VARIATIONAL_COMPOSITION_H

#include <functional>

#include "ForwardDecls.h"
#include "ScalarFunction.h"

namespace Rodin::Variational
{

   /**
    * @brief Composition of a scalar valued function on the real line and a
    * scalar function on the mesh.
    *
    * Represents the composition of two functions @f$ f : \mathbb{R}
    * \rightarrow \mathbb{R} @f$ and @f$ g : \Omega \rightarrow \mathbb{R} @f$:
    * @f[
    *    (f \circ g)(x) = f(g(x))
    * @f]
    */
   template <>
   class Composition<std::function<double(double)>, FunctionBase>
      : public ScalarFunctionBase
   {
      public:
         Composition(std::function<double(double)> f, const FunctionBase& g);

         Composition(const Composition& other);

         Composition(Composition&& other);

         double getValue(
               mfem::ElementTransformation& trans,
               const mfem::IntegrationPoint& ip) const override;

         Composition* copy() const noexcept override
         {
            return new Composition(*this);
         }

      private:
         std::function<double(double)> m_f;
         std::unique_ptr<FunctionBase> m_g;
   };
   template <class Lhs>
   Composition(const Lhs&, const FunctionBase&)
      -> Composition<std::enable_if_t<
         std::is_invocable_r_v<double, Lhs, double>, std::function<double(double)>>, FunctionBase>;

   /**
    * @brief Composes two functions
    * @param f
    * @param g
    *
    * Represents the composition of two functions @f$ f @f$ and @f$ g @f$:
    * @f[
    *    (f \circ g)(x) = f(g(x))
    * @f]
    *
    */
   template <class Lhs, class Rhs>
   auto compose(Lhs&& f, Rhs&& g)
   {
      return Composition(std::forward<Lhs>(f), std::forward<Rhs>(g));
   };
}

#endif
