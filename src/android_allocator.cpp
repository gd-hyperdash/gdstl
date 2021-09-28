#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>

#if defined(__aarch64__) || defined(__x86_64__)
#define GDSTL_TARGET_64
#elif defined(__arm__) || defined(__thumb__) || defined(__i386__)
#define GDSTL_TARGET_32
#endif

// Types

#if defined(GDSTL_TARGET_64)
typedef void*(*alloc_new_T)(unsigned long);
#elif defined(GDSTL_TARGET_32)
typedef void*(*alloc_new_T)(unsigned int);
#endif

typedef void(*alloc_delete_T)(void*);

// Globals

static auto constexpr MOD_NAMES_N = 2u;

static char const* MOD_NAMES[MOD_NAMES_N] =
{
    "libcocos2dcpp.so",
    "libgame.so",
};

#if defined(GDSTL_TARGET_64)
static auto constexpr NEW_SYM = "_Znwm";
#elif defined(GDSTL_TARGET_32)
static auto constexpr NEW_SYM = "_Znwj";
#endif

static auto constexpr DELETE_SYM = "_ZdlPv";

// Helpers

[[noreturn]]
void throwex()
{
    raise(SIGTRAP);
    abort();
}

static void* get_handle()
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
            throwex();
    }

    return h;
}

// Allocators

extern "C"
{
    void* gdstd_allocate(size_t const size)
    {
        static alloc_new_T callNew = nullptr;

        if (!callNew)
        {
            callNew = reinterpret_cast<alloc_new_T>(
                dlsym(get_handle(), NEW_SYM));

            if (!callNew)
                throwex();
        }

        return callNew(size);
    }

    void gdstd_free(void* p)
    {
        static alloc_delete_T callDelete = nullptr;

        if (!callDelete)
        {
            callDelete = reinterpret_cast<alloc_delete_T>(
                dlsym(get_handle(), DELETE_SYM));

            if (!callDelete)
                throwex();
        }

        callDelete(p);
    }
}