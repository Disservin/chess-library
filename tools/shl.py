#!/usr/bin/env python3
# Source: https://gist.github.com/martinfinke/c474ac992155f29f0f0d871ddd8cd99d

import argparse
import os
import re

# STL and C headers as of 04.12.2018, taken from cppreference.com
stl_headers = [
    "algorithm",
    "any",
    "array",
    "assert.h",
    "atomic",
    "bit",
    "bitset",
    "cassert",
    "ccomplex",
    "cctype",
    "cerrno",
    "cfenv",
    "cfloat",
    "charconv",
    "chrono",
    "cinttypes",
    "ciso646",
    "climits",
    "clocale",
    "cmath",
    "codecvt",
    "compare",
    "complex",
    "complex.h",
    "concepts",
    "condition_variable",
    "contract",
    "csetjmp",
    "csignal",
    "cstdalign",
    "cstdarg",
    "cstdbool",
    "cstddef",
    "cstdint",
    "cstdio",
    "cstdlib",
    "cstring",
    "ctgmath",
    "ctime",
    "ctype.h",
    "cuchar",
    "cwchar",
    "cwctype",
    "deque",
    "errno.h",
    "exception",
    "execution",
    "experimental/algorithm",
    "experimental/any",
    "experimental/array",
    "experimental/atomic",
    "experimental/barrier",
    "experimental/chrono",
    "experimental/deque",
    "experimental/exception_list",
    "experimental/execution_policy",
    "experimental/filesystem",
    "experimental/forward_list",
    "experimental/functional",
    "experimental/future",
    "experimental/iterator",
    "experimental/latch",
    "experimental/list",
    "experimental/map",
    "experimental/memory",
    "experimental/memory_resource",
    "experimental/numeric",
    "experimental/optional",
    "experimental/propagate_const",
    "experimental/random",
    "experimental/ranges/algorithm",
    "experimental/ranges/concepts",
    "experimental/ranges/functional",
    "experimental/ranges/iterator",
    "experimental/ranges/random",
    "experimental/ranges/range",
    "experimental/ranges/tuple",
    "experimental/ranges/type_traits",
    "experimental/ranges/utility",
    "experimental/ratio",
    "experimental/regex",
    "experimental/set",
    "experimental/source_location",
    "experimental/string",
    "experimental/string_view",
    "experimental/system_error",
    "experimental/tuple",
    "experimental/type_traits",
    "experimental/unordered_map",
    "experimental/unordered_set",
    "experimental/utility",
    "experimental/vector",
    "fenv.h",
    "filesystem",
    "float.h",
    "forward_list",
    "fstream",
    "functional",
    "future",
    "initializer_list",
    "inttypes.h",
    "iomanip",
    "ios",
    "iosfwd",
    "iostream",
    "iso646.h",
    "istream",
    "iterator",
    "limits",
    "limits.h",
    "list",
    "locale",
    "locale.h",
    "map",
    "math.h",
    "memory",
    "memory_resource",
    "mutex",
    "new",
    "numeric",
    "optional",
    "ostream",
    "queue",
    "random",
    "ratio",
    "regex",
    "scoped_allocator",
    "set",
    "setjmp.h",
    "shared_mutex",
    "signal.h",
    "span",
    "sstream",
    "stack",
    "stdalign.h",
    "stdarg.h",
    "stdatomic.h",
    "stdbool.h",
    "stddef.h",
    "stdexcept",
    "stdint.h",
    "stdio.h",
    "stdlib.h",
    "stdnoreturn.h",
    "streambuf",
    "string",
    "string.h",
    "string_view",
    "strstream",
    "syncstream",
    "system_error",
    "tgmath.h",
    "thread",
    "threads.h",
    "time.h",
    "tuple",
    "type_traits",
    "typeindex",
    "typeinfo",
    "uchar.h",
    "unordered_map",
    "unordered_set",
    "utility",
    "valarray",
    "variant",
    "vector",
    "version",
    "wchar.h",
    "wctype.h",
    "algorithm",
    "any",
    "array",
    "atomic",
    "barrier",
    "chrono",
    "deque",
    "exception_list",
    "execution_policy",
    "filesystem",
    "forward_list",
    "functional",
    "future",
    "iterator",
    "latch",
    "list",
    "map",
    "memory",
    "memory_resource",
    "numeric",
    "optional",
    "propagate_const",
    "random",
    "ranges/algorithm",
    "ranges/concepts",
    "ranges/functional",
    "ranges/iterator",
    "ranges/random",
    "ranges/range",
    "ranges/tuple",
    "ranges/type_traits",
    "ranges/utility",
    "ratio",
    "regex",
    "set",
    "source_location",
    "string",
    "string_view",
    "system_error",
    "tuple",
    "type_traits",
    "unordered_map",
    "unordered_set",
    "utility",
    "vector",
    "immintrin.h",
    "intrin.h",
]


