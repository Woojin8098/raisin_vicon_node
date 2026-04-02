// Copyright (c) 2025 Raion Robotics Inc.
//
// Any unauthorized copying, alteration, distribution, transmission,
// performance, display or use of this material is prohibited.
//
// All rights reserved.

#ifndef RAISIN_VICON_PUBLISHER_HPP_
#define RAISIN_VICON_PUBLISHER_HPP_

#include "raisin_network/node.hpp"
#include "DataStreamClient.h"
#include "raisin_interfaces/msg/pose.hpp"

namespace raisin
{

class ViconPublisher : public Node {

 public:
  ViconPublisher(std::shared_ptr<Network> network, std::string host_address, std::vector<std::string> subjects);
  ~ViconPublisher();
  void viconUpdate();

 private:
  ViconDataStreamSDK::CPP::Client client_;
  std::string host_address_;
  int buffer_size_;
  std::vector<std::string> subjects_;
  raisin_interfaces::msg::Pose pose_;
  std::vector<Publisher<raisin_interfaces::msg::Pose>::SharedPtr> publishers_;
};

}  // namespace raisin

#endif  // RAISIN_VICON_PUBLISHER_HPP_
