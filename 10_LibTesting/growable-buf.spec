Name:           growable-buf
Version:        0.1.0
Release:        alt1
Summary:        Growable memory buffers for C99
Group:          System/Libraries
License:        Unlicense
Packager:       headshog <craaaaaachind@gmail.com>
URL:            https://github.com/skeeto/growable-buf
Source:         %name-%version.tar.gz

BuildRequires:  gcc make glibc-devel autoconf automake libtool pkg-config libcheck-devel

%description
Shared-library adaptation of skeeto/growable-buf. Buffer operations remain
macros in buf.h and allocation is implemented by a single library function.

%package -n libgrowablebuf0
Summary:        Runtime library for growable C99 buffers
Group:          System/Libraries

%description -n libgrowablebuf0
Versioned shared library containing the buffer allocation function.

%package -n libgrowablebuf-devel
Summary:        Development files for growable C99 buffers
Group:          Development/C
Requires:       libgrowablebuf0 = %version-%release

%description -n libgrowablebuf-devel
Public macros and declarations, the linker symlink and pkg-config metadata
for building applications with libgrowablebuf.

%prep
%setup -q

%build
%autoreconf
%configure --disable-static --disable-coverage
%make_build

%install
%makeinstall_std
rm -f "%buildroot%_libdir/libgrowablebuf.la"

%check
%make_build check

%files -n libgrowablebuf0
%doc UNLICENSE README.md
%_libdir/libgrowablebuf.so.0*

%files -n libgrowablebuf-devel
%_includedir/growable-buf/
%_libdir/libgrowablebuf.so
%_libdir/pkgconfig/growable-buf.pc

%changelog
* Wed Nov 18 2026 headshog <craaaaaachind@gmail.com> 0.1.0-alt1
- Split the upstream header into a shared library and public macros.
- Add Check unit tests and optional gcov instrumentation.
