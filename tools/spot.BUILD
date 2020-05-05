cc_library(
    name = "spot",
    srcs = glob(
        [
            "spot/**/*.cc",
            "spot/**/*.h",
            "spot/**/*.hxx",
            "spot/*.h",
            "spot/**/*.hh",
        ],
    ),
    hdrs = glob(
        [
            "spot/**/*.hh",
        ],
    ),
    copts = [
        "-std=c++14",
    ],
    includes = ["buddy/src", "spot/misc"],
    defines = [
        "VERSION=23",
        "BUDDY_DLL_EXPORTS",
        "BUDDY_DLL",
        "SPOT_WANT_STRONG_X"
    ],
    linkstatic = 0,
    alwayslink = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":bddx",
        ":configure",
        ":picosat",
        ":utf8",
        #":ltdl",
    ],
    linkopts = ["-ldl", "-lltdl"],
)

cc_library(
    name = "bddx",
    srcs = glob(
        [
            "buddy/src/*.c",
            "buddy/src/*.h",
            "buddy/src/*.cxx",
        ],
    ),
    hdrs = [
        "buddy/src/bddx.h",
        "buddy/src/bvecx.h",
        "buddy/src/fddx.h",
    ],
    defines = [
        "VERSION=23",
        "BUDDY_DLL_EXPORTS",
        "BUDDY_DLL",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "configure",
    srcs = [],
    hdrs = [
        "config.h",
    ],
    includes = ["./"],
    linkstatic = 1,
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "picosat",
    srcs = [
        "picosat/picosat.c",
        "picosat/picosat.h",
    ],
    hdrs = [
        "picosat/picosat.h",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "utf8",
    srcs = [
        "utf8/utf8/checked.h",
        "utf8/utf8/core.h",
        "utf8/utf8/unchecked.h",
    ],
    hdrs = [
        "utf8/utf8.h",
    ],
    linkstatic = 1,
    visibility = [
        "//visibility:public",
    ],
)

# BUG: Unfortunately there is an issue with libltdl: "lt_libltdl_LTX_preloaded_symbols" is undefined in the compiled library
# Temporary fix: Link spot with prebuild library libltdl-dev
cc_library(
    name = "ltdl",
    srcs = [
        "ltdl/ltdl.c",
        "ltdl/loaders/preopen.c",
        "ltdl/loaders/dlopen.c",
        "ltdl/config.h",
        "ltdl/lt__alloc.c",
        "ltdl/lt__dirent.c",
        "ltdl/lt_dlloader.c",
        "ltdl/lt_error.c",
        "ltdl/lt__strl.c",
        "ltdl/slist.c",
    ],
    copts = ["-I external/spotremote/ltdl/libltdl", "-I external/spotremote/ltdl"],
    defines = ["LTDLOPEN=libltdl", "LTDL"],
    hdrs = glob(
        [
            "ltdl/ltdl.h",
            "ltdl/config.h",
            "ltdl/libltdl/*.h",
        ],
    ),
    visibility = [
        "//visibility:public",
    ],
    linkopts = ["-ldl"],
)
