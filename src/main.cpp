// Copyright (c) 2025 Raion Robotics Inc.
//
// Any unauthorized copying, alteration, distribution, transmission,
// performance, display or use of this material is prohibited.
//
// All rights reserved.

#include "raisin_vicon_node/raisin_vicon_publisher.hpp"
#include "raisin_network/network.hpp"

using namespace raisin;

int main() {
  parameter::ParameterContainer& param = parameter::ParameterContainer::getRoot();
  param.loadFromPackageParameterFile("raisin_vicon_node");

  std::string id = param("id", "");
  int network_type = param("network_type", 0);
  std::vector<std::string> network_interface = param("network_interface", std::vector<std::string>{});
  std::string peer_id = param("peer_id", "");
  std::string peer_ip = param("peer_ip", "");
  std::string vicon_address = param("vicon_address", "");
  std::vector<std::string> subjects = param("subjects", std::vector<std::string>{});

  std::vector<std::vector<std::string>> thread = {{std::string("main")}};
  std::shared_ptr<raisin::Network> network = std::make_shared<raisin::Network>(id, "test", thread, network_interface);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::shared_ptr<raisin::Remote::Connection> connection_;
  while (!connection_) {
    if (network_type == 0) {
      connection_ = network->connect(peer_id);
    } else if (network_type == 1) {
      connection_ = network->connect(peer_ip, 9002);
    } else {
      std::cerr<<"invalid network type"<<std::endl;
    }
    if (!connection_) {
      if (network_type == 0) {
        std::cerr<<"Failed to connect to server " << peer_id <<std::endl;
      } else if (network_type == 1) {
        std::cerr<<"Failed to connect to server " << peer_ip <<std::endl;
      }
    } else {
      if (network_type == 0) {
        std::cout<<"Connected to server " << peer_id  <<std::endl;
      } else if (network_type == 1) {
        std::cout<<"Connected to server " << peer_ip << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  ViconPublisher ps(network, vicon_address, subjects);

  while (true) {}
  return 0;
}