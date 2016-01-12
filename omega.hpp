/* omega.hpp */
// a tiny tuple meta library
// written by Gian Lorenzo Meocci <glmeocci@gmail.com>

#include <tuple>
#include <utility>
#include <type_traits>
#include <iostream>


namespace omega
{
    namespace detail
    {
        template <int i>
            using _int_c = std::integral_constant<int, i>;

        template <int B, int E>
            using _same_index = std::is_same<_int_c<B>, _int_c<E>>;


        /* make_list implementation */

        template <int B, int E, typename Tup>
            decltype(auto) _make_list(Tup&& tup, std::true_type)
            {
                return std::make_tuple(std::get<B>(std::forward<Tup>(tup)));
            }

        template <int B, int E, typename Tup>
            decltype(auto) _make_list(Tup&& tup, std::false_type)
            {
                return std::tuple_cat(
                    std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
                    _make_list<B+1, E>(std::forward<Tup>(tup), _same_index<B+1, E>{})
                    );
            }

        template <int B, int E, typename Tup>
            decltype(auto) _check_bound(Tup&& tup, std::true_type)
            {
                return _make_list<B, E>(std::forward<Tup>(tup), _same_index<B, E>{});
            }

        template <int B, int E, typename Tup>
            decltype(auto) _check_bound(Tup&&, std::false_type)
            {
                return std::make_tuple();
            }

        /* make_reverse_list implementation */

        template <int B, int E, typename Tup>
            decltype(auto) _make_reverse_list(Tup&& tup, std::true_type)
            {
                return std::make_tuple(std::get<B>(std::forward<Tup>(tup)));
            }

        template <int B, int E, typename Tup>
            decltype(auto) _make_reverse_list(Tup&& tup, std::false_type)
            {
                return std::tuple_cat(
                    std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
                    _make_reverse_list<B-1, E>(std::forward<Tup>(tup), _same_index<B-1, E>{})
                    );
            }

        /* make_index_list implementation */

        template <class Tup>
            decltype(auto) _make_index_list(Tup&&)
            {
                return std::make_tuple();
            }

        template <class Tup, int B, int ...Index>
            decltype(auto) _make_index_list(Tup&& tup)
            {
                return std::tuple_cat(
                    std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
                    _make_index_list<Tup, Index...>(std::forward<Tup>(tup))
                    );
            }


        /* invoke function implementation */

        template<typename Func, typename Tup, std::size_t... index>
            decltype(auto) _invoke (Func&& func, Tup&& tup, std::index_sequence<index...>)
            {
                return func(std::get<index>(std::forward<Tup>(tup))...);
            }

        /* utility implementation */

        template<typename Array, std::size_t... I>
            decltype(auto) _array2tuple(const Array& a, std::index_sequence<I...>)
            {
                return std::make_tuple(a[I]...);
            }

        /* show tuple implementation */

        template <int B, int E, typename Tup>
            void _show(std::ostream& out, Tup&& tup, std::true_type)
            {
                out << std::get<B>(std::forward<Tup>(tup));
            }

        template <int B, int E, typename Tup>
            void _show(std::ostream& out, Tup&& tup, std::false_type)
            {
                out << std::get<B>(std::forward<Tup>(tup)) << ", ";
                _show<B+1, E>(out, std::forward<Tup>(tup), _same_index<B+1, E>{});
            }


    }//namespace detail


    /* make_list */
	template <int B, int E, typename Tup>
	decltype(auto) make_list(Tup&& tup)
	{
		return detail::_check_bound<B, E>(tup, std::integral_constant<bool, B <= E>{});
	}

	template <int B, int E, typename Tup>
	decltype(auto) make_reverse_list(Tup&& tup)
	{
		static_assert(B >= E, "");
		return detail::_make_reverse_list<B, E>(std::forward<Tup>(tup), detail::_same_index<B, E>{});
	}


	/* make_index_list */

	template <int B, int ...Index, class Tup>
	decltype(auto) make_index_list(Tup&& tup)
	{
		return detail::_make_index_list<Tup, B, Index...>(std::forward<Tup>(tup));
	}


	/* tail, head, last, init, reverse */

	template <class Tup>
	decltype(auto) tail(Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		static_assert(S > 1, "");

		return make_list<1, S-1>(tup);
	}

	template <class T>
	decltype(auto) tail(std::tuple<T>&& tup)
	{
		return std::make_tuple();
	}

	template <class Tup>
	decltype(auto) head(Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		static_assert(S > 0, "");
		return make_index_list<0>(tup);
	}

	template <class Tup>
	decltype(auto) last(Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		static_assert(S > 0, "");
		return make_index_list<S-1>(tup);
	}

	template <class Tup>
	decltype(auto) reverse(Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		static_assert(S > 0, "");
		return make_reverse_list<S-1, 0>(tup);
	}

	decltype(auto) reverse(std::tuple<>)
	{
		return std::make_tuple();
	}


	template <class Tup>
	decltype(auto) init(Tup&& tup)
	{
		return reverse(tail(reverse(tup)));
	}


	/* drop, take */

	template <int N, class Tup>
	decltype(auto) drop(Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		static_assert(N <= S && S > 0, "");

		return make_list<N, S-1>(tup);
	}

	template <int N, class Tup>
	decltype(auto) take(Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		static_assert(N >= 0 && N <= S && S > 0, "");

		return make_list<0, N-1>(tup);
	}


	/* invoke function */

	template<typename Func, typename Tup>
	decltype(auto) invoke(Func&& func, Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		return detail::_invoke(std::forward<Func>(func),
							 std::forward<Tup>(tup),
							 std::make_index_sequence<S>{});
	}


	/* utility */

	template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
	decltype(auto) array2tuple(const std::array<T, N>& a)
	{
		return detail::_array2tuple(a, Indices());
	}

    /* show tuple */

	template <typename Tup>
	void show(std::ostream& out, Tup&& tup)
	{
		constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
		out << "<";
        detail::_show<0, S-1>(out, std::forward<Tup>(tup), detail::_same_index<0, S-1>{});
		out << ">";
	}

	void show(std::ostream& out, std::tuple<>)
	{
		out << "<>";
	}
}

template <typename T, typename ...Ts>
std::ostream& operator<< (std::ostream& stream, const std::tuple<T, Ts...>& tup)
{
	omega::show(stream, tup);
	return stream;
}
