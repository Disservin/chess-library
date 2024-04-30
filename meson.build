project(
    'chess-library',
    'cpp',
    default_options: [
        'warning_level=3',
        'buildtype=release',
    ],
)

# Compiler and Linker Flags
add_global_link_arguments('-flto', language: 'cpp')


# Create SHL
python = find_program('python3', 'python')

subdir('include')
subdir('tests')
