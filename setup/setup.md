---
jupyter:
  jupytext:
    text_representation:
      extension: .md
      format_name: markdown
      format_version: '1.3'
      jupytext_version: 1.19.3
  kernelspec:
    display_name: Python 3 (ipykernel)
    language: python
    name: python3
---

## Training setup

**If you are using Jupyterlab via the Mooc or via https://labs.iot-lab.info, please don't follow this notebook** because your account is already setup.

### Setup your credentials

IoT-LAB CLI tools used in Jupyter notebooks request a REST API to interact with your experiments on the testbed. All requests require your IoT-LAB credentials, so to avoid having to specify them each time, you have to setup your IoT-LAB account on the VM:

1. Start a new JupyterLab Terminal (use `File > New > Terminal`)
2. Run the following command, replacing "login" with your actual login on the testbed:

<!-- #raw -->
iotlab-auth -u $IOTLAB_LOGIN
<!-- #endraw -->

3. Verify the setup by executing the following cell, which prints status of the resources on the Lyon site:

```python
!iotlab-status --nodes-ids --site lyon
```

### Setup your SSH key

To be able to connect to an IoT-LAB site server, you have to add your SSH key to your user account.

1. Generate a pair of SSH keys from the JupyterLab Terminal:

```python
!rm -f ~/.ssh/id_rsa* && ssh-keygen -q -N "" -C iot-lab-training -f ~/.ssh/id_rsa
```

2. Get the SSH public key by running the following cell:

```python
!iotlab-auth --add-ssh-key
```

3. Copy the output of the previous command and add it as authorized SSH key for your account:  
https://www.iot-lab.info/testbed/account (SSH Keys tab)

4. Verify the setup by connecting to the Lyon's server site running the following command in the JupyterLab Terminal:

<!-- #raw -->
ssh $IOTLAB_LOGIN@lyon.iot-lab.info
<!-- #endraw -->
