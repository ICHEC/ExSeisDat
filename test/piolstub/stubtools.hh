#ifndef PIOLSTUBSTUBTOOLS_HEADER_GUARD
#define PIOLSTUBSTUBTOOLS_HEADER_GUARD

#include <functional>
#include <type_traits>
#include <iostream>
#include <cxxabi.h>
#include <cassert>

//
// Filthy hack for const reference returns
//
// Some functions want to return a const reference.
// We want const reference variables to be static variables, so they don't go
// out of scope after the stub function returns.
// Some functions want to return void, so we make another specialization of this
// class where get() returns void with no reference.
//
// I couldn't do this with a template function for some reason, maybe due to T
// returning T& in one case and void, with no reference, in the other.
// A class specialization works, because there's no guarantee that members of
// two class specializations have anything in common.
//
template<typename T>
class MAKE_STUB_HANDLE_CONST_REF
{
public:
    typedef typename std::remove_reference<T>::type TType;
    const T& get() const
    {
        static TType t = TType();
        return t;
    }
};

template<>
class MAKE_STUB_HANDLE_CONST_REF<void>
{
public:
    void get() const { return; }
};


static std::string demangle(const char* name)
{
    int err = 0;
    char* demangled = abi::__cxa_demangle(name, NULL, NULL, &err);

    assert(err == 0);
    assert(demangled != NULL);

    std::string r{demangled};
    free(demangled);

    return r;
}


//
// General macro for defining function CXX_NAME with N arguments with a body calling a
// defined function callback CALLBACK_NAME with the same signature and return
// value as the defined function.
//
// Place your definitions in a header file, so they're accessible by other
// programs, then define MAKE_STUB_DEFINITION before including the header in
// a translation unit.
//
// For example, to wrap
//     My::Function::Name(int) const;
// and have a callback My_Function_name_cb, use the setup:
//
//     mystub.hh:
//     #include "my_function.hh"
//     #include "stubhelper.hh"
//     MAKE_STUB_1(My_Function_name_cb, My::Function::Name, const, int)
//
//     mystub.cc:
//     #define MAKE_STUB_DEFINITION
//     #include "mystub.hh"
//
// and just compile / link mystub.cc.
//
// Libraries using the stub can use the callback mechanism e.g.
//
//     stub_user.cc
//     #include "mystub.hh"
//     int main()
//     {
//         My_Funtion_name_cb = [](int i)
//         {
//             std::cout << "Ran My_Function_name_cb with argument " << i << "!" << std::endl;
//         }
//
//         My::Function::Name(10);
//     }
//
// will print out "Ran My_Function_name_cb with argument 10!", as given by the
// lambda.
//
#ifdef MAKE_STUB_DEFINITION
// Make std::function instance and define body (for a translation unit)
#define MAKE_STUB_N_FULL(CALLBACK_NAME, CXX_NAME, QUALIFIER, STUB_RETURN_TYPE, LAMBDA_RETURN_TYPE, SIGNATURE, CALL, RETURN) \
    std::function<LAMBDA_RETURN_TYPE SIGNATURE> CALLBACK_NAME; \
    STUB_RETURN_TYPE CXX_NAME SIGNATURE QUALIFIER { \
        if(CALLBACK_NAME) { \
            RETURN CALLBACK_NAME CALL; \
        } else { \
            std::cerr \
                << "Calling unset lambda " << #CALLBACK_NAME \
                << " in method " << #CXX_NAME \
                << " with signature " \
                << demangle(typeid(LAMBDA_RETURN_TYPE SIGNATURE).name()) \
                << std::endl; \
            RETURN CALLBACK_NAME CALL; \
        } \
    }
#else
// Make extern std::function forward declaration (for a header file)
#define MAKE_STUB_N_FULL(CALLBACK_NAME, CXX_NAME, QUALIFIER, STUB_RETURN_TYPE, LAMBDA_RETURN_TYPE, SIGNATURE, CALL, RETURN) \
    extern std::function<LAMBDA_RETURN_TYPE SIGNATURE> CALLBACK_NAME;
#endif

// Specialization of MAKE_STUB_N_FULL for functions
// Here, STUB_RETURN_TYPE and LAMBDA_RETURN_TYPE are both set to the RETURN_TYPE
// argument, and RETURN is set to return.
#define MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, RETURN_TYPE, SIGNATURE, CALL) \
    MAKE_STUB_N_FULL(CALLBACK_NAME, CXX_NAME, QUALIFIER, RETURN_TYPE, RETURN_TYPE, SIGNATURE, CALL, return)

