import os

EnsureSConsVersion( 2, 3, 0 )

# Versions
loudnessAssessmentVersionMajor = "1"
loudnessAssessmentVersionMinor = "3"
loudnessAssessmentVersionMicro = "8"

loudnessAssessmentVersion = [
    loudnessAssessmentVersionMajor,
    loudnessAssessmentVersionMinor,
    loudnessAssessmentVersionMicro ]

loudnessAssessmentVersionStr = ".".join( loudnessAssessmentVersion )

### Command line options ###

# Get build mode
AddOption(
    '--mode',
    dest='mode',
    type='string',
    nargs=1,
    action='store',
    default='release',
    metavar='MODE',
    help='Select build mode: debug, release. [default: release].'
)
buildMode = GetOption('mode')
if not ( buildMode in [ 'debug', 'release' ] ) :
        raise Exception( "Can't select build mode ['debug', 'release']" )

# Get libboost install path
AddOption(
    '--boost',
    dest='boost',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Path to root of boost library (only need accumulators headers).'
)

# Get libsndfile install path
AddOption(
    '--sndfile',
    dest='sndfile',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Path to root of sndfile library.'
)

# Get qt install path
AddOption(
    '--qt',
    dest='qt',
    type='string',
    nargs=1,
    action='store',
    default='/usr',
    metavar='DIR',
    help='Path to root of qt library, can be overwrited with QTDIR environement variable.'
)
AddOption(
    '--qt-suffix',
    dest='qtSuffix',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Use this option to specify the path to qt library (is case of conflict between several versions for example).'
)

# Get ffmpeg install path
AddOption(
    '--ffmpeg',
    dest='ffmpeg',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Path to root of ffmpeg library.'
)

# Get avtranscoder install path
AddOption(
    '--avtranscoder',
    dest='avtranscoder',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Path to root of avtranscoder library.'
)
AddOption(
    '--avtranscoder-version',
    dest='avtranscoderVersion',
    type='string',
    nargs=1,
    action='store',
    help='Version of avtranscoder library to link (needed only if compile on Windows).'
)

# Get gtest install path
AddOption(
    '--gtest',
    dest='gtest',
    type='string',
    nargs=1,
    action='store',
    metavar='DIR',
    help='Path to root of gtest framework (used for the tests).'
)

# Get gtest install path
AddOption(
    '--ebu-test-essences',
    dest='ebuTestEssences',
    type='string',
    nargs=1,
    action='store',
    default='.',
    metavar='DIR',
    help='Path to essences used during tests based on EBU samples.'
)

# Get target arch
AddOption(
    '--target-arch',
    dest='targetArch',
    type='string',
    nargs=1,
    action='store',
    default='',
    help='Use this option to specify the target arch (x86, x64...). By default the arch is choosen depending on the compiler plateform.'
)

# Option to generate code coverage
AddOption(
    '--coverage',
    dest='coverage',
    action='append_const',
    const='--coverage',
    help='To run the tests and get a code coverage report.'
)

### Create env ###

env = Environment(ENV = {
    'PATH': os.environ['PATH'],
    'TARGET_ARCH': GetOption('targetArch'),
})

boost_root = GetOption('boost')
boost_include = ''
boost_lib = ''
if boost_root:
    boost_include = os.path.join( boost_root, 'include' )
    boost_lib = os.path.join( boost_root, 'lib' )

sndfile_root = GetOption('sndfile')
sndfile_include = ''
sndfile_lib = ''
if sndfile_root:
    sndfile_include = os.path.join( sndfile_root, 'include' )
    sndfile_lib = os.path.join( sndfile_root, 'lib' )

qt_dir = GetOption('qt')
if 'QTDIR' in os.environ:
    qt_dir = os.environ['QTDIR']

qt_include_suffix = GetOption('qtSuffix')

qt_include = ''
if qt_include_suffix:
    qt_include = os.path.join(qt_dir, 'include', qt_include_suffix)

env.Append(
    CPPPATH = [
        '#src',
        sndfile_include,
        boost_include,
        qt_include,
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
    ],
    SHLIBVERSION = loudnessAssessmentVersionStr,
)

# Set QTDIR if specify
if qt_dir:
    env.Append(
        QTDIR = qt_dir,
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
    if GetOption('coverage'):
        env['CXXFLAGS'].extend( ['-fprofile-arcs', '-ftest-coverage'] )
        env.Append( LINKFLAGS = ['-fprofile-arcs'] )
    if buildMode == 'release':
        env.Append( CXXFLAGS = ['-O3'] )
    else:
        env.Append( CXXFLAGS = ['-g'] )
elif env['CC'] == 'cl':  # msvc
    # Exception handling used by the compiler
    env.Append( CXXFLAGS = ['/EHsc'] )
    if buildMode == 'release':
        # /MT: Use the multithread, static version of the run-time library
        # /Ox: maximal optimization
        env.Append( CXXFLAGS = ['/MT', '/Ox'] )
    else:
        # Plus DEBUG option
        env.Append( CXXFLAGS = ['/MTd'] )

# Build src, apps and tests

Export( 'env' )
Export( 'loudnessAssessmentVersionStr' )

VariantDir( 'build/' + buildMode + '/src', 'src', duplicate = 0 )
VariantDir( 'build/' + buildMode + '/app', 'app', duplicate = 0 )

SConscript( 'src/SConscript', variant_dir = 'build/' + buildMode + '/src' )
SConscript( 'app/SConscript', variant_dir = 'build/' + buildMode + '/app' )
SConscript( 'test/SConscript', variant_dir = 'build/' + buildMode + '/test' )
