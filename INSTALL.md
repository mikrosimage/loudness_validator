# Compilation

## Getting the source

```
git clone git://github.com/mikrosimage/loudness_validator.git  
cd loudness_validator
```


## Build

#### Build all tools in release mode
```
scons
```

#### Build in debug mode
```
scons mode=debug
```

#### Build with a specific path to libraries
```
scons BOOST_ROOT=/path/to/boost SNDFILE_ROOT=/path/to/sndfile QTDIR=/path/to/qt4
```


## Dependencies

#### Libraries
__libloudnessAnalyser__ and __loudnessTools__ require __libsndfile__ and __boost accumulators__.

#### Apps
loudnessValidator requires also Qt4.

[libsndfile](http://www.mega-nerd.com/libsndfile/)  
[Qt4](http://qt.nokia.com/products/)  


## Tested compilers

* GCC 4.4.1 on OpenSUSE 11.2
* GCC 4.6.2 on OpenSUSE 11.2
* CLang 3.1 on OpenSUSE 11.2

* GCC 4.5.2 on Windows XP


## Tested libraries

* Qt 4.5.3  
* Qt 4.8.6

* libsndfile 1.0.24-22.2
* libsndfile 1.0.25
