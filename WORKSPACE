workspace(name="rule_monitor_project")

load("//util:deps.bzl", "rule_monitor_dependencies")
rule_monitor_dependencies()

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python")