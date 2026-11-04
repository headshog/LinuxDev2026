Name:           guess-number
Version:        0.1.0
Release:        alt1
Summary:        Guess a number using binary search, with Russian localization
Group:          Games/Other
License:        Other
Packager:       headshog <craaaaaachind@gmail.com>
URL:            https://uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/08_I18n
Source:         %name-%version.tar.gz

BuildRequires:  gcc make glibc-devel autoconf automake gettext gettext-tools python3

%description
guess-number guesses an integer between 1 and 100 by asking yes/no
questions. GNU gettext provides a Russian translation of the interface
and accepted answers. The program uses at most seven questions.

%prep
%setup -q

%build
%autoreconf
%configure
%make_build

%check
%make_build check

%install
%makeinstall_std

%files
%doc README.md
%_bindir/%name
%_datadir/locale/ru/LC_MESSAGES/%name.mo

%changelog
* Tue Nov 03 2026 headshog <craaaaaachind@gmail.com> 0.1.0-alt1
- Initial package.
