#include<iostream>
using sizet = std::size_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

// -----------------------------------------
//
//   Compile time GCD
//
// -----------------------------------------

template<int64 N, int64 M, int64 K>
struct A;

template<int64 N, int64 M>
struct GCD {
    static constexpr int64 value = A<N, M, N% M>::value;
};
template<int64 N, int64 M, int64 K>
struct A {
    static constexpr int64 value = A<M, K, M% K>::value;
};
template<int64 N, int64 M>
struct A<N, M, 0> {
    static constexpr int64 value = M;
};

// -----------------------------------------
//
//   Fractlaration
//
// -----------------------------------------

template<int64 _Num, int64 _DeNum>
struct Fraction;
template<typename _Fract>
struct Inverse;

template<typename _Fract1, typename _Fract2, typename ... _Rest>
struct FractionAdd;
template<typename _Fract1, int64 scalar>
struct FractionAddScalar;

template<typename _Fract1, typename _Fract2, typename ... _Rest>
struct FractionSub;
template<typename _Fract1, int64 scalar>
struct FractionSubScalar;

template<typename _Fract1, typename _Fract2, typename ... _Rest>
struct FractionMul;
template<typename _Fract1, int64 scalar>
struct FractionMulScalar;

template<typename _Fract1, typename _Fract2>
struct FractionDiv;
template<typename _Fract1, int64 scalar>
struct FractionDivScalar;

// -----------------------------------------
//
//   Definition
//
// -----------------------------------------

// Basic struct representing a fraction
template<int64 _Num, int64 _DeNum>
struct Fraction {
    static constexpr int64 num = _Num / GCD<_Num, _DeNum>::value;
    static constexpr int64 denum = _DeNum / GCD<_Num, _DeNum>::value;
    static constexpr double value = static_cast<double>(_Num) / _DeNum;
};

// Inverse a fraction
template<typename _Fract>
struct Inverse {
    static constexpr int64  num = _Fract::denum;
    static constexpr int64  denum = _Fract::num;
    static constexpr double value = static_cast<double>(num) / denum;
};

// Add two fractions together
template<typename _Fract1, typename _Fract2>
struct FractionAdd<_Fract1, _Fract2> {
    static constexpr int64 num      =   Fraction<
        _Fract1::num* _Fract2::denum + _Fract1::denum * _Fract2::num,
        _Fract1::denum* _Fract2::denum
    >::num;
    static constexpr int64 denum    =   Fraction<
        _Fract1::num* _Fract2::denum + _Fract1::denum * _Fract2::num,
        _Fract1::denum* _Fract2::denum
    >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Add multiple fractions together
template<typename _Fract1 = Fraction<0, 1>, typename _Fract2 = Fraction<0, 1>, typename ... _Rest>
struct FractionAdd {
    static constexpr int64 num      =   FractionAdd<FractionAdd<_Fract1, _Fract2>, FractionAdd<_Rest...> >::num;
    static constexpr int64 denum    =   FractionAdd<FractionAdd<_Fract1, _Fract2>, FractionAdd<_Rest...> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Add a scalar to a fraction
template<typename _Fract1, int64 scalar>
struct FractionAddScalar {
    static constexpr int64  num     =   FractionAdd<Fraction<scalar, scalar> >::num;
    static constexpr int64  denum   =   FractionAdd<Fraction<scalar, scalar> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Subtract two fractions
template<typename _Fract1, typename _Fract2>
struct FractionSub<_Fract1, _Fract2> {
    static constexpr int64  num = FractionAdd<_Fract1, FractionMulScalar<_Fract2, -1> >::num;
    static constexpr int64  denum = FractionAdd<_Fract1, FractionMulScalar<_Fract2, -1> >::denum;
    static constexpr double value = static_cast<double>(num) / denum;
};

// Subtract multiple fractions
template<typename _Fract1 = Fraction<0, 1>, typename _Fract2 = Fraction<0, 1>, typename ... _Rest>
struct FractionSub {
    static constexpr int64  num     =   FractionSub<FractionSub<_Fract1, _Fract2>, FractionAdd<_Rest...> >::num;
    static constexpr int64  denum   =   FractionSub<FractionSub<_Fract1, _Fract2>, FractionAdd<_Rest...> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Subtract a scalar from a fraction 
template<typename _Fract1, int64 scalar>
struct FractionSubScalar {
    static constexpr int64  num     =   FractionAddScalar<_Fract1, Fraction<-scalar, scalar> >::num;
    static constexpr int64  denum   =   FractionAddScalar<_Fract1, Fraction<-scalar, scalar> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Multiply two fractions
template<typename _Fract1, typename _Fract2>
struct FractionMul<_Fract1, _Fract2> {
    static constexpr int64  num     =   Fraction< (_Fract1::num* _Fract2::num), (_Fract1::denum* _Fract2::denum) >::num;
    static constexpr int64  denum   =   Fraction< (_Fract1::num* _Fract2::num), (_Fract1::denum* _Fract2::denum) >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Multiply multiple fractions
template<typename _Fract1 = Fraction<1, 1>, typename _Fract2 = Fraction<1, 1>, typename ... _Rest>
struct FractionMul {
    static constexpr int64  num     =   FractionMul<FractionMul<_Fract1, _Fract2>, FractionMul<_Rest...> >::num;
    static constexpr int64  denum   = FractionMul<FractionMul<_Fract1, _Fract2>, FractionMul<_Rest...> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Multiply a scalar to a fraction
template<typename _Fract1, int64 scalar>
struct FractionMulScalar {
    static constexpr int64  num     =   Fraction< (_Fract1::num * scalar), (_Fract1::denum) >::num;
    static constexpr int64  denum   =   Fraction< (_Fract1::num * scalar), (_Fract1::denum) >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Divide two fractions
template<typename _Fract1, typename _Fract2>
struct FractionDiv {
    static constexpr int64  num     =   FractionMul<_Fract1, Inverse<_Fract2> >::num;
    static constexpr int64  denum   =   FractionMul<_Fract1, Inverse<_Fract2> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

// Divide a fraction from a scalar
template<typename _Fract1, int64 scalar>
struct FractionDivScalar {
    static constexpr int64  num     =   FractionMul<_Fract1, Fraction<1, scalar> >::num;
    static constexpr int64  denum   =   FractionMul<_Fract1, Fraction<1, scalar> >::denum;
    static constexpr double value   =   static_cast<double>(num) / denum;
};

void print() {
    std::cout << std::endl;
}

template<typename T, typename ... Tail>
void print(const T& t, Tail... tail) {
    std::cout << t << " ";
    print(tail...);
}

int main1() {
    using res = FractionAdd<Fraction<-1, 3>, Fraction<1, 3>, Fraction<1, 2> >;
    print(res::num, "/", res::denum, " = ", res::value);
    return 0;
}