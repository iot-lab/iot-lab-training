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

# Welcome to the IoT-LAB testbed training

This repository contains training resources to easily start using the IoT-LAB testbed.

## Short introduction to JupyterLab

[JupyterLab](https://jupyterlab.readthedocs.io/en/latest/) enables you to work with documents and activities such as Jupyter notebooks, code editors and terminals. You can arrange multiple documents and activities side by side in the work area using tabs and splitters.

Jupyter notebooks are documents that combine live runnable code with narrative text (Markdown), equations (LaTeX), images, interactive visualizations and other rich output. It is used by scientists and scholars to create and share their research, especially in the data science community.

For a complete introduction to Jupyter Notebooks, refer to the [official user documentation](https://jupyterlab.readthedocs.io/en/latest/user/notebook.html).

In the sections below, we describe the minimum required functions that will allow to follow the notebooks available in this training.

The notebook consists of a sequence of cells:
- A cell is a multiline text input field, and its contents can be executed by using **Ctrl+Enter**, or by clicking either **the “Play” button**
- Use the _up_ and _down_ arrows of the keyboard to navigate between cells
- The **Shift+Enter** keyboard shortcut executes the currently selected cell and selects the next one automatically

### Code cells

A code cell allows you to edit and write new code, with full syntax highlighting and tab completion.

In general, Jupyter notebooks contain Python code. Once selected the cell should be executed with **Ctrl+Enter** shortcut. The results that are returned from this computation are then displayed in the notebook as the cell’s output.
 

```python
print("Hello Jupyter!")
```

In our notebooks you will mainly find shell commands executed in the Jupyterlab server. In this case the cell should start with exclamation mark.

```python
!echo "Hello Jupyter!"
```

Finally you will also find lines starting with the '%' character. They are used to set environment variables.  

```python
%env HELLO_JUPYTER="Hello Jupyter!"
!echo $HELLO_JUPYTER
```


### Raw cells

A raw cell provides a place in which you can write output directly. Raw cells are not evaluated by the notebook. 

In our context we use raw cells when commands need user interaction. In this case you must copy/paste them from the raw cell to the good destination using the usual **Ctrl+C** and **Ctrl+V** keyboard shortcuts and execute them directly.

There are 3 categories of commands:

1. Commands that must be run in a **JupyterLab terminal**:

To open a Jupyterlab terminal, select the top leftmost menu entry in `File > New > Terminal`. You can try with the cell below which display string that is passed as an argument.

<!-- #raw -->
echo "Hello Jupyter!"
<!-- #endraw -->

2. Commands that must be run on the **SSH frontend** of an IoT-LAB site: they are prefixed with the prompt `<login>@<site>:~$ `.

This means that before running this command after the prompt, you should open a SSH connection to the site frontend with a JupyterLab terminal as below.

<!-- #raw -->
ssh $IOTLAB_LOGIN@<site>.iot-lab.info
<!-- #endraw -->

You can replace `<site>` string among the following IoT-LAB sites name: **grenoble**, **lille**, **saclay** and **strasbourg**.

For example, to execute the `echo` command the raw cell will contain:

<!-- #raw -->
<login>@<site>:~$ echo "Hello SSH frontend!"
<!-- #endraw -->

3. Commands that must be run in an **embedded OS shell** (i.e. RIOT or Contiki-NG) : they are prefixed with the prompt `>`.

In the following example you use the `ifconfig` command of the RIOT shell which displays the configuration of the network interfaces.

<!-- #raw -->
> ifconfig
<!-- #endraw -->

Sometimes the expected output of the command is also given in the raw cells to help you and check the good execution:

<!-- #raw -->
> ifconfig
Iface  7  HWaddr: 11:15  Channel: 26  Page: 0  NID: 0x23
          Long HWaddr: 22:5C:FC:65:10:6B:11:15 
          ...
<!-- #endraw -->

## How to update the notebooks

The first time you login to [https://labs.iot-lab.info](https://labs.iot-lab.info), the training repository is clone in the `~/work/training` directory. If you want to fetch the latest version of the training exercises, you can follow the [update notebook](update.md) at the root of jupyterlab server.

If you feel confortable with git, you can also use git directly from a terminal.
