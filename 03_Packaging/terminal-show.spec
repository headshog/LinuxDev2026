Name:           terminal-show
Version:        0.0.1
Release:        alt1
Summary:        A simple ncurses text file viewer
Group:          Text tools
License:        Other
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/01_TerminalProject
Source:         %name-%version.tar.gz

BuildRequires:  gcc make glibc-devel pkg-config libncurses-devel libncursesw-devel

%description
Show displays a text file in a framed ncurses window. Press Space to scroll
one line down or Escape to exit.

%prep
%setup -q -n 01_TerminalProject

%build
%make_build CFLAGS="%optflags -std=c11 -Wall -Wextra -Wpedantic"

%install
make install DESTDIR="%buildroot" PREFIX="%_prefix" bindir="%_bindir"

%files
%_bindir/Show