class IncludeInliner:
    def __init__(self, header_search_paths, ignored_headers):
        self.header_search_paths = header_search_paths
        self.ignored_headers = ignored_headers
        self.included_stl_headers = []
        self.included_headers = []
        self.include_regex = re.compile('^\s*#(?:include|import)\s+["<]([^">]+)[">]')
        self.pragma_once_regex = re.compile("^\s*#pragma\s+once")
        # self.has_pragma_once = False
        self.output_lines = []

    def run(self, full_header_path):
        if not full_header_path:
            raise Exception("Header not found: " + "")

        with open(full_header_path, "r") as header:
            for lineIndex, line in enumerate(header):
                lineInfo = full_header_path + ":" + str(lineIndex + 1) + ": "

                # Check if line is a #pragma once
                result = self.pragma_once_regex.match(line)
                if result:
                    print(lineInfo + "Removing unneeded #pragma once")
                    continue
                # if self.has_pragma_once:
                #     print(lineInfo + "Removing unneeded #pragma once")
                #     continue

                # self.output_lines.append(line)
                # self.has_pragma_once = True
                # continue

                # Check if line is an #include
                result = self.include_regex.match(line)
                if not result:
                    self.output_lines.append(line)
                    continue

                path = result.group(1)

                # Remove ignored headers
                if path in self.ignored_headers:
                    print(lineInfo + "Removing ignored #include: " + path)
                    continue

                # Don't touch STL headers
                if path in stl_headers:
                    if not path in self.included_stl_headers:
                        print(lineInfo + "Keeping STL #include: " + path)
                        self.output_lines.append(line)
                        self.included_stl_headers.append(path)

                    continue

                full_include_path = self.get_full_header_path(
                    IncludeInliner._get_parent_folder(full_header_path), path
                )
                if not full_include_path in self.included_headers:
                    print(lineInfo + "Inlining #include: " + path)
                    self.run(full_include_path)
                    self.included_headers.append(full_include_path)

    def get_output(self):
        return "".join(self.output_lines)

    def get_full_header_path(self, cwd, header_file_path):
        def is_valid(header_search_path):
            full_path = os.path.join(header_search_path, header_file_path)
            if os.path.isfile(full_path):
                return full_path

        relative_to_cwd = is_valid(cwd)
        if relative_to_cwd:
            return relative_to_cwd

        return next(filter(lambda x: x, map(is_valid, self.header_search_paths)), None)

    @staticmethod
    def _get_parent_folder(path):
        return os.path.abspath(os.path.join(path, os.pardir))


def main():
    parser = argparse.ArgumentParser(description="Process some integers.")
    parser.add_argument(
        "main_header", help="The main header file", type=argparse.FileType("r")
    )
    parser.add_argument(
        "--ignored_headers",
        help="A list of header files whose #include should be removed (without inlining them)",
        nargs="*",
        default=[],
    )
    parser.add_argument(
        "--header_search_paths", help="A list of header search paths", nargs="*"
    )
    parser.add_argument(
        "-o",
        "--output_file",
        help="The single-header output file",
        required=True,
        type=argparse.FileType("w"),
    )
    args = parser.parse_args()
    inliner = IncludeInliner(args.header_search_paths, args.ignored_headers)
    inliner.run(inliner.get_full_header_path(os.getcwd(), args.main_header.name))
    args.output_file.write(inliner.get_output())


if __name__ == "__main__":
    main()
