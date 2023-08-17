from setuptools import setup, Extension, find_packages

setup(
    name='barpy',
    version='1.0',
    packages=find_packages(),
    package_data={'barpy': ['barpy.pyd']},
)