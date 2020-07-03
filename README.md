![CI Build](https://github.com/bark-simulator/rule-monitoring/workflows/CI/badge.svg)

# Rule Monitor
Runtime Verification of LTL Rules implementation in C++.

## Installation & Test
- Install [bazel](https://docs.bazel.build/versions/master/install.html)
- Run `bazel test //...` in the WORKSPACE directory

# Dependencies
- libltdl-dev (should be part of Ubuntu xenial and bionic already)

Please have a look into the corresponding [publication](https://arxiv.org/abs/2007.00330)

If you use code in this modules, please cite it as:

```
@misc{esterle2020,
    title={Formalizing Traffic Rules for Machine Interpretability},
    author={Klemens Esterle and Luis Gressenbuch and Alois Knoll},
    year={2020},
    eprint={2007.00330},
    archivePrefix={arXiv},
    primaryClass={cs.RO}
}
```
