%def_with readline

Name:           rhasher
Version:        0.1.0
Release:        alt1
Summary:        Interactive calculator of file and string hashes based on librhash
Group:          File tools
License:        Other
Packager:       headshog <craaaaaachind@gmail.com>
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/07_Environmental
Source:         %name-%version.tar.gz

BuildRequires:  librhash-devel
%if_with readline
BuildRequires:  libreadline-devel
%endif

%description
rhasher reads commands like "MD5 file" or "sha1 "string" and prints the
hash of the file or the string. An algorithm name starting with a capital
letter gives hexadecimal output, a lowercase one gives Base64. MD5, SHA1,
TTH and several other algorithms of librhash are supported. Commands are
read with GNU readline if the package is built with it.

%prep
%setup -q

%build
%configure %{subst_with readline}
%make_build

%install
%makeinstall_std

%check
%make_build check

%files
%_bindir/rhasher

%changelog
* Tue Oct 27 2026 headshog <craaaaaachind@gmail.com> 0.1.0-alt1
- Initial package.
