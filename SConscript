import os
import sys
import threading
import subprocess
import glob

################################################################################

Import('env')

verbose = env['verbose']
board   = env['board']
project = env['project']
chip    = env['chip']

################################################################################

openmote_usb = {
    'name'      : 'openmote-usb',
    'platform'  : 'cc2538',
    'cpu'       : 'cortex-m3',
    'toolchain' : 'arm-none-eabi',
    'os'        : 'freertos',
    'linker'    : env['chip'] + '.lds'
}

openmote_tbd = {
    'name'      : 'openmote-tbd',
    'platform'  : 'cc2538',
    'cpu'       : 'cortex-m3',
    'toolchain' : 'arm-none-eabi',
    'os'        : 'freertos',
    'linker'    : env['chip']+'.lds'
}

boards = {
    'openmote-usb'  : openmote_usb,
    'openmote-tbd'  : openmote_tbd,
}

################################################################################

# Convert ELF to HEX
elf2iHexFunc = Builder(
   action = 'arm-none-eabi-objcopy --output-target=ihex $SOURCE $TARGET',
   suffix = '.hex',
)
env.Append(BUILDERS = {'Elf2iHex' : elf2iHexFunc})

# Convert ELF to BIN
elf2BinFunc = Builder(
   action = 'arm-none-eabi-objcopy --output-target=binary $SOURCE $TARGET',
   suffix = '.bin',
)
env.Append(BUILDERS = {'Elf2iBin' : elf2BinFunc})

# Print sizes
printSizeFunc = Builder(
    action = 'arm-none-eabi-size $SOURCE',
    suffix = '.phonysize',
)
env.Append(BUILDERS = {'PrintSize' : printSizeFunc})

################################################################################

def postBuildSize(env, source):
    returnVal  = []
    returnVal += [env.PrintSize(source=source)]
    return returnVal
env.AddMethod(postBuildSize, 'PostBuildSize')

def postBuildBin(env, source):
    returnVal  = []
    returnVal += [env.Elf2iHex(source=source)]
    returnVal += [env.Elf2iBin(source=source)]
    return returnVal
env.AddMethod(postBuildBin, 'PostBuildBin')

def postBuildLoad(env, source):
    returnVal = []
    if env['bootload']:
        returnVal += [env.Bootload(env.Elf2iHex(source))]
    return returnVal
env.AddMethod(postBuildLoad, 'PostBuildLoad')

################################################################################

build_dir = os.path.join("#/build", board)

platform  = boards[board]["platform"]
cpu       = boards[board]["cpu"]
toolchain = boards[board]["toolchain"]

linker    = os.path.join(".", 'platform', platform, boards[board]["linker"])

lib_name = ['board', 'drivers', 'net', 'platform', 'stack', 'freertos']
lib_path = [os.path.join('#', 'bin', board),
            os.path.join('#', 'platform', platform)]

env['board']     = board
env['platform']  = platform
env['cpu']       = cpu
env['toolchain'] = toolchain
env['os']        = os
env['linker']    = linker
env['lib_name']  = lib_name
env['lib_path']  = lib_path

library_folders = ['board', 'drivers', 'freertos', 'net', 'stack', 'platform']
project_folders = ['projects', 'test']

src_folders = project_folders + library_folders

################################################################################

