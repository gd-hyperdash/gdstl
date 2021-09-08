#include <stdlib.h>
#include <dlfcn.h>

// Types

typedef void*(*alloc_new_T)(unsigned int);
typedef void(*alloc_delete_T)(void *);

// Globals

static auto constexpr MOD_NAMES_N = 2u;

static char const* MOD_NAMES[MOD_NAMES_N] =
{
    "libcocos2dcpp.so",
    "libgame.so",
};

static auto constexpr NEW_SYM = "_Znwj";
static auto constexpr DELETE_SYM = "_ZdlPv";

// Helpers

static void* getHandle()
{
    static void* h = NULL;

    if (h == NULL)
    {
        for (auto i = 0u; i < MOD_NAMES_N; ++i)
        {
            h = dlopen(
                MOD_NAMES[i],
                RTLD_LAZY | RTLD_NOLOAD);

            if (h != NULL)
                break;
        }

        if (h == NULL)
            abort();
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
                dlsym(getHandle(), NEW_SYM));

            if (!callNew)
                abort();
        }

        return reinterpret_cast<uintptr_t>(callNew(size));
    }

    void gdstd_free_raw(uintptr_t p)
    {
        static alloc_delete_T callDelete = nullptr;

        if (!callDelete)
        {
            callDelete = reinterpret_cast<alloc_delete_T>(
                dlsym(getHandle(), DELETE_SYM));

            if (!callDelete)
                abort();
        }

        callDelete(reinterpret_cast<void*>(p));
    }
}