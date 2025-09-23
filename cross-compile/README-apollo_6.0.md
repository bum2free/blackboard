# Prequest
Put this repository to folder:
/apollo/cyber/examples

# Build Script
## Add to /apollo/WORKSPACE
```
local_repository(
    name = "blackboard",
    path = "cyber/examples/blackboard",
)
```
## Add to /apollo/cyber/examples/BUILD
```
cc_binary(
    name = "test_blackboard_cyber_docker",
    srcs = [
        "blackboard/compare_test/srcs/main.cpp",
        "blackboard/compare_test/srcs/payload.cpp",
        "blackboard/compare_test/srcs/cpu_load.cpp",
        "blackboard/compare_test/srcs/writer.cpp",
        "blackboard/compare_test/srcs/reader.cpp",
        "blackboard/compare_test/srcs/statistics.cpp",
        "blackboard/compare_test/include/auto_test/cpu_load.h",
        "blackboard/compare_test/include/auto_test/payload.h",
        "blackboard/compare_test/include/auto_test/reader.h",
        "blackboard/compare_test/include/auto_test/statistics.h",
        "blackboard/compare_test/include/auto_test/writer.h",
        "blackboard/compare_test/include/json/json.hpp",
        "blackboard/compare_test/cyber/main.cpp",
        "blackboard/compare_test/cyber/writer.cpp",
        "blackboard/compare_test/cyber/reader.cpp",
        "blackboard/compare_test/cyber/writer.h",
        "blackboard/compare_test/cyber/reader.h",
    ],
    # Translate target_include_directories:
    # Using 'includes' so headers can be included directly (<> form).
    includes = [
        "blackboard/compare_test/include",
        "blackboard/compare_test/include/json",
        "blackboard/compare_test",
    ],
    deps = [
        "@blackboard//pt-dummy:pt_dummy",
        "@blackboard//pt-shared_any:pt_shared_any",
        "@blackboard//pt-intrusive_variant:pt_intrusive_variant",
        "//cyber",
        "//cyber/parameter",
        "//cyber/examples/proto:examples_cc_proto",
    ],
    # CMake had -DUSE_THREAD_TIMER and C++17.
    copts = [
        "-DUSE_THREAD_TIMER",
        "-std=c++17",
        "-DENABLE_CYBER_BLACKBOARD",
    ],
    visibility = ["//visibility:public"],
)
```
