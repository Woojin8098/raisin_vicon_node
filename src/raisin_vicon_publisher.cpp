// Copyright (c) 2025 Raion Robotics Inc.
//
// Any unauthorized copying, alteration, distribution, transmission,
// performance, display or use of this material is prohibited.
//
// All rights reserved.

// raisin inclusion
#include "raisin_vicon_node/raisin_vicon_publisher.hpp"

// std inclusion
#include <string>

namespace raisin
{

ViconPublisher::ViconPublisher(std::shared_ptr<Network> network, std::string host_address, std::vector<std::string> subjects) :
  Node(network), host_address_(host_address), subjects_(subjects) {

  publishers_.reserve(subjects_.size());
  for (const auto& subject : subjects_) {
    publishers_.emplace_back(createPublisher<raisin_interfaces::msg::Pose>(subject));
  }

  while (!client_.IsConnected().Connected) {
    std::cout << "Connecting Vicon  " << host_address_ <<  std::endl;
    client_.Connect(host_address_);
  }
  std::cout << "Connected" << std::endl;

  client_.EnableSegmentData();
  client_.EnableMarkerData();
  if (client_.EnableMarkerData().Result != ViconDataStreamSDK::CPP::Result::Success) {
    throw std::runtime_error("Failed to enable marker data.");
  }
  if (client_.EnableSegmentData().Result != ViconDataStreamSDK::CPP::Result::Success) {
    throw std::runtime_error("Failed to enable segment data.");
  }
  client_.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ServerPush);
  client_.SetBufferSize(10);

  createTimedLoop("vicon_publisher",
    [&](){ viconUpdate(); },
    300.,
    "main");
}

ViconPublisher::~ViconPublisher() {
  /// YOU MUST CALL THIS METHOD IN ALL NODES
  cleanupResources();
}

void ViconPublisher::viconUpdate()
{
  auto Output = client_.GetFrame();
  if(Output.Result == ViconDataStreamSDK::CPP::Result::Success) {
    ViconDataStreamSDK::CPP::Output_GetFrameNumber frame_number = client_.GetFrameNumber();
    unsigned int subject_num = client_.GetSubjectCount().SubjectCount;
    for (unsigned int subject_idx = 0; subject_idx < subject_num; subject_idx++) {
      std::string subject_name = client_.GetSubjectName(subject_idx).SubjectName;
      unsigned int segment_num = client_.GetSegmentCount(subject_name).SegmentCount;
      std::string segment_name = client_.GetSegmentName(subject_name, 0).SegmentName;
      ViconDataStreamSDK::CPP::Output_GetTimecode outputTimeCode = client_.GetTimecode();
      ViconDataStreamSDK::CPP::Output_GetFrameRate outputFrameRate = client_.GetFrameRate();
      ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation trans =
          client_.GetSegmentGlobalTranslation(subject_name, segment_name);
      ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationMatrix rot =
          client_.GetSegmentGlobalRotationMatrix(subject_name, segment_name);
      ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationQuaternion quat =
          client_.GetSegmentGlobalRotationQuaternion(subject_name, segment_name);

      const auto now = std::chrono::system_clock::now();
      const double now_sec = std::chrono::duration<double>(now.time_since_epoch()).count();
      pose_.timestamp = now_sec;
      pose_.position[0] = trans.Translation[0] / 1000.0;
      pose_.position[1] = trans.Translation[1] / 1000.0;
      pose_.position[2] = trans.Translation[2] / 1000.0;
      pose_.quaternion[0] = quat.Rotation[0];
      pose_.quaternion[1] = quat.Rotation[1];
      pose_.quaternion[2] = quat.Rotation[2];
      pose_.quaternion[3] = quat.Rotation[3];

      publishers_[subject_idx]->publish(pose_);
    }
  }
}

}  // namespace raisin
