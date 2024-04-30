
srcs = files(
    'bitboard.cpp',
    'board.cpp',
    'color.cpp',
    'coords.cpp',
    'hash.cpp',
    'main.cpp',
    'move.cpp',
    'movelist.cpp',
    'perft.cpp',
    'pgn.cpp',
    'piece.cpp',
    'san.cpp',
)

e = executable(
    'tests',
    cpp_args: [ '-std=c++17', '-g3', '-fno-omit-frame-pointer'],
    sources: srcs,
    dependencies: [],
    link_args: [ '-g3', '-fno-omit-frame-pointer'],
)

test(
    'chess-library-tests',
    e,
    timeout: 0,
    verbose: true,
    workdir: meson.project_source_root(),
)
