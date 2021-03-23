Name:    scrot
Version: 0.9.0
Release: 1%{?dist}
Summary: Simple command-line screenshot utility for X
License: MIT
Url:     https://github.com/dreamer/scrot/
Source:  https://github.com/dreamer/scrot/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc
BuildRequires: meson
BuildRequires: imlib2-devel
BuildRequires: libX11-devel

%description
A nice and straightforward screen capture utility implementing the dynamic
loaders of imlib2.

%prep
%autosetup

%build
%meson
%meson_build

%install
%meson_install

%files
%license COPYING
%doc AUTHORS ChangeLog
%{_bindir}/*
%{_mandir}/man1/*

%changelog
* Tue Mar 23 2021 Patryk Obara (pbo) <dreamer.tan@gmail.com>
- 0.9.0-1
- Rewrite the spec file for new buildsystem and modern rpm

* Thu Oct 26 2000 Tom Gilbert <tom@linuxbrit.co.uk>
- 0.8-1
- Created spec file