// Specialization of MAKE_STUB_N_FULL for constructors
// Here, STUB_RETURN_TYPE is omitted because constructors don't have a
// return type, LAMBDA_RETURN_TYPE is set to void, and RETURN is omitted because
// you can't return a value from a constructor.
#define MAKE_STUB_N_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE, CALL) \
    MAKE_STUB_N_FULL(CALLBACK_NAME, CXX_NAME, QUALIFIER, , void, SIGNATURE, CALL, )


template<typename T>
struct Function;

template<typename Ret_, typename...Args_>
struct Function<Ret_(Args_...)>{
    typedef Ret_ Ret;

    // 1-indexed args
    template<int I>
    struct Args {
        typedef typename std::tuple_element<I-1, std::tuple<Args_...>>::type type;
    };
};

// Convenient specializations of MAKE_STUB_N for 1, 2, 3, ... arguments.
#define MAKE_STUB_0(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, (void), ())

#define MAKE_STUB_1(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, \
                (::Function< SIGNATURE >::Args<1>::type arg1), (arg1))

#define MAKE_STUB_2(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, \
                (::Function< SIGNATURE >::Args<1>::type arg1, \
                 ::Function< SIGNATURE >::Args<2>::type arg2), \
                (arg1, arg2))

#define MAKE_STUB_3(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, \
                (::Function< SIGNATURE >::Args<1>::type arg1, \
                 ::Function< SIGNATURE >::Args<2>::type arg2, \
                 ::Function< SIGNATURE >::Args<3>::type arg3), \
                (arg1, arg2, arg3))

#define MAKE_STUB_4(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, \
                (::Function< SIGNATURE >::Args<1>::type arg1, \
                 ::Function< SIGNATURE >::Args<2>::type arg2, \
                 ::Function< SIGNATURE >::Args<3>::type arg3, \
                 ::Function< SIGNATURE >::Args<4>::type arg4), \
                (arg1, arg2, arg3, arg4))

#define MAKE_STUB_7(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, \
                (::Function< SIGNATURE >::Args<1>::type arg1, \
                 ::Function< SIGNATURE >::Args<2>::type arg2, \
                 ::Function< SIGNATURE >::Args<3>::type arg3, \
                 ::Function< SIGNATURE >::Args<4>::type arg4, \
                 ::Function< SIGNATURE >::Args<5>::type arg5, \
                 ::Function< SIGNATURE >::Args<6>::type arg6, \
                 ::Function< SIGNATURE >::Args<7>::type arg7), \
                (arg1, arg2, arg3, arg4, arg5, arg6, arg7))

#define MAKE_STUB_8(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N(CALLBACK_NAME, CXX_NAME, QUALIFIER, ::Function< SIGNATURE >::Ret, \
                (::Function< SIGNATURE >::Args<1>::type arg1, \
                 ::Function< SIGNATURE >::Args<2>::type arg2, \
                 ::Function< SIGNATURE >::Args<3>::type arg3, \
                 ::Function< SIGNATURE >::Args<4>::type arg4, \
                 ::Function< SIGNATURE >::Args<5>::type arg5, \
                 ::Function< SIGNATURE >::Args<6>::type arg6, \
                 ::Function< SIGNATURE >::Args<7>::type arg7, \
                 ::Function< SIGNATURE >::Args<8>::type arg8), \
                (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8))



// Convenient specializations of MAKE_STUB_N_CTOR for 1, 2, 3, ... arguments.
#define MAKE_STUB_0_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER) \
    MAKE_STUB_N_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, (void), ())

#define MAKE_STUB_1_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, \
                     (::Function< SIGNATURE >::Args<1>::type arg1), \
                     (arg1))

#define MAKE_STUB_2_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, \
                     (::Function< SIGNATURE >::Args<1>::type arg1, \
                      ::Function< SIGNATURE >::Args<2>::type arg2), \
                     (arg1, arg2))

#define MAKE_STUB_3_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, \
                     (::Function< SIGNATURE >::Args<1>::type arg1, \
                      ::Function< SIGNATURE >::Args<2>::type arg2, \
                      ::Function< SIGNATURE >::Args<3>::type arg3), \
                     (arg1, arg2, arg3))

#define MAKE_STUB_4_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, SIGNATURE) \
    MAKE_STUB_N_CTOR(CALLBACK_NAME, CXX_NAME, QUALIFIER, \
                     (::Function< SIGNATURE >::Args<1>::type arg1, \
                      ::Function< SIGNATURE >::Args<2>::type arg2, \
                      ::Function< SIGNATURE >::Args<3>::type arg3, \
                      ::Function< SIGNATURE >::Args<4>::type arg4), \
                     (arg1, arg2, arg3, arg4))

#endif
