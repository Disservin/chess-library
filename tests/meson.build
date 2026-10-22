
test_files = [
    ['bitboard', 'bitboard.cpp'],
    ['board', 'board.cpp'],
    ['color', 'color.cpp'],
    ['coords', 'coords.cpp'],
    ['hash', 'hash.cpp'],
    ['legal', 'legal.cpp'],
    ['move', 'move.cpp'],
    ['movelist', 'movelist.cpp'],
    ['perft', 'perft.cpp'],
    ['pgn', 'pgn.cpp'],
    ['piece', 'piece.cpp'],
    ['san', 'san.cpp'],
    ['uci', 'uci.cpp'],
]

foreach test_file : test_files
    executable_name = test_file[0]
    e = executable(
        executable_name,
        cpp_args: [ '-std=c++17', '-g3', '-fno-omit-frame-pointer'],
        sources: [test_file[1], 'main.cpp'],
        dependencies: [],
        link_args: [ '-g3', '-fno-omit-frame-pointer'],
    )

    test(
        executable_name,
        e,
        timeout: 0,
        verbose: true,
        workdir: meson.project_source_root(),
    )
endforeach
