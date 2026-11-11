Name:           maze-generator
Version:        0.1.0
Release:        alt1
Summary:        Four versions of a random maze generator built with sequential patches
Group:          Games/Other
License:        Other
Packager:       headshog <craaaaaachind@gmail.com>
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/09_PatchDiff
Source:         %name-%version.tar.gz
Patch0:         0001-size.patch
Patch1:         0002-symbols.patch
Patch2:         0003-seed.patch

BuildRequires:  gcc make glibc-devel patch python3

%description
The package contains a fixed-size maze generator and three versions with
command-line options for size, display symbols and a random seed.
Every maze is connected and has a unique path between any two rooms.

%prep
%setup -q -n 09_PatchDiff
cp %{PATCH0} %{PATCH1} %{PATCH2} .

%build
%make_build CFLAGS="%optflags -std=c11 -Wall -Wextra -Wpedantic"

%check
make test

%install
make install DESTDIR="%buildroot" PREFIX="%_prefix" bindir="%_bindir"

%files
%doc README.md
%_bindir/maze
%_bindir/maze-size
%_bindir/maze-symbols
%_bindir/maze-seed

%changelog
* Tue Nov 10 2026 headshog <craaaaaachind@gmail.com> 0.1.0-alt1
- Initial package.
