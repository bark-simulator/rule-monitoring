package(default_visibility = ["//visibility:public"])
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension", "pybind_library")


pybind_library(
    name = "rule_monitor",
    srcs = [
        "define_rule_monitor.cpp",
        "define_rule_monitor.hpp",
        "common.hpp",
    ],
    copts = ["-O3"],
    linkstatic = 1,
    deps = [
        "//ltl:rule_monitor",
        "@spot"
    ],
)

# the following rules are only for testing purposes

pybind_extension(
    name = "test_module_rule_monitor",
    srcs = [
        "module.cpp",
    ],
    deps = ["//python/bindings:rule_monitor"],
)