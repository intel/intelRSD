Summary: Package for RackScale PSME storage
Name: psme-storage
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ libsysfs lvm2-libs >= 2.02.111 scsi-target-utils
Source: %{expand:%%(pwd)}
BuildRoot: %{_topdir}/BUILD/%{name}-%{version}

%description
%{summary}

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/etc/psme
cd $RPM_BUILD_ROOT
cp -p %{SOURCEURL0}%{_bindir}/* .%{_bindir}
cp -p %{SOURCEURL0}/etc/psme/psme-storage-configuration.json ./etc/psme/

%pre
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-storage-configuration.json ]; then
		cp /etc/psme/psme-storage-configuration.json /etc/psme/psme-storage-configuration.json.old
	fi
else
	exit 0
fi

%post
cat <<EOF >/lib/systemd/system/psme-storage.service
[Unit]
Description=Managing PSME storage
After=network.target network-online.target

[Service]
TimeoutStartSec=60
Type=simple
ExecStartPre=/usr/bin/bash -c "while ! tgtadm --op show --mode sys --name State | grep -q 'State: ready' ; do sleep 1 ; done"
ExecStart=/usr/bin/psme-storage /etc/psme/psme-storage-configuration.json
PIDFile=/var/run/psme-storage.pid

[Install]
WantedBy=multi-user.target
EOF
if [ ! -f /etc/systemd/system/psme-storage.service ]; then
    ln -s /lib/systemd/system/psme-storage.service /etc/systemd/system/psme-storage.service
fi
systemctl daemon-reload
systemctl enable psme-storage

%preun
systemctl stop psme-storage

%postun
if [ "$1" == 0 ]; then
	systemctl disable psme-storage
	if [ -f /etc/systemd/system/psme-storage.service ]; then
	    rm /etc/systemd/system/psme-storage.service
	fi
	if [ -f /lib/systemd/system/psme-storage.service ]; then
	    rm /lib/systemd/system/psme-storage.service
	fi
	systemctl daemon-reload
else
	exit 0
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(600,root,root) /etc/psme/psme-storage-configuration.json
%attr(700,root,root) %{_bindir}/psme-storage


%changelog
