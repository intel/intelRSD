# Simulators

This component contains mocks written in JavaScript for PSME/PNC/RMM/LUI/RSS services. It exposes an example REST API of those services. It is not connected to current HW or PSME agents and can be run on any PC or other machine with preinstalled NodeJS.


## Installation

1. Install Node.js
```
curl --silent --location https://deb.nodesource.com/setup_6.x | sudo bash -
sudo apt-get install --yes nodejs
```

2. Install npm package manager (ex. from Ubuntu repositories)
```
sudo npm install npm -g
```

3. To install all nodejs required packages - execute the command below in `node-ssdp` directory and then in `simulators` directory
```
npm install
```

## Usage
Main mocks file are places in the `mock_entry_points` directory. From this path the user can run the desired mock.

Most of the mocks have two versions: with or without sending events. Those that can acutally send events on some actions has a suffix `_eventable_mock`.


### With events mocks 
This is an example of using PSME Eventable Mock.

1. Run command:
```
nodejs --harmony psme_eventable_mock.js
```
2. (optional) - to test event messages run the command:
```
nodejs listener.js
```

It will show all events sent by psme_eventable_mock (it is not a proxy)

### Without event mocks
This is an example of using PSME Mock.

1. Run command:
```
nodejs --harmony psme_mock.js
```