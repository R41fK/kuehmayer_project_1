# NVS project: elevator control

A small example of an elevator control.

Each floor and each Elevator is represented by a thread.
There is a central coordinator, that decides where and which elevator should move to a certain floor.
To simulate actions a REPL is built-in, with that the user can simulate button clicks.
In the CLI there are options to choose how much elevators and floors are simulated.
There are also options for the travel time between the floors and if there the possibility of an override in the CLI.

## Usage
The programme is started by calling `./elevator_control` on the command line. 

When called without parameters, the programme starts with 3 floors, 1 elevator and 3 seconds travel time.

## Parameter
### Programm parameters
#### -s,--seconds-between-floors <positive float>
change the travel time of an elevator between floors.

#### -f,--floor-number <unsigned int>
change the number of floors. 

#### -e,--elevators <unsigned int>
change the number of elevators.

#### -o,--override
activate the override flag.

### Simulation parameters
#### --simulation
use a simulation instead of the REPL.

#### --simulation-time <positive float>
change the time the simulation waits, between commands.

### Config parameters
#### -j,--config-file-json
Use a JOSN-file instead of the Programm parameters.

#### -t,--config-file-toml
Use a TOML-file instead of the Programm parameters.

### Logging parameters
#### -l,--log-to-file
Activate logging into a file. Default logginglevel is info.

#### -d,--log-level-debug
Change the logginglevel to debug.

#### --log-file <filepath>
Change the logfile. (File must not exist)


## Libraries
[CLI11](https://github.com/CLIUtils/CLI11)

[cpp-peglib](https://github.com/yhirose/cpp-peglib)

[json](https://github.com/nlohmann/json)

[rang](https://github.com/agauniyal/rang)

[spdlog](https://github.com/gabime/spdlog)

[toml++](https://github.com/marzer/tomlplusplus/)
