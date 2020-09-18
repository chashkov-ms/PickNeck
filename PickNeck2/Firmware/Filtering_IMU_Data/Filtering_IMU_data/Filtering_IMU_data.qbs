import qbs

CppApplication {
    consoleApplication: true
    files: [
        "filter.c",
        "filter.h",
        "main.cpp",
        "quaternion.c",
        "quaternion.h",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}
