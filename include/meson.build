custom_target(
    'create_shl',
    command: [
        python,
        '../tools/shl.py',
        '../src/include.hpp',
        '--header_search_paths', '../src/',
        '-o', '../include/chess.hpp',
    ],
    output: 'chess.hpp',
    build_by_default: true,
)
