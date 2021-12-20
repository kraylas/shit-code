#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>
#include <concepts>
namespace KUtils {
template <typename K, typename C = std::less<K>>
concept Ops = requires {
	C{}(std::declval<K>(), std::declval<K>());
};
template <typename T, typename U>
inline constexpr bool is_rmcvref_same_v = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
namespace details {
template <std::uint8_t N, typename... Ts>
struct TypeSwitcher;

template <std::uint8_t N, typename T>
struct TypeSwitcher<N, T> {
	static_assert(N == 0, "N out of range");
	using type = T;
};

template <typename T, typename T2, typename... Ts>
struct TypeSwitcher<0, T, T2, Ts...> {
	using type = T;
};

template <std::uint8_t N, typename T, typename... Ts>
struct TypeSwitcher<N, T, Ts...> {
	using type = typename TypeSwitcher<N - 1, Ts...>::type;
};

} // namespace details
template <std::uint8_t N, typename... Ts>
using type_switch_t = typename details::TypeSwitcher<N, Ts...>::type;

// Remember destruct!!!
template <typename T>
struct StackRawStorage {
	std::byte pool[sizeof(T)];
	// Construct T on pool storage
	template <typename... Ts>
	requires std::is_constructible_v<T, Ts...>
	void ctor(Ts &&...args) {
		new (pool) T{std::forward<Ts>(args)...};
	}
	// get T* value
	T *getPtr() {
		return reinterpret_cast<T *>(pool);
	}
	// get T& value
	T &get() {
		return *reinterpret_cast<T *>(pool);
	}
	// get const T& value
	const T &get() const {
		return *reinterpret_cast<T *>(pool);
	}
	// destruct T on pool storage
	void dtor() {
		reinterpret_cast<T *>(pool)->~T();
	}
};

} // namespace KUtils
