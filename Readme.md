[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/Kreijstal/electro-plating-lines)

# Electro Plating Lines Project

This project contains two programs: `multiproductrewrite` and `multi_robot`. Both programs are designed to help model and manage tasks in a multi-product, multi-robot electroplating environment.

## Prerequisites 

This project relies on the `Hadamard_product_counters` library which should be located in the directory neighboring the current one. 

If you do not already have this library, you can clone it from the Git repository:

```
git clone git@git.tu-berlin.de:kreijstal/Hadamard_product_counters.git
```

Please ensure that the `Hadamard_product_counters` directory is located next to the `electro_plating_lines` directory.

## Cloning This Repository

To clone this repository, use the following command:

```
git clone git@git.tu-berlin.de:kreijstal/electro_plating_lines.git
```

## Quick Setup

The project includes an `init.sh` script for bash that checks/clones the `Hadamard_product_counters` dependency and builds the programs. 

To use it, navigate to the project directory and run:

```
./init.sh
```

If the `Hadamard_product_counters` directory exists and is a Git repository, the script will proceed to build the programs. If the directory exists but is not a Git repository, the script will exit with an error message.

If the `Hadamard_product_counters` directory does not exist, the script will attempt to clone the repository from GitHub and then build the programs.

## How to Compile

If you prefer to compile manually, use the `make` command in the project directory.

To compile all programs, run:

```bash
make all
```

## How to Run the Programs

After successfully compiling the project, you can run either of the programs using the following commands:

For `multiproductrewrite`:

```bash
./multiproductrewrite
```

For `multi_robot`:

```bash
./multi_robot
```

Both programs are interactive and will prompt for user input once started. Please follow the on-screen instructions to provide the necessary input.

## Description of Programs

1. `multiproductrewrite`: This program models and manages tasks for electroplating lines with multiple products. 

2. `multi_robot`: This program extends the capabilities of `multiproductrewrite` by adding the management of multiple robots.