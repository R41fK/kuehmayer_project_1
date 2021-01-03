# kuehmayer_project_1

A small example of an elevator control.

Each floor and each Elevator is represented by a thread.
There is a central coordinator, that decides where and which elevator should move to a certain floor.
To simulate actions a REPL is built-in, with that the user can simulate button clicks.
In the CLI there are options to choose how much elevators and floors are simulated.
There are also options for the travel time between the floors and if there the possibility of an override in the CLI.