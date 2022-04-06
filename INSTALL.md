# Compilation

## Getting the source

```
git clone git://github.com/mikrosimage/loudness_validator.git  
cd loudness_validator
```


## Build with Scons

#### Build all tools in release mode
```
scons
```

#### More build info ('Local Options' section)
```
scons --help
```

## Build with CMake

#### Typical build commands
```
mkdir build
cd build
cmake .. [OPTIONS]
make
make install
```
With (non-exhaustive but typical) `OPTIONS`:
```
  -G "Unix Makefiles"
  -DCMAKE_BUILD_TYPE=[Debug|Release|...]
  -DCMAKE_PREFIX_PATH=/path/to/dependencies/install;...
  -DCMAKE_INSTALL_PREFIX=/path/to/target/install/directory
  -DEBU_TEST_ESSENCES=/path/to/ebu-loudness-test-material
  ...
```

## Test

C++ tests, based on [GTest](https://github.com/google/googletest) framework, are used to check the loudness analysis with [this EBU test material](https://tech.ebu.ch/files/live/sites/tech/files/shared/testmaterial/ebu-loudness-test-setv04.zip).

#### Run tests with Scons
```
scons --ebu-test-essences=/path/to/ebu/test/material test
```

#### Run tests with CMake

Add the following option to the `cmake` command (if not already set):
```
  -DEBU_TEST_ESSENCES=/path/to/ebu-loudness-test-material
```
Then run the following command (enabling verbose mode):
```
make test ARGS="-V"
```


## Dependencies

#### Libraries

* __loudnessAnalyser__  
*Depends on boost accumulators (headers only).*  
To analyse audio streams regardless loudness standards (CST R017, EBU R128, ATSC A85).

* __loudnessCorrector__  
*Depends on loudnessAnalyser.*  
To correct audio streams.

* __loudnessTools__  
*Depends on loudnessAnalyser.*  
To writer XML reports.

* __loudnessIO__  
*Depends on loudnessAnalyser and [libsndfile](http://www.mega-nerd.com/libsndfile/).*  
To read data from audio streams.

#### Apps

* __loudness-analyser__  
To analyse given audio files.

* __loudness-corrector__  
To analyse and correct given audio files.

* __loudness-validator__  
*External dependencies to [Qt4](http://qt.nokia.com/products/).*  
Simple GUI to analyse and correct given audio files.

* __loudness-media-analyser__  
*External dependencies to [avtranscoder](https://github.com/avTranscoder/avTranscoder) v0.9.2 or upper.*  
To analyse audio in given files of any types.

## Tested compilers

* GCC 4.4.1 on OpenSUSE 11.2
* GCC 4.6.2 on OpenSUSE 11.2
* CLang 3.1 on OpenSUSE 11.2

* GCC 4.5.2 on Windows XP
* MSVC 12 on Windows 7


## Tested libraries

* Qt 4.5.3  
* Qt 4.8.6

* libsndfile 1.0.24-22.2
* libsndfile 1.0.25
