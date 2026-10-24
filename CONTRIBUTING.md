# Contributing

Open an issue or pull request in this repository on GitHub. Describe the problem,
proposed behavior and validation; keep unrelated changes separate. Preserve public
APIs and native host/thread ownership, and add focused tests for behavior changes.

Use a C++23 module-capable compiler and the CMake version required by CMakeLists.txt. Configure a separate
build directory with BUILD_TESTING=ON, build Debug and Release, and run CTest.
Follow README for the relevant library/example dependency graph. Report platform
coverage and distinguish compilation, CPU tests and interactive runtime evidence.
Never include credentials, generated caches or unrelated local files.

Submit only work you have authority to license. Preserve third-party notices and
identify borrowed material and its exact license/provenance. Do not submit private
engine self-coding source, credentials, or unlicensed code found elsewhere.
Contribution acceptance requires reviewed permission for the maintainer's use and
relicensing; discuss that agreement with the maintainer before merging. A pull
request alone does not prove ownership or remove third-party obligations.
