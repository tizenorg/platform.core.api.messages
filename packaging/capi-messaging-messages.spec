Name:       capi-messaging-messages
Summary:    A SMS/MMS library in Tizen Native API
Version:    0.2.1
Release:    1
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(msg-service)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(privacy-manager-client)
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
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

cd cmake_build_tmp
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-messaging-messages.so.*
%{_datadir}/license/%{name}
#%{_bindir}/*

%files devel
%{_includedir}/messaging/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-messaging-messages.so


