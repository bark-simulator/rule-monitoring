package(default_visibility = ["//visibility:public"])

# the following rules are only for testing purposes

cc_binary(
    name = "rule_monitor.so",
    srcs = [
        "module.cpp",
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
    linkshared = 1,
    linkstatic = 1,
    copts=["-O3"],
)