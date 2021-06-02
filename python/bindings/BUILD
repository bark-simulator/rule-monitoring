package(default_visibility = ["//visibility:public"])
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension", "pybind_library")

# the following rules are only for testing purposes

cc_library(
    name = "rule_monitor",
    srcs = [
        "common.hpp",
        "define_rule_monitor.cpp",
        "define_rule_monitor.hpp",
    ],
    deps = [
        "//ltl:rule_monitor",
        "@spot",
    ],
)

pybind_extension(
    name = "rule_monitor",
    srcs = [
        "common.hpp",
        "define_rule_monitor.cpp",
        "define_rule_monitor.hpp",
        "module.cpp",
    ],
    copts = ["-O3"],
    linkstatic = 1,
    deps = [
        "//ltl:rule_monitor",
        "@spot"
    ],
)
