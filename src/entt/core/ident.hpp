#ifndef ENTT_CORE_IDENT_HPP
#define ENTT_CORE_IDENT_HPP


#include <type_traits>
#include <cstddef>
#include <utility>
#include <tuple>
#include "../config/config.h"


namespace entt {


/**
 * @brief Types identifiers.
 *
 * Variable template used to generate identifiers at compile-time for the given
 * types. Use the `get` member function to know what's the identifier associated
 * to the specific type.
 *
 * @note
 * Identifiers are constant expression and can be used in any context where such
 * an expression is required. As an example:
 * @code{.cpp}
 * using ID = entt::Identifier<AType, AnotherType>;
 *
 * switch(aTypeIdentifier) {
 * case ID::get<AType>():
 *     // ...
 *     break;
 * case ID::get<AnotherType>():
 *     // ...
 *     break;
 * default:
 *     // ...
 * }
 * @endcode
 *
 * @tparam Types List of types for which to generate identifiers.
 */
template<typename... Types>
class Identifier final {
    using tuple_type = std::tuple<std::decay_t<Types>...>;

    template<typename Type, std::size_t... Indexes>
    static constexpr std::size_t get(std::index_sequence<Indexes...>) ENTT_NOEXCEPT {
        static_assert(std::disjunction_v<std::is_same<Type, Types>...>);
        return (0 + ... + (std::is_same_v<Type, std::tuple_element_t<Indexes, tuple_type>> ? Indexes : std::size_t{}));
    }

public:
    /*! @brief Unsigned integer type. */
    using identifier_type = std::size_t;

    /**
     * @brief Returns the identifier associated with a given type.
     * @tparam Type of which to return the identifier.
     * @return The identifier associated with the given type.
     */
    template<typename Type>
    static constexpr identifier_type get() ENTT_NOEXCEPT {
        return get<std::decay_t<Type>>(std::make_index_sequence<sizeof...(Types)>{});
    }
};


}


#endif // ENTT_CORE_IDENT_HPP
