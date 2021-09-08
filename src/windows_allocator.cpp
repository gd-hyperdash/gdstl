#include <Windows.h>

// Types

typedef void*(__cdecl *alloc_new_T)(unsigned int);
typedef void(__cdecl *alloc_delete_T)(void *);

// Globals

static auto constexpr MOD_NAME = L"MSVCR120.dll";
static auto constexpr NEW_SYM = "??2@YAPAXI@Z";
static auto constexpr DELETE_SYM = "??3@YAXPAX@Z";

// Helpers

static HMODULE getHandle()
{
    static HMODULE h = NULL;

    if (h == NULL)
    {
        h = GetModuleHandleW(MOD_NAME);

        if (h == NULL)
            ExitProcess(1);
    }

    return h;
}

// Allocators

extern "C"
{
    uintptr_t gdstd_allocate_raw(size_t const size)
    {
        static alloc_new_T callNew = nullptr;

        if (!callNew)
        {
            callNew = reinterpret_cast<alloc_new_T>(
                GetProcAddress(getHandle(), NEW_SYM));

            if (!callNew)
                ExitProcess(1);
        }

        return reinterpret_cast<uintptr_t>(callNew(size));
    }

    void gdstd_free_raw(uintptr_t p)
    {
        static alloc_delete_T callDelete = nullptr;

        if (!callDelete)
        {
            callDelete = reinterpret_cast<alloc_delete_T>(
                GetProcAddress(getHandle(), DELETE_SYM));

            if (!callDelete)
                ExitProcess(1);
        }

        callDelete(reinterpret_cast<void*>(p));
    }
}