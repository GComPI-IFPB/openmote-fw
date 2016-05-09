import os

cmd_options = {
    'board'     :          ['openmote-cc2538'],
    'project'   :          ['earthquake', 'freertos-cc2538', 'freertos-tickless-cc2538', 'ieee802154-sniffer'],
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
        'project',                                         # key
        '',                                                # help
        cmd_options['project'][0],                         # default
        validate_option,                                   # validator
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

