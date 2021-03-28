project('scrot', 'c',
        version : '0.9.0',
        license : 'MIT-feh',
        default_options : ['c_std=c11', 'b_ndebug=if-release'],
        meson_version : '>= 0.49.0')

add_global_arguments('-D_POSIX_C_SOURCE=200809L', language : 'c')

configure_file(output : 'config.h',
  configuration : {
    'PACKAGE' : '"@0@"'.format(meson.project_name()),
    'VERSION' : '"@0@"'.format(meson.project_version()),
  }
)

giblib_src = files(
  'giblib/gib_btree.c',
  'giblib/gib_hash.c',
  'giblib/gib_imlib.c',
  'giblib/gib_list.c',
  'giblib/gib_queue.c',
  'giblib/gib_stack.c',
  'giblib/gib_style.c',
  'giblib/gib_utils.c',
)

scrot_src = files(
  'src/imlib.c',
  'src/main.c',
  'src/options.c',
)

imlib2_dep = dependency('imlib2')
x11_dep = dependency('x11')
xfixes_dep = dependency('xfixes')

giblib = static_library('giblib', giblib_src,
                        include_directories : '.',
                        dependencies : imlib2_dep)

giblib_dep = declare_dependency(link_with : giblib)

executable('scrot', scrot_src,
           dependencies : [giblib_dep, x11_dep, xfixes_dep],
           include_directories : '.',
           install : true)

data_dir = get_option('datadir')

install_man('scrot.1')
install_data('COPYING',   install_dir : data_dir / 'licenses' / 'scrot')
install_data('AUTHORS',   install_dir : data_dir / 'doc' / 'scrot')
install_data('README.md', install_dir : data_dir / 'doc' / 'scrot')
install_data('ChangeLog', install_dir : data_dir / 'doc' / 'scrot')
