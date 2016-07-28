Summary: Package for RackScale PSME compute intel
Name: psme-compute-intel
Version: 0.0.0.0
Release: PreAlpha
License: Apache License 2.0
Group: Applications/System
Requires: jsoncpp libmicrohttpd libcurl libstdc++ uuid-c++ OpenIPMI-libs
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
cp -p %{SOURCEURL0}/etc/psme/psme-compute-intel-configuration.json ./etc/psme/
cp -p %{SOURCEURL0}/etc/psme/bdc_r_intel_configuration.json ./etc/psme/

%pre
if [ "$1" != 1 ]; then
	if [ -f /etc/psme/psme-compute-intel-configuration.json ]; then
		cp /etc/psme/psme-compute-intel-configuration.json
        /etc/psme/psme-compute-intel-configuration.json.old
	fi
	if [ -f /etc/psme/bdc_r_intel_configuration.json ]; then
		cp /etc/psme/bdc_r_intel_configuration.json /etc/psme/bdc_r_intel_configuration.json.old
	fi
	if [ -f /etc/psme/psme-compute-bdc-a-configuration.json ]; then
		cp /etc/psme/psme-compute-bdc-a-configuration.json /etc/psme/psme-compute-bdc-a-configuration.json.old
	fi
else
	exit 0
fi

%post
if [ -f /etc/ONPSS_VERSION ]; then
	mv /etc/psme/psme-compute-intel-configuration.json
    /etc/psme/psme-compute-bdc-a-intel-configuration.json
	mv /etc/psme/bdc_r_intel_configuration.json /etc/psme/psme-compute-intel-configuration.json
fi

cat <<EOF >/lib/systemd/system/psme-compute-intel.service
[Unit]
Description=Managing PSME compute intel
After=network.target network-online.target

[Service]
Type=simple
ExecStart=/usr/bin/psme-compute-intel /etc/psme/psme-compute-intel-configuration.json
PIDFile=/var/run/psme-compute-intel.pid

[Install]
WantedBy=multi-user.target
EOF
if [ ! -f /etc/systemd/system/psme-compute-intel.service ]; then
    ln -s /lib/systemd/system/psme-compute-intel.service
    /etc/systemd/system/psme-compute-intel.service
fi
systemctl daemon-reload
systemctl enable psme-compute-intel

%preun
systemctl stop psme-compute-intel
if [ "$1" == 0 ]; then
	if [ -f /etc/ONPSS_VERSION ]; then
		mv /etc/psme/psme-compute-intel-configuration.json
        /etc/psme/bdc_r_intel-configuration.json
		mv /etc/psme/psme-compute-bdc-a-intel-configuration.json
        /etc/psme/psme-compute-intel-configuration.json
	fi
else
	exit 0
fi

%postun
if [ "$1" == 0 ]; then
	systemctl disable psme-compute-intel
	if [ -f /etc/systemd/system/psme-compute-intel.service ]; then
		rm /etc/systemd/system/psme-compute-intel.service
	fi
	if [ -f /lib/systemd/system/psme-compute-intel.service ]; then
		rm /lib/systemd/system/psme-compute-intel.service
	fi
	systemctl daemon-reload
else
	exit 0
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/*
/etc/psme/*

%changelog
