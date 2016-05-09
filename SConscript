import os

################################################################################

Import('env')

verbose = env['verbose']
board   = env['board']
project = env['project']

################################################################################

openmote_cc2538 = {
    'name'      : 'openmote-cc2538',
    'platform'  : 'cc2538',
    'cpu'       : 'cortex-m3',
    'toolchain' : 'arm-none-eabi',
    'linker'    : 'cc2538_linker.lds',
}

boards = {
    'openmote-cc2538'  : openmote_cc2538,
}

################################################################################

test = {
    'name'  : 'test'
}

projects = {
    'test' : test
}

################################################################################

build_dir = os.path.join("#/build", board)

platform  = boards[board]["platform"]
cpu       = boards[board]["cpu"]
toolchain = boards[board]["toolchain"]

linker    = os.path.join(".", 'platform', platform, boards[board]["linker"])

env['board']     = board
env['platform']  = platform
env['cpu']       = cpu
env['toolchain'] = toolchain
env['linker']    = linker

folders = ['board', 'drivers', 'kernel/freertos', 'stack', 'platform', 'projects']

################################################################################

if board not in boards.keys():
    raise SystemError("Error, target not defined!")

if verbose:
    print("Building project={0} for board={1} using platform={2} and toolchain={3}.".format(project, board, platform, toolchain))

################################################################################

env.Replace(CC          = toolchain + '-gcc')
env.Replace(CXX         = toolchain + '-g++')
env.Replace(AS          = toolchain + '-as')
env.Replace(OBJCOPY     = toolchain + '-objcopy')
env.Replace(OBJDUMP     = toolchain + '-objdump')
env.Replace(AR          = toolchain + '-ar')
env.Replace(RANLIB      = toolchain + '-ranlib')
env.Replace(NM          = toolchain + '-nm')
env.Replace(SIZE        = toolchain + '-size')

if (cpu == 'cortex-m3'):
    env.Append(CFLAGS       = '-mthumb')
    env.Append(CFLAGS       = '-mcpu=cortex-m3')
    env.Append(CFLAGS       = '-mlittle-endian')
    env.Append(CFLAGS       = '-ffunction-sections')
    env.Append(CFLAGS       = '-fdata-sections')
    env.Append(CFLAGS       = '-fno-strict-aliasing')
    env.Append(CFLAGS       = '-fshort-enums')
    env.Append(CFLAGS       = '-fomit-frame-pointer')
    env.Append(CFLAGS       = '-std=c99')
    env.Append(CFLAGS       = '-Wall')
    env.Append(CFLAGS       = '-pedantic')
    env.Append(CFLAGS       = '-Wstrict-prototypes')
    env.Append(CFLAGS       = '-O0')
    env.Append(CFLAGS       = '-g')
    env.Append(CFLAGS       = '-ggdb')

    env.Append(CXXFLAGS     = '-mthumb')
    env.Append(CXXFLAGS     = '-mcpu=cortex-m3')
    env.Append(CXXFLAGS     = '-mlittle-endian')
    env.Append(CXXFLAGS     = '-ffunction-sections')
    env.Append(CXXFLAGS     = '-fdata-sections')
    env.Append(CXXFLAGS     = '-fno-strict-aliasing')
    env.Append(CXXFLAGS     = '-fshort-enums')
    env.Append(CXXFLAGS     = '-fomit-frame-pointer')
    env.Append(CXXFLAGS     = '-fno-unwind-tables')
    env.Append(CXXFLAGS     = '-fno-exceptions')
    env.Append(CXXFLAGS     = '-fno-builtin')
    env.Append(CXXFLAGS     = '-fno-rtti')
    env.Append(CXXFLAGS     = '-std=c++11')
    env.Append(CXXFLAGS     = '-Wall')
    env.Append(CXXFLAGS     = '-pedantic')
    env.Append(CXXFLAGS     = '-O0')
    env.Append(CXXFLAGS     = '-g')
    env.Append(CXXFLAGS     = '-ggdb')

    env.Append(LINKFLAGS    = '-mcpu=cortex-m3')
    env.Append(LINKFLAGS    = '-mthumb')
    env.Append(LINKFLAGS    = '-mlittle-endian')
    env.Append(LINKFLAGS    = '-Wl,--gc-sections')
    env.Append(LINKFLAGS    = '-Wl,--defsym')
    env.Append(LINKFLAGS    = '-Wl,__cxa_pure_virtual=0')
    env.Append(LINKFLAGS    = '-nodefaultlibs')
    env.Append(LINKFLAGS    = '-T' + linker)

else:
    raise SystemError("Error, cpu not valid!")

################################################################################

if not verbose:
   env[    'CCCOMSTR']  = "Compiling          $SOURCE"
   env[   'CXXCOMSTR']  = "Compiling          $SOURCE"
   env[  'SHCCCOMSTR']  = "Compiling (shared) $TARGET"
   env[    'ARCOMSTR']  = "Archiving          $TARGET"
   env['RANLIBCOMSTR']  = "Indexing           $TARGET"
   env[  'LINKCOMSTR']  = "Linking            $ARGET"
   env['SHLINKCOMSTR']  = "Linking (shared)   $TARGET"
   env[  'INSTALLSTR']  = "Installing         $TARGET"

################################################################################

env.Append(
    CPPPATH = [
        os.path.join('#','platform', 'inc'),
        os.path.join('#','platform', platform),
        os.path.join('#','kernel', 'freertos', 'common'),
        os.path.join('#','kernel', 'freertos', 'inc'),
        os.path.join('#','kernel', 'freertos', cpu),
        os.path.join('#','projects', project),
        os.path.join('#','drivers', 'inc'),
        os.path.join('#','drivers', 'adxl346'),
        os.path.join('#','drivers', 'cc1200'),
        os.path.join('#','drivers', 'enc28j60'),
        os.path.join('#','drivers', 'max44009'),
        os.path.join('#','drivers', 'sht21'),
        os.path.join('#','drivers', 'tps62730'),
        os.path.join('#','stack', 'ethernet'),
        os.path.join('#','stack', 'ieee802154'),
        os.path.join('#','stack', 'utils'),
    ]
)

################################################################################

if (board == 'openmote-cc2538'):
    env.Append(
        CPPPATH = [
            os.path.join('#','board', 'openmote-cc2538')
        ]
    )
else:
    raise SystemError("Error, board not valid!")

################################################################################

if (platform == 'cc2538'):
    env.Append(
        CPPPATH = [
            os.path.join('#','platform', 'cc2538', 'libcc2538'),
            os.path.join('#','platform', 'cc2538', 'libcc2538', 'inc'),
            os.path.join('#','platform', 'cc2538', 'libcc2538', 'src')
        ]
    )
else:
    raise SystemError("Error, platform not valid!")

################################################################################

# Build all targets and dependencies
for folder in folders:
    if verbose:
        print("Parsing folder={0}.".format(folder))
    script = os.path.join("#", folder, "SConscript")
    build  = os.path.join(build_dir, folder)
    output = env.SConscript(script, variant_dir=build, duplicate=0, exports = 'env')
