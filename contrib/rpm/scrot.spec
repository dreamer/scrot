%define name	scrot
%define ver	0.8
%define RELEASE 1
#%define rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define prefix	/usr

Name: %name
Summary:    Screen-shot capture using Imlib 2
Version: 	%ver
Release: 	%rel
License:        MIT
Group:		Applications/Graphics
Source: 	%{name}-%{ver}.tar.gz
Url: 		http://www.linuxbrit.co.uk
BuildRoot:	/var/tmp/%{name}-%{ver}-root
Docdir: 	%{prefix}/doc

%description
A nice and straightforward screen capture utility implementing the dynamic
loaders of imlib2.

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix}

make CFLAGS="$RPM_OPT_FLAGS"

%install
rm -fr $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

%files
%defattr(-, root, root)
%{prefix}/bin/*
%{prefix}/man/man1/*

%doc AUTHORS ChangeLog README TODO

%changelog
* Thu Oct 26 2000 Tom Gilbert <tom@linuxbrit.co.uk>
- created spec file

