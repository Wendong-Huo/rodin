#ifndef RODIN_FORMLANGUAGE_TRAITS_H
#define RODIN_FORMLANGUAGE_TRAITS_H

#include <type_traits>

namespace Rodin::FormLanguage
{
  template <class ... Args>
  struct Traits;

  template <typename ...>
  struct IsOneOf
  {
    static constexpr bool Value = false;
  };

  template <typename F, typename S, typename ... T>
  struct IsOneOf<F, S, T...> {
    static constexpr bool Value =
      std::is_same<F, S>::Value || IsOneOf<F, T...>::Value;
  };
}

#endif
