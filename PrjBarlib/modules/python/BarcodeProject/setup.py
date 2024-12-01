from setuptools import setup, find_packages

setup(
    name="raster_barcode",
    version="1.0.0",
    description="Python wrapper for the Barcode library.",
    author="Art A",
    author_email="your.email@example.com",
    url="https://your_project_website_or_repo",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    license="MIT",
    classifiers=[
        "Programming Language :: Python :: 3.13",
        "Operating System :: OS Independent",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS",
    ],
    packages=find_packages(),
    include_package_data=True,
    python_requires="==3.13",
    install_requires=[
        "numpy",
        "matplotlib",
    ],
    package_data={
        "raster_barcode": [
            "libbarpy.so;platform_system=='Linux'",
            "libbarpy.pyi",
            "libbarpy.so;platform_system=='Macos'",
            "libbarpy.pyd;platform_system=='Windows'",
        ],
    },
    has_ext_modules = lambda: True,
)