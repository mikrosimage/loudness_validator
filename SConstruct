import os

EnsureSConsVersion( 2, 3, 0 )

# Versions
loudnessAssessmentVersionMajor = "0"
loudnessAssessmentVersionMinor = "0"
loudnessAssessmentVersionMicro = "1"

loudnessAssessmentVersion = [
    loudnessAssessmentVersionMajor,
    loudnessAssessmentVersionMinor,
    loudnessAssessmentVersionMicro ]

loudnessAssessmentVersionStr = ".".join( loudnessAssessmentVersion )

# Get build mode
buildMode = ARGUMENTS.get( 'mode', 'release' )
if not ( buildMode in [ 'debug', 'release' ] ) :
        raise Exception( "Can't select build mode ['debug', 'release']" )

# Get libsndfile install path
sndfile_root = ARGUMENTS.get( 'SNDFILE_ROOT', '' )
sndfile_include = ''
sndfile_lib = ''
if sndfile_root:
    sndfile_include = os.path.join( sndfile_root, 'include' )
    sndfile_lib = os.path.join( sndfile_root, 'lib' )

# Get libboost install path
boost_root = ARGUMENTS.get( 'BOOST_ROOT', '' )
boost_include = ''
boost_lib = ''
if boost_root:
    boost_include = os.path.join( boost_root, 'include' )
    boost_lib = os.path.join( boost_root, 'lib' )

# Get avtranscoder install path
avtranscoder_root = ARGUMENTS.get( 'AVTRANSCODER_ROOT', '' )
avtranscoder_include = ''
avtranscoder_lib = ''
if avtranscoder_root:
    avtranscoder_include = os.path.join( avtranscoder_root, 'include' )
    avtranscoder_lib = os.path.join( avtranscoder_root, 'lib' )

# Get ffmpeg install path
ffmpeg_root = ARGUMENTS.get( 'FFMPEG_ROOT', '' )
ffmpeg_include = ''
ffmpeg_lib = ''
if ffmpeg_root:
    ffmpeg_include = os.path.join( ffmpeg_root, 'include' )
    ffmpeg_libs = [os.path.join( ffmpeg_root, 'lib' ), os.path.join( ffmpeg_root, 'lib64' )]

# Get qt4 install path
qt4_dir = ARGUMENTS.get( 'QTDIR', '/usr/' )

# Create env
env = Environment()

env.Append(
        CPPPATH = [
                '#src',
                sndfile_include,
                boost_include,
                avtranscoder_include,
                ffmpeg_include,
        ],
        CXXFLAGS = [
                '-DLOUDNESS_ASSESSMENT_VERSION_MAJOR=' + loudnessAssessmentVersionMajor,
                '-DLOUDNESS_ASSESSMENT_VERSION_MINOR=' + loudnessAssessmentVersionMinor,
                '-DLOUDNESS_ASSESSMENT_VERSION_MICRO=' + loudnessAssessmentVersionMicro,
        ],
        LIBPATH = [
                '#src',
                sndfile_lib,
                boost_lib,
                avtranscoder_lib,
                ffmpeg_libs,
        ],
        SHLIBVERSION = loudnessAssessmentVersionStr,
        )

# Set QTDIR if specify
if qt4_dir:
    env.Append(
        QTDIR = qt4_dir,
    )

# Set frameworks path if MacOS
if env['PLATFORM'] == "darwin":
    env.Append(
        FRAMEWORKPATH = [
            '/usr/local/Frameworks',
        ],
    )

# Add compile flags
if env['CC'] == 'gcc':
    env.Append( CXXFLAGS = ['-Wall', '-fPIC'] )
    if buildMode == 'release':
        env.Append( CXXFLAGS = ['-O3'] )
    else:
        env.Append( CXXFLAGS = ['-g'] )
elif env['CC'] == 'cl':  # msvc
    # Exception handling used by the compiler
    env.Append( CXXFLAGS = ['/EHsc'] )
    if buildMode == 'release':
        # Use the multithread, static version of the run-time library
        env.Append( CXXFLAGS = ['/MT'] )
    else:
        # Plus DEBUG option
        env.Append( CXXFLAGS = ['/MTd'] )

# Build src and app

Export( 'env' )
Export( 'loudnessAssessmentVersionStr' )

VariantDir( 'build/' + buildMode + '/src', 'src', duplicate = 0 )
VariantDir( 'build/' + buildMode + '/app', 'app', duplicate = 0 )

SConscript( 'src/SConscript', variant_dir = 'build/' + buildMode + '/src' )
SConscript( 'app/SConscript', variant_dir = 'build/' + buildMode + '/app' )
