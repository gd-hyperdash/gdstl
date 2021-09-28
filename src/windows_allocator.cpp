#include <Windows.h>

// Types

typedef LPVOID(__cdecl *alloc_new_T)(unsigned int);
typedef void(__cdecl *alloc_delete_T)(LPVOID);

// Globals

static auto constexpr MOD_NAME = L"MSVCR120.dll";
static auto constexpr NEW_SYM = "??2@YAPAXI@Z";
static auto constexpr DELETE_SYM = "??3@YAXPAX@Z";

// Helpers

[[noreturn]]
void ThrowException()
{
    DebugBreak();
    abort();
}

static HMODULE GetHandle()
{
    static HMODULE h = NULL;

    if (h == NULL)
    {
        h = GetModuleHandleW(MOD_NAME);

        if (h == NULL)
            ThrowException();
    }

    return h;
}

// Allocators

extern "C"
{
    LPVOID gdstd_allocate(SIZE_T const size)
    {
        static alloc_new_T callNew = nullptr;

        if (!callNew)
        {
            callNew = reinterpret_cast<alloc_new_T>(
                GetProcAddress(GetHandle(), NEW_SYM));

            if (!callNew)
                ThrowException();
        }

        return callNew(size);
    }

    void gdstd_free(LPVOID p)
    {
        static alloc_delete_T callDelete = nullptr;

        if (!callDelete)
        {
            callDelete = reinterpret_cast<alloc_delete_T>(
                GetProcAddress(GetHandle(), DELETE_SYM));

            if (!callDelete)
                ThrowException();
        }

        callDelete(p);
    }
}