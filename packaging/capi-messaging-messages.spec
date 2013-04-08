Name:       capi-messaging-messages
Summary:    A SMS/MMS library in Tizen Native API
Version: 0.1.25
Release:    1
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(msg-service)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-appfw-application)
Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig

%description


%package devel
Summary:  A SMS/MMS library in Tizen Native API (Development)
Requires: %{name} = %{version}-%{release}
Requires:  pkgconfig(msg-service)
Requires:  pkgconfig(capi-base-common)

%description devel



%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
rm -fr cmake_build_tmp; mkdir cmake_build_tmp
cd cmake_build_tmp
%cmake .. -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install

mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

cd cmake_build_tmp
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-messaging-messages.so.*
%{_datadir}/license/%{name}

%files devel
%{_includedir}/messaging/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-messaging-messages.so
