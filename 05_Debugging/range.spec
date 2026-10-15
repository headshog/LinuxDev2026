Name:           range
Version:        0.1.0
Release:        alt1
Summary:        An integer arithmetic progression generator
Group:          Text tools
License:        Other
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/05_Debugging
Source:         %name-%version.tar.gz

BuildRequires:  gcc make glibc-devel gdb diffutils grep

%description
range prints an arithmetic progression using one, two or three integer
arguments. The sources include GDB scripts and automated tests.

%prep
%setup -q -n 05_Debugging

%build
%make_build CFLAGS="%optflags -std=c11 -Wall -Wextra -Wpedantic -O0 -g"

%check
make test

%install
make install DESTDIR="%buildroot" PREFIX="%_prefix" bindir="%_bindir"

%files
%doc readme.md
%_bindir/range
