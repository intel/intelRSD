# Copyright (c) 2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

# norootforbuild

Name:      cyMUX
Version:   1.07
Release:   0
Summary:   cyMUX and cytool used for accessing Cypress part
Group:     Applications/Engineering
Requires:  systemd >= 208, libcyusbserial
License:   Apache 2.0
Source0:   %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build
BuildArch: x86_64
Vendor:    Intel Corporation


%define owner root
%define bin_dir /usr/bin
%define install_bin_dir $RPM_BUILD_ROOT/%{bin_dir}
%define sbin_dir /usr/sbin
%define install_sbin_dir $RPM_BUILD_ROOT/%{sbin_dir}
%define systemd_dir /usr/lib/systemd/system
%define install_systemd_dir $RPM_BUILD_ROOT/%{systemd_dir}


%description
software IPMB multiplexer for access to the Cypress USB to I2C driver


%pre
# Stop the service if it exists (shouldn't exist with new
# installation, but the existence of the file should handle
# that)
if [ -e %{systemd_dir}/cyreceive.service ]; then
    # stop old service
    systemctl stop cyreceive
    systemctl disable cyreceive
fi
if [ -e %{systemd_dir}/cyMUX.service ]; then
   systemctl stop cyMUX
fi
exit 0


%post
if [ "$1" -eq 1 ]; then # new installation
    systemctl enable cyMUX.service
elif [ "$1" -eq 2 ]; then # upgrade
    systemctl daemon-reload
fi
# Always start the service after installation or upgrade
systemctl start cyMUX.service
exit 0


%preun
if [ $1 -eq 0 ]; then # Full uninstallation, only (do nothing if upgrade)
    systemctl stop cyMUX
   if [ -e /etc/systemd/system/multi-user.target.wants/cyMUX.service ]; then
      systemctl disable cyMUX
   fi
fi
exit 0


%postun
%prep
%build
%install
######################
# cyMUX daemon support
######################
mkdir -p %{install_systemd_dir} %{install_sbin_dir}
install -m 644 cyMUX/cyMUX.service %{install_systemd_dir}
install -m 644 cyMUX/cyMUX %{install_sbin_dir}

#############
# Core Binary
#############
mkdir -p %{install_bin_dir}
install -m 755 cyMUX/cytool %{install_bin_dir}


%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{_tmppath}/%{name}


%files
# Set permission on files dedicated to root
%attr(644, root, root) %{systemd_dir}/cyMUX.service
%attr(540, root, root) %{sbin_dir}/cyMUX
%attr(555, root, root) %{bin_dir}/cytool


%changelog
* Mon Jun 27 2016 - 1.07-0
- Added ability to change logging level of cyMUX without stopping and restarting
- Minor changes to dual build under Fedora and Ubuntu
- Directory rename for source release to public

* Fri Jun 17 2016 - 1.06-0
- Added time out management to responder connection
- Added missing flushing of the responder queue when responder connection is dropped
- Added more diagnostic logging to monitor queue management
- Bumped urgency of several messages (INFO -> WARNING, WARNING -> ERR)
- Reordered start up sequence of threads to handle immediate i2c flood of messages upon start up

* Fri Jun 10 2016 - 1.05-0
- Handle invalid zero length I2C packets received by logging and dropping them.

* Thu Mar 10 2016 - 1.04-0
- Refactored most asserts to errors to the console with an exit
- Removed closing of the standard in/out/err to allow systemd to log
- Added 'ping' to current responder when new responder request is made and resource is allocated

* Mon Nov 17 2014 - 0.11-0
- Intitial RPM creation


%global __os_install_post %{nil}
%global _enable_debug_package 0
%global debug_package %{nil}
