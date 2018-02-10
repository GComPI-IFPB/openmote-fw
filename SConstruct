import os

cmd_options = {
    'board'     :          ['openmote-b'],
    'chip'      :          ['cc2538sf53'],
    'project'   :          ['freertos-cc2538', 'freertos-cc2538-tickless', 'ieee802154-sniffer',
                            'test-aes', 'test-board', 'test-crc', 'test-openmote-b', 'test-radio-at86rf215', 
                            'test-radio-cc2538', 'test-radiotimer', 'test-rendezvous', 'test-serial',
                            'test-sleeptimer', 'test-spi', 'test-pwm', 'test-task', 'test-timer', 'test-uart'],
    'verbose'   :          ['0','1']
}

def validate_option(key, value, env):
    if key not in cmd_options:
        raise ValueError("Unknown switch {0}.".format(key))
    if value not in cmd_options[key]:
        raise ValueError("Unknown {0} \"{1}\". Options are {2}.\n\n".format(key,value,','.join(cmd_options[key])))

cmd_vars = Variables()
cmd_vars.AddVariables(
    (
        'board',                                           # key
        '',                                                # help
        cmd_options['board'][0],                           # default
        validate_option,                                   # validator
        None,                                              # converter
    ),
    (
        'chip',                                            # key
        '',                                                # help
        cmd_options['chip'][0],                            # default
        validate_option,                                   # validator
        None,                                              # converter
    ),
    (
        'project',                                         # key
        '',                                                # help
        None,                                              # default
        validate_option,                                   # validator
        None,                                              # converter
    ),
    (
        'bootload',                                        # key
        '',                                                # help
        '',                                                # default
        None,                                              # validator
        None,                                              # converter
    ),
    (
        'params',                                          # key
        '',                                                # help
        '',                                                # default
        None,                                              # validator
        None,                                              # converter
    ),
    (
        'verbose',                                         # key
        '',                                                # help
        cmd_options['verbose'][0],                         # default
        validate_option,                                   # validator
        int,                                               # converter
    )
)

path = ["/bin", "/usr/bin", "/opt/gcc-arm-none-eabi/bin"]
env = Environment(variables = cmd_vars, ENV = {"PATH" : path})

Export('env')

env.SConscript(
    'SConscript',
    exports = ['env'],
)