class OpenMoteCC2538_bootloadThread(threading.Thread):
    def __init__(self, port, binary, semaphore):       
        # Initialize parent class
        threading.Thread.__init__(self)

        # Store parameters
        self.port         = port
        self.binary       = binary
        self.semaphore    = semaphore
        self.name         = 'OpenMoteCC2538_bootloadThread_{0}'.format(self.port)
        self.bsl_name     = 'cc2538-bsl.py'
        self.bsl_path     = os.path.join('tools', 'cc2538-bsl')
        self.bsl_params   = ' -e -w --bootloader-invert-lines -b 400000 -p '
    
    def run(self):
        print 'Starting bootloading on {0}'.format(self.port)
        command = 'python ' + os.path.join(self.bsl_path, self.bsl_name) + self.bsl_params + '{0} {1}'.format(self.port, self.binary)
        if (verbose):
			subprocess.Popen(command, shell = True).communicate()
        else:
        	subprocess.Popen(command, shell = True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        print 'Done bootloading on {0}'.format(self.port)
        
        # Indicate done
        self.semaphore.release()
        
def OpenMoteCC2538_bootload(target, source, env):
    threadList = []
    semaphore  = threading.Semaphore(0)
    
    # Enumerate ports
    ports = env['bootload'].split(',')

    # Check pots to bootload
    ports = OpenMoteCC2538_expand(ports)

    # Create threads
    for port in ports:
        threadList += [
            OpenMoteCC2538_bootloadThread(
                port      = port,
                binary    = source[0].path.split('.')[0] + '.hex',
                semaphore = semaphore
            )
        ]

    # Start threads
    for t in threadList:
        t.start()

    # Wait for threads to finish
    for t in threadList:
        semaphore.acquire()

##
# Pass a list, a range or all ports to do the bootloading
# list  -> /dev/ttyUSB0,ttyUSB1,/dev/ttyUSB2
# range -> /dev/ttyUSB[0-2] = /dev/ttyUSB0,ttyUSB1,/dev/ttyUSB2
# all   -> /dev/ttyUSBX = /dev/ttyUSB0,ttyUSB1,/dev/ttyUSB2
##
def OpenMoteCC2538_expand(ports):
    # Process only when there is a single port
    if (len(ports) == 1):
        port  = ports[0]
        ports = []
        last_char = port[-1:]
        base_dir  = os.path.dirname(port)      

        # /dev/ttyUSBX means bootload all ttyUSB ports
        if (last_char == "X"):
            base_file = os.path.basename(port[:-1])
            ports     = sorted(glob.glob(os.path.join(base_dir, base_file) + "*"))

        # /dev/ttyUSB[1-2] means bootload a range of ttyUSB ports
        elif (last_char == "]"):
            base_file   = os.path.basename(port.split('[')[0])
            first, last = sorted(map(int, ((port.split('['))[1].split(']')[0]).split('-')))

            # For all elements in range
            for i in range(first, last + 1):
                p = os.path.join(base_dir, base_file + str(i))
                ports.append(p)
        else:
            ports = [port]

    # Check that the ports exist
    # ports = [p for p in ports if os.path.isfile(p)]

    # Check if new list is empty
    if (not ports):
        raise SystemError("Bootload port expansion is empty or erroneous!")

    return ports


################################################################################

def BootloadFunc():
    if (env['board'] == 'openmote-usb') or (env['board'] == 'openmote-tbd'):
        return Builder(
            action      = OpenMoteCC2538_bootload,
            suffix      = '.phonyupload',
            src_suffix  = '.bin',
        )
    else:
        raise SystemError('Bootloading on board={0} unsupported.'.format(env['board']))

if env['bootload']:
    env.Append(BUILDERS = {'Bootload' : BootloadFunc()})

################################################################################

if board not in boards.keys():
    raise SystemError("Error, target not defined!")

if verbose:
    print("Building project={0} for board={1} using platform={2} and toolchain={3}.".format(project, board, platform, toolchain))

################################################################################

if (cpu == 'cortex-m3'):
    env.Replace(CC          = toolchain  + '-gcc')
    env.Replace(CXX         = toolchain + '-g++')
    env.Replace(AS          = toolchain + '-as')
    env.Replace(OBJCOPY     = toolchain + '-objcopy')
    env.Replace(OBJDUMP     = toolchain + '-objdump')
    env.Replace(AR          = toolchain + '-ar')
    env.Replace(RANLIB      = toolchain + '-ranlib')
    env.Replace(NM          = toolchain + '-nm')
    env.Replace(SIZE        = toolchain + '-size')

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
   env[  'LINKCOMSTR']  = "Linking            $TARGET"
   env['SHLINKCOMSTR']  = "Linking (shared)   $TARGET"
   env[  'INSTALLSTR']  = "Installing         $TARGET"

################################################################################

env.Append(
    CPPPATH = [
        os.path.join('#','platform', 'inc'),
        os.path.join('#','platform', platform),
        os.path.join('#','freertos', 'common'),
        os.path.join('#','freertos', 'inc'),
        os.path.join('#','freertos', cpu),
        os.path.join('#','projects', project),
        os.path.join('#','board', board),
        os.path.join('#','drivers', 'inc'),
        os.path.join('#','drivers', 'adxl34x'),
        os.path.join('#','drivers', 'cc1200'),
        os.path.join('#','drivers', 'max44009'),
        os.path.join('#','drivers', 'si7006'),
        os.path.join('#','net', 'ethernet'),
        os.path.join('#','net', 'ieee802154'),
        os.path.join('#','stack', 'inc'),
        os.path.join('#','stack', 'src'),
        os.path.join('#', 'test', project)
    ]
)

################################################################################

if (platform == 'cc2538'):
    env.Append(
        CPPPATH = [
            os.path.join('#','platform', platform, 'libcc2538'),
            os.path.join('#','platform', platform, 'libcc2538', 'inc'),
            os.path.join('#','platform', platform, 'libcc2538', 'src')
        ]
    )
    lib_name += [platform]
else:
    raise SystemError("Error, platform not valid!")

################################################################################

# Build all targets and dependencies
for folder in src_folders:
    if verbose:
        print("Parsing folder={0}.".format(folder))
    script = os.path.join("#", folder, "SConscript")
    build  = os.path.join(build_dir, folder)
    output = env.SConscript(script, variant_dir=build, duplicate=0, exports = 'env')
