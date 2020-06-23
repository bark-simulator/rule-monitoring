cc_library(
    name = "rule_monitor",
    srcs = [
        "rule_monitor.cpp",
        "rule_state.cpp",
    ],
    hdrs = [
        "common.h",
        "rule_monitor.h",
        "rule_state.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_eigen_eigen//:eigen",
        "@com_github_google_glog//:glog",
        "@bark_project//bark/world/evaluation/ltl/label:include",
        "@spot",
    ],
)

cc_library(
    name = "include",
    hdrs = glob(["*.h"]),
    deps = [
        "@com_github_eigen_eigen//:eigen",
        "@com_github_google_glog//:glog",
        "@spot",
    ],
)
