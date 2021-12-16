#include <UTemplate/Func.hpp>
#include <bits/stdc++.h>
#include <fmt/format.h>

namespace details {
template <typename... Ts>
struct FHelper {
	template <typename R>
	struct RT {
		using StdFunc = std::function<R(std::function<R(Ts...)>, Ts...)>;
	};
};
template <typename T>
struct YHelper {
	static_assert(std::is_function_v<T>, "T must be a function");
	using Func = Ubpa::FuncTraits<T>;
	using R = typename Func::Return;
	using ArgList = typename Func::ArgList;
	using Sig = typename Func::Signature;
	using StdFunc = std::function<Sig>;
	using StdYf = typename Ubpa::ToOtherList_t<ArgList, FHelper>::template RT<R>::StdFunc;
	static constexpr StdFunc Comb(StdYf f) {
		return [&f](auto... args) {
			return f(Comb(f), args...);
		};
	}
};
} // namespace details
template <typename T>
inline constexpr auto Y = details::YHelper<T>::Comb;
using i32 = std::int32_t;
int main() {
	auto f = Y<i32(i32)>([](auto &&self, auto x) {
		if (x == 0)
			return 0;
		else
			return x + self(x - 1);
	});
	fmt::print("{}\n", f(100));
	return 0;
}
