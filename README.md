# GDSTL
STL containers, adapted for Geometry Dash.

### Why?

- On android, older versions of the game are statically linked against [libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/), which has been deprecated by the NDK and replaced by [libc++](https://libcxx.llvm.org/).
- On windows, compiling in debug mode modifies the size of all containers, to embed debugging related data.

In those cases, the ABI of the standard library is not compatible with the one used by Geometry Dash. This wrapper aims at fixing these issues, providing a simple, cross platform and stl-agnostic API.

### Compiling

GDSTL must link to the game memory allocator. The support library can be compiled using the pre built [scripts](scripts), which must be executed in the root of the project.

**Note**: [cmake](https://cmake.org/), [ninja](https://ninja-build.org/) and [clang](https://clang.llvm.org/) are required. Additionally, you will need the [NDK](https://developer.android.com/ndk) to build for android. Remember to set the NDK path inside the `scripts/vars` file.

### How to

The containers can be accessed from the `gdstd` namespace. While conversion requires the appropriate routines, construction can be done directly.

```cpp
std::string s1("Hello World!");
gdstd::string s2 = s1;
std::string s3 = gdstd::to_string(s2);
```

### Progress

| Container         | Status                                                             |
|-------------------|--------------------------------------------------------------------|
| `map<K, V>`       | Not implemented                                                    |
| `vector<T>`       | Untested, see [#1](https://github.com/gd-hyperdash/gdstl/issues/1) |
| `basic_string<T>` | Implemented                                                        |