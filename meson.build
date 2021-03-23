project('scrot', 'c',
        version : '0.8.0',
        license : 'MIT-feh',
        default_options : ['b_ndebug=if-release'],
        meson_version : '>= 0.51.0')

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

giblib = static_library('giblib', giblib_src,
                        include_directories : '.',
                        dependencies : imlib2_dep)

giblib_dep = declare_dependency(link_with : giblib)

executable('scrot', scrot_src,
           dependencies : [giblib_dep, x11_dep],
           include_directories : '.',
           install : true)

data_dir = get_option('datadir')

install_man('scrot.1')
install_data('COPYING', install_dir : data_dir / 'licenses' / 'scrot')
