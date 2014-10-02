/* CE3 Plugin Manager - for licensing and copyright see license.txt */

// preprocessor tools
#define PPCAT_NX(A, B) A ## B //!< concatenate identifier
#define PPCAT(A, B) PPCAT_NX(A, B) //!< concatenate identifier

#define GVT(x) virtual int PPCAT(Get,x)() { return x; }

#define getVT(ptr) (*(void***)ptr)

#include <mhook.h>

template <class T, typename F>
int VTableIndex( F f )
{
    struct VTableCounter
    {
        GVT( 0 );
        GVT( 1 );
        GVT( 2 );
        GVT( 3 );
        GVT( 4 );
        GVT( 5 );
        GVT( 6 );
        GVT( 7 );
        GVT( 8 );
        GVT( 9 );
        GVT( 10 );
        GVT( 11 );
        GVT( 12 );
        GVT( 13 );
        GVT( 14 );
        GVT( 15 );
        GVT( 16 );
        GVT( 17 );
        GVT( 18 );
        GVT( 19 );
        GVT( 20 );
        GVT( 21 );
        GVT( 22 );
        GVT( 23 );
        GVT( 24 );
        GVT( 25 );
        GVT( 26 );
        GVT( 27 );
        GVT( 28 );
        GVT( 29 );
        GVT( 30 );
        GVT( 31 );
        GVT( 32 );
        GVT( 33 );
        GVT( 34 );
        GVT( 35 );
        GVT( 36 );
        GVT( 37 );
        GVT( 38 );
        GVT( 39 );
        GVT( 40 );
        GVT( 41 );
        GVT( 42 );
        GVT( 43 );
        GVT( 44 );
        GVT( 45 );
        GVT( 46 );
        GVT( 47 );
        GVT( 48 );
        GVT( 49 );
        GVT( 50 );
        GVT( 51 );
        GVT( 52 );
        GVT( 53 );
        GVT( 54 );
        GVT( 55 );
        GVT( 56 );
        GVT( 57 );
        GVT( 58 );
        GVT( 59 );
        GVT( 60 );
        GVT( 61 );
        GVT( 62 );
        GVT( 63 );
        GVT( 64 );
        GVT( 65 );
        GVT( 66 );
        GVT( 67 );
        GVT( 68 );
        GVT( 69 );
        GVT( 70 );
        GVT( 71 );
        GVT( 72 );
        GVT( 73 );
        GVT( 74 );
        GVT( 75 );
        GVT( 76 );
        GVT( 77 );
        GVT( 78 );
        GVT( 79 );
    } vt;

    T* t = reinterpret_cast<T*>( &vt );

    typedef int ( T::*GetIndex )();
    GetIndex getIndex = ( GetIndex )f;
    return ( t->*getIndex )();
};
