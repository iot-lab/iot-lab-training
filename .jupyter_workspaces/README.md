## Managing workspaces

Doc adapted from Jupyter official documentation:
https://jupyterlab.readthedocs.io/en/stable/user/urls.html#managing-workspaces-ui

1. Prepare your Jupyter Lab environment:
  - Open a specific workspace url: `http://localhost:8888/lab/workspaces/<workspace name>`
  - Prepare the tabs and layout (notebook, files, terminal)

2. Export the workspace using Jupyter:
  - SSH in the VM:
    ```
    $ cd iot-lab-training
    vagrant ssh
    ```
  - Export the workspace:
    ```
    jupyter lab workspaces export <workspace name> > iot-lab-training/.jupyter_workspaces/<workspace name>.json
    ```

## Import a workspace

In a new session:

```
jupyter lab workspaces import iot-lab-training/.jupyter_workspaces/<workspace name>.json
```
