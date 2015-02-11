import distutils.sysconfig
from pprint import pprint
import re
import os
import warnings
import sys

import yaml
from path_helpers import path

import auto_config
from get_libs import get_lib
from git_util import GitUtil


def get_version_string():
    version = GitUtil(None).describe()
    branch = GitUtil(None).branch()
    if branch == "master":
        tags = ""
    else:
        tags = "-" + branch
    m = re.search('^v(?P<major>\d+)\.(?P<minor>\d+)(-(?P<micro>\d+))?', version)
    if m.group('micro'):
        micro = m.group('micro')
    else:
        micro = '0'
    return "%s.%s.%s%s" % (m.group('major'), m.group('minor'), micro, tags)


PYTHON_VERSION = "%s.%s" % (sys.version_info[0],
                            sys.version_info[1])

env = Environment()

SOFTWARE_VERSION = get_version_string()
Export('SOFTWARE_VERSION')

Import('PYTHON_LIB')

# # Build Arduino binaries #
sketch_build_root = path('build/arduino').abspath()
Export('sketch_build_root')
SConscript('signal_generator_board/Arduino/signal_generator_board/SConscript')

Import('arduino_hex')

# # Install compiled firmwares to `firmware` directory #
firmware_path = path('signal_generator_board').joinpath('firmware')
package_hex = env.Install(firmware_path, arduino_hex)
