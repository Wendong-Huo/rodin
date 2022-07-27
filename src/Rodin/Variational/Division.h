/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_DIVISION_H
#define RODIN_VARIATIONAL_DIVISION_H

#include "ForwardDecls.h"
#include "Function.h"

namespace Rodin::Variational
{
   /**
    * @brief Division of VectorFunctionBase by ScalarFunctionBase.
    */
   template <>
   class Division<FunctionBase, FunctionBase>
      : public FunctionBase
   {
      public:
         Division(const FunctionBase& lhs, const FunctionBase& rhs);

         Division(const Division& other);

         Division(Division&& other);

         RangeShape getRangeShape() const override;

         Division& traceOf(const std::set<int>& attrs) override;

         void getValue(
            mfem::DenseMatrix& value,
            mfem::ElementTransformation& trans,
            const mfem::IntegrationPoint& ip) const override;

         Division* copy() const noexcept override
         {
            return new Division(*this);
         }

      private:
         std::unique_ptr<FunctionBase> m_lhs;
         std::unique_ptr<FunctionBase> m_rhs;
   };
   Division(const FunctionBase&, const FunctionBase&)
      -> Division<FunctionBase, FunctionBase>;

   Division<FunctionBase, FunctionBase>
   operator/(const FunctionBase& lhs, const FunctionBase& rhs);

   template <class T>
   std::enable_if_t<std::is_arithmetic_v<T>,
      Division<FunctionBase, FunctionBase>>
   operator/(const FunctionBase& lhs, T rhs)
   {
      return Division(lhs, ScalarFunction(rhs));
   }
}
#endif
