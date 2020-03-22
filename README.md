## Welcome to the IoT-LAB training resources

### Description

This repository contains training resources to easily start using the IoT-LAB
testbed.

The training resources are based on Jupyter notebooks that are run in a
preconfigured virtual machine.

### Setup

1. Close this repository:

  ```
  $ git clone --recursive https://github.com/iot-lab/iot-lab-training
  ```

2. Setup your working environment. You have 2 choices:

  1. Using the [vagrant](https://www.vagrantup.com/downloads.html) virtual machine:

  ```
  $ cd iot-lab-training
  $ IOTLAB_LOGIN=<login> vagrant up (Replace <login> with your IoT-LAB login)
  ```

  2. Using [Docker](https://www.docker.com/) (only tested on Linux)

  ```
  $ cd iot-lab-training
  $ IOTLAB_LOGIN=<login> make run-docker
  ```

3. Connect to JupyterLab at http://localhost:8888/?token=iotlab
  
4. In JupyterLab, click on the start.ipynb file

*Graceful shutdown of the VM:*
```
$ vagrant halt
```
