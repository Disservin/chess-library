project(
    'chess-library',
    'cpp',
    default_options: [
        'warning_level=3',
        'buildtype=release',
        'b_ndebug=true',
    ],
)

# Compiler and Linker Flags
add_global_link_arguments('-flto', language: 'cpp')

executable(
    'example',
    sources: './main.cpp',
    c_args: [ '-march=native'],
    cpp_args: [ '-std=c++17', '-march=native', '-g3', '-fno-omit-frame-pointer'],
    link_args: [ '-g3', '-fno-omit-frame-pointer'],
)
