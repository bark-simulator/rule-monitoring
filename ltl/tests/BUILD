
cc_test(
    name = "automaton_test",
    srcs = ["automaton_test.cpp"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "//ltl:rule_monitor",
        "@com_github_gflags_gflags//:gflags",
        "@gtest//:main",
    ],
)

cc_test(
    name = "zipper_merge_formula_test",
    srcs = ["zipper_merge_formula_test.cpp"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "//ltl:rule_monitor",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@gtest//:main",
    ],
)
