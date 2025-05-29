benchmark_files = [
  'pgn_benchmark.cpp',
  'getfen_benchmark.cpp',
  'perft_benchmark.cpp',
]

foreach bench_file : benchmark_files
  bench_name = bench_file.split('.')[0]
  
  executable(bench_name,
    sources : [bench_file],
    cpp_args: [ '-std=c++17', '-g3', '-O3', '-fno-omit-frame-pointer', '-march=native'],
    dependencies : [],
    install : true,
    install_dir : 'bin/benchmarks')
endforeach