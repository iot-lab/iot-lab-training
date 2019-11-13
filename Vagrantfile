Vagrant.configure("2") do |config|
  config.vm.box = "iotlab/iotlab-vm"
  config.ssh.username = "user"
  config.vm.network "forwarded_port", guest: 8888, host: 8888
  config.vm.synced_folder ".", "/home/user/iot-lab-training"
  config.vm.provider "virtualbox" do |v|
    v.name = "iotlab-vm"
  end
end
