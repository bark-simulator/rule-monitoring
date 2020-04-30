cc_library(
    name = "rule_monitor",
    srcs = [
        "rule_monitor.cpp",
        "label.cpp",
        "rule_state.cpp",
    ],
    hdrs = [
        "common.h",
        "evaluator_label_base.h",
        "rule_monitor.h",
        "label.h",
        "rule_state.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_eigen_eigen//:eigen",
        "@com_github_google_glog//:glog",
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
