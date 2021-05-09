src_inc = include_directories('.')

test_sources = [
    'tree-printer.c',
    'utils.c',
    '../src/vnr-file.c',
    '../src/vnr-tree.c',
]

test('tree-singlefile',        executable('test-tree-singlefile',        test_sources + ['test-tree-singlefile.c'],        dependencies: viewnior_deps))
test('tree-folder',            executable('test-tree-folder',            test_sources + ['test-tree-folder.c'],            dependencies: viewnior_deps))
test('tree-urilist',           executable('test-tree-urilist',           test_sources + ['test-tree-urilist.c'],           dependencies: viewnior_deps))
test('tree-next-nofiles',      executable('test-tree-next-nofiles',      test_sources + ['test-tree-next-nofiles.c'],      dependencies: viewnior_deps))
test('tree-next-iteration',    executable('test-tree-next-iteration',    test_sources + ['test-tree-next-iteration.c'],    dependencies: viewnior_deps))
test('tree-getchildindir',     executable('test-tree-getchildindir',     test_sources + ['test-tree-getchildindir.c'],     dependencies: viewnior_deps))
test('tree-addnode',           executable('test-tree-addnode',           test_sources + ['test-tree-addnode.c'],           dependencies: viewnior_deps))
test('tree-numberofleaves',    executable('test-tree-numberofleaves',    test_sources + ['test-tree-numberofleaves.c'],    dependencies: viewnior_deps))

test('filemon-create',         executable('test-filemon-create',         test_sources + ['test-filemon-create.c'],         dependencies: viewnior_deps))
test('filemon-urilist-create', executable('test-filemon-urilist-create', test_sources + ['test-filemon-urilist-create.c'], dependencies: viewnior_deps))
test('filemon-delete',         executable('test-filemon-delete',         test_sources + ['test-filemon-delete.c'],         dependencies: viewnior_deps))
test('filemon-urilist-delete', executable('test-filemon-urilist-delete', test_sources + ['test-filemon-urilist-delete.c'], dependencies: viewnior_deps))
test('filemon-move',           executable('test-filemon-move',           test_sources + ['test-filemon-move.c'],           dependencies: viewnior_deps))
