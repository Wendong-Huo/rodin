/*
 *          Copyright Carlos BRITO PACHECO 2021 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *       (See accompanying file LICENSE or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef RODIN_VARIATIONAL_LEQ_H
#define RODIN_VARIATIONAL_LEQ_H

#include "ForwardDecls.h"
#include "Exceptions.h"
#include "GridFunction.h"
#include "BooleanFunction.h"

namespace Rodin::Variational
{
   /**
    * @defgroup LEQSpecializations LEQ Template Specializations
    * @brief Template specializations of the LEQ class.
    * @see LEQ
    */

   template <>
   class LEQ<FunctionBase, FunctionBase> : public BooleanFunctionBase
   {
      public:
         LEQ(const FunctionBase& lhs, const FunctionBase& rhs)
            : m_lhs(lhs.copy()), m_rhs(rhs.copy())
         {
            if (lhs.getRangeType() != RangeType::Scalar)
               UnexpectedRangeTypeException(RangeType::Scalar, lhs.getRangeType()).raise();
            if (rhs.getRangeType() != RangeType::Scalar)
               UnexpectedRangeTypeException(RangeType::Scalar, rhs.getRangeType()).raise();
         }

         LEQ(const LEQ& other)
            : BooleanFunctionBase(other),
              m_lhs(other.m_lhs->copy()),
              m_rhs(other.m_rhs->copy())
         {}

         LEQ(LEQ&& other)
            : BooleanFunctionBase(std::move(other)),
              m_lhs(std::move(other.m_lhs)),
              m_rhs(std::move(other.m_rhs))
         {}

         FunctionValue getValue(const Geometry::Point& p) const override
         {
            return m_lhs->getValue(p).scalar() <= m_rhs->getValue(p).scalar();
         }

         LEQ* copy() const noexcept override
         {
            return new LEQ(*this);
         }

      private:
         std::unique_ptr<FunctionBase> m_lhs;
         std::unique_ptr<FunctionBase> m_rhs;
   };
   LEQ(const FunctionBase&, const FunctionBase&) -> LEQ<FunctionBase, FunctionBase>;

   LEQ<FunctionBase, FunctionBase>
   operator<=(const FunctionBase&, const FunctionBase&);

   template <class T>
   std::enable_if_t<std::is_arithmetic_v<T>, LEQ<FunctionBase, FunctionBase>>
   operator<=(T lhs, const FunctionBase& rhs)
   {
      return LEQ(ScalarFunction(lhs), rhs);
   }

   template <class T>
   std::enable_if_t<std::is_arithmetic_v<T>, LEQ<FunctionBase, FunctionBase>>
   operator<=(const FunctionBase& lhs, T rhs)
   {
      return LEQ(lhs, ScalarFunction(rhs));
   }
}

#endif

