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

        template <typename Tup>
            constexpr size_t _tuple_size = std::tuple_size<typename std::remove_reference<Tup>::type>::value;

        /* make_[reverse_]list implementation */

        //meta-function interface declaration
        template <size_t B, size_t E, bool enabled = (B <= E)>
            struct indexes;

        //specialize for direct sequence of indexes
        template <size_t B, size_t E>
            struct indexes<B, E, true>
            {
                template <class Tup>
                    static decltype(auto) make_list(Tup &&tup)
                    {
                        return std::tuple_cat (
                            std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
                            indexes<B+1, E>::make_list(std::forward<Tup>(tup)));

                    }
            };

        //specialize for reverse sequence of indexes
        template <size_t B, size_t E>
            struct indexes<B, E, false>
            {
                template <class Tup>
                    static decltype(auto) make_list(Tup &&tup)
                    {
                        return std::tuple_cat (
                            std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
                            indexes<B-1, E>::make_list(std::forward<Tup>(tup)));

                    }
            };

        //recursion termination
        template <size_t N>
            struct indexes<N, N, true>
            {
                template <class Tup>
                    static decltype(auto) make_list (Tup &&tup)
                    {
                        return std::make_tuple(std::get<N>(std::forward<Tup>(tup)));
                    }
            };

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


    /* make_[reverse_]list */

    template <int B, int E, typename Tup>
        decltype(auto) make_list(Tup&& tup)
        {
            return detail::indexes<B, E>::make_list(std::forward<Tup>(tup));
        }

    /* convenience forwarder, not really necessary */
    template <int B, int E, typename Tup>
        decltype(auto) make_reverse_list(Tup&& tup)
        {
            static_assert(B >= E, "");
            return detail::indexes<E, B>::make_list(std::forward<Tup>(tup));
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
		constexpr auto S = detail::_tuple_size<Tup>;
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
		constexpr auto S = detail::_tuple_size<Tup>;
		static_assert(S > 0, "");
		return make_index_list<0>(tup);
	}

	template <class Tup>
	decltype(auto) last(Tup&& tup)
	{
		constexpr auto S = detail::_tuple_size<Tup>;
		static_assert(S > 0, "");
		return make_index_list<S-1>(tup);
	}

	template <class Tup>
	decltype(auto) reverse(Tup&& tup)
	{
		constexpr auto S = detail::_tuple_size<Tup>;
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
		constexpr auto S = detail::_tuple_size<Tup>;
		static_assert(N <= S && S > 0, "");

		return make_list<N, S-1>(tup);
	}

	template <int N, class Tup>
	decltype(auto) take(Tup&& tup)
	{
		constexpr auto S = detail::_tuple_size<Tup>;
		static_assert(N >= 0 && N <= S && S > 0, "");

		return make_list<0, N-1>(tup);
	}


	/* invoke function */

	template<typename Func, typename Tup>
	decltype(auto) invoke(Func&& func, Tup&& tup)
	{
		constexpr auto S = detail::_tuple_size<Tup>;
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
		constexpr auto S = detail::_tuple_size<Tup>;
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
