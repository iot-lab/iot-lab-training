## Welcome to the IoT-LAB training resources
## non
### Description

This repository contains the Jupyter Notebooks available at
https://labs.iot-lab.info and used ad training resources to easily start using
the IoT-LAB testbed.

### Getting started

We recommend that you follow the `start.ipynb` notebook to learn how to use
Jupyter Notebooks.

- Notebooks in **riot** will let you discover the [RIOT](https://riot-os.org)
  operating system:
  - **riot/basics** contains exercises on basics features of RIOT
  - **riot/networking** contains networking exercises (802.15.4, IPv6, UDP, CoAP
    and LwM2M)
  - **riot/lorawan** contains exercises on LoRaWAN LPWAN networks
  - **riot/security** contains exercises on security aspects for IoT (hash,
    crypto, DTLS and OTA)
- Notebooks in **contiki-ng** contains exercises to discover the
  [Contiki-NG](https://www.contiki-ng.org/) operating system
- Notebooks in **testbed** shows advanced features of the IoT-LAB testbed:
  - for measuring power consumption on IoT-LAB M3 devices
  - for sniffing radio paquets between IoT-LAB M3 devices

### Contribute

1. Clone this repository:

  ```
  $ git clone --recursive https://github.com/iot-lab/iot-lab-training
  ```

2. Start a local Jupyterlab server using [Docker](https://www.docker.com/)
  (only tested on Linux and OSX)

  ```
  $ cd iot-lab-training
  $ IOTLAB_LOGIN=<login> make run-docker
  ```

3. Connect to JupyterLab at http://localhost:8888/?token=iotlab

You can then edit the notebooks in your browser. Make sure to clear all the
outputs before commiting your changes

## License

All content - notebooks and code snippets - is licensed under the Creative commons license
[CC-BY-ND](https://creativecommons.org/licenses/by-nd/4.0/)

![CC-BY-ND](https://mirrors.creativecommons.org/presskit/buttons/80x15/png/by-nd.png)

