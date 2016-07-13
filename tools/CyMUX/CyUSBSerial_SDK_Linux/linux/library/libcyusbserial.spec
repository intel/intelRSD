#
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

# norootforbuild

Name:      libcyusbserial
Version:   1.0
Release:   0
Summary:   Cypress USB to I2C library
Group:     System Environment/Libraries
Requires:  libusbx >= 1.0
License:   Cypress Semiconductor
Source0:   %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build
BuildArch: x86_64
Vendor:    Cypress Semiconductor Corporation / ATR-LABS


%define owner root
%define lib_dir /usr/lib64
%define install_lib_dir $RPM_BUILD_ROOT/%{lib_dir}
%define udev_dir /etc/udev/rules.d
%define install_udev_dir $RPM_BUILD_ROOT/%{udev_dir}


%description
Cypress USB to I2C library


%pre
%post
%preun

%postun
if [ $1 -eq 0 ]; then # Full uninstallation
    ldconfig
fi
exit 0

%prep
%build


%install
##############
# udev support
##############
mkdir -p %{install_udev_dir}

install -m 644 libcyusbserial/90-cyusb.rules %{install_udev_dir}

#################
# Library support
#################
mkdir -p %{install_lib_dir}

install -m 755 libcyusbserial/libcyusbserial.so.1 %{install_lib_dir}
ln -s libcyusbserial.so.1 %{install_lib_dir}/libcyusbserial.so


%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{_tmppath}/%{name}


%files

# Set permission on files dedicated to root
%attr(755, root, root) %{lib_dir}/libcyusbserial.so.1
%attr(755, root, root) %{lib_dir}/libcyusbserial.so
%attr(644, root, root) %{udev_dir}/90-cyusb.rules


%changelog
* Tue Dec  1 2015 - 1.0
- Intitial RPM creation from existing Cypress library SDK


%global __os_install_post %{nil}
%global _enable_debug_package 0
%global debug_package %{nil}
