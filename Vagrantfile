# encoding: utf-8
# -*- mode: ruby -*-
# vi: set ft=ruby :

$set_environment_variables = <<SCRIPT
tee "/etc/profile.d/iotlab-env.sh" > "/dev/null" <<EOF
IOTLAB_USER_NAME=#{ENV['IOTLAB_USER_NAME']}
EOF
SCRIPT


Vagrant.configure("2") do |config|
  config.vm.box = "iotlab/iotlab-vm"
  config.ssh.username = "user"
  config.vm.network "forwarded_port", guest: 8888, host: 8888
  config.vm.provision "shell", inline: "su - user bash -c 'cd ~/iot-lab-training && ./bootstrap_jupyter.sh &'", run: "always"
  config.vm.provision "shell", inline: $set_environment_variables, run: "always"
  config.vm.synced_folder ".", "/home/user/iot-lab-training"
  config.vm.provider "virtualbox" do |v|
    v.name = "iotlab-vm"
  end
end
