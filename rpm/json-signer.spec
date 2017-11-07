%global commit0 31af8c8f3f4d80d9389b9a0d20ce8af0e90db60d
%global shortcommit0 %(c=%{commit0}; echo ${c:0:7})

Name:           json-signer
Version:        1.0.0
Release:        0.4%{?dist}
Summary:        JSON Signer written in C

License:        MIT
URL:            https://github.com/fkooman/json-signer
Source0:        https://github.com/fkooman/json-signer/archive/%{commit0}.tar.gz#/%{name}-%{shortcommit0}.tar.gz

BuildRequires:  meson
BuildRequires:  libsodium-devel
BuildRequires:  json-c-devel
BuildRequires:  libxdg-basedir-devel

%description
EXPERIMENTAL reimplementation of php-json-signer in C.

%prep
%autosetup -n json-signer-%{commit0}

%build
%meson
%meson_build

%install
%meson_install

%files
%{_bindir}/*
%license LICENSE
%doc README.md

%changelog
* Tue Nov 07 2017 François Kooman <fkooman@tuxed.net> - 1.0.0-0.4
- rebuilt

* Wed Nov 01 2017 François Kooman <fkooman@tuxed.net> - 1.0.0-0.3
- rebuilt

* Tue Oct 31 2017 François Kooman <fkooman@tuxed.net> - 1.0.0-0.2
- rebuilt

* Tue Oct 31 2017 François Kooman <fkooman@tuxed.net> - 1.0.0-0.1
- initial package
