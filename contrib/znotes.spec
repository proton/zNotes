# norootforbuild

%if 0%{?fedora}
	%define breq qt4-devel
	%define qmake /usr/bin/qmake-qt4
	%define lrelease /usr/bin/lrelease-qt4
%endif  
%if 0%{?mandriva_version}
	%define breq libqt4-devel, qt4-linguist
	%define qmake /usr/lib/qt4/bin/qmake
	%define lrelease /usr/lib/qt4/bin/lrelease
%endif
%if 0%{?suse_version}
	%define breq update-desktop-files, libqt4-devel
	%define qmake /usr/bin/qmake
	%define lrelease /usr/bin/lrelease
%endif  

Name:		znotes
Version:	0.4.1
Release:	0
License:	GPL
Vendor:		Peter Savichev <psavichev@gmail.com>
Packager:	TI_Eugene <ti.eugene@gmail.com>
Source:		%{name}_%{version}.tar.gz
Summary:	Simple notes
Group:		Office
BuildRequires:	gcc-c++, %{breq}
BuildRoot:	%{_tmppath}/%{name}-%{version}-build
Prefix:		/usr

%description
Simple notes.
When I migrated from XFCE, I could not find a program simular to xfce4-Notes-plugin, so I created this program.

%if 0%{?mandriva_version} == 0
	%debug_package
%endif

%prep
%setup -q

%build
%{lrelease} %{name}.pro
%{qmake}
make

%install
%{__rm} -rf %{buildroot}
%{makeinstall} INSTALL_ROOT=%{buildroot}
%if 0%{?suse_version}
	%suse_update_desktop_file %{name}
%endif

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/%{name}

%changelog
* Sun Feb 14 2010 TI_Eugene <ti.eugene@gmail.com>
- 0.4.1

* Mon Feb 01 2010 TI_Eugene <ti.eugene@gmail.com>
- 0.4.0

* Sun Oct 25 2009 TI_Eugene <ti.eugene@gmail.com>
- initial package in OBS
