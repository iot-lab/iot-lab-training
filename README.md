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

2. Setup the virtual machine using [vagrant](https://www.vagrantup.com/downloads.html):

  ```
  $ cd iot-lab-training
  $ IOTLAB_LOGIN=<login> vagrant up (Replace <login> with your IoT-LAB login)
  ```

3. Connect to JupyterLab at http://localhost:8888/?token=iotlab
  
4. In JupyterLab, click on the start.ipynb file

*Graceful shutdown of the VM:*
```
$ vagrant halt
```
