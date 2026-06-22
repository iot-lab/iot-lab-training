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

## Update the training exercises

The training exercises comes from the [https://github.com/iot-lab/iot-lab-training](https://github.com/iot-lab/iot-lab-training) repository and are continuously evolving in order to add new exercises or to simply follow the updates added to the new IoT operating systems (RIOT, contiki-ng) versions.

The training directory in the jupyterlab server is just a git clone of the [https://github.com/iot-lab/iot-lab-training](https://github.com/iot-lab/iot-lab-training) repository so you can use git to update it.

For those who are not comfortable with git, this notebook proposes an easy way to:
- snapshot your current exercises status in a git branch,
- fetch the latest versions of the notebooks and submodules (riot, contiki-ng)

The snapshot branch will be called `snapshot-<year><month><day>-<hour><minute><second>`.

**Warning:** after performing an update with the proposed method, all your current changes will be commited to the `snapshot-<year><month><day>-<hour><minute><second>` branch and you will start with a clean and updated version of the training notebooks.

Just execute the following cell with `Ctrl+Enter` keyboard shortcut:

```python
# Switch to new snapshot branch
!git checkout -b snapshot-$(date +"%Y%m%d-%H%M%S")

# Commit the current state
!git -c user.name='Jupyter Labs' -c user.email='labs@iot-lab.info' commit -am "Training snapshot from $(date +"%Y%m%d-%H%M%S")"

# Switch back to main
!git checkout main

# Pull the latest version of the training and update the submodules
!git pull --recurse-submodules
!git submodule update --recursive
```

If you want to list the available branches, run:

```python
!git branch
```

To switch to an old snapshot branch, run (replace `<date>` and `<time>` with values from a valid branch name):

```python
!git checkout snapshot-<date>-<time>
```
