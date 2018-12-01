package(default_visibility = ["//visibility:public"])

load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
load("@mxebzl//tools/windows:rules.bzl", "pkg_winzip")

config_setting(
    name = "windows",
    values = {
        "crosstool_top": "@mxebzl//tools/windows:toolchain",
    }
)

cc_binary(
    name = "trolleys",
    data = ["//content"],
    linkopts = select({
        ":windows": ["-mwindows", "-lSDL2main" ],
        "//conditions:default": [],
    }) + [
        "-lSDL2",
        "-lSDL2_image",
        "-lSDL2_mixer",
        "-static-libstdc++",
        "-static-libgcc",
    ],
    srcs = ["main.cc"],
    deps = [
        "@libgam//:game",
        ":screens",
    ],
)

pkg_winzip(
    name = "trolleys-windows",
    files = [
        ":trolleys",
        "//content",
    ]
)

pkg_tar(
    name = "trolleys-linux",
    extension = "tar.gz",
    strip_prefix = "/",
    package_dir = "trolleys/",
    srcs = [
        ":trolleys",
        "//content",
    ],
)

cc_library(
    name = "screens",
    srcs = [
        "title_screen.cc",
        "game_screen.cc",
    ],
    hdrs = [
        "title_screen.h",
        "game_screen.h",
    ],
    deps = [
        "@libgam//:backdrop",
        "@libgam//:screen",
        "@libgam//:sprite",
        "@libgam//:spritemap",
        "@libgam//:text",
        ":map",
        ":train",
    ],
)

cc_library(
    name = "switch",
    srcs = [
        "switch.cc",
    ],
    hdrs = [
        "switch.h",
    ],
    deps = [
        "@libgam//:graphics",
        "@libgam//:spritemap",
    ],
)

cc_library(
    name = "map",
    srcs = [
        "map.cc",
    ],
    hdrs = [
        "map.h",
    ],
    deps = [
        "@libgam//:graphics",
        "@libgam//:spritemap",
        "@libgam//:util",
        ":switch",
    ],
);

cc_library(
    name = "train",
    srcs = [
        "train.cc",
    ],
    hdrs = [
        "train.h",
    ],
    deps = [
        "@libgam//:graphics",
        "@libgam//:spritemap",
        "@libgam//:util",
        ":map",
    ],
);
