/* CE3 Plugin Manager - for licensing and copyright see license.txt */
#pragma once

#include <CryWindows.h>

// Check memory address access
const DWORD dwForbiddenArea = PAGE_GUARD | PAGE_NOACCESS;
const DWORD dwReadRights = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
const DWORD dwWriteRights = PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

template<DWORD dwAccessRights>
bool CheckAccess( void* pAddress, size_t nSize )
{
    if ( !pAddress || !nSize )
    {
        return false;
    }

    MEMORY_BASIC_INFORMATION sMBI;
    bool bRet = false;

    UINT_PTR pCurrentAddress = UINT_PTR( pAddress );
    UINT_PTR pEndAdress = pCurrentAddress + ( nSize - 1 );

    do
    {
        ZeroMemory( &sMBI, sizeof( sMBI ) );
        VirtualQuery( LPCVOID( pCurrentAddress ), &sMBI, sizeof( sMBI ) );

        bRet = ( sMBI.State & MEM_COMMIT ) // memory allocated and
               && !( sMBI.Protect & dwForbiddenArea ) // access to page allowed and
               && ( sMBI.Protect & dwAccessRights ); // the required rights

        pCurrentAddress = ( UINT_PTR( sMBI.BaseAddress ) + sMBI.RegionSize );
    }
    while ( bRet && pCurrentAddress <= pEndAdress );

    return bRet;
}

#define IsBadWritePtr(p,n) (!CheckAccess<dwWriteRights>(p,n))
#define IsBadReadPtr(p,n) (!CheckAccess<dwReadRights>(p,n))

// preprocessor tools
#define PPCAT_NX(A, B) A ## B //!< concatenate identifier
#define PPCAT(A, B) PPCAT_NX(A, B) //!< concatenate identifier

#define GVT(x) virtual int PPCAT(Get,x)() { return x; }

#define getVT(ptr) (IsBadWritePtr(ptr, sizeof(INT_PTR)) ? NULL : *(void***)ptr)

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
        GVT( 80 );
        GVT( 81 );
        GVT( 82 );
        GVT( 83 );
        GVT( 84 );
        GVT( 85 );
        GVT( 86 );
        GVT( 87 );
        GVT( 88 );
        GVT( 89 );
        GVT( 90 );
        GVT( 91 );
        GVT( 92 );
        GVT( 93 );
        GVT( 94 );
        GVT( 95 );
        GVT( 96 );
        GVT( 97 );
        GVT( 98 );
        GVT( 99 );
        GVT( 100 );
        GVT( 101 );
        GVT( 102 );
        GVT( 103 );
        GVT( 104 );
        GVT( 105 );
        GVT( 106 );
        GVT( 107 );
        GVT( 108 );
        GVT( 109 );
        GVT( 110 );
        GVT( 111 );
        GVT( 112 );
        GVT( 113 );
        GVT( 114 );
        GVT( 115 );
        GVT( 116 );
        GVT( 117 );
        GVT( 118 );
        GVT( 119 );
        GVT( 120 );
        GVT( 121 );
        GVT( 122 );
        GVT( 123 );
        GVT( 124 );
        GVT( 125 );
        GVT( 126 );
        GVT( 127 );
        GVT( 128 );
        GVT( 129 );
        GVT( 130 );
        GVT( 131 );
        GVT( 132 );
        GVT( 133 );
        GVT( 134 );
        GVT( 135 );
        GVT( 136 );
        GVT( 137 );
        GVT( 138 );
        GVT( 139 );
        GVT( 140 );
        GVT( 141 );
        GVT( 142 );
        GVT( 143 );
        GVT( 144 );
        GVT( 145 );
        GVT( 146 );
        GVT( 147 );
        GVT( 148 );
        GVT( 149 );
        GVT( 150 );
        GVT( 151 );
        GVT( 152 );
        GVT( 153 );
        GVT( 154 );
        GVT( 155 );
        GVT( 156 );
        GVT( 157 );
        GVT( 158 );
        GVT( 159 );
        GVT( 160 );
        GVT( 161 );
        GVT( 162 );
        GVT( 163 );
        GVT( 164 );
        GVT( 165 );
        GVT( 166 );
        GVT( 167 );
        GVT( 168 );
        GVT( 169 );
        GVT( 170 );
        GVT( 171 );
        GVT( 172 );
        GVT( 173 );
        GVT( 174 );
        GVT( 175 );
        GVT( 176 );
        GVT( 177 );
        GVT( 178 );
        GVT( 179 );
        GVT( 180 );
        GVT( 181 );
        GVT( 182 );
        GVT( 183 );
        GVT( 184 );
        GVT( 185 );
        GVT( 186 );
        GVT( 187 );
        GVT( 188 );
        GVT( 189 );
        GVT( 190 );
        GVT( 191 );
        GVT( 192 );
        GVT( 193 );
        GVT( 194 );
        GVT( 195 );
        GVT( 196 );
        GVT( 197 );
        GVT( 198 );
        GVT( 199 );
        GVT( 200 );
    } vt;

    T* t = reinterpret_cast<T*>( &vt );

    typedef int ( T::*GetIndex )();
    GetIndex getIndex = ( GetIndex )f;
    return ( t->*getIndex )();
};
