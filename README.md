## Welcome to the IoT-LAB training resources

### Description

This repository contains training resources to easily start using the IoT-LAB
testbed.

The training resources are based on Jupyter notebooks that are run in a
preconfigured virtual machine.

### Setup

1. Close this repository:

  ```
  $ git clone https://github.com/iot-lab/iot-lab-training
  ```

2. Setup the virtual machine using [vagrant](https://www.vagrantup.com/downloads.html):

  ```
  $ cd iot-lab-training
  $ vagrant up
  ```

3. Start JupyterLab

  1. Connect to the VM:

  ```
  $ vagrant ssh
  ```

  2. Launch JupyterLab:

  ```
  $ cd iot-lab-training
  $ ./bootstrap_jupyter.sh
  ```
  Keep this terminal opened during your training session.

  3. Shift+click on the link returned by the previous script: `http://127.0.0.1:8888/?token=...`
  
  4. In JupyterLab, click on the start.ipynb file

*Shutdown:*
  - Stop JupyterLab by pressing Ctrl + C keyboard shortcut twice
  - Exit the VM with the `exit` command (or use the Ctrl + D keyboard shortcut)
  ```
  user@iotlab-vm:~/iot-lab-training$ exit
  ```
  - Halt the VM:
  ```
  $ vagrant halt
  ```
