#include <array>

namespace sx {

namespace detail {
template <typename T, std::size_t N, std::size_t... Is>
constexpr auto make_array_impl(std::array<T, N> const &arr, T const &t,
                               std::index_sequence<Is...>)
    -> std::array<T, N + 1> {
  return {arr[Is]..., t};
}

template <typename T, std::size_t N, std::size_t... Is>
constexpr auto make_array_impl(T const &t, std::array<T, N> const &arr,
                               std::index_sequence<Is...>)
    -> std::array<T, N + 1> {
  return {t, arr[Is]...};
}

template <typename T, std::size_t N1, std::size_t N2, std::size_t... Is1,
          std::size_t... Is2>
constexpr auto
make_array_impl(std::array<T, N1> const &arr1, std::array<T, N2> const &arr2,
                std::index_sequence<Is1...>, std::index_sequence<Is2...>)
    -> std::array<T, N1 + N2> {
  return {arr1[Is1]..., arr2[Is2]...};
}
} // namespace detail

template <typename T>
constexpr auto make_array(T const &elem1, T const &elem2) -> std::array<T, 2> {
  return {elem1, elem2};
}

template <typename T, std::size_t N>
constexpr auto make_array(std::array<T, N> const &arr, T const &t) {
  return detail::make_array_impl(arr, t, std::make_index_sequence<N>{});
}

template <typename T, std::size_t N>
constexpr auto make_array(T const &t, std::array<T, N> const &arr) {
  return detail::make_array_impl(t, arr, std::make_index_sequence<N>{});
}

template <typename T, std::size_t N1, std::size_t N2>
constexpr auto make_array(std::array<T, N1> const &arr1,
                          std::array<T, N2> const &arr2) {
  return detail::make_array_impl(arr1, arr2, std::make_index_sequence<N1>{},
                                 std::make_index_sequence<N2>{});
}

template <typename T, typename... Ts>
constexpr auto make_array(T const &t, Ts... ts) {
  return make_array(t, make_array(ts...));
}

} // namespace sx
