<<<<<<< HEAD
Node.js for Mobile Apps
====================================

This is the main repository for [Node.js for Mobile Apps](https://code.janeasystems.com/nodejs-mobile), a toolkit for integrating Node.js into mobile applications.

## Resources for Newcomers
* [Website](https://code.janeasystems.com/nodejs-mobile)
* [Gitter channel](https://gitter.im/nodejs-mobile/community)
* [Frequently Asked Questions](FAQ.md)

This is the central repository for reporting **all issues** related to the Node.js for Mobile Apps project, including issues pertaining to the React Native and Cordova plugins. 

The core library source code is in this repo. If you are looking for the *source code* for the plugins, you can find it at:

* [React Native plugin source repo](https://github.com/janeasystems/nodejs-mobile-react-native)
* [Cordova plugin source repo](https://github.com/janeasystems/nodejs-mobile-cordova)


## Project Goals

1. To provide the fixes necessary to run Node.js on mobile operating systems.
1. To investigate which features need to be added to Node.js in order to make it a useful tool for mobile app development.
1. To diverge as little as possible from nodejs/node, while fulfilling goals (1) and (2).

## Download
Binaries for Android and iOS are available at https://github.com/janeasystems/nodejs-mobile/releases.

## Documentation
Documentation can be found on the [project website](https://code.janeasystems.com/nodejs-mobile). Sample code is available in the [samples repo](https://github.com/janeasystems/nodejs-mobile-samples/).

***Disclaimer:***  documentation found in this repository is currently unchanged from the parent repository and may only be applicable to upstream node.

## Build Instructions

### Prerequisites to build the Android library on Linux Ubuntu/Debian:

#### Basic build tools:
```sh
sudo apt-get install -y build-essential git python
=======
### Node-HTTPP - Run HTTP over UDP and Bring Web in Peer or P2P styles.

![test-linux](https://github.com/InstantWebP2P/node-httpp/workflows/test-linux/badge.svg?branch=v12.x-httpp)
![test-macOS](https://github.com/InstantWebP2P/node-httpp/workflows/test-macOS/badge.svg?branch=v12.x-httpp)
![build-windows](https://github.com/InstantWebP2P/node-httpp/workflows/build-windows/badge.svg?branch=v12.x-httpp)

* Consists of four modules: udt.js, httpp.js, udts.js, httpps.js with the compatible api to net.js,http.js,tls.js and https.js

[Wiki page](https://github.com/InstantWebP2P/node-httpp/wiki/An-introduction-to-node-httpp)

### Features

* Run HTTP and HTTPS over UDP, taking UDP high performance
* Support multi-process/thread UDT server by utilizing multi-cores' CPU
* Enhance UDT security by authenticating both control and data packet
* Configurable Congestion Control
* Per-socket network performance monitor
* Per-socket bandwidth limitation
* Extend client/central server style web service transparently
* Easy to use API, reuse existing http/web and node.js technology
* Support windows/linux/OSx, RaspberryPi, Android

### TODO

* Add more test cases
* Improve documents
* RFC protocol draft


### third-party source

* [UDT4](http://udt.sourceforge.net)


### Support us

* Welcome contributing on document, codes, tests and issues


### To checkout the sourcecode:

    git clone https://github.com/InstantWebP2P/node-httpp.git


### Build on Linux with Docker

  building:
  
    ./appbld ./configure --prefix=/appins
    ./appbld make -j 6
    ./appbld make install
    
  packaging and start Node.js REPL:
    
    ./apppkg
    
### To build on other platforms

Prerequisites (Unix only):

    * Python 2.6 or 2.7
    * GNU Make 3.81 or newer
    * libexecinfo (FreeBSD and OpenBSD only)

Unix/Macintosh:

    ./configure
    make
    make install

Windows:

    vcbuild.bat
    
Resources for Newcomers
---

  - [The SO_REUSEPORT socket option](https://lwn.net/Articles/542629/)


<p align="center">
  <a href="https://nodejs.org/">
    <img
      alt="Node.js"
      src="https://nodejs.org/static/images/logo-light.svg"
      width="400"
    />
  </a>
</p>

Node.js is an open-source, cross-platform, JavaScript runtime environment. It
executes JavaScript code outside of a browser. For more information on using
Node.js, see the [Node.js Website][].

The Node.js project uses an [open governance model](./GOVERNANCE.md). The
[OpenJS Foundation][] provides support for the project.

**This project is bound by a [Code of Conduct][].**

# Table of Contents

* [Support](#support)
* [Release Types](#release-types)
  * [Download](#download)
    * [Current and LTS Releases](#current-and-lts-releases)
    * [Nightly Releases](#nightly-releases)
    * [API Documentation](#api-documentation)
  * [Verifying Binaries](#verifying-binaries)
* [Building Node.js](#building-nodejs)
* [Security](#security)
* [Contributing to Node.js](#contributing-to-nodejs)
* [Current Project Team Members](#current-project-team-members)
  * [TSC (Technical Steering Committee)](#tsc-technical-steering-committee)
  * [Collaborators](#collaborators)
  * [Release Keys](#release-keys)

## Support

Looking for help? Check out the
[instructions for getting support](.github/SUPPORT.md).

## Release Types

* **Current**: Under active development. Code for the Current release is in the
  branch for its major version number (for example,
  [v10.x](https://github.com/nodejs/node/tree/v10.x)). Node.js releases a new
  major version every 6 months, allowing for breaking changes. This happens in
  April and October every year. Releases appearing each October have a support
  life of 8 months. Releases appearing each April convert to LTS (see below)
  each October.
* **LTS**: Releases that receive Long-term Support, with a focus on stability
  and security. Every even-numbered major version will become an LTS release.
  LTS releases receive 12 months of _Active LTS_ support and a further 18 months
  of _Maintenance_. LTS release lines have alphabetically-ordered codenames,
  beginning with v4 Argon. There are no breaking changes or feature additions,
  except in some special circumstances.
* **Nightly**: Code from the Current branch built every 24-hours when there are
  changes. Use with caution.

Current and LTS releases follow [Semantic Versioning](https://semver.org). A
member of the Release Team [signs](#release-keys) each Current and LTS release.
For more information, see the
[Release README](https://github.com/nodejs/Release#readme).

### Download

Binaries, installers, and source tarballs are available at
<https://nodejs.org/en/download/>.

#### Current and LTS Releases
<https://nodejs.org/download/release/>

The [latest](https://nodejs.org/download/release/latest/) directory is an
alias for the latest Current release. The latest-_codename_ directory is an
alias for the latest release from an LTS line. For example, the
[latest-carbon](https://nodejs.org/download/release/latest-carbon/) directory
contains the latest Carbon (Node.js 8) release.

#### Nightly Releases
<https://nodejs.org/download/nightly/>

Each directory name and filename contains a date (in UTC time) and the commit
SHA at the HEAD of the release.

#### API Documentation

Documentation for the latest Current release is at <https://nodejs.org/api/>.
Version-specific documentation is available in each release directory in the
_docs_ subdirectory. Version-specific documentation is also at
<https://nodejs.org/download/docs/>.

### Verifying Binaries

Download directories contain a `SHASUMS256.txt` file with SHA checksums for the
files.

To download `SHASUMS256.txt` using `curl`:

```console
$ curl -O https://nodejs.org/dist/vx.y.z/SHASUMS256.txt
>>>>>>> bed41836030949b74f5a3e591e582b753cfcbbbc
```

#### Install curl and unzip (needed to download the Android NDK):
```sh
sudo apt-get install -y curl unzip
```

#### Install Android NDK r21b for Linux:
Choose a location where you want to install the Android NDK and run:
```sh
curl https://dl.google.com/android/repository/android-ndk-r21b-linux-x86_64.zip -o ndk.zip
unzip ndk.zip
```
It will create a `android-ndk-r21b` folder. Save that path for later.

### Prerequisites to build the Android library on macOS:

#### Git:

Run `git` in a terminal window, it will show a prompt to install it if not already present.
As an alternative, installing one of these will install `git`:
* Xcode, with the Command Line Tools.
* [Homebrew](https://brew.sh/)
* [Git-SCM](https://git-scm.com/download/mac)

#### Install Android NDK r21b for macOS:
Choose a location where you want to install the Android NDK and run:
```sh
curl https://dl.google.com/android/repository/android-ndk-r21b-darwin-x86_64.zip -o ndk.zip
unzip ndk.zip
```
It will create a `android-ndk-r21b` folder. Save that path for later.

### Building the Android library on Linux or macOS:

#### 1) Clone this repo and check out the `mobile-master` branch:

```sh
git clone https://github.com/janeasystems/nodejs-mobile
cd nodejs-mobile
git checkout mobile-master
```

#### 2a) Using the Android helper script:

The `tools/android_build.sh` script takes as first argument the Android NDK path (in our case is `~/android-ndk-r21b`). The second argument is optional and is the target architecture, which can be one of the following: `arm`, `x86`, `arm64` or `x86_64`. If no target architecture is provided, it will build all available architectures.
Run:

```sh
./tools/android_build.sh ~/android-ndk-r21b
```

When done, each built shared library will be placed in `out_android/$(ARCHITECTURE)/libnode.so`.

#### 2b) Configure and build manually:
Run the `android-configure` script to configure the build with the path to the downloaded NDK and the desired target architecture.

```sh
source ./android-configure ../android-ndk-r21b arm
```

Start the build phase:
```sh
make
```

This will create the Android `armeabi-v7a` shared library in `out/Release/lib.target/libnode.so`.

### Prerequisites to build the iOS .framework library on macOS:

#### Xcode 11 with Command Line Tools

Install Xcode 11 or higher, from the App Store, and then install the Command Line Tools by running the following command:

```sh
xcode-select --install
```

That installs `git`, as well.

#### CMake

To install `CMake`, you can use a package installer like [Homebrew](https://brew.sh/).

First, install `HomeBrew`, if you don't have it already.

```sh
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Then, use it to install `CMake`:

```sh
brew install cmake
```

### Building the iOS library using CocoaPods:

Add this to your `Podfile`:

```ruby
pod 'NodeMobile', :git => 'https://github.com/janeasystems/nodejs-mobile.git'
```

### Building the iOS .framework library on macOS:

#### 1) Clone this repo and check out the `mobile-master` branch:

```sh
git clone https://github.com/janeasystems/nodejs-mobile
cd nodejs-mobile
git checkout mobile-master
```

#### 2) Run the helper script:

```sh
./tools/ios_framework_prepare.sh
```

That will configure `gyp` to build Node.js and its dependencies as static libraries for iOS on the arm64 and x64 architectures, using the `v8` engine configured to start with JIT disabled. The script copies those libraries to `tools/ios-framework/bin/arm64` and `tools/ios-framework/bin/x64`, respectively. It also merges them into static libraries that contain strips for both architectures, which will be placed in `tools/ios-framework/bin` and used by the `tools/ios-framework/NodeMobile.xcodeproj` Xcode project.

The helper script builds the `tools/ios-framework/NodeMobile.xcodeproj` Xcode project into three frameworks:
  - The framework to run on iOS devices: `out_ios/Release-iphoneos/NodeMobile.framework`
  - The framework to run on the iOS simulator: `out_ios/Release-iphonesimulator/NodeMobile.framework`
  - The universal framework, that runs on iOS devices and simulators: `out_ios/Release-universal/NodeMobile.framework`

While the universal framework is useful for faster Application development, due to supporting both iOS devices and simulators, frameworks containing simulator strips will not be accepted on the App Store. Before trying to submit your application, it would be advisable to use the `Release-iphoneos/NodeMobile.framework` in your submission archive or strip the x64 slices from the universal framework's binaries before submitting.

## Running tests
Please see the [TESTING.md](./doc_mobile/TESTING.md) file in the `doc_mobile` folder in this source distribution.

## Contributing
Please see the [CONTRIBUTING](./doc_mobile/CONTRIBUTING.md) file in the `doc_mobile` folder in this source distribution.
