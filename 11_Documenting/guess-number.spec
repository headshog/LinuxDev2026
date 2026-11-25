Name:           guess-number
Version:        0.2.0
Release:        alt1
Summary:        Localized number guessing game with Roman numerals and documentation
Group:          Games/Other
License:        Other
Packager:       headshog <craaaaaachind@gmail.com>
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/11_Documenting
Source:         %name-%version.tar.gz

BuildRequires:  gcc make glibc-devel autoconf automake gettext gettext-tools
BuildRequires:  doxygen help2man graphviz fonts-ttf-freefont python3

%description
guess-number guesses an integer in 1..3999 using binary search. It supports
decimal and Roman notation and English/Russian interfaces. The package
includes an English manual and generated Doxygen HTML documentation.

%prep
%setup -q

%build
%autoreconf
%configure --docdir=%_docdir/%name-%version
%make_build

%check
%make_build check

%install
%makeinstall_std
install -m 644 README.md "%buildroot%_docdir/%name-%version/README.md"

%files
%doc %_docdir/%name-%version/
%_bindir/%name
%_datadir/locale/ru/LC_MESSAGES/%name.mo
%_mandir/man1/%name.1*

%changelog
* Wed Nov 25 2026 headshog <craaaaaachind@gmail.com> 0.2.0-alt1
- Add Roman numeral conversion for 1..3999 and a Roman game mode.
- Generate HTML and the manual from documented sources and English help.
