import subprocess
import setuptools
import sys

try:
    from conans import client
except ImportError:
    subprocess.call([sys.executable, '-m', 'pip', 'install', 'conan'])
    from conans import client

try:
    from skbuild import setup
except ImportError:
    subprocess.call([sys.executable, '-m', 'pip', 'install', 'scikit-build'])
    from skbuild import setup

with open('README.md', 'r') as file:
    long_description = file.read()

setup_requirements = ['scikit-build>=0.11.1',
                      'cmake>=3.16',
                      'conan>=1.25'
                      ]

setup (name = 'kbw',
       version='0.1b2',
       cmake_source_dir='.',
       author='Evandro Chagas Ribeiro da Rosa',
       author_email='evandro.crr@posgrad.ufsc.br',
       description='Ket Bitwise Simulator Server.',
       long_description=long_description,
       long_description_content_type='text/markdown',
       url='https://gitlab.com/quantum-ket/kbw',
       license='Apache-2.0',
       packages=setuptools.find_namespace_packages(include=['kbw', 'kbw.*']),
       setup_requires=setup_requirements,
       classifiers=['Programming Language :: Python :: 3 :: Only',
                    'Programming Language :: C++',
                    'Intended Audience :: Science/Research',
                    'Topic :: Scientific/Engineering',
                    'License :: OSI Approved :: MIT License',
                   ],
        entry_points={'console_scripts': ['kbw = kbw.__main__:main']},
       )
