from setuptools import setup, find_packages
setup(
    name = "RedFishClient",
    version = "1.1",
    packages = ['event_subscribe'],
    author = "Intel",
    scripts = [ "subscribe.py", "receiver.py" ],
    author_email = "intel@intel.com",
    description = "This is an redfish client Package",
    license = "PSF",
    keywords = "Red fish client",
    url = "http://intel.com",
)