package(default_visibility = ["//visibility:public"])

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
        "@pybind11",
        "@python_linux//:python-lib",
        "@spot",
    ],
)

cc_binary(
    name = "rule_monitor.so",
    srcs = [
        "common.hpp",
        "define_rule_monitor.cpp",
        "define_rule_monitor.hpp",
        "module.cpp",
    ],
    copts = ["-O3"],
    linkshared = 1,
    linkstatic = 1,
    deps = [
        "//ltl:rule_monitor",
        "@pybind11",
        "@python_linux//:python-lib",
        "@spot",
    ],
)
