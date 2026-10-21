Name:           safemove
Version:        0.1.0
Release:        alt1
Summary:        File moving utility with an LD_PRELOAD protection library
Group:          File tools
License:        Other
Packager:       headshog <craaaaaachind@gmail.com>
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/06_Tracing
Source:         %name-%version.tar.gz

BuildRequires:  gcc make glibc-devel strace diffutils coreutils grep sed
BuildRequires:  /proc

%description
move copies a regular file to its destination and removes the source only
after successfully syncing and closing the destination. safemove loads a
library which refuses to unlink files with PROTECT in their basename.

%prep
%setup -q -n 06_Tracing

%build
%make_build CFLAGS="%optflags -std=c11 -Wall -Wextra -Wpedantic"

%check
make test

%install
make install DESTDIR="%buildroot" PREFIX="%_prefix" \
    bindir="%_bindir" libdir="%_libdir"

%files
%doc README.md
%_bindir/move
%_bindir/safemove
%dir %_libdir/safemove
%_libdir/safemove/libprotect.so

%changelog
* Wed Oct 21 2026 headshog <craaaaaachind@gmail.com> 0.1.0-alt1
- Initial package.
